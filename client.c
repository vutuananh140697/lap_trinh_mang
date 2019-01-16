#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "roomlist.h"
#include "linklist.h"
#include "login_protocol.h"
#include "web_protocol.h"
#include "tcp.h"
#include "queue.h"
#include "roomlist.h"
#include "web_protocol.h"
#include "auction_protocol.h"

#define BUFF_SIZE 8192
//validate if string s is contain non-number character or not
//return 1 if no non-number character exist else return 0 
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

//function:check if a string is IP format or not
// input: string to check
//output: 1 if string is IP format else return 0
int is_ip_address(char *str){
  char *ch;	
  // split the string by "."
  ch = strtok(str, ".");
  int number_segment=0;
  while (ch != NULL) {
    //check if string contain only number or not
    if(numbers_only(ch)==0)
      return 0;
    //check if number value is between 0 and 255
    if(atoi (ch)<0 ||atoi (ch)>255)
      {
	return 0;
      }
    ch = strtok(NULL, ".");
    number_segment+=1;
    // dem so luong split ( neu so luong  khac 4 se bao loi)
  }
  if(number_segment!=4)
    return 0;
  return 1;
}
//validate input of main function
void validate_input(int argc,char *argv[]){

	if(argc!=3){
		printf("Usage: ./client 127.0.0.1 5000\n");
		exit(1);
	}
	if(is_ip_address(strdup(argv[1]))==0 ||numbers_only(argv[2])!=1){
		printf("Invalid parameter\n");
		exit(2);
	}

}

void join_auction_handle(int socket){
	printf("welcome to room \n");
	int nready=-1;
	SET_PRICE_PARAM data;
	data.price =2000000;
	send_REQUEST_SET_PRICE(socket, data);
	fd_set checkfds_read,checkfds_write,checkfds_exception, readfds, writefds, exceptfds;
	auction_message msg;
	FD_ZERO(&checkfds_read);FD_ZERO(&checkfds_write);FD_ZERO(&checkfds_exception);
	FD_SET(socket, &checkfds_read);
	while(1){
		readfds = checkfds_read;
		writefds = checkfds_write;
		exceptfds = checkfds_exception;
		nready = select(1025,&readfds,&writefds,&exceptfds,NULL);
		if (FD_ISSET(socket, &readfds)){
			if(receive_auction_message(socket,&msg) != 0){
				close(socket);
				exit(-1);
			}
			NOTIFY_NEW_PRICE_RESPOND *notify_new_price_respond;
			NOTIFY_SUCCESS_ONE_RESPOND *notify_success_one;
			NOTIFY_SUCCESS_TWO_RESPOND *notify_success_two;
			NOTIFY_SUCCESS_THREE_RESPOND *notify_success_three;
			NOTIFY_PHASE_ONE_RESPOND *notify_phase_one;
			NOTIFY_PHASE_TWO_RESPOND *notify_phase_two;
			NOTIFY_PHASE_THREE_RESPOND *notify_phase_three;
			switch(msg.code){
				case NOTIFY_NEW_PRICE:
				notify_new_price_respond = (NOTIFY_NEW_PRICE_RESPOND*)msg.data;
				printf("New Price: %s - %d\n", notify_new_price_respond->winner_name, notify_new_price_respond->newprice);
				break;
				case RESPOND_SET_PRICE:
				printf("Set price successfully\n");
				break;
				case NOTIFY_SUCCESS_ONE:
				notify_success_one = (NOTIFY_SUCCESS_ONE_RESPOND*)msg.data;
				printf("%s\n", notify_success_one->message);
				break;
				case NOTIFY_SUCCESS_TWO:
				notify_success_two = (NOTIFY_SUCCESS_TWO_RESPOND*)msg.data;
				printf("%s\n", notify_success_two->message);
				break;
				case NOTIFY_SUCCESS_THREE:
				notify_success_three = (NOTIFY_SUCCESS_THREE_RESPOND*)msg.data;
				printf("%s\n", notify_success_three->message);
				break;
				case NOTIFY_PHASE_ONE:
				notify_phase_one = (NOTIFY_PHASE_ONE_RESPOND*)msg.data;
				printf("%s win phase 1 with %d\n", notify_phase_one->winner_name, notify_phase_one->newprice);
				break;
				case NOTIFY_PHASE_TWO:
				notify_phase_two = (NOTIFY_PHASE_TWO_RESPOND*)msg.data;
				printf("%s win phase 2 with %d\n", notify_phase_two->winner_name, notify_phase_two->newprice);
				break;
				case NOTIFY_PHASE_THREE:
				notify_phase_three = (NOTIFY_PHASE_THREE_RESPOND*)msg.data;
				printf("%s win phase 3 with %d\n", notify_phase_three->winner_name, notify_phase_three->newprice);
				break;
			}
		}
	}


	printf("goodbye\n");
}

int main(int argc,char *argv[]){
	//validate input
    validate_input(argc,argv);
    //declare some variable
    char SERVER_ADDR[50];
    int SERVER_PORT;
	int client_sock;
	int code;
	char filename[BUFF_SIZE];
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received;
	char userid[200],password[200];
	int choosemenu;
	int choose;
	int enter_room;
	int page_id;

	Item *item = (Item*)malloc(sizeof(Item));
	Queue *Q;
	
	Room *room;

	//get server info
	strcpy(SERVER_ADDR,argv[1]);
	SERVER_PORT=atoi(argv[2]);
	login_message msg;
	
	
	
	// get connect to the server
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}

	while(1){
		
		printf("enter your user id:('q' to quit)");scanf("%[^\n]%*c",userid);

		if(strcmp(userid,"q")==0)
			break;
		//send login request with user id
		if(send_USERID(client_sock,userid)==-1)
			{close(client_sock);printf("connect is die\n");return -1;}
		//receive respond from server
		if(receive_message(client_sock,&msg)==-1){
			printf("connect is die\n");return -1;}
		// if no userid is match to userid have been entered
		if(msg.code==USERID_NOTFOUND){
			printf("wrong user id\n");
		}
		// if no userid is blocked
		else if(msg.code==USERID_BLOCK){
			printf("user id is block\n");
		}
		// if no userid is found
		else if(msg.code==USERID_FOUND){
			while(1){
				//get password
				printf("enter your password:('q' to sign in another account): ");

				scanf("%[^\n]%*c",password);
				if(strcmp(password,"q")==0){
					send_RESET(client_sock);
					break;
				}
				//send password
				if(send_PASSWORD(client_sock,password))
					{close(client_sock);printf("connect is die\n");return -1;}
				// if password is "q" then break;

				//get respond from server
				if(receive_message(client_sock,&msg)==-1){
					printf("connect is die\n");return -1;}
				//if enter a right password
				if(msg.code==PASSWORD_RIGHT){
					//stop enter password
					break;
				}
				//if enter a wrong password
				else if(msg.code==PASSWORD_WRONG){
					printf("wrong password\n");
				}
				//if enter a wrong password made this account to be block
				else if(msg.code==PASSWORD_BLOCK){
					printf("account is block\n");
					break;
				}
			}
			//if passsword is right
			if(msg.code==PASSWORD_RIGHT){
					do{

						//display some menu
						printf("MENU:\n1.menu\n2.logout\nchoose:");
						scanf("%d%*c",&choosemenu);
						switch(choosemenu){
							case 1:

								printf("MENU:\n1.Room List\n2.Create new room\n3.My room\n");
								printf("Nhap lua chon: \n");
								scanf("%d", &choose);
								if(choose == 1){
									ROOM_LIST_PARAM room_list_param;
									room_list_param.page_id = 1;
									if(send_REQUEST_ROOM_LIST(client_sock, room_list_param) != 0)
										{printf("connect is die\n");return -1;}
									web_message message;
									if(receive_web_message(client_sock, &message) != 0)
										{printf("connect is die\n");return -1;}
									else{
										ROOM_LIST_RESPOND *room_list_respond = (ROOM_LIST_RESPOND*)message.data;
										print_all_room(&room_list_respond->header);

										// room detail
										ROOM_DETAIL_PARAM room_detail_param;

										printf("Select Room by ID:");
										scanf("%d%*c",&room_detail_param.room_id);

										if(send_REQUEST_ROOM_DETAIL(client_sock, room_detail_param) != 0)
											{printf("connect is die\n");return -1;}
										if(receive_web_message(client_sock, &message) != 0)
											{printf("connect is die\n");return -1;}
										ROOM_DETAIL_RESPOND *room_detail_respond = (ROOM_DETAIL_RESPOND*)message.data;

										if(room_detail_respond->result == 0) 
											printf("Cannot found room %d\n", room_detail_param.room_id);
										else{
											printRoom(room_detail_respond->room);
											
											// buy now
											printf("1. Enter room\n2. Buy now\n");

											int enter_room;
											scanf("%d%*c", &enter_room);
											if(enter_room == 2){
												BUY_NOW_PARAM buy_now_param;
												printf("Choose product by ID:");
												scanf("%d%*c", &buy_now_param.id);
												if(send_REQUEST_BUY_NOW(client_sock, buy_now_param) != 0)
													{printf("connect is die\n");return -1;}
												if(receive_web_message(client_sock, &message) != 0)
													{printf("connect is die\n");return -1;}
												BUY_NOW_RESPOND *buy_now_respond = (BUY_NOW_RESPOND*)message.data;
												printf("%s\n", buy_now_respond->message);
											}
											else{
												ENTER_ROOM_PARAM enter_room_param;
												enter_room_param.room_id=1;

												if(send_REQUEST_ENTER_ROOM(client_sock, enter_room_param) != 0)
													{printf("connect is die\n");return -1;}
												if(receive_web_message(client_sock, &message) != 0)
													{printf("connect is die\n");return -1;}
												join_auction_handle(client_sock);

											}
										}

									}

								}
								else if(choose == 2){

									Q = Init(Q);
									item->id = 1;
									item->name = "Binh co";
									item->description = "Binh co ngan nam";
									item->price = 1000000;
									item->price_immediately = 10000000;
									Push(Q, item->id, item->name, item->description, item->price, item->price_immediately);
									item->name = "Quan tai";
									Push(Q, item->id+1, item->name, "Quan tai co ngan nam", item->price, 106000000);
									item->name = "Piano";
									Push(Q, item->id+2, item->name, item->description, item->price, 4300000);

									MAKE_ROOM_PARAM make_room_param;
									make_room_param.product_list = Q;

									if(send_REQUEST_MAKE_ROOM(client_sock, make_room_param) != 0)
										{printf("connect is die\n");return -1;}

									web_message message;
									if(receive_web_message(client_sock, &message) != 0)
										{printf("connect is die\n");return -1;}
									MAKE_ROOM_RESPOND *make_room_respond = (MAKE_ROOM_RESPOND*)message.data;
									printf("%s\n", make_room_respond->message);

								}
								else if(choose == 3){
									MY_ROOM_LIST_PARAM my_room_list_param;
									my_room_list_param.page_id = 1;
									if(send_REQUEST_MY_ROOM_LIST(client_sock, my_room_list_param) != 0)
										{printf("connect is die\n");return -1;}
									web_message message;
									if(receive_web_message(client_sock, &message) != 0)
										{printf("connect is die\n");return -1;}
									else{
										MY_ROOM_LIST_RESPOND *my_room_list_respond = (MY_ROOM_LIST_RESPOND*)message.data;
										print_all_room(&my_room_list_respond->header);
									}

								}
								break;
							case 2:
							//send logout request
								if(send_LOGOUT(client_sock)==-1)
									{close(client_sock);printf("connect is die\n");return -1;}
								if(receive_message(client_sock,&msg)==-1){
									printf("connect is die\n");return -1;}
								//if logout success
								if(msg.code==LOGOUT_SUCCESS){
									printf("goodbye!\n");
								}
								else if(msg.code==LOGOUT_UNSUCCESS){
									printf("logout uncess!\n");
								}
								else{
									printf("unknown code from server!\n");
								}
								break;
							default:
								printf("please enter a number in range 1 and 2!\n");
								break;
						}
					}while(choosemenu!=2);
				
		}
		
	}
}
		
	close(client_sock);

	// send user id
	// send_USERID(client_sock,"huy");

	// //receive message

	// receive_message(client_sock,&msg);
	// // //send password
	// send_PASSWORD(client_sock,"1");
	// //receive message
	// receive_message(client_sock,&msg);
	// printf("%s",(char *)msg.data);


	// int choose;
	// scanf("%d%*c",&choose);
	// if(choose==0){
	// 	ROOM_LIST_PARAM param;
	// 	send_REQUEST_ROOM_LIST(client_sock, param);
	// 	ROOM_LIST_PARAM room_list;
	// 	receive_REQUEST_ROOM_LIST(client_sock,&room_list);
	// 	// int next_page;
	// 	// scanf("%d%*c",&next_page);
	// 	// if(next_page==0){
	// 	// 	int send_REQUEST_ROOM_DETAIL(int socket,ROOM_LIST_PARAM param);

	// 	// 	int receive_REQUEST_ROOM_DETAIL(int socket,ROOM_LIST_RESPOND *data);

	// 	// 	int enter_room;
	// 	// 	scanf("%d%*c",&enter_room);
	// 	// 	if(enter_room==0){
	// 	// 		int send_REQUEST_BUY_NOW(int socket,ROOM_LIST_PARAM param);
	// 	// 		int receive_REQUEST_BUY_NOW(int socket,ROOM_LIST_RESPOND *data);
	// 	// 	}
	// 	// 	else if(enter_room==1){
	// 	// 			int send_ENTER_ROOM();
	// 	// 			int receive_message(int socket,message *msg);
	// 	// 			send_SET_PRICE();
	// 	// 			int receive_message(int socket,message *msg);

	// 	// 			if(have any message come){
	// 	// 				int receive_message(int socket,message *msg);
	// 	// 				if(message is new price){

	// 	// 				}
	// 	// 				else{ // thong bao thang cuoc lan 1, lan 2, lan 3
	// 	// 				}
	// 	// 			}
	// 	// 			}
	// 	// 	else if(enter_room==2){
	// 	// 		int send_REQUEST_ROOM_LIST(int socket,ROOM_LIST_PARAM param);
	// 	// 		int receive_REQUEST_ROOM_LIST(int socket,ROOM_LIST_PARAM *param);
	// 	// 	}
	// 	// }
	// 	// else if(next_page==1){
	// 	// 	int send_REQUEST_ROOM_LIST(int socket,ROOM_LIST_PARAM param);
	// 	// 	int receive_REQUEST_ROOM_LIST(int socket,ROOM_LIST_PARAM *param);
	// 	// }
	// }
	// else if(choose==1){

	// 	int send_REQUEST_MAKE_ROOM(int socket,ROOM_LIST_PARAM param);
	// 	int receive_REQUEST_MAKE_ROOM(int socket,ROOM_LIST_RESPOND *data);
	// }
	// else if(choose==2){
	// 	send_REQUEST_MY_ROOM_LIST();
	// 	receive_REQUEST_MY_ROOM_LIST();
	// }



		
	return 0;
}
