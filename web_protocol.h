#ifndef __WEB_PROTOCOL__
#define __WEB_PROTOCOL__
#include "tcp.h"
#include "roomlist.h"
#include <string.h>
#include <stdlib.h>
#define MAXUSERID 200;
//state of protocol
enum Web_ProtocolState{
	web_not_authorzied =0,
	wbe_authorized = 1
};
typedef struct Web_Data{
	int user_id;
}Web_Data;
enum web_message_code{
	// user id case
	REQUEST_ROOM_LIST=10,
	RESPOND_ROOM_LIST=11,
	//mua dut
	REQUEST_BUY_NOW=20,
	RESPOND_BUY_NOW=21,
	//tao phong
	REQUEST_MAKE_ROOM=30,
	RESPOND_MAKE_ROOM=31,
	// room detai
	REQUEST_ROOM_DETAIL=41,
	RESPOND_ROOM_DETAIL=41,
	// my room list
	REQUEST_MY_ROOM_LIST=50,
	RESPOND_MY_ROOM_LIST=51,

	//unknow
	LOGIC_NOTFOUND=-1
};

typedef struct message{
	enum web_message_code code;
	int data_len;
	void* data;
} web_message;
//============== ROOM LIST =============//
typedef struct ROOM_LIST_PARAM
{
	int page_id;
}ROOM_LIST_PARAM;
typedef struct ROOM_LIST_RESPOND
{
	int *room_ids;
	char **name_of_makers;
	char **descriptions;
}ROOM_LIST_RESPOND;
//============== BUY NOW =============//
typedef struct BUY_NOW_PARAM
{
	int id;
}BUY_NOW_PARAM;
typedef struct BUY_NOW_RESPOND
{
	char *message;
}BUY_NOW_RESPOND;

//============== MAKE_ROOM =============//
typedef struct MAKE_ROOM_PARAM
{
	Queue *product_list;
}MAKE_ROOM_PARAM;
typedef struct MAKE_ROOM_RESPOND
{
	char *message;
}MAKE_ROOM_RESPOND;

//============== ROOM_DETAIL =============//
typedef struct ROOM_DETAIL_PARAM
{
	int room_id;
}ROOM_DETAIL_PARAM;
typedef struct ROOM_DETAIL_RESPOND
{
	char *username;
	Queue *product_list;
}ROOM_DETAIL_RESPOND;

typedef ROOM_LIST_PARAM MY_ROOM_LIST_PARAM;
typedef ROOM_LIST_RESPOND MY_ROOM_LIST_RESPOND;
//core function
int send_web_message(int socket,web_message msg);
int receive_web_message(int socket,web_message *msg);

// send message function
int send_REQUEST_ROOM_LIST(int socket,ROOM_LIST_PARAM param);
int send_ROOM_LIST(int socket,ROOM_LIST_RESPOND data);
int receive_REQUEST_ROOM_LIST(int socket,ROOM_LIST_RESPOND *data);
// in all function socket input is socket to communicate,



int send_REQUEST_BUY_NOW(int socket,BUY_NOW_PARAM param);
int send_RESPOND_BUY_NOW(int socket,BUY_NOW_RESPOND data);
int receive_REQUEST_BUY_NOW(int socket,BUY_NOW_RESPOND *data);



int send_REQUEST_MAKE_ROOM(int socket,MAKE_ROOM_PARAM param);
int send_RESPOND_MAKE_ROOM(int socket,MAKE_ROOM_RESPOND data);
int receive_REQUEST_MAKE_ROOM(int socket,MAKE_ROOM_RESPOND *data);


int send_REQUEST_ROOM_DETAIL(int socket,ROOM_DETAIL_PARAM param);
int send_RESPOND_ROOM_DETAIL(int socket,ROOM_DETAIL_RESPOND data);
int receive_REQUEST_ROOM_DETAIL(int socket,ROOM_DETAIL_RESPOND *data);


int send_REQUEST_MY_ROOM_LIST(int socket,MY_ROOM_LIST_PARAM param);
int send_MY_ROOM_LIST(int socket,MY_ROOM_LIST_RESPOND data);
int receive_REQUEST_MY_ROOM_LIST(int socket,MY_ROOM_LIST_RESPOND *data);

#endif