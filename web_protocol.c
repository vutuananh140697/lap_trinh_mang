#include "web_protocol.h"

int send_a_item(int socket, Item *item){
	if(send_a_int(socket, item->id) != 0)
		return -1;
	if(send_a_string(socket, item->name) != 0)
		return -1;
	if(send_a_string(socket, item->description) != 0)
		return -1;
	if(send_a_int(socket, item->price) != 0)
		return -1;
	if(send_a_int(socket, item->price_immediately) != 0)
		return -1;
	return 0;
}
// malloc item before using
int recv_a_item(int socket, Item *item){
	int id;
	char name[ITEM_NAME];
	char description[ITEM_DESCRIPTION];
	int price;
	int price_immediately;
	if(recv_a_int(socket, &id) != 0)
		return -1;
	if(recv_a_string(socket, name) != 0)
		return -1;
	if(recv_a_string(socket, description) != 0)
		return -1;
	if(recv_a_int(socket, &price) != 0)
		return -1;
	if(recv_a_int(socket, &price_immediately) != 0)
		return -1;
	item->id = id;
	item->name = name;
	item->description = description;
	item->price = price;
	item->price_immediately = price_immediately;
	return 0;
}

int send_a_queue(int socket, Queue *Q){
    if(send_a_int(socket, count_products(Q)) != 0)
      return -1;
    Qnode *first = Q->Front;
    while(first != NULL){
      if(send_a_item(socket, first->item) != 0)
        return -1;
      first = first->Next;
    }
    return 0;
}
// init queue before using
int recv_a_queue(int socket, Queue *Q){
	//todo
	int count_products;
	int i;
	char name[CLIENT_MAX][ITEM_NAME];
	char description[CLIENT_MAX][ITEM_DESCRIPTION];

	Item *item = (Item*)malloc(sizeof(Item));

	if(recv_a_int(socket, &count_products) != 0)
		return -1;
	for(i=0; i<count_products; i++){
		if(recv_a_item(socket, item) != 0)
			return -1;
		strcpy(name[i], item->name);
		strcpy(description[i], item->description);
		Push(Q, item->id, name[i], description[i], item->price, item->price_immediately);
	}
	return 0;
}

int send_a_room(int socket, Room *room){
	if(send_a_int(socket, room->id) != 0) 
		return -1;
	if(send_a_string(socket, room->username) != 0)
		return -1;
	if(send_a_queue(socket, room->product_list) != 0)
		return -1;
	if(send_a_int(socket, room->number_of_member) != 0)
		return -1;
	return 0;
}

int recv_a_room(int socket, Room *room){
	//todo
	Queue *Q;
	Q = Init(Q);
	char username[USERNAME];
	if(recv_a_int(socket, &room->id) != 0)
		return -1;
	if(recv_a_string(socket, username) != 0)
		return -1;
	room->username = username;
	if(recv_a_queue(socket, Q) != 0)
		return -1;
	room->product_list = Q;
	if(recv_a_int(socket, &room->number_of_member) != 0)
		return -1;
	return 0;
}

int send_room_list(int socket, Room *header){
	if(send_a_int(socket, count_room(header)) != 0)
		return -1;
	while(header != NULL){
		if(send_a_room(socket, header) != 0)
			return -1;
		header = header->next;
 	}	
 	return 0;
}

//input Room **header = create_room_list() 
int recv_room_list(int socket, Room **header){
	int count_room;
	int i;
	char username[CLIENT_MAX][USERNAME];
	Room *room = (Room *)malloc(sizeof(Room));

	if(recv_a_int(socket, &count_room) != 0)
		return -1;
	for(i=0; i<count_room; i++){
		if(recv_a_room(socket, room) != 0)
			return -1;
		strcpy(username[i], room->username);
		add_new_room(header, room->id, username[i], room->product_list, room->number_of_member);
	}
	return 0;
}



// int send_web_message(int socket, web_message msg){
// 	int a=send_a_int(socket,msg.code);
// 	switch(msg.code){
// 		case REQUEST_ROOM_LIST:
// 			int b =  send_a_int(socket,msg.data_len);
// 			int c = send_all_byte(socket, msg.data, msg.data_len);
// 			break;
// 		case RESPOND_ROOM_LIST:
// 			int b = send_a_int(socket,msg.data_len);
// 			int c = send_room_list(socket, msg.data);
// 			break;
// 		case REQUEST_BUY_NOW:
// 			int b=send_a_int(socket,msg.data_len);
// 			int c=send_all_byte(socket,msg.data,msg.data_len);
// 			break;
// 		case RESPOND_BUY_NOW:
// 			int b=send_a_int(socket,msg.data_len);
// 			int c=send_all_byte(socket,msg.data,msg.data_len);
// 			break;
// 		case REQUEST_MAKE_ROOM:
// 			int b=send_a_int(socket,msg.data_len);
// 			int c = send_a_queue(socket, msg.data); 
// 			break;
// 		case RESPOND_MAKE_ROOM:
// 			int b=send_a_int(socket,msg.data_len);
// 			int c=send_all_byte(socket,msg.data,msg.data_len);
// 			break;
// 		case REQUEST_ROOM_DETAIL:
// 			int b=send_a_int(socket,msg.data_len);
// 			int c=send_all_byte(socket,msg.data,msg.data_len);
// 			break;
// 		case RESPOND_ROOM_DETAIL:
// 			int b=send_a_int(socket,msg.data_len);
// 			int c=send_a_room(socket,msg.data);
// 			break;
// 		case REQUEST_MY_ROOM_LIST:
// 			int b=send_a_int(socket,msg.data_len);
// 			int c=send_all_byte(socket,msg.data,msg.data_len);
// 			break;
// 		case RESPOND_MY_ROOM_LIST:
// 			int b = send_a_int(socket,msg.data_len);
// 			int c = send_room_list(socket, msg.data);
// 			break;
// 	}
// 	if(a==-1||b==-1||c==-1)
// 		return -1;
// 	return 0;
// }

// int receive_web_message(int socket,web_message *msg){
// 	int temp;
// 	ROOM_LIST_RESPOND room_list;
// 	MAKE_ROOM_PARAM 

// 	int a=recv_a_int(socket,&temp);
// 	msg->code=temp;
// 	switch(msg->code){
// 		case REQUEST_ROOM_LIST:
// 			int b=recv_a_int(socket,&(msg->data_len));
// 			msg->data=malloc(sizeof(msg->data_len));
// 			int c=recv_all_byte(socket,msg->data,msg->data_len);
// 			break;
// 		case RESPOND_ROOM_LIST:
// 			data = (ROOM_LIST_RESPOND *)msg->data;
// 			int b=recv_a_int(socket,&(msg->data_len));
// 			int c = recv_room_list(socket, data->header);
// 			break;
// 		case REQUEST_BUY_NOW:
// 			int b=recv_a_int(socket,&(msg->data_len));
// 			msg->data=malloc(sizeof(msg->data_len));
// 			int c=recv_all_byte(socket,msg->data,msg->data_len);
// 			break;
// 		case RESPOND_BUY_NOW:
// 			int b=recv_a_int(socket,&(msg->data_len));
// 			msg->data=malloc(sizeof(msg->data_len));
// 			int c=recv_all_byte(socket,msg->data,msg->data_len);
// 			break;
// 		case REQUEST_MAKE_ROOM:
// 			int b=send_a_int(socket,msg.data_len);
// 			int c = send_a_queue(socket, msg.data); 
// 			break;
// 		case RESPOND_MAKE_ROOM:
// 			int b=recv_a_int(socket,&(msg->data_len));
// 			msg->data=malloc(sizeof(msg->data_len));
// 			int c=recv_all_byte(socket,msg->data,msg->data_len);
// 			break;
// 		case REQUEST_ROOM_DETAIL:
// 			int b=recv_a_int(socket,&(msg->data_len));
// 			msg->data=malloc(sizeof(msg->data_len));
// 			int c=recv_all_byte(socket,msg->data,msg->data_len);
// 			break;
// 		case RESPOND_ROOM_DETAIL:
// 			int b=send_a_int(socket,msg.data_len);
// 			int c=send_a_room(socket,msg.data->room);
// 			break;
// 		case REQUEST_MY_ROOM_LIST:
// 			int b=recv_a_int(socket,&(msg->data_len));
// 			msg->data=malloc(sizeof(msg->data_len));
// 			int c=recv_all_byte(socket,msg->data,msg->data_len);
// 		case RESPOND_MY_ROOM_LIST:
// 			int b = send_a_int(socket,msg.data_len);
// 			int c = send_room_list(socket, msg.data->header);
// 			break;
// 	}
// 	if(a==-1 || b==-1 ||c==-1)
// 		return -1;
// 	return 0;
// }

// int receive_web_message(int socket,web_message *msg){
// 	int Opcode;

// 	int a = recv_a_int(socket, &Opcode);
// 	switch(Opcode){
// 		case REQUEST_ROOM_LIST:
// 			// ROOM_LIST_PARAM *param;
// 			// receive_REQUEST_ROOM_LIST(socket, param);
// 			break;
// 		case RESPOND_ROOM_LIST:
// 			break;
// 		case REQUEST_BUY_NOW:
// 			break;
// 		case RESPOND_BUY_NOW:
// 			break;
// 		case REQUEST_MAKE_ROOM:
// 			break;
// 		case RESPOND_MAKE_ROOM:
// 			break;
// 		case REQUEST_ROOM_DETAIL:
// 			break;
// 		case RESPOND_ROOM_DETAIL:
// 			break;
// 		case REQUEST_MY_ROOM_LIST:
// 			break;
// 		case RESPOND_MY_ROOM_LIST:
// 			break;
// 		default:
// 			break;
// 	}
// }

int check_error(int a, int b, int c){
	if(a==-1 || b==-1 ||c==-1)
		return -1;
	return 0;
}

int send_REQUEST_ROOM_LIST(int socket,ROOM_LIST_PARAM param){
	int a = 0; //send_a_int(socket, REQUEST_ROOM_LIST);
	int b = 0 ;//send_a_int(socket, sizeof(ROOM_LIST_PARAM));
	int c = send_a_int(socket, param.page_id);
	return check_error(a,b,c);
}

int receive_REQUEST_ROOM_LIST(int socket, ROOM_LIST_PARAM *param){
	int data_len;
	int b = recv_a_int(socket, &data_len);
	int c = recv_a_int(socket, &param->page_id);
	return check_error(0, b, c);
}

int send_ROOM_LIST(int socket,ROOM_LIST_RESPOND data){
	int a = send_a_int(socket, RESPOND_ROOM_LIST);
	int b = send_room_list(socket, data.header);
	return check_error(a, b, 0);
}

// int receive_REQUEST_ROOM_LIST(int socket,ROOM_LIST_RESPOND *data){

// 	web_message *msg;
// 	int recv;
// 	recv = receive_web_message(socket, msg);
// 	data = msg->data;
// 	return recv;

// 	int b=recv_a_int(socket,&(msg->data_len));
// 	msg->data=malloc(sizeof(msg->data_len));
// 	int c=recv_all_byte(socket,msg->data,msg->data_len);
// }

int send_REQUEST_BUY_NOW(int socket,BUY_NOW_PARAM param){
	int a = send_a_int(socket, REQUEST_BUY_NOW);
	int b = send_a_int(socket, sizeof(BUY_NOW_PARAM));
	int c = send_all_byte(socket, &param, sizeof(BUY_NOW_PARAM));
	return check_error(a, b, c);
}

int send_RESPOND_BUY_NOW(int socket,BUY_NOW_RESPOND data){
	int a = send_a_int(socket, RESPOND_BUY_NOW);
	int b = send_a_string(socket, data.message);
	return check_error(a, b, 0);
}

// int receive_REQUEST_BUY_NOW(int socket,BUY_NOW_RESPOND *data){
// 	web_message *msg;
// 	int recv = receive_web_message(socket, msg);
// 	data = msg->data;
// 	return recv;
// }

int send_REQUEST_MAKE_ROOM(int socket,MAKE_ROOM_PARAM param){
	int a = send_a_int(socket, REQUEST_MAKE_ROOM);
	int b = send_a_queue(socket, param.product_list);
	return check_error(a, b, 0);
}

int send_RESPOND_MAKE_ROOM(int socket,MAKE_ROOM_RESPOND data){
	int a = send_a_int(socket, RESPOND_MAKE_ROOM);
	int b = send_a_string(socket, data.message);
	return check_error(a, b, 0);
}

// int receive_REQUEST_MAKE_ROOM(int socket,MAKE_ROOM_RESPOND *data){
// 	web_message *msg;
// 	int recv = receive_web_message(socket, msg);
// 	data = msg->data;
// 	return recv;
// }

int send_REQUEST_ROOM_DETAIL(int socket,ROOM_DETAIL_PARAM param){
	int a = send_a_int(socket, REQUEST_ROOM_DETAIL);
	int b = send_a_int(socket, sizeof(ROOM_DETAIL_PARAM));
	int c = send_all_byte(socket, &param, sizeof(ROOM_DETAIL_PARAM));
	return check_error(a, b, c);
}

int send_RESPOND_ROOM_DETAIL(int socket,ROOM_DETAIL_RESPOND data){
	int a = send_a_int(socket, RESPOND_ROOM_DETAIL);
	int b = send_a_room(socket, data.room);
	return check_error(a, b, 0);
}

// int receive_REQUEST_ROOM_DETAIL(int socket,ROOM_DETAIL_RESPOND *data){
// 	web_message *msg;
// 	int recv = receive_web_message(socket, msg);
// 	data = msg->data;
// 	return recv;
// }

int send_REQUEST_MY_ROOM_LIST(int socket,MY_ROOM_LIST_PARAM param){
	int a = send_a_int(socket, REQUEST_MY_ROOM_LIST);
	int b = send_a_int(socket, sizeof(MY_ROOM_LIST_PARAM));
	int c = send_all_byte(socket, &param, sizeof(MY_ROOM_LIST_PARAM));
	return check_error(a,b,c);
}

int send_MY_ROOM_LIST(int socket,MY_ROOM_LIST_RESPOND data){
	int a = send_a_int(socket, RESPOND_MY_ROOM_LIST);
	int b = send_room_list(socket, data.header);
	return check_error(a, b, 0);
}

// int receive_REQUEST_MY_ROOM_LIST(int socket,MY_ROOM_LIST_RESPOND *data){
// 	web_message *msg;
// 	int recv = receive_web_message(socket, msg);
// 	data = msg->data;
// 	return recv;
// }

