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

int check_error(int a, int b, int c){
	if(a==-1 || b==-1 ||c==-1){
		return -1;
	}
	return 0;
}

int send_REQUEST_ROOM_LIST(int socket,ROOM_LIST_PARAM param){

	int a = send_a_int(socket, REQUEST_ROOM_LIST);if(a!=0){return -1;}
	int b = send_a_int(socket, sizeof(ROOM_LIST_PARAM));if(b!=0){return -1;}
	int c = send_a_int(socket, param.page_id);if(c!=0){return -1;}
	return check_error(a,b,c);
}

int receive_REQUEST_ROOM_LIST(int socket, web_message *message){
	ROOM_LIST_PARAM *param = (ROOM_LIST_PARAM*)malloc(sizeof(ROOM_LIST_PARAM));
	int b = recv_a_int(socket, &message->data_len);if(b!=0){return -1;}
	int c = recv_a_int(socket, &param->page_id);if(c!=0){return -1;}
	message->data = param;
	return check_error(0, b, c);
}

int send_RESPOND_ROOM_LIST(int socket,ROOM_LIST_RESPOND data){
	int a = send_a_int(socket, RESPOND_ROOM_LIST);if(a!=0){return -1;}
	int b = send_room_list(socket, data.header);if(b!=0){return -1;}
	return check_error(a, b, 0);
}

int receive_RESPOND_ROOM_LIST(int socket, web_message *message){
	ROOM_LIST_RESPOND *param = (ROOM_LIST_RESPOND*)malloc(sizeof(ROOM_LIST_RESPOND));
	Room **header = create_room_list();
	int b = recv_room_list(socket, header);if(b!=0){return -1;}
	param->header = *header;
	message->data = param;
	return check_error(0, b, 0);
}

int send_REQUEST_BUY_NOW(int socket,BUY_NOW_PARAM param){
	int a = send_a_int(socket, REQUEST_BUY_NOW);if(a!=0){return -1;}
	int b = send_a_int(socket, sizeof(BUY_NOW_PARAM));if(b!=0){return -1;}
	int c = send_a_int(socket, param.id);if(c!=0){return -1;}
	return check_error(a, b, c);
}

int receive_REQUEST_BUY_NOW(int socket, web_message *message){
	BUY_NOW_PARAM *param = (BUY_NOW_PARAM*)malloc(sizeof(BUY_NOW_PARAM));
	int b = recv_a_int(socket, &message->data_len);if(b!=0){return -1;}
	int c = recv_a_int(socket, &param->id);if(c!=0){return -1;}
	message->data = param;
	return check_error(0, b, c);
}

int send_RESPOND_BUY_NOW(int socket,BUY_NOW_RESPOND data){
	int a = send_a_int(socket, RESPOND_BUY_NOW);if(a!=0){return -1;}
	int b = send_a_string(socket, data.message);if(b!=0){return -1;}
	return check_error(a, b, 0);
}

int receive_RESPOND_BUY_NOW(int socket, web_message *message){
	BUY_NOW_RESPOND *param = (BUY_NOW_RESPOND*)malloc(sizeof(BUY_NOW_RESPOND));
	char mess[1000];
	int b = recv_a_string(socket, mess);if(b!=0){return -1;}
	param->message = mess;
	message->data = param;
	return check_error(0, b, 0);
}

int send_REQUEST_MAKE_ROOM(int socket,MAKE_ROOM_PARAM param){
	int a = send_a_int(socket, REQUEST_MAKE_ROOM);if(a!=0){return -1;}
	int b = send_a_queue(socket, param.product_list);if(b!=0){return -1;}
	return check_error(a, b, 0);
}

int receive_REQUEST_MAKE_ROOM(int socket, web_message *message){
	MAKE_ROOM_PARAM *param = (MAKE_ROOM_PARAM*)malloc(sizeof(MAKE_ROOM_PARAM));
	Queue *Q;
	Q = Init(Q);
	int b = recv_a_queue(socket, Q);if(b!=0){return -1;}
	param->product_list = Q;
	message->data = param;
	return check_error(0, b, 0);
}

int send_RESPOND_MAKE_ROOM(int socket,MAKE_ROOM_RESPOND data){
	int a = send_a_int(socket, RESPOND_MAKE_ROOM);if(a!=0){return -1;}
	int b = send_a_string(socket, data.message);if(b!=0){return -1;}
	return check_error(a, b, 0);
}

int receive_RESPOND_MAKE_ROOM(int socket, web_message *message){
	MAKE_ROOM_RESPOND *param = (MAKE_ROOM_RESPOND*)malloc(sizeof(MAKE_ROOM_RESPOND));
	char mess[1000];
	int b = recv_a_string(socket, mess);if(b!=0){return -1;}
	param->message = mess;
	message->data = param;
	return check_error(0, b, 0);
}

int send_REQUEST_ROOM_DETAIL(int socket,ROOM_DETAIL_PARAM param){
	int a = send_a_int(socket, REQUEST_ROOM_DETAIL);if(a!=0){return -1;}
	int b = send_a_int(socket, sizeof(ROOM_DETAIL_PARAM));if(b!=0){return -1;}
	int c = send_a_int(socket, param.room_id);if(c!=0){return -1;}
	return check_error(a, b, c);
}

int receive_REQUEST_ROOM_DETAIL(int socket, web_message *message){
	ROOM_DETAIL_PARAM *param = (ROOM_DETAIL_PARAM*)malloc(sizeof(ROOM_DETAIL_PARAM));
	int b = recv_a_int(socket, &message->data_len);if(b!=0){return -1;}
	int c = recv_a_int(socket, &param->room_id);if(c!=0){return -1;}
	message->data = param;
	return check_error(0, b, c);
}

int send_RESPOND_ROOM_DETAIL(int socket,ROOM_DETAIL_RESPOND data){
	int a = send_a_int(socket, RESPOND_ROOM_DETAIL);if(a!=0){return -1;}
	int b = send_a_int(socket, data.result);if(b!=0){return -1;}
	int c = 0;
	if(data.result != 0)
		c = send_a_room(socket, data.room);if(c!=0){return -1;}
	return check_error(a, b, c);
}

int receive_RESPOND_ROOM_DETAIL(int socket, web_message *message){
	ROOM_DETAIL_RESPOND *param = (ROOM_DETAIL_RESPOND*)malloc(sizeof(ROOM_DETAIL_RESPOND));
	Room *room = (Room*)malloc(sizeof(Room));
	int b = recv_a_int(socket, &param->result);if(b!=0){return -1;}
	int c = 0;
	if(param->result != 0){
		c = recv_a_room(socket, room);if(c!=0){return -1;}
		param->room = room;
	}
	message->data = param;
	return check_error(0, b, 0);
}

int send_REQUEST_MY_ROOM_LIST(int socket,MY_ROOM_LIST_PARAM param){
	int a = send_a_int(socket, REQUEST_MY_ROOM_LIST);if(a!=0){return -1;}
	int b = send_a_int(socket, sizeof(MY_ROOM_LIST_PARAM));if(b!=0){return -1;}
	int c = send_a_int(socket, param.page_id);if(c!=0){return -1;}
	return check_error(a,b,c);
}

int receive_REQUEST_MY_ROOM_LIST(int socket, web_message *message){
	MY_ROOM_LIST_PARAM *param = (MY_ROOM_LIST_PARAM*)malloc(sizeof(MY_ROOM_LIST_PARAM));
	int b = recv_a_int(socket, &message->data_len);if(b!=0){return -1;}
	int c = recv_a_int(socket, &param->page_id);if(c!=0){return -1;}
	message->data = param;
	return check_error(0, b, c);
}

int send_RESPOND_MY_ROOM_LIST(int socket,MY_ROOM_LIST_RESPOND data){
	int a = send_a_int(socket, RESPOND_MY_ROOM_LIST);if(a!=0){return -1;}
	int b = send_room_list(socket, data.header);if(b!=0){return -1;}
	return check_error(a, b, 0);
}

int receive_RESPOND_MY_ROOM_LIST(int socket, web_message *message){
	MY_ROOM_LIST_RESPOND *param = (MY_ROOM_LIST_RESPOND*)malloc(sizeof(MY_ROOM_LIST_RESPOND));
	Room **header = create_room_list();
	int b = recv_room_list(socket, header);if(b!=0){return -1;}
	param->header = *header;
	message->data = param;
	return check_error(0, b, 0);
}
int send_REQUEST_ENTER_ROOM(int socket,ENTER_ROOM_PARAM param){
	int a = send_a_int(socket, REQUEST_ENTER_ROOM);if(a!=0){return -1;}
	int b = send_a_int(socket, sizeof(ENTER_ROOM_PARAM));if(b!=0){return -1;}
	int c = send_a_int(socket, param.room_id);if(c!=0){return -1;}
	return check_error(a, b, c);
}
int receive_REQUEST_ENTER_ROOM(int socket, web_message *message){
	ENTER_ROOM_PARAM *param = (ENTER_ROOM_PARAM*)malloc(sizeof(ENTER_ROOM_PARAM));
	int b = recv_a_int(socket, &message->data_len);if(b!=0){return -1;}
	int c = recv_a_int(socket, &param->room_id);if(c!=0){return -1;}
	message->data = param;
	return check_error(0, b, c);
}
int send_RESPOND_ENTER_ROOM(int socket,ENTER_ROOM_RESPOND data){
	int a = send_a_int(socket, RESPOND_ENTER_ROOM);if(a!=0){return -1;}
	int b = send_a_int(socket, data.result);if(b!=0){return -1;}
	int c = 0;
	if(data.result != 0)
		c = send_a_room(socket, data.room);if(c!=0){return -1;}
	return check_error(a, b, c);
}
int receive_RESPOND_ENTER_ROOM(int socket, web_message *message){
	ENTER_ROOM_RESPOND *param = (ENTER_ROOM_RESPOND*)malloc(sizeof(ENTER_ROOM_RESPOND));
	Room *room = (Room*)malloc(sizeof(Room));
	int b = recv_a_int(socket, &param->result);if(b!=0){return -1;}
	int c = 0;
	if(param->result != 0){
		c = recv_a_room(socket, room);if(c!=0){return -1;}
		param->room = room;
	}
	message->data = param;
	return check_error(0, b, 0);
}
int send_LOG_OUT_REQUEST(int socket){
	int a = send_a_int(socket, REQUEST_LOG_OUT);if(a!=0){return -1;}
	return check_error(a, 0, 0);
}
int send_LOG_OUT_RESPOND(int socket){
	int a = send_a_int(socket, RESPOND_LOG_OUT);if(a!=0){return -1;}
	return check_error(a, 0, 0);
}
int receive_web_message(int socket, web_message *message){
	int a = recv_a_int(socket, &message->code);if(a!=0){return -1;}
	int b = -1;
	switch(message->code){
		case REQUEST_ROOM_LIST:
			b = receive_REQUEST_ROOM_LIST(socket, message);
			break;
		case RESPOND_ROOM_LIST:
			b = receive_RESPOND_ROOM_LIST(socket, message);
			break;
		case REQUEST_BUY_NOW:
			b = receive_REQUEST_BUY_NOW(socket, message);
			break;
		case RESPOND_BUY_NOW:
			b = receive_RESPOND_BUY_NOW(socket, message);
			break;
		case REQUEST_MAKE_ROOM:
			b = receive_REQUEST_MAKE_ROOM(socket, message);
			break;
		case RESPOND_MAKE_ROOM:
			b = receive_RESPOND_MAKE_ROOM(socket, message);
			break;
		case REQUEST_ROOM_DETAIL:
			b = receive_REQUEST_ROOM_DETAIL(socket, message);
			break;
		case RESPOND_ROOM_DETAIL:
			b = receive_RESPOND_ROOM_DETAIL(socket, message);
			break;
		case REQUEST_MY_ROOM_LIST:
			b = receive_REQUEST_MY_ROOM_LIST(socket, message);
			break;
		case RESPOND_MY_ROOM_LIST:
			b = receive_RESPOND_MY_ROOM_LIST(socket, message);
			break;
		case REQUEST_ENTER_ROOM:
			b = receive_REQUEST_ENTER_ROOM(socket, message);
			break;
		case RESPOND_ENTER_ROOM:
			b = receive_RESPOND_ENTER_ROOM(socket, message);
			break;	
		case REQUEST_LOG_OUT:
			b = 0;
			break;
		case RESPOND_LOG_OUT:
			b = 0;
			break;
		default:
			break;
	}
	int ans = check_error(a, b, 0);
	return ans;
}