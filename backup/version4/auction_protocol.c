#include"auction_protocol.h"
int check_error_auction(int a, int b, int c){
	if(a==-1 || b==-1 ||c==-1){
		// printf("%d %d %d\n",a,b,c );
		return -1;
	}
	return 0;
}
int receive_REQUEST_SET_PRICE(int socket,auction_message *msg)
{
	SET_PRICE_PARAM *param=(SET_PRICE_PARAM*)malloc(sizeof(SET_PRICE_PARAM));
	int a= recv_a_int(socket,&(param->price));
	// printf("price %d\n",param->price);
	(*msg).data=param;
	return check_error_auction(a,0,0);
}
int receive_RESPOND_SET_PRICE(int socket,auction_message *msg){
	SET_PRICE_RESPOND param;
	param.message=(char *)malloc(sizeof(char)*1000);
	param.message[0]='\0';
	int a= recv_a_string(socket,(param.message));
	msg->data=&param;
	return check_error_auction(a,0,0);
}
int receive_NOTIFY_NEW_PRICE(int socket,auction_message *msg){
	NOTIFY_NEW_PRICE_RESPOND param;
	param.winner_name=(char *)malloc(sizeof(char)*1000);
	param.winner_name[0]='\0';
	int a= recv_a_int(socket,&(param.newprice));
	int b= recv_a_string(socket,(param.winner_name));
	// int c= recv_a_time_t(socket,&(param.start));
	int c=0;
	int d= recv_a_int(socket,&(param.count));
	msg->data=&param;
	return check_error_auction(a,b,check_error_auction(c,d,0));
}
int receive_NOTIFY_SUCCESS_ONE(int socket,auction_message *msg){
	NOTIFY_SUCCESS_ONE_RESPOND param;
	param.message=(char *)malloc(sizeof(char)*1000);
	param.message[0]='\0';
	int a= recv_a_string(socket,(param.message));
	msg->data=&param;
	return check_error_auction(a,0,0);
}
int receive_NOTIFY_SUCCESS_TWO(int socket,auction_message *msg){
	NOTIFY_SUCCESS_TWO_RESPOND param;
	param.message=(char *)malloc(sizeof(char)*1000);
	param.message[0]='\0';
	int a= recv_a_string(socket,(param.message));
	msg->data=&param;
	return check_error_auction(a,0,0);
}
int receive_NOTIFY_SUCCESS_THREE(int socket,auction_message *msg){
	NOTIFY_SUCCESS_THREE_RESPOND param;
	param.message=(char *)malloc(sizeof(char)*1000);
	param.message[0]='\0';
	int a= recv_a_string(socket,(param.message));
	msg->data=&param;
	return check_error_auction(a,0,0);
}
int receive_NOTIFY_PHASE_ONE(int socket,auction_message *msg){
	NOTIFY_PHASE_ONE_RESPOND param;
	param.winner_name=(char *)malloc(sizeof(char)*1000);
	param.winner_name[0]='\0';
	int a= recv_a_int(socket,&(param.newprice));
	int b= recv_a_string(socket,(param.winner_name));
	msg->data=&param;
	return check_error_auction(a,b,0);
}
int receive_NOTIFY_PHASE_TWO(int socket,auction_message *msg){
	NOTIFY_PHASE_TWO_RESPOND param;
	param.winner_name=(char *)malloc(sizeof(char)*1000);
	param.winner_name[0]='\0';
	int a= recv_a_int(socket,&(param.newprice));
	int b= recv_a_string(socket,(param.winner_name));
	msg->data=&param;
	return check_error_auction(a,b,0);
}
int receive_NOTIFY_PHASE_THREE(int socket,auction_message *msg){
	NOTIFY_PHASE_THREE_RESPOND param;
	param.winner_name=(char *)malloc(sizeof(char)*1000);
	param.winner_name[0]='\0';
	int a= recv_a_int(socket,&(param.newprice));
	int b= recv_a_string(socket,(param.winner_name));
	msg->data=&param;
	return check_error_auction(a,b,0);
}
int receive_REQUEST_EXIT_ROOM(int socket,auction_message *msg){
	REQUEST_EXIT_PARAM param;
	msg->data=&param;
	return check_error_auction(0,0,0);
}
int receive_RESPOND_EXIT_ROOM(int socket,auction_message *msg){
	EXIT_ROOM_RESPOND param;
	param.message=(char *)malloc(sizeof(char)*1000);
	param.message[0]='\0';
	int a= recv_a_string(socket,(param.message));
	msg->data=&param;
	return check_error_auction(a,0,0);
}
int receive_AUCTION_REQUEST_BUY_NOW(int socket,auction_message *msg){
	AUCTION_BUY_NOW_PARAM param;
	int a= recv_a_int(socket,&(param.item_id));
	int b= recv_a_int(socket,&(param.price));
	msg->data=&param;
	return check_error_auction(a,b,0);
}
int receive_AUCTION_RESPOND_BUY_NOW(int socket,auction_message *msg){
	AUCTION_BUY_NOW_RESPOND param;
	param.message=(char *)malloc(sizeof(char)*1000);
	param.message[0]='\0';
	int a= recv_a_int(socket,&(param.item_id));
	int b= recv_a_string(socket,(param.message));
	msg->data=&param;
	return check_error_auction(a,b,0);
}
int receive_AUCTION_RESPOND_BUY_NOW_NOTIFY(int socket,auction_message *msg){
	AUCTION_BUY_NOW_NOTIFY_RESPOND param;
	param.winner_name=(char *)malloc(sizeof(char)*1000);
	param.winner_name[0]='\0';
	int a= recv_a_int(socket,&(param.item_id));
	int b= recv_a_string(socket,(param.winner_name));
	msg->data=&param;
	return check_error_auction(a,b,0);
}
int receive_AUCTION_UNKNOWN(int socket,auction_message *msg){
	return 0;
}

int receive_auction_message(int socket,auction_message *message){
	int a = recv_a_int(socket, &message->code);
	if(a!=0){
		return -1;
	}
	int b = -1;
	switch(message->code){
		case REQUEST_SET_PRICE:
			b = receive_REQUEST_SET_PRICE(socket, message);
			break;
		case RESPOND_SET_PRICE:
			b = receive_RESPOND_SET_PRICE(socket, message);
			break;
		case NOTIFY_NEW_PRICE:
			b = receive_NOTIFY_NEW_PRICE(socket, message);
			break;
		case NOTIFY_SUCCESS_ONE:
			b = receive_NOTIFY_SUCCESS_ONE(socket, message);
			break;
		case NOTIFY_SUCCESS_TWO:
			b = receive_NOTIFY_SUCCESS_TWO(socket, message);
			break;
		case NOTIFY_SUCCESS_THREE:
			b = receive_NOTIFY_SUCCESS_THREE(socket, message);
			break;
		case NOTIFY_PHASE_ONE:
			b = receive_NOTIFY_PHASE_ONE(socket, message);
			break;
		case NOTIFY_PHASE_TWO:
			b = receive_NOTIFY_PHASE_TWO(socket, message);
			break;
		case NOTIFY_PHASE_THREE:
			b = receive_NOTIFY_PHASE_THREE(socket, message);
			break;
		case REQUEST_EXIT_ROOM:
			b = receive_REQUEST_EXIT_ROOM(socket, message);
			break;
		case RESPOND_EXIT_ROOM:
			b = receive_RESPOND_EXIT_ROOM(socket, message);
			break;
		case AUCTION_REQUEST_BUY_NOW:
			b = receive_AUCTION_REQUEST_BUY_NOW(socket, message);
			break;

		case AUCTION_RESPOND_BUY_NOW:
			b = receive_AUCTION_RESPOND_BUY_NOW(socket, message);
			break;
		case AUCTION_RESPOND_BUY_NOW_NOTIFY:
			b = receive_AUCTION_RESPOND_BUY_NOW_NOTIFY(socket, message);
			break;
		case AUCTION_UNKNOWN:
			b = receive_AUCTION_UNKNOWN(socket, message);
			break;
		default:
			break;
	}
	int ans = check_error_auction(a, b, 0);
	return ans;

}
int send_REQUEST_SET_PRICE(int socket,SET_PRICE_PARAM param){
	int a= send_a_int(socket, REQUEST_SET_PRICE);
	int b=send_a_int(socket, param.price);
	return check_error_auction(a, b, 0);
}
int send_RESPOND_SET_PRICE(int socket,SET_PRICE_RESPOND data){
	int a= send_a_int(socket, RESPOND_SET_PRICE);
	int b = send_a_string(socket, data.message);
	return check_error_auction(a, b, 0);
}
int send_NOTIFY_NEW_PRICE(int socket,NOTIFY_NEW_PRICE_RESPOND data){
	int a= send_a_int(socket, NOTIFY_NEW_PRICE);
	int b= send_a_int(socket, data.newprice);
	int c=send_a_string(socket, data.winner_name);
	// int d= send_a_time_t(socket,data.start);
	int e=send_a_int(socket, data.count);
	return check_error_auction(a,b,check_error_auction(c,e,0));
}
int send_NOTIFY_SUCCESS_ONE(int socket,NOTIFY_SUCCESS_ONE_RESPOND data){
	int a= send_a_int(socket, NOTIFY_SUCCESS_ONE);
	int b= send_a_string(socket,data.message);
	int ans = check_error_auction(a, b, 0);
	return ans;
}
int send_NOTIFY_SUCCESS_TWO(int socket,NOTIFY_SUCCESS_TWO_RESPOND data){
	int a= send_a_int(socket, NOTIFY_SUCCESS_TWO);
	int b= send_a_string(socket,data.message);
	int ans = check_error_auction(a, b, 0);
	return ans;
}
int send_NOTIFY_SUCCESS_THREE(int socket,NOTIFY_SUCCESS_THREE_RESPOND data){
	int a= send_a_int(socket, NOTIFY_SUCCESS_THREE);
	int b= send_a_string(socket,data.message);
	int ans = check_error_auction(a, b, 0);
	return ans;
}
int send_NOTIFY_PHASE_ONE(int socket,NOTIFY_PHASE_ONE_RESPOND data){
	int a= send_a_int(socket, NOTIFY_PHASE_ONE);
	int b= send_a_int(socket, data.newprice);
	int c=send_a_string(socket, data.winner_name);
	return check_error_auction(a, b, c);
}
int send_NOTIFY_PHASE_TWO(int socket,NOTIFY_PHASE_TWO_RESPOND data){
	int a= send_a_int(socket, NOTIFY_PHASE_TWO);
	int b= send_a_int(socket, data.newprice);
	int c=send_a_string(socket, data.winner_name);

}
int send_NOTIFY_PHASE_THREE(int socket,NOTIFY_PHASE_THREE_RESPOND data){
	int a= send_a_int(socket, NOTIFY_PHASE_THREE);
	int b= send_a_int(socket, data.newprice);
	int c=send_a_string(socket, data.winner_name);
	return check_error_auction(a, b, c);
}
int send_REQUEST_EXIT_ROOM(int socket,REQUEST_EXIT_PARAM data){
	int a= send_a_int(socket, REQUEST_EXIT_ROOM);
	int ans = check_error_auction(a, 0, 0);
	return ans;
}
int send_RESPOND_EXIT_ROOM(int socket,EXIT_ROOM_RESPOND data){
	int a= send_a_int(socket, RESPOND_EXIT_ROOM);
	int b= send_a_string(socket,data.message);
	int ans = check_error_auction(a, b, 0);
	return ans;
}
int send_AUCTION_UNKNOWN(int socket){
	int a= send_a_int(socket, AUCTION_UNKNOWN);
	int ans = check_error_auction(a, 0, 0);
	return ans;
}


int send_AUCTION_REQUEST_BUY_NOW(int socket,AUCTION_BUY_NOW_PARAM data){
	int a= send_a_int(socket, AUCTION_REQUEST_BUY_NOW);
	int b= send_a_int(socket,data.item_id);
	int c= send_a_int(socket,data.price);
	int ans = check_error_auction(a, b, 0);
	return ans;
}
int send_AUCTION_RESPOND_BUY_NOW(int socket,AUCTION_BUY_NOW_RESPOND data){
	int a= send_a_int(socket, RESPOND_EXIT_ROOM);
	int b= send_a_int(socket,data.item_id);
	int c= send_a_string(socket,data.message);
	int ans = check_error_auction(a, b, 0);
	return ans;
}
int send_AUCTION_RESPOND_BUY_NOW_NOTIFY(int socket,AUCTION_BUY_NOW_NOTIFY_RESPOND data){
	int a= send_a_int(socket, RESPOND_EXIT_ROOM);
	int b= send_a_int(socket,data.item_id);
	int c= send_a_string(socket,data.winner_name);
	int ans = check_error_auction(a, b, 0);
	return ans;
}




