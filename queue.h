#ifndef __QUEUE__
#define __QUEUE__
#include <stdio.h>
#include<stdlib.h>
#include <string.h>
typedef struct Item
{
	int id;
	char *name;
	char *description;
	int price;
	int price_immediately;
} Item;
typedef struct Qnode
{
	struct Qnode *Next;
	Item *item;
} Qnode;

typedef struct Queue
{
    Qnode * Front, *Rear; 	
    int count; 			
} Queue;

Queue *Init (Queue *Q); //khoi tao Queue rong
int Isempty(Queue *Q); //kiem tra Queue rong
Item *MakeItem(int id, char *name, char *description, int price, int price_immediately);
Qnode *MakeQnode(int id, char *name, char *description, int price, int price_immediately);
void Push(Queue *Q, int id, char *name, char *description, int price, int price_immediately); //them phan tu vao cuoi hang doi
void PushItem(Queue *Q, Item *item);
Item *Pop(Queue *Q); //Loai bo phan tu khoi dau hang doi
Item *searchItem(Queue Q, int id);
void printItem(Item *item);
void Output(Queue Q); //Xuat 

int count_products(Queue *Q);

#endif
