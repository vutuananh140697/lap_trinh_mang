#include "queue.h"
#include <string.h>

void Init(Queue *Q)
{
    Q->Front = Q->Rear = NULL;
    Q->count = 0;
}
int Isempty (Queue *Q) //kiem tra Queue rong
{
    if (Q->count == 0) //so phan tu = 0 => rong
        return 1;
    return 0;
}
 
Item *MakeItem(char *name, char *description, int price, int price_immediately){
  Item *item = (Item*)malloc(sizeof(Item));
  item->name = name;
  item->description = description;
  item->price = price;
  item->price_immediately = price_immediately;
  return item;
}

Qnode *MakeQnode(char *name, char *description, int price, int price_immediately) //tao 1 Qnode
{
    Item *item = MakeItem(name, description, price, price_immediately);
    Qnode *P = (Qnode*)malloc(sizeof(Qnode));
    P->Next = NULL;
    P->item = item;
    return P;
}
 
void Push(Queue *Q, char *name, char *description, int price, int price_immediately) //them phan tu vao cuoi Queue
{
    Qnode *P = MakeQnode(name, description, price, price_immediately); //Neu Q rong
    if (Isempty(Q))
    {
        Q->Front = Q->Rear = P; //dau va cuoi deu tro den P
    }
    else //Khong rong
    { 
        Q->Rear->Next = P;
        Q->Rear = P;
    }
    Q->count ++ ; //tang so phan tu len
}
 
Item *Pop(Queue *Q) //Loai bo phan tu khoi dau hang doi
{
    if (Isempty(Q)) 
    {
        return NULL;
    }
    else
    {
        Item *item = Q->Front->item;
        if (Q->count == 1) //neu co 1 phan tu
            Init(Q);
        else{
            Q->Front = Q->Front->Next;
            Q->count --;
        }
        return item; //tra ve phan tu lay ra
    }
}

void Output(Queue Q)
{
    Qnode *qnode = Q.Front;
    while (qnode != NULL)
    {
        printf("%s\n",qnode->item->name);
        qnode = qnode->Next;
    }
}
