#ifndef __TCP__
#define __TCP__
#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define SOCKET_MAXIMUM_BUFFER 1024
#define FILE_EXIST_ERROR "Error File Exist:"
#define OK "OK"
#define INTERRUPT 1
void printBits(size_t const size, void const * const ptr);
int recv_all_byte(int socket,void *buff,int byte_size);

int recv_a_int(int socket,int *result);
int recv_a_string(int socket,char buff[]);

int recv_a_file(int socket,char *path);
int send_a_file(int socket,char *filename);
int send_all_byte(int socket,void *buff,int number_byte_to_send);
int send_a_int(int socket,int value);
int send_a_string(int socket,char *str);
int check_if_a_file_exist(char *filename);

int send_room_list(int socket, roomlist *header, int room_length);
int send_a_queue(int socket, Queue *item);
#endif