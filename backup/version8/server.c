#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h> 
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include "linklist.h"
#include "tcp.h"

#include "login_protocol.h"
#include "auction_protocol.h"
#include "web_protocol.h"
#include "user_database.h"
#include "roomlist.h"
#include "queue.h"

#define BACKLOG 1024   /* Number of allowed connections */
#define MAX_WRONGPASSWORD 3
#define BUFF_SIZE 1024
Node **database_link_list;
typedef struct map_socket_to_room_type{
	Room *room;
}map_socket_to_room_type;


enum Protocol_Group_Id{
	login_protocol =1,
	web_protocol =2,
	auction_protocol=3
};
typedef struct SESSION
{
	enum Protocol_Group_Id protocol_group_id;
	
	enum Login_ProtocolState login_state;
	enum Web_ProtocolState web_state;
	enum Auction_ProtocolState auction_state;

	Auction_Data auction_data;
	Login_Data login_data;
	Web_Data web_data;
}SESSION;


// enum Auction_ProtocolState[BACKLOG];



//============================ FUNCTION =======================
//load data from account.txt to database_link_list

//check all character in string s is number or not
int numbers_only(char *s)
{
  int i=0;
  while (s[i]!='\0') {
    // compare current character to 0 and 9
    if (s[i]<'0' || s[i]>'9') return 0;
    i++;
  }
  return 1;
}
//validate console param
void validate_input(int argc,char *argv[]){

	if(argc!=2){
		printf("Usage: ./server 5000\n");
		exit(1);
	}
	if(numbers_only(argv[1])!=1){
		printf("port number must be a number\n");
		exit(2);
	}
}


int init_new_listen_socket(int PORT){
	struct sockaddr_in server;
	int listenfd;
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
		perror("\nError: ");
		exit(-1);
	}
	
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(PORT);   
	server.sin_addr.s_addr = htonl(INADDR_ANY);  
	if(bind(listenfd, (struct sockaddr*)&server, sizeof(server))==-1){ 
		perror("\nError: ");
		exit(-1);
	}     
	
	//Listen request from client
	if(listen(listenfd, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		exit(-1);
	}
	return listenfd;
}

int login_protocol_handle(SESSION * session,int sockfd,fd_set *readfds,fd_set *writefds,fd_set *exceptfds,fd_set *checkfds_read,fd_set *checkfds_write){
	login_message msg;
	int code;
	printf("login protocol handle\n");
	switch(session->login_state){
					case no_connect:
							printf("\tid handle\n");
							if (FD_ISSET(sockfd, readfds))
							 {
								if(receive_message(sockfd,&msg)!=0){return -1;}
								switch(msg.code){
									case USERID:
									printf("%s\n",(char*)msg.data );
										code=search_userid(&(session->login_data).user,(char*)msg.data);
										if (code==-1){
											if(send_USERID_NOTFOUND(sockfd)==-1){return -1;}
										}
										else if(code==-2){
											if(send_USERID_BLOCK(sockfd)==-1){return -1;};
										}
										else if(code ==-3){
											if(send_USERID_ISSIGNIN(sockfd)==-1){return -1;};
										}
										else{
											if(send_USERID_FOUND(sockfd)==-1){return -1;};
												session->login_state=correct_id;
												FD_SET(sockfd,checkfds_read);
												FD_CLR(sockfd,checkfds_write);
											}
										}
										break;
									default:
										if(send_UNKNOWN(sockfd,msg))
											{return -1;};
										break;
							}
						break;
					case correct_id:
						printf("\tpassword handle\n");
						if(receive_message(sockfd,&msg)==-1)
							{return -1;}
						if(msg.code==PASSWORD){
							if(strcmp((session->login_data).user->password,(char*)msg.data)==0){
								if(send_PASSWORD_RIGHT(sockfd)==-1){
									return -1;
								}
								session->login_state=authorized;
								FD_CLR(sockfd,checkfds_write);
								FD_SET(sockfd,checkfds_read);
								(session->login_data).user->soluongdangnhapsai=0;
								session->protocol_group_id=web_protocol;
								session->web_state=web_authorized;
								(session->web_data).user = (session->login_data).user;
							}
							else{
								if(strcmp("q",(char*)msg.data)!=0){
									printf("%d\n",(session->login_data).user->soluongdangnhapsai);
									if((session->login_data).user->soluongdangnhapsai>=MAX_WRONGPASSWORD){
										(session->login_data).user->status=0;
										export_to_data_file();
										if(send_PASSWORD_BLOCK(sockfd)==-1)
											{return -1;}
										session->login_state=no_connect;
									}
									else{
										(session->login_data).user->soluongdangnhapsai+=1;
										// 
										if(send_PASSWORD_WRONG(sockfd)==-1)
											{return -1;}

									}
								}
								else{
									session->login_state=no_connect;
								}
							}
						}
						else if(msg.code==RESET){
							session->login_state=no_connect;
							FD_CLR(sockfd,checkfds_write);
							FD_SET(sockfd,checkfds_read);
						}
						else{
							if(send_UNKNOWN(sockfd,msg)==-1)
								{return -1;}
						}

						 break;
					case authorized:
						if(receive_message(sockfd,&msg)==-1)
							{return -1;}
						switch(msg.code){
							case LOGOUT:
								if(send_LOGOUT_SUCCESS(sockfd)==-1)
									{return -1;}
								session->login_state=no_connect;
							break;
							default:
								if(send_UNKNOWN(sockfd,msg)==01)
									{return -1;}
							break;
						}
						break;
					}
					return 0;
}
int web_protocol_handle(SESSION * session,SESSION * all_sesssion,int *client,int sockfd,
	fd_set *readfds,fd_set *writefds,fd_set *exceptfds,fd_set *checkfds_read,fd_set *checkfds_write,
	map_socket_to_room_type map_socket_to_seeing_room,
	Room **header,Room **my_room){

	web_message msg;
	switch(session->web_state){
		case web_not_authorzied:
			break;
		case web_authorized:
			printf("authorized\n");
			if(receive_web_message(sockfd, &msg) == 0)
			{
				printf("receive_message successfully\n");
				ROOM_LIST_PARAM *room_list_param;
				MAKE_ROOM_PARAM *make_room_param;
				MY_ROOM_LIST_PARAM *my_room_list_param;
				ROOM_DETAIL_PARAM *room_detail_param;
				BUY_NOW_PARAM *buy_now_param;
				ENTER_ROOM_PARAM *enter_room_param;
				switch(msg.code){
					case REQUEST_ROOM_LIST:
						room_list_param = (ROOM_LIST_PARAM*)msg.data;
						//todo filter room by param
						ROOM_LIST_RESPOND room_list_respond;
						room_list_respond.header = *get_running_room(*header);
						if(send_RESPOND_ROOM_LIST(sockfd, room_list_respond) != 0)
							{close(sockfd);printf("connect is die\n");return -1;}
						break;
					case REQUEST_ROOM_DETAIL:
						room_detail_param = (ROOM_DETAIL_PARAM*)msg.data;
						ROOM_DETAIL_RESPOND room_detail_respond;
						Room *searchRoom = search_room(header, room_detail_param->room_id);
						if(searchRoom == NULL) 
							room_detail_respond.result = 0;
						else{
							room_detail_respond.result = 1;
							(session->web_data).room = searchRoom;
						}
						room_detail_respond.room = searchRoom;
						if(send_RESPOND_ROOM_DETAIL(sockfd, room_detail_respond) != 0)
							{close(sockfd);printf("connect is die\n");return -1;}
						break;
					case REQUEST_BUY_NOW:
						buy_now_param = (BUY_NOW_PARAM*)msg.data;
						BUY_NOW_RESPOND buy_now_respond;
						char message_buy_now_respond[1000];
						Item *search_item = searchItem(*((session->web_data).room)->product_list, buy_now_param->id);
						//delete product
						if(search_item == NULL){
							strcpy(message_buy_now_respond, "Cannot found product");
						}
						else
						{
							
							deleteItem(((session->web_data).room)->product_list, buy_now_param->id);
							print_all_room(header);
							strcpy(message_buy_now_respond, "Buy successfully");
							
							// NOTIFY_NEW_PRICE_RESPOND notify_new_price_respond;
							// notify_new_price_respond.newprice=search_item->price;
							// notify_new_price_respond.winner_name=(char *)malloc(sizeof(char)*100);
							// strcpy(notify_new_price_respond.winner_name,(session->login_data).user->name);
							// notify_new_price_respond.start=search_item->start;
							// notify_new_price_respond.count=2;

							// for(int i=0;i<FD_SETSIZE;i++){
							// 	if(all_sesssion[i].protocol_group_id==auction_protocol&&(all_sesssion[i].auction_data).room->id==(session->auction_data).room->id){
							// 		if(client[i]!=sockfd){
							// 			if(send_NOTIFY_NEW_PRICE(client[i], notify_new_price_respond))
							// 				return -1;
							// 			printf("send notify to client %d\n",i);
							// 		}


							// 	}
								// else if((all_sesssion[i].web_protocol).room->id==(session->auction_data).room->id && all_sesssion[i]->protocol_state==web_protocol){
								// 	send_NOTIFY_NEW_PRICE(client[i],NOTIFY_NEW_PRICE_RESPOND data);
								// }
							// }
							
						}
						buy_now_respond.message = message_buy_now_respond;
						if(send_RESPOND_BUY_NOW(sockfd, buy_now_respond) != 0)
							{close(sockfd);printf("connect is die\n");return -1;}
						break;
					case REQUEST_MAKE_ROOM:
						make_room_param = (MAKE_ROOM_PARAM*)msg.data;
						Room *new_room = add_new_room(header, count_room(*header)+1, (session->login_data).user->name, make_room_param->product_list, 0);
						add_room(my_room, new_room);
						// todo delete my room and add function to filter my room instead of using my_room array
						char mess[1000] = "Create room successfull";
						MAKE_ROOM_RESPOND make_room_param;
						make_room_param.message = mess;
						if(send_RESPOND_MAKE_ROOM(sockfd, make_room_param) != 0)
							{close(sockfd);printf("connect is die\n");return -1;}
						break;
					case REQUEST_MY_ROOM_LIST:
						my_room_list_param = (MY_ROOM_LIST_PARAM*)msg.data;
						MY_ROOM_LIST_RESPOND my_room_list_respond;
						Room *top = *header;
						Room **header_my_room = create_room_list();
						while(top != NULL){
							if(strcmp(top->username, (session->web_data).user->name) == 0)
								add_room(header_my_room, top);
							top = top->next;
						}
						my_room_list_respond.header = *header_my_room;
						if(send_RESPOND_ROOM_LIST(sockfd, my_room_list_respond) != 0)
							{close(sockfd);printf("connect is die\n");return -1;}
						break;
					case REQUEST_ENTER_ROOM:
						enter_room_param = (ENTER_ROOM_PARAM*)msg.data;
						ENTER_ROOM_RESPOND enter_room_respond;
						Room *enterroom = search_room(header, enter_room_param->room_id);
						if(enterroom == NULL) 
							enter_room_respond.result = 0;
						else{
							enter_room_respond.result = 1;
							enterroom->number_of_member++;
							session->protocol_group_id = auction_protocol;
							(session->auction_data).room = enterroom;
							(session->auction_data).user = (session->web_data).user;
						}
						enter_room_respond.room = enterroom;
						if(send_RESPOND_ENTER_ROOM(sockfd, enter_room_respond) != 0)
							{close(sockfd);printf("connect is die\n");return -1;}
						break;
						break;
					case REQUEST_LOG_OUT:
						printf("logout hanlde\n");
						session->protocol_group_id = login_protocol;
						session->login_state = no_connect;
						session->web_state = web_not_authorzied;
						session->auction_state = no_set_price_or_passed;
						if(send_LOG_OUT_RESPOND(sockfd) != 0)
							{close(sockfd);printf("connect is die\n");return -1;}
						printf("logout hanlde end \n");
						break;
				}
			}else{
				return -1;
			}

			break;
	}
	return 0;

}
int auction_protocol_handle(SESSION * session,SESSION * all_sesssion,int *client,int sockfd,fd_set *readfds,fd_set *writefds,fd_set *exceptfds,fd_set *checkfds_read,fd_set *checkfds_write){

	auction_message msg;
	SET_PRICE_RESPOND set_price_respond;
	EXIT_ROOM_RESPOND exit_room_respond;
	char exit_room[100] = "Exit room successfully";
    exit_room_respond.message = exit_room;
	if(receive_auction_message(sockfd, &msg)!=0)
		return -1;
	printf("receive_message successfully\n");
	printf("%d\n", Isempty((session->auction_data).room->product_list));
	if(Isempty((session->auction_data).room->product_list) == 0)
		printItem((session->auction_data).room->product_list->Front->item);

	//get price
	if(msg.code==REQUEST_SET_PRICE){
		int price=((SET_PRICE_PARAM*)(msg.data))->price;
		Room *room=(session->auction_data).room;
		Item *item=room->product_list->Front->item;
		int current_price=item->price;
		int price_immediately=item->price_immediately;
		

		if(price <current_price+5){
			printf("invalide price, price %d, current_price %d\n",price,current_price);
			set_price_respond.message=(char *)malloc(sizeof(char)*100);
			strcpy(set_price_respond.message,"set price fail");
			if(send_RESPOND_SET_PRICE(sockfd,set_price_respond)!=0)
				return -1;
		}
		else{
			item->price=price;
			item->start=time(NULL);
			item->best_user=(session->login_data).user;
			item->count = 0;

			set_price_respond.message=(char *)malloc(sizeof(char)*100);
			strcpy(set_price_respond.message,"set price successfully");
			if(send_RESPOND_SET_PRICE(sockfd,set_price_respond)!=0)
				return -1;
			// notify to all users
			NOTIFY_NEW_PRICE_RESPOND notify_new_price_respond;
			notify_new_price_respond.newprice=item->price;
			notify_new_price_respond.winner_name=(char *)malloc(sizeof(char)*100);
			strcpy(notify_new_price_respond.winner_name,(session->login_data).user->name);
			notify_new_price_respond.start=item->start;
			notify_new_price_respond.count=0;

			for(int i=0;i<FD_SETSIZE;i++){
				if(all_sesssion[i].protocol_group_id==auction_protocol&&(all_sesssion[i].auction_data).room->id==(session->auction_data).room->id){
					if(client[i]!=sockfd){
						if(send_NOTIFY_NEW_PRICE(client[i], notify_new_price_respond))
							return -1;
						printf("send notify to client %d\n",i);
					}


				}
				// else if((all_sesssion[i].web_protocol).room->id==(session->auction_data).room->id && all_sesssion[i]->protocol_state==web_protocol){
				// 	send_NOTIFY_NEW_PRICE(client[i],NOTIFY_NEW_PRICE_RESPOND data);
				// }
			}
			printf("successfully set price\n");

		}

	}
	else if(msg.code==REQUEST_EXIT_ROOM){
			printf("exit message");
			(session->auction_data).room->number_of_member--;
		// if(session[i].auction_data.room->id == top->id){
			send_RESPOND_EXIT_ROOM(sockfd, exit_room_respond);
			session->protocol_group_id = web_protocol;
		// }
	}
	else{
		if(send_AUCTION_UNKNOWN(sockfd)!=0){
			return -1;
		}
	}
	return 0;

}


void clearSession(SESSION *session){
	session->protocol_group_id=login_protocol;
	session->login_state=no_connect;
	Login_Data login_data;
	session->login_data=login_data;
	Web_Data web_data;
	session->web_data=web_data;
	Auction_Data auction_data;
	session->auction_data=auction_data;
	session->web_state=web_not_authorzied;
}
int main(int argc,char *argv[])
{


	//
	validate_input(argc,argv);
	load_data_base();

 	int PORT=atoi(argv[1]);
	int listenfd, connfd; 
	char recv_data[BUFF_SIZE];
	int bytes_sent, bytes_received;
	
	struct sockaddr_in cliaddr;
	int sin_size;
	char output[BUFF_SIZE],buff[BUFF_SIZE];
	
	//protocol
	int code;
	time_t start_time;
	// multiplexing info
	int client[FD_SETSIZE];
	web_message webmsg[FD_SETSIZE];
	fd_set checkfds_read,checkfds_write,checkfds_exception, readfds, writefds, exceptfds;
	SESSION session[FD_SETSIZE];
	enum Login_ProtocolState loginProtocolState[BACKLOG];
	int maxfd,maxi,i,nready,sockfd;
	int count;
	Item *item;
	Node *user_account[FD_SETSIZE];

	Room *top;
    int interval =10;
    int socket_of_best_user;
    char success_one_message[100] = "Congratulation! You win phase 1";
    char success_two_message[100] = "Congratulation! You win phase 2";
    char success_three_message[100] = "Congratulation! You win";
    EXIT_ROOM_RESPOND exit_room_respond;
    char exit_room[100] = "Exit room successfully";
    exit_room_respond.message = exit_room;

	listenfd=init_new_listen_socket(PORT);
	Room **header = create_room_list();
	Room **my_room[FD_SETSIZE];
	for(count = 0; count < FD_SETSIZE; count++)
		my_room[count] = create_room_list();

	Queue *Q;
	Q = Init(Q);
	Room *room;
	int room_id = 0;
	map_socket_to_room_type map_socket_to_room[FD_SETSIZE];
	map_socket_to_room_type map_socket_to_seeing_room[FD_SETSIZE];
	
	//Listen request from client
	if(listen(listenfd, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}

	maxfd = listenfd;			/* initialize */
	maxi = -1;			/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++){
		client[i] = -1;
		session[i].protocol_group_id=login_protocol;
		session[i].login_state=no_connect;
	}
	FD_ZERO(&checkfds_read);FD_ZERO(&checkfds_write);FD_ZERO(&checkfds_exception);FD_SET(listenfd, &checkfds_read);

	printf("Server is ready\n");
	//Communicate with client
	int value=0;
	struct timeval tv={0,100};

	char clear_error_buff[20];
	while(1){
		readfds = checkfds_read;
		writefds = checkfds_write;
		exceptfds = checkfds_exception;
		nready = select(1025,&readfds,NULL,&exceptfds,&tv);
		
		for (i = 0; i <= maxi; i++) {
			if(nready<0&& FD_ISSET(client[i],&readfds)){
				int error = 0;
				socklen_t len = sizeof (error);
				int retval = getsockopt (client[i], SOL_SOCKET, SO_ERROR, &error, &len);
				if (retval != 0) {
				    printf("client %d\n",client[i]);
					printf("start %d\n",nready );
					FD_CLR(client[i],&checkfds_read);
					FD_CLR(client[i],&checkfds_write);
					FD_CLR(client[i],&checkfds_exception);
					close((int)client[i]);
					client[i]=-1;
					clearSession(&session[i]);
				}

			}
		}
		/* new client connection */
		if (FD_ISSET(listenfd, &readfds) &&nready>0) {
			sin_size = sizeof(struct sockaddr_in);
			if((connfd = accept(listenfd, (struct sockaddr *) &cliaddr,&sin_size)) < 0)
				perror("\nError: ");
			else{
				printf("You got a connection from %s\n", inet_ntoa(cliaddr.sin_addr)); /* prints client's IP */
				for (i = 0; i < FD_SETSIZE; i++)
					if (client[i] < 0) {
						client[i] = connfd;	/* save descriptor */
						FD_SET(connfd, &checkfds_read);
						FD_SET(connfd, &checkfds_exception);
						session[i].protocol_group_id=login_protocol;
						session[i].login_state=no_connect;
						printf("new connect %d\n", connfd);
						break;
					}
				if (i == FD_SETSIZE){
					printf("\nToo many clients");
					close(connfd);
				}
				else{
						/* add new descriptor to set */
					if (connfd > maxfd)
						maxfd = connfd;		/* for select */
					if (i > maxi)
						maxi = i;		/* max index in client[] array */

					if (nready-=1,nready <= 0)
						continue;		/* no more readable descriptors */
					}
				
			}
		}
		if (nready>0){
			for (i = 0; i <= maxi; i++) {	/* check all clients for data */
				if ( (sockfd = client[i]) < 0)
					continue;

				if(session[i].protocol_group_id==login_protocol && FD_ISSET(sockfd,&readfds))
				{
					printf("xu ly login_protocol cho client %d\n",i);
					if(login_protocol_handle(&session[i],client[i],&readfds,&writefds,&exceptfds,&checkfds_read,&checkfds_write)!=0){
						FD_CLR(client[i],&checkfds_read);
						FD_CLR(client[i],&checkfds_write);
						FD_CLR(client[i],&checkfds_exception);
						close((int)client[i]);
						client[i]=-1;
						clearSession(&session[i]);
					}

					printf("ket thuc login_protocol%d\n-------\n", session[i].protocol_group_id);
					FD_SET(client[i], &checkfds_read);

				}
				else if (session[i].protocol_group_id==web_protocol && FD_ISSET(sockfd,&readfds))
				{
					printf("xu ly web_protocol\n");
					if(web_protocol_handle(&session[i],session,client,client[i],&readfds,&writefds,&exceptfds,&checkfds_read,&checkfds_write,map_socket_to_seeing_room[i],header,my_room[i])!=0){
						FD_CLR(client[i],&checkfds_read);
						FD_CLR(client[i],&checkfds_write);
						FD_CLR(client[i],&checkfds_exception);
						close((int)client[i]);
						client[i]=-1;
						clearSession(&session[i]);
					}
					printf("ket thuc web_protocol %d\n-------\n",session[i].protocol_group_id);

				}
				else if (session[i].protocol_group_id==auction_protocol && FD_ISSET(sockfd,&readfds))
				{
					printf("xu ly auction_protocol\n");
					if(auction_protocol_handle(&session[i],session,client,client[i],&readfds,&writefds,&exceptfds,&checkfds_read,&checkfds_write)!=0){
						FD_CLR(client[i],&checkfds_read);
						FD_CLR(client[i],&checkfds_write);
						FD_CLR(client[i],&checkfds_exception);
						close((int)client[i]);
						client[i]=-1;
						clearSession(&session[i]);
					}
					printf("ket thuc auction_protocol %d\n-------\n",session[i].protocol_group_id);

				}
				// if (--nready < 0)
				// 	break;		 //no more readable descriptors  // try fix bug without commenting the code
			}
		}


		/* HANDLE WIN PHASE EVENT*/
        top=(*header);
		while(top!=NULL){
		if(top->number_of_member != 0){
			if(top->product_list->Front != NULL){
				item=top->product_list->Front->item;
				start_time=item->start;
				if(time(NULL)-start_time>interval&&time(NULL)-start_time<interval*2 && item->count == 0){
					NOTIFY_SUCCESS_ONE_RESPOND notify_success_one;
					NOTIFY_PHASE_ONE_RESPOND notify_phase_one;
					notify_success_one.message = success_one_message;
					item->count=1;
					notify_phase_one.newprice = item->price;
					notify_phase_one.winner_name = item->best_user->name;
					for(i=0;i<FD_SETSIZE;i++){
						if(session[i].auction_data.user != NULL)
							if(strcmp((session[i].auction_data.user)->name,item->best_user->name) == 0)
								break;
					}
					socket_of_best_user = client[i];

					send_NOTIFY_SUCCESS_ONE(socket_of_best_user, notify_success_one);
					for(int i=0;i<FD_SETSIZE;i++){
						if(session[i].auction_data.room != NULL)
							if(client[i] != -1 && client[i] != socket_of_best_user && (session[i].auction_data).room->id==top->id && session[i].protocol_group_id==auction_protocol){
								send_NOTIFY_PHASE_ONE(client[i], notify_phase_one);
							}
					}
				}
				else if(time(NULL)-start_time>=interval*2&&time(NULL)-start_time<interval*3 && item->count == 1){
					item->count=2;
					NOTIFY_SUCCESS_TWO_RESPOND notify_success_two;
					NOTIFY_PHASE_TWO_RESPOND notify_phase_two;
					notify_success_two.message = success_two_message;
					notify_phase_two.newprice = item->price;
					notify_phase_two.winner_name = item->best_user->name;

					for(i=0;i<FD_SETSIZE;i++){
						if(session[i].auction_data.user != NULL)
							if(strcmp((session[i].auction_data.user)->name,item->best_user->name) == 0)
								break;
					}
					socket_of_best_user = client[i];

					send_NOTIFY_SUCCESS_TWO(socket_of_best_user, notify_success_two);
					for(int i=0;i<FD_SETSIZE;i++){
						if(session[i].auction_data.room != NULL)
							if(client[i] != -1 && client[i] != socket_of_best_user && (session[i].auction_data).room->id==top->id && session[i].protocol_group_id==auction_protocol){
								send_NOTIFY_PHASE_TWO(client[i],notify_phase_two);
							}
					}
				}
				else if(time(NULL)-start_time>=interval*3 && item->count == 2){
					Pop(top->product_list);
					item->count=3;
					NOTIFY_SUCCESS_THREE_RESPOND notify_success_three;
					NOTIFY_PHASE_THREE_RESPOND notify_phase_three;
					notify_success_three.message = success_three_message;
					notify_phase_three.newprice = item->price;
					notify_phase_three.winner_name = item->best_user->name;

					for(i=0;i<FD_SETSIZE;i++){
						if(session[i].auction_data.user != NULL)
							if(strcmp((session[i].auction_data.user)->name,item->best_user->name) == 0)
								break;
					}
					socket_of_best_user = client[i];

					send_NOTIFY_SUCCESS_THREE(socket_of_best_user, notify_success_three);
					for(int i=0;i<FD_SETSIZE;i++){
						if(session[i].auction_data.room != NULL)
							if(client[i] != -1 && client[i] != socket_of_best_user && (session[i].auction_data).room->id==top->id && session[i].protocol_group_id==auction_protocol){
								send_NOTIFY_PHASE_THREE(client[i], notify_phase_three);
							}
					}
				}


				top=top->next;
			}
			else{
				for(i=0; i<FD_SETSIZE; i++){
					if(session[i].auction_data.room != NULL)
						if(client[i] != -1 && session[i].auction_data.room->id == top->id
							&&session[i].protocol_group_id==auction_protocol){
							printf("send force quit\n");
							send_RESPOND_EXIT_ROOM(client[i], exit_room_respond);
							session[i].protocol_group_id = web_protocol;
						}
				}
				Room *tmp = top;
				top=top->next;
				delete_room(header, tmp->id);
			}
		}
		else{
			top=top->next;
		}
		}


	}

	close(listenfd);

	return 0;
}
