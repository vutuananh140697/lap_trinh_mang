#ifndef __LINKLIST__
#define __LINKLIST__
#include <stdio.h>
#include<stdlib.h>
typedef struct Node {
struct Node *next;
char *name;
char *password;
int status;
int soluongdangnhapsai;
int islogin;
} Node;



//create a link list
//return a new linklist
Node ** create_link_list();
//create a new node 
//new node will have soluongdangnhapsai=0,islogin=0
//return new node
Node *creat_newnode(char *name,char *password,int status);
// add new node to link list
//head: head of link list,name,password,status: info of newnode
Node* add_new_node(Node **head,char *name,char *password,int status);
//print value in a linklist
//use for debug
//param:head[IN]:head of link list
void print_all_node(Node **head);
Node *newnode(char *name,char *password,int status);
// search that any node in link list have name
//param: head[IN]:head of link list, name[IN]:name of node want to find
//return : node that match name, return NULL if not found
Node *search(Node **head,char *name);
//delete a node have name equal to paramater name
//param: head[IN,OUT]:head of link list,head may be change if the first node match the name, name[IN]:name that want to match
//return :void
void delete(Node **head,char *name);
//free a list to releash memory
void free_list(Node **head);

//block a count
//param: head[IN]:head of link list,name[IN]:name of account that want to block
void block_account(Node **head,char *name);
// check if a list have at least one sign-in account
int have_signed_in_account(Node **head);
#endif
