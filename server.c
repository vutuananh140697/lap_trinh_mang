#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h> 
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include "linklist.h"
#include "tcp.h"

#include "login_protocol.h"
#include "auction_protocol.h"
#include "web_protocol.h"

#include "user_database.h"
#include "roomlist.h"
#define BACKLOG 1024   /* Number of allowed connections */
#define MAX_WRONGPASSWORD 3
#define BUFF_SIZE 1024
Node **database_link_list;
typedef struct map_socket_to_room_type{
	Room *room;
}map_socket_to_room_type;
map_socket_to_room_type map_socket_to_room;

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

// int login_handle(int socket,int sessionid){
// 	//receive data from client
// 	login_message msg;
// 	Node *user_account;
// 	int code;
// 	int connect_state;
// 	while(1){
// 		switch(loginProtocolState[sessionid]){
// 			case no_connect:
// 				connect_state=receive_message(socket,&msg);
// 				if(connect_state==-1){
// 					close(socket);
// 					printf("connect is die\n");
// 					return 1;
// 				}
// 				switch(msg.code){
// 					case USERID:
// 						code=search_userid(&user_account,(char*)msg.data);
// 						if (code==-1){
// 							if(send_USERID_NOTFOUND(socket)==-1){
// 								close((int)socket);printf("connect is die\n");return -1;}
// 						}
// 						else if(code==-2){
// 							if(send_USERID_BLOCK(socket)==-1){
// 								close(socket);printf("connect is die\n");return -1;};
// 						}
// 						else if(code ==-3){
// 							if(send_USERID_ISSIGNIN(socket)==-1){
// 								close(socket);printf("connect is die\n");return -1;};
// 						}
// 						else{
// 							if(send_USERID_FOUND(socket)==-1){
// 								close(socket);printf("connect is die\n");return -1;};
// 							loginProtocolState[sessionid]=correct_id;
// 						}
// 						break;
// 					default:
// 						if(send_UNKNOWN(socket,msg))
// 							{close(socket);printf("connect is die\n");return -1;};
// 						break;
// 				}
// 				break;
// 			case correct_id:
// 				if(receive_message(socket,&msg)==-1)
// 					{close(socket);printf("connect is die\n");return -1;}
// 				if(msg.code==PASSWORD){
// 					if(strcmp(user_account->password,(char*)msg.data)==0){
// 						if(send_PASSWORD_RIGHT(socket)==-1){
// 						}
// 						loginProtocolState[sessionid]=authorized;
// 						user_account->soluongdangnhapsai=0;
// 					}
// 					else{
// 						if(strcmp("q",(char*)msg.data)!=0){
// 							printf("%d\n",user_account->soluongdangnhapsai );
// 							if(user_account->soluongdangnhapsai>=MAX_WRONGPASSWORD){
// 								user_account->status=0;
// 								export_to_data_file();
// 								if(send_PASSWORD_BLOCK(socket)==-1)
// 									{close(socket);printf("connect is die\n");return -1;}
// 								loginProtocolState[sessionid]=no_connect;
// 							}
// 							else{
// 								user_account->soluongdangnhapsai+=1;
// 								// 
// 								if(send_PASSWORD_WRONG(socket)==-1)
// 									{close(socket);printf("connect is die\n");return -1;}

// 							}
// 						}
// 						else{
// 							loginProtocolState[sessionid]=no_connect;
// 						}
// 					}
// 				}
// 				else{
// 					if(send_UNKNOWN(socket,msg)==-1)
// 						{close(socket);printf("connect is die\n");return -1;}
// 				}
// 				break;
// 			case authorized:
// 				if(receive_message(socket,&msg)==-1)
// 					{close(socket);printf("connect is die\n");return -1;}
// 				switch(msg.code){
// 					case LOGOUT:
// 						if(send_LOGOUT_SUCCESS(socket)==-1)
// 							{close(socket);printf("connect is die\n");return -1;}
// 						loginProtocolState[sessionid]=no_connect;
// 					break;
// 					default:
// 						if(send_UNKNOWN(socket,msg)==01)
// 							{close(socket);printf("connect is die\n");return -1;}
// 					break;
// 				}
// 				break;
// 		}
// 		printf(" state %d\n", loginProtocolState[sessionid]);
// 	}
// 	//
// }

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
	switch(session->login_state){
					case no_connect:
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

						if(receive_message(sockfd,&msg)==-1)
						{close(sockfd);printf("connect is die\n");return -1;}
						if(msg.code==PASSWORD){
							if(strcmp((session->login_data).user->password,(char*)msg.data)==0){
								if(send_PASSWORD_RIGHT(sockfd)==-1){
								}
								session->login_state=authorized;
								printf("dang nhap thanh cong\n");
								FD_CLR(sockfd,checkfds_write);
								FD_SET(sockfd,checkfds_read);
								(session->login_data).user->soluongdangnhapsai=0;
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
int web_protocol_handle(SESSION * session,int sockfd,fd_set *readfds,fd_set *writefds,fd_set *exceptfds,fd_set *checkfds_read,fd_set *checkfds_write){
	// web_message msg;
	// switch(session->web_state){
	// 	case web_not_authorzied:
	// 		break;
	// 	case wbe_authorized:
	// 		receive_web_message(msg);
	// 		switch(msg.code){
	// 			case REQUEST_ROOM_LIST:
	// 				//get parame
	// 				//get room list
	// 				ROOM_LIST_RESPOND data;
	// 				send_ROOM_LIST(sockfd, data);
	// 			break;
	// 			case REQUEST_BUY_NOW:
	// 				ROOM_LIST_RESPOND data;
	// 				send_RESPOND_BUY_NOW(int socket,ROOM_LIST_RESPOND data);
	// 			break;
	// 			case REQUEST_MAKE_ROOM:
	// 			break;
	// 			case REQUEST_ROOM_DETAIL:
	// 			break;
	// 			case REQUEST_MY_ROOM_LIST:
	// 			break;
	// 		}
	// 		break;
	// }

}
int auction_protocol_handle(SESSION * session,int sockfd,fd_set *readfds,fd_set *writefds,fd_set *exceptfds,fd_set *checkfds_read,fd_set *checkfds_write){
	
}


int main(int argc,char *argv[])
{
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
	// multiplexing info
	int client[FD_SETSIZE];
	// message msg[FD_SETSIZE];
	fd_set checkfds_read,checkfds_write,checkfds_exception, readfds, writefds, exceptfds;
	// enum ProtocolState protocol_state[FD_SETSIZE][2];
	SESSION session[FD_SETSIZE];

	int maxfd,maxi,i,nready,sockfd;

	Node *user_account[FD_SETSIZE];

	listenfd=init_new_listen_socket(PORT);

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
					if (nready-=1,nready <= 0)
						continue;		/* no more readable descriptors */
					}
				
			}
		}

		for (i = 0; i <= maxi; i++) {	/* check all clients for data */
		if ( (sockfd = client[i]) < 0)
			continue;
		switch(session[i].protocol_group_id){
			case login_protocol:{
				login_protocol_handle(&session[i],client[i],&readfds,&writefds,&exceptfds,&checkfds_read,&checkfds_write);
				break;
			}
			case web_protocol:{
				web_protocol_handle(&session[i],client[i],&readfds,&writefds,&exceptfds,&checkfds_read,&checkfds_write);
				break;
			}
			case auction_protocol:{
				// auction_protocol_handle(protocol_state[i],client[i],&readfds,&writefds,&exceptfds);
				break;
			}

				

		}
		

		if (--nready <= 0)
			break;		/* no more readable descriptors */
		}
		



		//accept request
		// loginProtocolState = no_connect;
		// sin_size = sizeof(struct sockaddr_in);
		// if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, &sin_size)) == -1) 
		// 	perror("\nError: ");
		// printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); /* prints client's IP */
		// // check if full connect or not
		// int a=find_empty_session();
		// //start conversation
		// if (fork() == 0) 
  //       	login_handle(conn_sock,a);
	}
	
	close(listenfd);

	// accept new socket

	// receive id message
	//check
	//send repsond message
	//change protocol
	//set new protocol state ==> web protocol
		//reviece and send message
		// if receive enter room ==> find room by id
		 //change protocol ==> change room's user attribute  and add matching from user to room
		//map_socket_to_room[socket] = room
	
	
	//receive set price 
		// tim tu socket --> room tuong ung
		// check and set new price, send to all user,

	return 0;
}
