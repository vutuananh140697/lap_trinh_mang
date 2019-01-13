#ifndef __ROOMLIST__
#define __ROOMLIST__
#include <stdio.h>
#include <stdlib.h>
#include "linklist.h"
#include "queue.h"

typedef struct Room {
	int id;
char *username;
Queue *product_list;
int number_of_member;
struct Room * next;
} Room;

//create a link list
//return a new linklist
Room** create_room_list();
//create a new node 
//new node will have soluongdangnhapsai=0,islogin=0
//return new node
Room *creat_newroom(int id, char* username, Queue *product_list, int number_of_member);
// add new node to link list
//head: head of link list,name,password,status: info of newnode
Room *add_new_room(Room **head, int id, char *username, Queue *product_list, int number_of_member);
// bound function of add_new_room
// add a room node to a roomlist
Room *add_room(Room **header, Room *room);
//print all information of a room
void printRoom(Room *room);
//print value in a linklist
//use for debug
//param:head[IN]:head of link list
void print_all_room(Room **head);
// search that any node in link list have name
//param: head[IN]:head of link list, name[IN]:name of node want to find
//return : node that match name, return NULL if not found
Room *search_room(Room **head, int id);
//delete a node have name equal to paramater name
//param: head[IN,OUT]:head of link list,head may be change if the first node match the name, name[IN]:name that want to match
//return :void
void delete_room(Room **head, int id);
//free a list to releash memory
void free_room_list(Room **head);
// int send_room_list(int socket, Room *header, int room_length);
int count_room(Room *header);
#endif
