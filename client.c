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
	message msg;
	
	
	
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
						printf("MENU:\n1.developing function\n2.logout:");
						scanf("%d%*c",&choosemenu);
						switch(choosemenu){
							case 1:
								printf("some funny function will be here soon\n");
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
	// }
	
		
	return 0;
}
