#ifndef __AUCTION_PROTOCOL__
#define __AUCTION_PROTOCOL__
#include "tcp.h"
#include "roomlist.h"
#include <string.h>
#include <stdlib.h>
#define MAXUSERID 200;
#define TIMEINTEVAL 30000;
#define NUMBER_NOTIFY 3;
//state of protocol
enum Auction_ProtocolState{
	no_set_price_or_passed =0,// this user still not set price or his price is smaller than some one
	first_price =1,// receive user id and waiting for password
	final_one_temp_price =2,
	final_two_temp_price =3,
	final_three_temp_price =4,
	final_set_price =5,
};
typedef struct Auction_Data{
	int user_id;
	Room *room;
} Auction_Data;

enum auction_message_code{
	//auction
	REQUEST_SET_PRICE=41,
	RESPOND_SET_PRICE=42,


	//notify:
	NOTIFY_NEW_PRICE=43,
	//enter room 
	// NOTIFY_ENTER_ROOM=50
	// NOTIFY_NEXT_ITEM=51
	//Exit room
	REQUEST_EXIT_ROOM =60,
	RESPOND_EXIT_ROOM =61,

	//Buy now
	AUCTION_REQUEST_BUY_NOW=20,
	AUCTION_RESPOND_BUY_NOW=21,
	AUCTION_RESPOND_BUY_NOW_NOTIFY=22,
	


	
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
//============== ROOM LIST =============//
typedef struct SET_PRICE_PARAM
{
	int price;
}SET_PRICE_PARAM;

typedef struct SET_PRICE_RESPOND
{
	char *message;
}SET_PRICE_RESPOND;

//============== NOTIFY_NEW =============//

typedef struct NOTIFY_NEW_PRICE_RESPOND
{
	int newprice;
	char *winner_name;
	time_t start;
	int count;
}NOTIFY_NEW_PRICE_RESPOND;
//============== REQUEST_EXIT =============//
typedef struct REQUEST_EXIT_PARAM
{
}REQUEST_EXIT_PARAM;
typedef struct EXIT_ROOM_RESPOND
{
	char *message;
}EXIT_ROOM_RESPOND;

//============== BUY_NOW =============//
typedef struct AUCTION_BUY_NOW_PARAM
{
	int item_id;
	int price;
}AUCTION_BUY_NOW_PARAM;
typedef struct AUCTION_BUY_NOW_RESPOND
{
	int item_id;
	char *message;
}AUCTION_BUY_NOW_RESPOND;
typedef struct AUCTION_BUY_NOW_NOTIFY_RESPOND
{
	int item_id;
	char *winner_name;
}AUCTION_BUY_NOW_NOTIFY_RESPOND;

int send_REQUEST_SET_PRICE(int socket,SET_PRICE_PARAM data);
int send_RESPOND_SET_PRICE(int socket,SET_PRICE_RESPOND data);

int send_NOTIFY_NEW_PRICE(int socket,NOTIFY_NEW_PRICE_RESPOND data);

int send_REQUEST_EXIT_ROOM(int socket,REQUEST_EXIT_PARAM data);
int send_RESPOND_EXIT_ROOM(int socket,EXIT_ROOM_RESPOND data);

int send_AUCTION_REQUEST_BUY_NOW(int socket,AUCTION_BUY_NOW_PARAM data);
int send_AUCTION_RESPOND_BUY_NOW(int socket,AUCTION_BUY_NOW_RESPOND data);
int send_AUCTION_RESPOND_BUY_NOW_NOTIFY(int socket,AUCTION_BUY_NOW_NOTIFY_RESPOND data);
int send_AUCTION_UNKNOWN(int socket);
#endif