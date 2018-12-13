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

int send_REQUEST_ROOM_LIST(int socket,ROOM_LIST_PARAM param){
	web_message msg;
	msg.code = REQUEST_ROOM_LIST;
	msg.data_len = sizeof(ROOM_LIST_PARAM);
	msg.data = &param;
	return send_web_message(socket, msg);
}

int send_ROOM_LIST(int socket,ROOM_LIST_RESPOND data){
	web_message msg;
	msg.code = RESPOND_ROOM_LIST;
	msg.data_len = sizeof(ROOM_LIST_RESPOND);
	msg.data = &data;
	return send_web_message(socket, msg);
}

int receive_REQUEST_ROOM_LIST(int socket,ROOM_LIST_RESPOND *data){

	web_message *msg;
	int recv;
	recv = receive_web_message(socket, msg);
	data = msg->data;
	return recv;
}

int send_REQUEST_BUY_NOW(int socket,BUY_NOW_PARAM param){
	web_message msg;
	msg.code = REQUEST_BUY_NOW;
	msg.data_len = sizeof(BUY_NOW_PARAM);
	msg.data = &param;
	return send_web_message(socket, msg);
}

int send_RESPOND_BUY_NOW(int socket,BUY_NOW_RESPOND data){
	web_message msg;
	msg.code = RESPOND_BUY_NOW;
	msg.data_len = sizeof(BUY_NOW_RESPOND);
	msg.data = &data;
	return send_web_message(socket, msg);
}

int receive_REQUEST_BUY_NOW(int socket,BUY_NOW_RESPOND *data){
	web_message *msg;
	int recv = receive_web_message(socket, msg);
	data = msg->data;
	return recv;
}

int send_REQUEST_MAKE_ROOM(int socket,MAKE_ROOM_PARAM param){
	web_message msg;
	msg.code = REQUEST_MAKE_ROOM;
	msg.data_len = sizeof(MAKE_ROOM_PARAM);
	msg.data = &param;
	return send_web_message(socket, msg);
}

int send_RESPOND_MAKE_ROOM(int socket,MAKE_ROOM_RESPOND data){
	web_message msg;
	msg.code = RESPOND_MAKE_ROOM;
	msg.data_len = sizeof(MAKE_ROOM_RESPOND);
	msg.data = &data;
	return send_web_message(socket, msg);
}

int receive_REQUEST_MAKE_ROOM(int socket,MAKE_ROOM_RESPOND *data){
	web_message *msg;
	int recv = receive_web_message(socket, msg);
	data = msg->data;
	return recv;
}

int send_REQUEST_ROOM_DETAIL(int socket,ROOM_DETAIL_PARAM param){
	web_message msg;
	msg.code = REQUEST_ROOM_DETAIL;
	msg.data_len = sizeof(ROOM_DETAIL_PARAM);
	msg.data = &param;
	return send_web_message(socket, msg);
}

int send_RESPOND_ROOM_DETAIL(int socket,ROOM_DETAIL_RESPOND data){
	web_message msg;
	msg.code = RESPOND_ROOM_DETAIL;
	msg.data_len = sizeof(ROOM_DETAIL_RESPOND);
	msg.data = &data;
	return send_web_message(socket, msg);
}

int receive_REQUEST_ROOM_DETAIL(int socket,ROOM_DETAIL_RESPOND *data){
	web_message *msg;
	int recv = receive_web_message(socket, msg);
	data = msg->data;
	return recv;
}

int send_REQUEST_MY_ROOM_LIST(int socket,MY_ROOM_LIST_PARAM param){
	web_message msg;
	msg.code = ;
	msg.data_len = sizeof(MY_ROOM_LIST_PARAM);
	msg.data = &param;
	return send_web_message(socket, msg);
}

int send_MY_ROOM_LIST(int socket,MY_ROOM_LIST_RESPOND data){
	web_message msg;
	msg.code = ;
	msg.data_len = sizeof(MY_ROOM_LIST_RESPOND);
	msg.data = &param;
	return send_web_message(socket, msg);
}

int receive_REQUEST_MY_ROOM_LIST(int socket,MY_ROOM_LIST_RESPOND *data){
	web_message *msg;
	int recv = receive_web_message(socket, msg);
	data = msg->data;
	return recv;
}

