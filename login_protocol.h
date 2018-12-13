#ifndef __LOGIN_PROTOCOL__
#define __LOGIN_PROTOCOL__
#include "tcp.h"
#include "linklist.h"
#include <string.h>
#include <stdlib.h>
#define MAXUSERID 200;
//state of protocol
enum Login_ProtocolState{
	no_connect,//still not receive user id
	correct_id,// receive user id and waiting for password
	authorized //receive right password
};
typedef struct Login_Data{
	int number_wrong_password;
	Node *user;
} Login_Data;
enum login_message_code{
	// user id case
	USERID=10,
	USERID_NOTFOUND=11,
	USERID_FOUND=12,
	USERID_BLOCK=13,
	USERID_ISSIGNIN=14, // implement to extend in future
	//password case
	PASSWORD=20,
	PASSWORD_RIGHT=21,
	PASSWORD_WRONG=22,
	PASSWORD_BLOCK=23,
	//logout
	LOGOUT=30,
	LOGOUT_SUCCESS=31,
	LOGOUT_UNSUCCESS=32,
	//reset
	RESET=40,
	//unknow
	LOGIC_UNKNOWN=-1
};

typedef struct login_message{
	enum login_message_code code;
	int data_len;
	void* data;
} login_message;
//core function
int send_message(int socket,login_message msg);
int receive_message(int socket,login_message *msg);

// send message function
// in all function socket input is socket to communicate,



//send userID message 
//param:userid[IN]:string contain user id
int send_USERID(int socket,char userid[]);
int send_USERID_NOTFOUND(int socket);
int send_USERID_FOUND(int socket);
int send_USERID_BLOCK(int socket);
int send_USERID_ISSIGNIN(int socket);

//send PASSWORD message 
//param:userid[IN]:string contain user id
int send_PASSWORD(int socket,char password[]);
int send_PASSWORD_RIGHT(int socket);
int send_PASSWORD_WRONG(int socket);
int send_PASSWORD_BLOCK(int socket);

int send_LOGOUT(int socket);
int send_LOGOUT_SUCCESS(int socket);
int send_LOGOUT_UNSUCCESS(int socket);

//RESET
int send_RESET(int socket);
//param:clientmessage[IN]:customize message to sent
int send_UNKNOWN(int socket,login_message clientmessage);




#endif