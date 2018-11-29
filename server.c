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
#define BACKLOG 1024   /* Number of allowed connections */
#define BUFF_SIZE 1024
const int MAXLENNAME= 200;
const int MAXLENPASSWORD= 200;
const int MAX_WRONGPASSWORD=2;
//============================ GLOBAL VARIABLE =================
Node **database_link_list;
enum ProtocolState loginProtocolState[BACKLOG];
//============================ FUNCTION =======================
//load data from account.txt to database_link_list

void load_data_base(){
  database_link_list=create_link_list();
  FILE *fin=(FILE *)fopen("account.txt","r");
  if(fin==NULL){
    printf("Don't exist file contain database\n");
    exit(1);
    return ;
  }
  char name[MAXLENNAME],password[MAXLENPASSWORD];
  int status=-1;
  while(fscanf(fin,"%s%*c%s%*c%d%*c",name,password,&status),!feof(fin)){
    
    add_new_node(database_link_list,strdup(name),strdup(password),status);
    //printf("%s,%s,%d\n",name,password,status);
  }
  fclose(fin);
}
//export data from database_link_list to account.txt
void export_to_data_file(){
  Node *top=(*database_link_list);
  FILE* fout=fopen("account.txt","w");
  while(top!=NULL){
    fprintf(fout,"%s %s %d\n",top->name,top->password,top->status);
    top=top->next;
  }
  fclose(fout);
}
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
// find user info in database
//user: [out]point to save the user info
//name: [in] user id
//return value: -1 if not found user id,-2 if account is block,-3 if account is already sign-in in at least one process
int search_userid(Node**user,char *name){

	Node *searchresult=search(database_link_list,name);
	if (searchresult==NULL){
		*user=NULL;
		return -1;
	}
	else if (searchresult->status==0){
		*user=NULL;
	  	return -2;
	}
	// check if account is already sign-in yet or not

	if (searchresult!=NULL && searchresult->islogin==1){
		*user=NULL;
		return -3;
	}
	*user=searchresult;
}
// session is a array to check is a client is already sign-in or not
// if this array is full so we can't control anymore client
//return value: -1 if array is full, or the i-th element in array is ready to be use
int find_empty_session(){

	for(int i=0;i<BACKLOG;i++){
		if(loginProtocolState[i]==no_connect){
			return i;
		}

	}
	return -1;
}
//function to do the handle login request
//socket:[in]:socket to communicate
//sessionid: i-th element to store the protocol state of server corresspond to a client
//return value: -1 if any error occur
int login_handle(int socket,int sessionid){
	//receive data from client
	message msg;
	Node *user_account;
	int code;
	int connect_state;
	while(1){
		switch(loginProtocolState[sessionid]){
			case no_connect:
				connect_state=receive_message(socket,&msg);
				if(connect_state==-1){
					close(socket);
					printf("connect is die\n");
					return 1;
				}
				switch(msg.code){
					case USERID:
						code=search_userid(&user_account,(char*)msg.data);
						if (code==-1){
							if(send_USERID_NOTFOUND(socket)==-1){
								close((int)socket);printf("connect is die\n");return -1;}
						}
						else if(code==-2){
							if(send_USERID_BLOCK(socket)==-1){
								close(socket);printf("connect is die\n");return -1;};
						}
						else if(code ==-3){
							if(send_USERID_ISSIGNIN(socket)==-1){
								close(socket);printf("connect is die\n");return -1;};
						}
						else{
							if(send_USERID_FOUND(socket)==-1){
								close(socket);printf("connect is die\n");return -1;};
							loginProtocolState[sessionid]=correct_id;
						}
						break;
					default:
						if(send_UNKNOWN(socket,msg))
							{close(socket);printf("connect is die\n");return -1;};
						break;
				}
				break;
			case correct_id:
				if(receive_message(socket,&msg)==-1)
					{close(socket);printf("connect is die\n");return -1;}
				if(msg.code==PASSWORD){
					if(strcmp(user_account->password,(char*)msg.data)==0){
						if(send_PASSWORD_RIGHT(socket)==-1){
						}
						loginProtocolState[sessionid]=authorized;
						user_account->soluongdangnhapsai=0;
					}
					else{
						if(strcmp("q",(char*)msg.data)!=0){
							printf("%d\n",user_account->soluongdangnhapsai );
							if(user_account->soluongdangnhapsai>=MAX_WRONGPASSWORD){
								user_account->status=0;
								export_to_data_file();
								if(send_PASSWORD_BLOCK(socket)==-1)
									{close(socket);printf("connect is die\n");return -1;}
								loginProtocolState[sessionid]=no_connect;
							}
							else{
								user_account->soluongdangnhapsai+=1;
								// 
								if(send_PASSWORD_WRONG(socket)==-1)
									{close(socket);printf("connect is die\n");return -1;}

							}
						}
						else{
							loginProtocolState[sessionid]=no_connect;
						}
					}
				}
				else{
					if(send_UNKNOWN(socket,msg)==-1)
						{close(socket);printf("connect is die\n");return -1;}
				}
				break;
			case authorized:
				if(receive_message(socket,&msg)==-1)
					{close(socket);printf("connect is die\n");return -1;}
				switch(msg.code){
					case LOGOUT:
						if(send_LOGOUT_SUCCESS(socket)==-1)
							{close(socket);printf("connect is die\n");return -1;}
						loginProtocolState[sessionid]=no_connect;
					break;
					default:
						if(send_UNKNOWN(socket,msg)==01)
							{close(socket);printf("connect is die\n");return -1;}
					break;
				}
				break;
		}
		printf(" state %d\n", loginProtocolState[sessionid]);
	}
	//


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
	struct sockaddr_in server;
	struct sockaddr_in cliaddr;
	int sin_size;
	char output[BUFF_SIZE],buff[BUFF_SIZE];
	
	//protocol
	int code;
	// multiplexing info
	int client[FD_SETSIZE];
	message msg[FD_SETSIZE];
	fd_set checkfds_read,checkfds_write,checkfds_exception, readfds, writefds, exceptfds;
	enum ProtocolState protocol_state[FD_SETSIZE];
	int maxfd,maxi,i,nready,sockfd;

	Node *user_account[FD_SETSIZE];

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
		perror("\nError: ");
		return 0;
	}
	
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(PORT);   
	server.sin_addr.s_addr = htonl(INADDR_ANY);  
	if(bind(listenfd, (struct sockaddr*)&server, sizeof(server))==-1){ 
		perror("\nError: ");
		return 0;
	}     
	
	//Listen request from client
	if(listen(listenfd, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}

	maxfd = listenfd;			/* initialize */
	maxi = -1;			/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++){
		client[i] = -1;
		protocol_state[i]=no_connect;
	}
	FD_ZERO(&checkfds_read);FD_ZERO(&checkfds_write);FD_ZERO(&checkfds_exception);
	FD_SET(listenfd, &checkfds_read);

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
						protocol_state[i]=no_connect;
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
			switch(protocol_state[i]){
				case no_connect:
						if (FD_ISSET(sockfd, &readfds)) {
							receive_message(sockfd,&msg[i]);
							switch(msg[i].code){
								case USERID:
									code=search_userid(&user_account[i],(char*)msg[i].data);
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
											protocol_state[i]=correct_id;
											FD_SET(sockfd,&checkfds_write);
											FD_CLR(sockfd,&checkfds_read);
									}
									}
									break;
								default:
									if(send_UNKNOWN(sockfd,msg[i]))
										{close(sockfd	);printf("connect is die\n");return -1;};
									break;
						}
					break;
				case correct_id:

					if(receive_message(sockfd,&msg[i])==-1)
					{close(sockfd);printf("connect is die\n");return -1;}
					if(msg[i].code==PASSWORD){
						if(strcmp(user_account[i]->password,(char*)msg[i].data)==0){
							if(send_PASSWORD_RIGHT(sockfd)==-1){
							}
							protocol_state[i]=authorized;
							FD_CLR(sockfd,&checkfds_write);
							FD_SET(sockfd,&checkfds_read);
							user_account[i]->soluongdangnhapsai=0;
						}
						else{
							if(strcmp("q",(char*)msg[i].data)!=0){
								printf("%d\n",user_account[i]->soluongdangnhapsai );
								if(user_account[i]->soluongdangnhapsai>=MAX_WRONGPASSWORD){
									user_account[i]->status=0;
									export_to_data_file();
									if(send_PASSWORD_BLOCK(sockfd)==-1)
										{close(sockfd);printf("connect is die\n");return -1;}
									protocol_state[i]=no_connect;
								}
								else{
									user_account[i]->soluongdangnhapsai+=1;
									// 
									if(send_PASSWORD_WRONG(sockfd)==-1)
										{close(sockfd);printf("connect is die\n");return -1;}

								}
							}
							else{
								protocol_state[i]=no_connect;
							}
						}
					}
					else if(msg[i].code==RESET){
						protocol_state[i]=no_connect;
						FD_CLR(sockfd,&checkfds_write);
						FD_SET(sockfd,&checkfds_read);
					}
					else{
						if(send_UNKNOWN(sockfd,msg[i])==-1)
							{close(sockfd);printf("connect is die\n");return -1;}
					}

					 break;
				case authorized:
					if(receive_message(sockfd,&msg[i])==-1)
						{close(sockfd);printf("connect is die\n");return -1;}
					switch(msg[i].code){
						case LOGOUT:
							if(send_LOGOUT_SUCCESS(sockfd)==-1)
								{close(sockfd);printf("connect is die\n");return -1;}
							protocol_state[i]=no_connect;
						break;
						default:
							if(send_UNKNOWN(sockfd,msg[i])==01)
								{close(sockfd);printf("connect is die\n");return -1;}
						break;
					}
					break;
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
		// if receive enter room ==> change protocol ==> set protocol state to be in room
	
	//
	return 0;
}
