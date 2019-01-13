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
							if (FD_ISSET(sockfd, readfds)) {
								receive_message(sockfd,&msg);
								switch(msg.code){
									case USERID:
									printf("%s\n",(char*)msg.data );
										code=search_userid(&(session->login_data).user,(char*)msg.data);
										if (code==-1){
											if(send_USERID_NOTFOUND(sockfd)==-1){
												close((int)sockfd);printf("connect is die\n");return -1;}
										}
										else if(code==-2){
											if(send_USERID_BLOCK(sockfd)==-1){
												close(sockfd);printf("connect is die\n");return -1;};
										}
										else if(code ==-3){
											if(send_USERID_ISSIGNIN(sockfd)==-1){
												close(sockfd);printf("connect is die\n");return -1;};
										}
										else{
											if(send_USERID_FOUND(sockfd)==-1){
												close(sockfd);printf("connect is die\n");return -1;};
												session->login_state=correct_id;
												FD_SET(sockfd,checkfds_write);
												FD_CLR(sockfd,checkfds_read);
										}
										}
										break;
									default:
										if(send_UNKNOWN(sockfd,msg))
											{close(sockfd	);printf("connect is die\n");return -1;};
										break;
							}
						break;
					case correct_id:
						printf("\tpassword handle\n");
						if(receive_message(sockfd,&msg)==-1)
						{close(sockfd);printf("connect is die\n");return -1;}
						if(msg.code==PASSWORD){
							if(strcmp((session->login_data).user->password,(char*)msg.data)==0){
								if(send_PASSWORD_RIGHT(sockfd)==-1){
								}
								session->login_state=authorized;
								FD_CLR(sockfd,checkfds_write);
								FD_SET(sockfd,checkfds_read);
								(session->login_data).user->soluongdangnhapsai=0;
								session->protocol_group_id=web_protocol;
								session->web_state=web_authorized;
							}
							else{
								if(strcmp("q",(char*)msg.data)!=0){
									printf("%d\n",(session->login_data).user->soluongdangnhapsai);
									if((session->login_data).user->soluongdangnhapsai>=MAX_WRONGPASSWORD){
										(session->login_data).user->status=0;
										export_to_data_file();
										if(send_PASSWORD_BLOCK(sockfd)==-1)
											{close(sockfd);printf("connect is die\n");return -1;}
										session->login_state=no_connect;
									}
									else{
										(session->login_data).user->soluongdangnhapsai+=1;
										// 
										if(send_PASSWORD_WRONG(sockfd)==-1)
											{close(sockfd);printf("connect is die\n");return -1;}

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
								{close(sockfd);printf("connect is die\n");return -1;}
						}

						 break;
					case authorized:
						if(receive_message(sockfd,&msg)==-1)
							{close(sockfd);printf("connect is die\n");return -1;}
						switch(msg.code){
							case LOGOUT:
								if(send_LOGOUT_SUCCESS(sockfd)==-1)
									{close(sockfd);printf("connect is die\n");return -1;}
								session->login_state=no_connect;
							break;
							default:
								if(send_UNKNOWN(sockfd,msg)==01)
									{close(sockfd);printf("connect is die\n");return -1;}
							break;
						}
						break;
					}
}
int web_protocol_handle(SESSION * session,int sockfd,
	fd_set *readfds,fd_set *writefds,fd_set *exceptfds,fd_set *checkfds_read,fd_set *checkfds_write,
	map_socket_to_room_type map_socket_to_seeing_room,
	Room **header,Room **my_room){

	web_message msg;
	// BUY_NOW_RESPOND data;
	// ROOM_LIST_RESPOND room_list_respond;
	// printf("xu ly web\n");
	// // return 0;
	switch(session->web_state){
		case web_not_authorzied:
			break;
		case web_authorized:
			// printf("authorized\n");
			if(receive_web_message(sockfd, &msg) == 0){
				// printf("receive_message successfully\n");
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
						room_list_respond.header = *header;
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
							// map_socket_to_seeing_room.room = searchRoom;
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
						// printf("1 ok \n");
						//delete product
						if(search_item == NULL){
							strcpy(message_buy_now_respond, "Cannot found product");
						}else{
							// printf("2.1 ok \n");
							deleteItem(((session->web_data).room)->product_list, buy_now_param->id);
							// printf("2.2 ok \n");
							print_all_room(header);
							// printf("2.3 ok \n");
							strcpy(message_buy_now_respond, "Buy successfully");
						}
						// printf("4 ok \n");
						buy_now_respond.message = message_buy_now_respond;
						if(send_RESPOND_BUY_NOW(sockfd, buy_now_respond) != 0)
							{close(sockfd);printf("connect is die\n");return -1;}
						// printf("5 ok \n");
						//todo: send all notification to all users who is in auction
						break;
					case REQUEST_MAKE_ROOM:
						// printf("\tREQUEST_MAKE_ROOM handle");
						make_room_param = (MAKE_ROOM_PARAM*)msg.data;
						// printf("1 ok \n");
						Room *new_room = add_new_room(header, count_room(*header)+1, (session->login_data).user->name, make_room_param->product_list, 0);
						// printf("2 ok \n");
						add_room(my_room, new_room);
						//print_all_room(header);
						// printf("3 ok \n");
						// todo delete my room and add function to filter my room instead of using my_room array
						char mess[1000] = "Create room successfull";
						MAKE_ROOM_RESPOND make_room_param;
						make_room_param.message = mess;
						if(send_RESPOND_MAKE_ROOM(sockfd, make_room_param) != 0)
							{close(sockfd);printf("connect is die\n");return -1;}
						break;
					case REQUEST_MY_ROOM_LIST:
						// printf("\tREQUEST_MY_ROOM_LIST handle");
						my_room_list_param = (MY_ROOM_LIST_PARAM*)msg.data;
						MY_ROOM_LIST_RESPOND my_room_list_respond;
						my_room_list_respond.header = *my_room;
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
							session->protocol_group_id = auction_protocol;
							(session->auction_data).room = enterroom;
						}
						enter_room_respond.room = enterroom;
						if(send_RESPOND_ENTER_ROOM(sockfd, enter_room_respond) != 0)
							{close(sockfd);printf("connect is die\n");return -1;}
						break;
					break;
					case REQUEST_LOG_OUT:
						session->protocol_group_id = login_protocol;
						session->login_state = no_connect;
						session->web_state = web_not_authorzied;
						session->auction_state = no_set_price_or_passed;
						if(send_LOG_OUT_RESPOND(sockfd) != 0)
							{close(sockfd);printf("connect is die\n");return -1;}
						break;
					}
				}
		break;
	}

}
int auction_protocol_handle(SESSION * session,SESSION * all_sesssion,int *client,int sockfd,fd_set *readfds,fd_set *writefds,fd_set *exceptfds,fd_set *checkfds_read,fd_set *checkfds_write){

	auction_message msg;
	SET_PRICE_RESPOND set_price_respond;
	receive_auction_message(sockfd, &msg);

	//get price
	if(msg.code==REQUEST_SET_PRICE){
		int price=((SET_PRICE_PARAM*)msg.data)->price;
		Room *room=(session->auction_data).room;
		Item *item=room->product_list->Front->item;
		int current_price=item->price;
		int price_immediately=item->price_immediately;

		if(price <current_price+5){
			set_price_respond.message=(char *)malloc(sizeof(char)*100);
			strcpy(set_price_respond.message,"set price fail");
			send_RESPOND_SET_PRICE(sockfd,set_price_respond);
		}
		else{
			item->price=price;
			item->start=time(NULL);
			item->best_user=(session->login_data).user;

			set_price_respond.message=(char *)malloc(sizeof(char)*100);
			strcpy(set_price_respond.message,"set price successfully");
			send_RESPOND_SET_PRICE(sockfd,set_price_respond);
			// notify to all users
			NOTIFY_NEW_PRICE_RESPOND notify_new_price_respond;
			notify_new_price_respond.newprice=price;
			notify_new_price_respond.winner_name=(char *)malloc(sizeof(char)*100);
			strcpy(notify_new_price_respond.winner_name,(session->login_data).user->name);
			notify_new_price_respond.start=item->start;
			notify_new_price_respond.count=0;

			for(int i=0;i<FD_SETSIZE;i++){
				if((all_sesssion[i].auction_data).room->id==(session->auction_data).room->id && all_sesssion[i].protocol_group_id==auction_protocol){
					send_NOTIFY_NEW_PRICE(client[i], notify_new_price_respond);
				}
				// else if((all_sesssion[i].web_protocol).room->id==(session->auction_data).room->id && all_sesssion[i]->protocol_state==web_protocol){
				// 	send_NOTIFY_NEW_PRICE(client[i],NOTIFY_NEW_PRICE_RESPOND data);
				// }
			}

		}

	}
	else{
		send_AUCTION_UNKNOWN(sockfd);
	}

	//

	// 	case final_

}


int main(int argc,char *argv[])
{


	//
	validate_input(argc,argv);
	load_data_base();

	print_all_node(database_link_list);
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
	// message msg[FD_SETSIZE];
	web_message webmsg[FD_SETSIZE];
	fd_set checkfds_read,checkfds_write,checkfds_exception, readfds, writefds, exceptfds;
	// enum ProtocolState protocol_state[FD_SETSIZE][2];
	SESSION session[FD_SETSIZE];
	enum Login_ProtocolState loginProtocolState[BACKLOG];
	int maxfd,maxi,i,nready,sockfd;
	int count;
	Item *item;
	Node *user_account[FD_SETSIZE];

	listenfd=init_new_listen_socket(PORT);
	Room **header = create_room_list();
	Room **my_room[FD_SETSIZE];
	for(count = 0; count < FD_SETSIZE; count++)
		my_room[count] = create_room_list();

	// Item *item = (Item*)malloc(sizeof(Item));
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
	printf("%d\n", listenfd	 );
	int value=0;
	
	while(1){
		readfds = checkfds_read;
		writefds = checkfds_write;
		exceptfds = checkfds_exception;
		nready = select(1025,&readfds,&writefds,&exceptfds,NULL);
		/* new client connection */
		if (FD_ISSET(listenfd, &readfds)) {	
			sin_size = sizeof(struct sockaddr_in);
			if((connfd = accept(listenfd, (struct sockaddr *) &cliaddr,&sin_size)) < 0)
				perror("\nError: ");
			else{
				printf("You got a connection from %s\n", inet_ntoa(cliaddr.sin_addr)); /* prints client's IP */
				for (i = 0; i < FD_SETSIZE; i++)
					if (client[i] < 0) {
						client[i] = connfd;	/* save descriptor */
						FD_SET(connfd, &checkfds_read);
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
					printf("start %d\n",nready );
					if (nready-=1,nready <= 0)
						continue;		/* no more readable descriptors */
					}
				
			}
		}

		for (i = 0; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i]) < 0)
				continue;

			if(session[i].protocol_group_id==login_protocol && FD_ISSET(sockfd,&readfds))
			{
				printf("xu ly login_protocol\n");
				login_protocol_handle(&session[i],client[i],&readfds,&writefds,&exceptfds,&checkfds_read,&checkfds_write);
				printf("ket thuc login_protocol%d\n-------\n", session[i].protocol_group_id);
				FD_SET(client[i], &checkfds_read);
				
			}
			else if (session[i].protocol_group_id==web_protocol && FD_ISSET(sockfd,&readfds))
			{
				printf("xu ly web_protocol\n");
				web_protocol_handle(&session[i],client[i],&readfds,&writefds,&exceptfds,&checkfds_read,&checkfds_write,map_socket_to_seeing_room[i],header,my_room[i]);
				printf("ket thuc web_protocol %d\n-------\n",session[i].protocol_group_id);
				
			}
			if (session[i].protocol_group_id==auction_protocol && FD_ISSET(sockfd,&readfds))
			{
				printf("xu ly auction_protocol\n");
				auction_protocol_handle(&session[i],session,client,client[i],&readfds,&writefds,&exceptfds,&checkfds_read,&checkfds_write);
				
			}
			// if (--nready < 0)
			// 	break;		 //no more readable descriptors  // try fix bug without commenting the code  
		}


		// Room *top=(*head);

		// while(top!=NULL){
		// 	item=top->product_list->Front->item;
		// 	start_time=item.start;
		// 	if(time(NULL)-start_time>3&&time(NULL)-start_time<6){
		// 		item->count=1;
		// 		for(int i=0;i<FD_SETSIZE;i++){
		// 			if((session[i].auction_data).room->id==top->id && all_sesssion[i]->protocol_state==auction_protocol){
		// 				send_NOTIFY_NEW_PRICE(client[i],NOTIFY_NEW_PRICE_RESPOND data);
		// 			}
		// 		}	
		// 	}
		// 	if(time(NULL)-start_time>=6&&time(NULL)-start_time<9){
		// 		item->count=2;
		// 		for(int i=0;i<FD_SETSIZE;i++){
		// 			if((session[i].auction_data).room->id==top->id && all_sesssion[i]->protocol_state==auction_protocol){
		// 				send_NOTIFY_NEW_PRICE(client[i],NOTIFY_NEW_PRICE_RESPOND data);
		// 			}
		// 		}	
		// 	}
		// 	if(time(NULL)-start_time>=9){
		// 		item->count=3;
		// 		send_NOTIFY_NEW_PRICE(int socket,NOTIFY_NEW_PRICE_RESPOND data);
		// 		for(int i=0;i<FD_SETSIZE;i++){
		// 			if((session[i].auction_data).room->id==top->id && all_sesssion[i]->protocol_state==auction_protocol){
		// 				send_NOTIFY_NEW_PRICE(client[i],NOTIFY_NEW_PRICE_RESPOND data);
		// 			}
		// 		}	
		// 	}
		// 	top=top->next;
		// }




	}

	close(listenfd);

	return 0;
}
