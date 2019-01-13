#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h> 
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BACKLOG 20   /* Number of allowed connections */
#define BUFF_SIZE 1024
#define NOTLOGIN 1
#define UNAUTHENTICATED 2
#define AUTHENTICATED 3

/* The processData function copies the input string to output */
void processData(char *in, char *out);

/* The recv() wrapper function*/
int receiveData(int s, char *buff, int size, int flags);

/* The send() wrapper function*/
int sendData(int s, char *buff, int size, int flags);

/* --------------  Data Structure --------------  */

typedef struct 
{
	char username[50];
	char password[50];
	int status;
	int login;
	int loginCount;
}account;

typedef struct node
{
	account acc;
	struct node* next;
}node;

FILE* f;
node* head = NULL;

/* --------------  Functions work with Link list --------------  */

// Make a new node in LinkedList
//input: a string of username, a string of password, status of user
//output: a pointer of node user
node *makeNode(char* username, char*password, int status){
	node* temp;
	temp = (node*)malloc(sizeof(node));
	temp->next = NULL;
	strcpy((temp->acc).username, username);
	strcpy((temp->acc).password, password);
	(temp->acc).status = status;
	(temp->acc).login = 0;
	(temp->acc).loginCount = 0;
	return temp;
}

// Insert a node in LinkedList
//input: a string is a username, a string is a password, status of user
//output: a update list
void insertList(char* username, char* password, int status){
	node* temp;
	temp = makeNode(username, password, status);
	temp->next = head;
	head = temp;
}

// Check node is exits in LinkedList
//input: a string is a username
//return 1 if list has username and 0 else
int searchList(char* username){
	node* cur;
	cur = head;
	while(cur != NULL){
		if(strcmp((cur->acc).username, username) == 0) return 1;
		cur = cur->next;
	}
	return 0;
}

// Search a node by username
//input: a string is a username
//output: return a pointer of user or NULL else
node* findAccount(char* username){
	node* cur;
	cur = head;
	while(cur != NULL){
		if(strcmp((cur->acc).username, username) == 0) return cur;
		cur = cur->next;
	}
	return NULL;
}

// Print node in LinkedList
void printList(){
	node* cur;
	cur = head;
	while(cur!= NULL){
		printf("%s %s\n",(cur->acc).username, (cur->acc).password );
		cur = cur->next;
	}
}

/* --------------  Functions work with File--------------  */

// Read data from file and insert to LinkedList
void readFile() {
	char username[50];
	char password[50];
	int status;

	f = fopen("account.txt", "r");
	while((fscanf(f,"%s",username))!=EOF){
		fscanf(f,"%s", password);
		fscanf(f, "%d", &status);
		insertList(username, password, status);
	}
	fclose(f);
}

// Write data from LinkedList to File
void writeToFile(){
	f = fopen("account.txt", "w");
	node* cur;
	cur = head;
	while(cur!= NULL){
		fprintf(f, "%s %s %d\n",(cur->acc).username, (cur->acc).password, (cur->acc).status);
		cur = cur->next;
	}
	fclose(f);
}

/*----------------------------------------------------------------------------------------*/

/*
split a message received from client to determine command and get content in command
input: a string is a message received from server, a string gets content
output: return 1 if received username
	return 2 if received password
	return 3 if received logout command
	return 4 if is not command
*/
int readMessage(char *message, char *content){
	char str[BUFF_SIZE];
	char *p;
	char *p1;

	strcpy(str, message);
	p = strtok(str, " ");
	if(p == NULL ) return 4;

	p1 = strtok(NULL, " ");
	if(p1 != NULL)
		strcpy(content, p1);
	else
		content[0] = '\0';

	if(strcmp(p,"USER") == 0) return 1;
	else if(strcmp(p, "PASS") == 0) return 2;
	else if(strcmp(p, "LOGOUT") == 0) return 3;
	else return 4;
}


int main(int argc, char **argv)
{

	int i, maxi, maxfd, listenfd, connfd, sockfd;
	int nready, client[FD_SETSIZE];
	ssize_t	ret;
	fd_set	readfds, allset;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	char recv_data[BUFF_SIZE];
	char message[BUFF_SIZE];
	int bytes_sent, bytes_received;

	int protocolStatus[FD_SETSIZE];
	int count;
	char username[50];
	char password[50];
	char string[50];
	node **user;
	int result;

	user = (node **)malloc(sizeof(node)*FD_SETSIZE);

	for(count = 0; count < FD_SETSIZE; count++)
		protocolStatus[count] = NOTLOGIN;

	//check input
	if(argc<2){
		printf("Input Error\n");
		return 1;
	}
	//read data from file to linked list
	readFile();

	//Step 1: Construct a TCP socket to listen connection request
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}

	//Step 2: Bind address to socket
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr))==-1){ /* calls bind() */
		perror("\nError: ");
		return 0;
	} 

	//Step 3: Listen request from client
	if(listen(listenfd, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}

	maxfd = listenfd;			/* initialize */
	maxi = -1;				/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;			/* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	
	//Step 4: Communicate with clients
	while (1) {
		readfds = allset;		/* structure assignment */
		nready = select(maxfd+1, &readfds, NULL, NULL, NULL);
		if(nready < 0){
			perror("\nError: ");
			return 0;
		}
		
		if (FD_ISSET(listenfd, &readfds)) {	/* new client connection */
			clilen = sizeof(cliaddr);
			if((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0)
				perror("\nError: ");
			else{
				printf("You got a connection from %s\n", inet_ntoa(cliaddr.sin_addr)); /* prints client's IP */
				for (i = 0; i < FD_SETSIZE; i++)
					if (client[i] < 0) {
						client[i] = connfd;	/* save descriptor */
						break;
					}
				if (i == FD_SETSIZE){
					printf("\nToo many clients");
					close(connfd);
				}

				FD_SET(connfd, &allset);	/* add new descriptor to set */
				if (connfd > maxfd)
					maxfd = connfd;		/* for select */
				if (i > maxi)
					maxi = i;		/* max index in client[] array */

				if (--nready <= 0)
					continue;		/* no more readable descriptors */
			}
		}

		for (i = 0; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &readfds)) {
				bytes_received = receiveData(sockfd, message, BUFF_SIZE - 1, 0);
				if (bytes_received <= 0){
					FD_CLR(sockfd, &allset);
					close(sockfd);
					client[i] = -1;
					user[i] = NULL;
					protocolStatus[i] = NOTLOGIN;
				}
				
				
				else {
					message[bytes_received] = '\0';
					switch(protocolStatus[i]){
						case NOTLOGIN:
								
							result = readMessage(message, username);
							if(result == 1){

								user[i] = findAccount(username);
								if(strlen(username) == 0){
									strcpy(message, "50");
									protocolStatus[i] = NOTLOGIN;
								}
								else if(user[i] != NULL){
									if((user[i]->acc).status == 1){
										strcpy(message,"10");
										protocolStatus[i] = UNAUTHENTICATED;
									}
									else{
										strcpy(message, "13");
										protocolStatus[i] = NOTLOGIN;
									}
										
								}
								else{
									strcpy(message, "11");
									protocolStatus[i] = NOTLOGIN;
										
								}
							}
							else if(result == 4 ){
								strcpy(message, "40");
								protocolStatus[i] = NOTLOGIN;
							}
							else{
								strcpy(message, "12");
								protocolStatus[i] = NOTLOGIN;
							}

							break;
						case UNAUTHENTICATED:

							if(readMessage(message, password) == 2){
								if(strcmp((user[i]->acc).password, password) == 0){
									strcpy(message, "20");
									protocolStatus[i] = AUTHENTICATED;
									(user[i]->acc).login = 1;
									(user[i]->acc).loginCount = 0;
								}
								else{
									strcpy(message, "21");
									protocolStatus[i] = NOTLOGIN;
									(user[i]->acc).loginCount++;
									if((user[i]->acc).loginCount == 3){
										strcpy(message, "23");
										(user[i]->acc).status = 0;
									}
								}
							}
							else if(readMessage(message, password) == 4){
								strcpy(message, "40");
								protocolStatus[i] = NOTLOGIN;
							}
							else{
								strcpy(message, "22");
								protocolStatus[i] = NOTLOGIN;
							}
							writeToFile();
								
							break;
						case AUTHENTICATED: 

							if(readMessage(message, string) == 3){
								(user[i]->acc).login = 0;
								strcpy(message, "30");
								protocolStatus[i] = NOTLOGIN;
							}
							else if(readMessage(message, string) == 4){
								strcpy(message, "40");
								protocolStatus[i] = AUTHENTICATED;
							}
							else{
								strcpy(message, "31");
								protocolStatus[i] = AUTHENTICATED;
							}

							break;
					}
					bytes_sent =  sendData(sockfd, message, strlen(message), 0);
					if (bytes_sent <= 0){
						FD_CLR(sockfd, &allset);
						close(sockfd);
						client[i] = -1;
						user[i] = NULL;
						protocolStatus[i] = NOTLOGIN;
					}
				}

				if (--nready <= 0)
					break;		/* no more readable descriptors */
			}
		}
	}
	
	return 0;
}


void processData(char *in, char *out){
	strcpy (out, in);
}

int receiveData(int s, char *buff, int size, int flags){
	int n;
	n = recv(s, buff, size, flags);
	if(n < 0)
		perror("Error: ");
	return n;
}

int sendData(int s, char *buff, int size, int flags){
	int n;
	n = send(s, buff, size, flags);
	if(n < 0)
		perror("Error: ");
	return n;
}

