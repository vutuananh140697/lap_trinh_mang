#include "queue.h"


Queue *Init(Queue *Q)
{
    Q = (Queue *)malloc(sizeof(Queue));
    Q->Front = Q->Rear = NULL;
    Q->count = 0;
    return Q;
}
int Isempty (Queue *Q) //kiem tra Queue rong
{
    if (Q->count == 0) //so phan tu = 0 => rong
        return 1;
    return 0;
}
 
Item *MakeItem(int id, char *name, char *description, int price, int price_immediately){
  Item *item = (Item*)malloc(sizeof(Item));
  item->id = id;
  item->name = name;
  item->description = description;
  item->price = price;
  item->price_immediately = price_immediately;
  return item;
}

Qnode *MakeQnode(int id, char *name, char *description, int price, int price_immediately) //tao 1 Qnode
{
    Item *item = MakeItem(id, name, description, price, price_immediately);
    Qnode *P = (Qnode*)malloc(sizeof(Qnode));
    P->Next = NULL;
    P->item = item;
    return P;
}
 
void Push(Queue *Q, int id, char *name, char *description, int price, int price_immediately) //them phan tu vao cuoi Queue
{
    Qnode *P = MakeQnode(id, name, description, price, price_immediately); 
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

void PushItem(Queue *Q, Item *item) //them phan tu vao cuoi Queue
{
    Push(Q, item->id, item->name, item->description, item->price, item->price_immediately);
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
        {
            Q->Front = NULL;
            Q->Rear = NULL;
            Q->count = 0;
        }
        else{
            Q->Front = Q->Front->Next;
            Q->count --;
        }
        return item; //tra ve phan tu lay ra
    }
}

Item *searchItem(Queue Q, int id){
    Qnode *qnode = Q.Front;
    while(qnode != NULL){
        if(qnode->item->id == id) 
            return qnode->item;
        qnode = qnode->Next;
    }
    return NULL;
}

void deleteItem(Queue *Q, int id){
    Qnode *qnode = Q->Front;
    if(qnode == NULL) return;
    if(qnode->item->id == id){
        Q->Front = Q->Front->Next;
        free(qnode);
        Q->count --;
        return;
    }
    else{
        Qnode *tmp = qnode;
        while(tmp != NULL){
            tmp = qnode->Next;
            if(tmp == NULL) return;
            if(tmp->item->id == id){
                qnode->Next = tmp->Next;
                free(tmp);
                Q->count --;
                return;
            }
            qnode = qnode->Next;
        }
    }

}

void printItem(Item *item){
    printf("\t%d %s %s %d %d\n", item->id, item->name, item->description, item->price, item->price_immediately);
}

void Output(Queue Q)
{
    Qnode *qnode = Q.Front;
    while (qnode != NULL)
    {
        printItem(qnode->item);
        qnode = qnode->Next;
    }
}


int count_products(Queue *Q){
    return Q->count;
}
