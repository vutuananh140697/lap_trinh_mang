#ifndef __WEB_PROTOCOL__
#define __WEB_PROTOCOL__
#include "tcp.h"
#include "queue.h"
#include "roomlist.h"
#include <string.h>
#include <stdlib.h>
#define MAXUSERID 200;
//state of protocol
enum Web_ProtocolState{
	web_not_authorzied =0,
	web_authorized = 1
};
typedef struct Web_Data{
	Node *user;
	Room* room;
}Web_Data;
#define USERNAME 1024
#define CLIENT_MAX 1024
#define ITEM_NAME 1024
#define ITEM_DESCRIPTION 1024
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
	REQUEST_ROOM_DETAIL=40,
	RESPOND_ROOM_DETAIL=41,
	// my room list
	REQUEST_MY_ROOM_LIST=50,
	RESPOND_MY_ROOM_LIST=51,
	// enter room
	REQUEST_ENTER_ROOM=60,
	RESPOND_ENTER_ROOM=61,
	// log out
	REQUEST_LOG_OUT=70,
	RESPOND_LOG_OUT=71,
	//unknow
	LOGIC_NOTFOUND=-1
};


typedef struct web_message{
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
	Room *header;
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
	int result;
	Room *room;
}ROOM_DETAIL_RESPOND;

typedef ROOM_LIST_PARAM MY_ROOM_LIST_PARAM;
typedef ROOM_LIST_RESPOND MY_ROOM_LIST_RESPOND;
typedef struct ENTER_ROOM_PARAM
{
	int room_id;
}ENTER_ROOM_PARAM;
typedef struct ENTER_ROOM_RESPOND
{
	int result;
	Room *room;
}ENTER_ROOM_RESPOND;
//core function
int send_a_item(int socket, Item *item);
int recv_a_item(int socket, Item *item);

// int send_web_message(int socket,web_message msg);
// int receive_web_message(int socket,web_message *msg);
int send_a_queue(int socket, Queue *Q);
int recv_a_queue(int socket, Queue *Q);
int send_a_room(int socket, Room *room);
int recv_a_room(int socket, Room *room);
int send_room_list(int socket, Room *header);
int recv_room_list(int socket, Room **header);
// check error
int check_error(int a, int b, int c); 

// send message function
int send_REQUEST_ROOM_LIST(int socket,ROOM_LIST_PARAM param);
int receive_REQUEST_ROOM_LIST(int socket, web_message *message);
int send_RESPOND_ROOM_LIST(int socket,ROOM_LIST_RESPOND data);
int receive_RESPOND_ROOM_LIST(int socket, web_message *message);

int send_REQUEST_BUY_NOW(int socket,BUY_NOW_PARAM param);
int receive_REQUEST_BUY_NOW(int socket, web_message *message);
int send_RESPOND_BUY_NOW(int socket,BUY_NOW_RESPOND data);
int receive_RESPOND_BUY_NOW(int socket, web_message *message);

int send_REQUEST_MAKE_ROOM(int socket,MAKE_ROOM_PARAM param);
int receive_REQUEST_MAKE_ROOM(int socket, web_message *message);
int send_RESPOND_MAKE_ROOM(int socket,MAKE_ROOM_RESPOND data);
int receive_RESPOND_MAKE_ROOM(int socket, web_message *message);

int send_REQUEST_ROOM_DETAIL(int socket,ROOM_DETAIL_PARAM param);
int receive_REQUEST_ROOM_DETAIL(int socket, web_message *message);
int send_RESPOND_ROOM_DETAIL(int socket,ROOM_DETAIL_RESPOND data);
int receive_RESPOND_ROOM_DETAIL(int socket, web_message *message);

int send_REQUEST_MY_ROOM_LIST(int socket,MY_ROOM_LIST_PARAM param);
int receive_REQUEST_MY_ROOM_LIST(int socket, web_message *message);
int send_RESPOND_MY_ROOM_LIST(int socket,MY_ROOM_LIST_RESPOND data);
int receive_RESPOND_MY_ROOM_LIST(int socket, web_message *message);

int send_REQUEST_ENTER_ROOM(int socket,ENTER_ROOM_PARAM param);
int receive_REQUEST_ENTER_ROOM(int socket, web_message *message);
int send_RESPOND_ENTER_ROOM(int socket,ENTER_ROOM_RESPOND data);
int receive_RESPOND_ENTER_ROOM(int socket, web_message *message);

int send_LOG_OUT_REQUEST(int socket);
int send_LOG_OUT_RESPOND(int socket);

int receive_web_message(int socket, web_message *message);

#endif