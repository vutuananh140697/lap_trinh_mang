#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <sys/types.h>

#define BUFF_SIZE 1024

int main(int argc, char **argv){
	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received;
	struct iovec iov[2];
	char alpha[BUFF_SIZE];
	char digit[BUFF_SIZE];

	iov[0].iov_base = alpha;
	iov[0].iov_len = sizeof(alpha);
	iov[1].iov_base = digit;
	iov[1].iov_len = sizeof(digit);

	//check input
	if(argc<3){
		printf("Input Error\n");
		return 1;
	}
	
	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately!\n ");
		return 0;
	}
		
	//Step 4: Communicate with server			
	while(1){
		//send message
		printf("\nInsert string to send:");
		memset(buff,'\0',(strlen(buff)+1));

		fgets(buff, BUFF_SIZE, stdin);	
		buff[strlen(buff) - 1] = '\0';

		msg_len = strlen(buff);
		if (msg_len == 0) break;
		
		bytes_sent = send(client_sock, buff, msg_len, 0);
		if(bytes_sent <= 0){
			printf("\nConnection closed!\n");
			break;
		}

		memset(iov[0].iov_base,'\0',(strlen(iov[0].iov_base)+1));
		memset(iov[1].iov_base,'\0',(strlen(iov[1].iov_base)+1));
		//receive echo reply
		bytes_received = readv(client_sock, iov, 2);
		if(bytes_received <= 0){
			printf("\nError!Cannot receive data from sever!\n");
			break;
		}
		
		if(strlen(iov[0].iov_base) != 0)
			printf("%s\n", (char *)iov[0].iov_base);
		if(strlen(iov[1].iov_base) != 0)
		printf("%s\n", (char *)iov[1].iov_base);
	}
	
	//Step 4: Close socket
	close(client_sock);
	return 0;
}
