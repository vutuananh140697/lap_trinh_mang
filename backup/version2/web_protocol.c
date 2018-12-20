#include "web_protocol.h"


int send_room_list(int socket, Room *header){
	if(send_a_int(socket, count_room(header)) != 0)
		return -1;
	while(header != NULL){
		if(send_a_int(socket, header->id) != 0) 
			return -1;
		if(send_a_string(socket, header->username) != 0)
			return -1;
		if(send_a_queue(socket, header->product_list) != 0)
			return -1;
		header = header->Next;
 	}	
 	return 0;
}

int recv_room_list(int socket, Room *)

int send_a_queue(int socket, Queue *Q){
    if(send_a_int(socket, count_products(Q)) != 0)
      return -1;
    Qnode *first = Q->Front;
    while(first != NULL){
      if(send_all_byte(socket, first->item, sizeof(Item)) != 0)
        return -1;
      first = first->Next;
    }
    return 0;
  }
}

int send_a_room(int socket, Room *room){
	if(send_a_int(socket, room->id) != 0) 
		return -1;
	if(send_a_string(socket, room->username) != 0)
		return -1;
	if(send_a_queue(socket, room->product_list) != 0)
		return -1;
	return 0;
}

int send_web_message(int socket, web_message msg){
	int a=send_a_int(socket,msg.code);
	switch(msg.code){
		case REQUEST_ROOM_LIST:
			int b =  send_a_int(socket,msg.data_len);
			int c = send_all_byte(socket, msg.data, msg.data_len);
			break;
		case RESPOND_ROOM_LIST:
			int b = send_a_int(socket,msg.data_len);
			int c = send_room_list(socket, msg.data);
			break;
		case REQUEST_BUY_NOW:
			int b=send_a_int(socket,msg.data_len);
			int c=send_all_byte(socket,msg.data,msg.data_len);
			break;
		case RESPOND_BUY_NOW:
			int b=send_a_int(socket,msg.data_len);
			int c=send_all_byte(socket,msg.data,msg.data_len);
			break;
		case REQUEST_MAKE_ROOM:
			int b=send_a_int(socket,msg.data_len);
			int c = send_a_queue(socket, msg.data); 
			break;
		case RESPOND_MAKE_ROOM:
			int b=send_a_int(socket,msg.data_len);
			int c=send_all_byte(socket,msg.data,msg.data_len);
			break;
		case REQUEST_ROOM_DETAIL:
			int b=send_a_int(socket,msg.data_len);
			int c=send_all_byte(socket,msg.data,msg.data_len);
			break;
		case RESPOND_ROOM_DETAIL:
			int b=send_a_int(socket,msg.data_len);
			int c=send_a_room(socket,msg.data);
			break;
		case REQUEST_MY_ROOM_LIST:
			int b=send_a_int(socket,msg.data_len);
			int c=send_all_byte(socket,msg.data,msg.data_len);
			break;
		case RESPOND_MY_ROOM_LIST:
			int b = send_a_int(socket,msg.data_len);
			int c = send_room_list(socket, msg.data);
			break;
	}
	if(a==-1||b==-1||c==-1)
		return -1;
	return 0;
}

int receive_web_message(int socket,web_message *msg){
	int temp;
	ROOM_LIST_RESPOND data;	

	int a=recv_a_int(socket,&temp);
	msg->code=temp;
	switch(msg->code){
		case REQUEST_ROOM_LIST:
			int b=recv_a_int(socket,&(msg->data_len));
			msg->data=malloc(sizeof(msg->data_len));
			int c=recv_all_byte(socket,msg->data,msg->data_len);
			break;
		case RESPOND_ROOM_LIST:

			data = (ROOM_LIST_RESPOND *)msg->data;
			int b=recv_a_int(socket,&(msg->data_len));
			int c = recv_room_list(socket, msg->data);
			break;
		case REQUEST_BUY_NOW:
			int b=recv_a_int(socket,&(msg->data_len));
			msg->data=malloc(sizeof(msg->data_len));
			int c=recv_all_byte(socket,msg->data,msg->data_len);
			break;
		case RESPOND_BUY_NOW:
			int b=recv_a_int(socket,&(msg->data_len));
			msg->data=malloc(sizeof(msg->data_len));
			int c=recv_all_byte(socket,msg->data,msg->data_len);
			break;
		case REQUEST_MAKE_ROOM:
			int b=send_a_int(socket,msg.data_len);
			int c = send_a_queue(socket, msg.data->product_list); 
			break;
		case RESPOND_MAKE_ROOM:
			int b=recv_a_int(socket,&(msg->data_len));
			msg->data=malloc(sizeof(msg->data_len));
			int c=recv_all_byte(socket,msg->data,msg->data_len);
			break;
		case REQUEST_ROOM_DETAIL:
			int b=recv_a_int(socket,&(msg->data_len));
			msg->data=malloc(sizeof(msg->data_len));
			int c=recv_all_byte(socket,msg->data,msg->data_len);
			break;
		case RESPOND_ROOM_DETAIL:
			int b=send_a_int(socket,msg.data_len);
			int c=send_a_room(socket,msg.data->room);
			break;
		case REQUEST_MY_ROOM_LIST:
			int b=recv_a_int(socket,&(msg->data_len));
			msg->data=malloc(sizeof(msg->data_len));
			int c=recv_all_byte(socket,msg->data,msg->data_len);
		case RESPOND_MY_ROOM_LIST:
			int b = send_a_int(socket,msg.data_len);
			int c = send_room_list(socket, msg.data->header);
			break;
	}

	int b=recv_a_int(socket,&(msg->data_len));
	msg->data=malloc(sizeof(msg->data_len));
	
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

