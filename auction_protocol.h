#ifndef __AUCTION_PROTOCOL__
#define __AUCTION_PROTOCOL__
#include "tcp.h"
#include <string.h>
#include <stdlib.h>
#define MAXUSERID 200;
#define TIMEINTEVAL 30000;
#define NUMBER_NOTIFY 3;
//state of protocol
enum Auction_ProtocolState{
	no_set_price =0,//still not receive user id
	have_set_price =1,// receive user id and waiting for password
	final_temp_set_price =2,
	final_set_price =3,
};
typedef struct Auction_Data{
	int user_id;
	int room_id;
} Auction_Data;
enum auction_message_code{
	//auction
	REQUEST_NEW_PRICE=40,
	REQUEST_SET_PRICE=41,
	RESPOND_SET_PRICE=42,

	//enter room:
	
	//Exit room
	REQUEST_EXIT_ROOM =50,
	RESPOND_EXIT_ROOM =51,
	
	//unknow
	AUCTION_UNKNOWN=-1
};

typedef struct auction_message{
	enum auction_message_code code;
	int data_len;
	void* data;
} auction_message;
//core function
int send_auction_message(int socket,auction_message msg);
int receive_auction_message(int socket,auction_message *msg);

// send message function
// in all function socket input is socket to communicate,

int send_ENTER_ROOM();
int send_SET_PRICE();
#endif