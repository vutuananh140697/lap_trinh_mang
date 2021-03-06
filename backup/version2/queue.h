#ifndef __QUEUE__
#define __QUEUE__
#include <stdio.h>
#include<stdlib.h>
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

void Init (Queue *Q); //khoi tao Queue rong
int Isempty(Queue *Q); //kiem tra Queue rong
Item *MakeItem(char *name, char *description, int price, int price_immediately);
Qnode *MakeQnode(char *name, char *description, int price, int price_immediately);
void Push(Queue *Q, char *name, char *description, int price, int price_immediately); //them phan tu vao cuoi hang doi
Item *Pop(Queue *Q); //Loai bo phan tu khoi dau hang doi
void Output(Queue Q); //Xuat 

int count_prducts(Queue *Q);

#endif
