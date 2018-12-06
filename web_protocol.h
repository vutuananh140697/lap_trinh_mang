#ifndef __LOGIN_PROTOCOL__
#define __LOGIN_PROTOCOL__
#include "tcp.h"
#include <string.h>
#include <stdlib.h>
#define MAXUSERID 200;
//state of protocol
enum ProtocolState{
	still_not_authorzied =0,
	authorized = 1
};
enum message_code{
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
	NOTFOUND=-1
};

typedef struct message{
	enum message_code code;
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
}BUY_NOW_PARAM;
typedef struct BUY_NOW_RESPOND
{
}BUY_NOW_RESPOND;

//============== MAKE_ROOM =============//
typedef struct MAKE_ROOM_PARAM
{
}MAKE_ROOM_PARAM;
typedef struct MAKE_ROOM_RESPOND
{
}MAKE_ROOM_RESPOND;

//============== MAKE_ROOM =============//
typedef struct ROOM_DETAIL_PARAM
{
}ROOM_DETAIL_PARAM;
typedef struct ROOM_DETAIL_RESPOND
{
}ROOM_DETAIL_RESPOND;

//core function
int send_web_message(int socket,web_message msg);
int receive_web_message(int socket,web_message *msg);

// send message function
int send_REQUEST_ROOM_LIST(int socket,ROOM_LIST_PARAM param);
int send_ROOM_LIST(int socket,ROOM_LIST_RESPOND data);
int receive_REQUEST_ROOM_LIST(int socket,ROOM_LIST_RESPOND *data);
// in all function socket input is socket to communicate,



int send_REQUEST_BUY_NOW(int socket,ROOM_LIST_PARAM param);
int send_RESPOND_BUY_NOW(int socket,ROOM_LIST_RESPOND data);
int receive_REQUEST_BUY_NOW(int socket,ROOM_LIST_RESPOND *data);



int send_REQUEST_MAKE_ROOM(int socket,ROOM_LIST_PARAM param);
int send_RESPOND_MAKE_ROOM(int socket,ROOM_LIST_RESPOND data);
int receive_REQUEST_MAKE_ROOM(int socket,ROOM_LIST_RESPOND *data);


int send_REQUEST_ROOM_DETAIL(int socket,ROOM_LIST_PARAM param);
int send_RESPOND_ROOM_DETAIL(int socket,ROOM_LIST_RESPOND data);
int receive_REQUEST_ROOM_DETAIL(int socket,ROOM_LIST_RESPOND *data);


int send_REQUEST_MY_ROOM_LIST(int socket,ROOM_LIST_PARAM param);
int send_MY_ROOM_LIST(int socket,ROOM_LIST_RESPOND data);
int receive_REQUEST_MY_ROOM_LIST(int socket,ROOM_LIST_RESPOND *data);
#endif