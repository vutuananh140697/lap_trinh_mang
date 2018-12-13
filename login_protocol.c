#include "login_protocol.h"

int send_message(int socket, login_message msg){
	int a=send_a_int(socket,msg.code);
	int b=send_a_int(socket,msg.data_len);
	int c=send_all_byte(socket,msg.data,msg.data_len);
	if(a==-1||b==-1||c==-1)
		return -1;
	return 0;
}

int receive_message(int socket,login_message *msg){
	int temp;
	int a=recv_a_int(socket,&temp);
	msg->code=temp;

	int b=recv_a_int(socket,&(msg->data_len));
	msg->data=malloc(sizeof(msg->	data_len));
	
	int c=recv_all_byte(socket,msg->data,msg->data_len);
	if(a==-1 || b==-1 ||c==-1)
		return -1;
	return 0;
}

int send_USERID(int socket,char userid[]){
	login_message msg;
	msg.code=USERID;
	msg.data_len=strlen(userid)+1;
	msg.data=(char*)malloc((strlen(userid)+1)*sizeof(char));
	strcpy(msg.data,userid);
	return send_message(socket,msg);
}
int send_USERID_NOTFOUND(int socket){
	login_message msg;
	char data[]="not found user id";
	msg.code=USERID_NOTFOUND;
	msg.data_len=strlen(data)+1;
	msg.data=(char*)malloc((strlen(data)+1)*sizeof(char));
	strcpy(msg.data,data);
	return send_message(socket,msg);
}

int send_USERID_FOUND(int socket){
	login_message msg;
	char data[]="user id is found";
	msg.code=USERID_FOUND;
	msg.data_len=strlen(data)+1;
	msg.data=(char*)malloc((strlen(data)+1)*sizeof(char));
	strcpy(msg.data,data);
	return send_message(socket,msg);
}

int send_USERID_BLOCK(int socket){
	login_message msg;
	char data[]="user id is block";
	msg.code=USERID_BLOCK;
	msg.data_len=strlen(data)+1;
	msg.data=(char*)malloc((strlen(data)+1)*sizeof(char));
	strcpy(msg.data,data);
	return send_message(socket,msg);
}
int send_USERID_ISSIGNIN(int socket){
	login_message msg;
	char data[]="user id is aleardy signed in";
	msg.code=USERID_BLOCK;
	msg.data_len=strlen(data)+1;
	msg.data=(char*)malloc((strlen(data)+1)*sizeof(char));
	strcpy(msg.data,data);
	return send_message(socket,msg);
}


int send_PASSWORD(int socket,char password[]){
	login_message msg;
	msg.code=PASSWORD;
	msg.data_len=strlen(password)+1;
	msg.data=(char*)malloc((strlen(password)+1)*sizeof(char));
	strcpy(msg.data,password);
	return send_message(socket,msg);
}
int send_PASSWORD_RIGHT(int socket){
	login_message msg;
	char data[]="password is correct";
	msg.code=PASSWORD_RIGHT;
	msg.data_len=strlen(data)+1;
	msg.data=(char*)malloc((strlen(data)+1)*sizeof(char));
	strcpy(msg.data,data);
	return send_message(socket,msg);
}
int send_PASSWORD_WRONG(int socket){
	{
	login_message msg;
	char data[]="password is wrong";
	msg.code=PASSWORD_WRONG;
	msg.data_len=strlen(data)+1;
	msg.data=(char*)malloc((strlen(data)+1)*sizeof(char));
	strcpy(msg.data,data);
	return send_message(socket,msg);
}
}
int send_PASSWORD_BLOCK(int socket){
	login_message msg;
	char data[]="account is block";
	msg.code=PASSWORD_BLOCK;
	msg.data_len=strlen(data)+1;
	msg.data=(char*)malloc((strlen(data)+1)*sizeof(char));
	strcpy(msg.data,data);
	return send_message(socket,msg);
}


int send_LOGOUT(int socket){
	login_message msg;
	char data[]="logout";
	msg.code=LOGOUT;
	msg.data_len=strlen(data)+1;
	msg.data=(char*)malloc((strlen(data)+1)*sizeof(char));
	strcpy(msg.data,data);
	return send_message(socket,msg);
}

int send_LOGOUT_SUCCESS(int socket){
	login_message msg;
	char data[]="log out success";
	msg.code=LOGOUT_SUCCESS;
	msg.data_len=strlen(data)+1;
	msg.data=(char*)malloc((strlen(data)+1)*sizeof(char));
	strcpy(msg.data,data);
	return send_message(socket,msg);
}
int send_LOGOUT_UNSUCCESS(int socket){
	login_message msg;
	char data[]="log out un success";
	msg.code=LOGOUT_UNSUCCESS;
	msg.data_len=strlen(data)+1;
	msg.data=(char*)malloc((strlen(data)+1)*sizeof(char));
	strcpy(msg.data,data);
	return send_message(socket,msg);
}

int send_RESET(int socket){
	login_message msg;
	char data[]="reset information";
	msg.code=RESET;
	msg.data_len=strlen(data)+1;
	msg.data=(char*)malloc((strlen(data)+1)*sizeof(char));
	strcpy(msg.data,data);
	return send_message(socket,msg);
}
 void reverse(char s[])
 {
     int i, j;
     char c;

     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
}  
 void itoa(int n, char s[])
 {
     int i, sign;

     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
}  
int send_UNKNOWN(int socket,login_message clientmessage){
	login_message msg;
	char data[2000]="unknow message code:";
	char temp[200];
	itoa(clientmessage.code, temp);
	strcat(data,temp);
	msg.code=LOGIC_UNKNOWN;
	msg.data_len=strlen(data)+1;
	msg.data=(char*)malloc((strlen(data)+1)*sizeof(char));
	strcpy(msg.data,data);
	return send_message(socket,msg);
}