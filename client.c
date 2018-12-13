#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "linklist.h"
#include "login_protocol.h"
#include "tcp.h"

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


		
	// close(client_sock);

	// send user id
	send_USERID(client_sock,"huy");

	//receive message

	receive_message(client_sock,&msg);
	// //send password
	send_PASSWORD(client_sock,"1");
	//receive message
	receive_message(client_sock,&msg);
	printf("%s",(char *)msg.data);

	
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
	



	}
	
		
	return 0;
}
