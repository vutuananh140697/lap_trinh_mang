#include "web_protocol.h"

int send_web_message(int socket, web_message msg){
	int a=send_a_int(socket,msg.code);
	int b=send_a_int(socket,msg.data_len);
	int c=send_all_byte(socket,msg.data,msg.data_len);
	if(a==-1||b==-1||c==-1)
		return -1;
	return 0;
}

int receive_web_message(int socket,web_message *msg){
	int temp;
	int a=recv_a_int(socket,&temp);
	msg->code=temp;

	int b=recv_a_int(socket,&(msg->data_len));
	msg->data=malloc(sizeof(msg->	data_len));
	
	int c=recv_all_byte(socket,msg->data,msg->data_len);
	if(a==-1 || b==-1 ||c==-1)
		return -1;
	return 0;
}