#include "roomlist.h"
#include <string.h>
//create a link list
//return a new linklist
Room ** create_room_list(){
  Room**ans=(Room **)malloc(sizeof(Room *));
  (*ans)=NULL;
  return ans;
}
//create a new node 
//new node will have soluongdangnhapsai=0,islogin=0
//return new node
Room *creat_newroom(int id, char *username, Queue *product_list, int number_of_member){
  Room *newroom=(Room *)malloc(sizeof(Room));
  newroom->id = id;
  newroom->username = username;
  newroom->product_list = product_list;
  newroom->number_of_member = number_of_member;
  newroom->next=NULL;
  return newroom;
}
// add new node to link list
//head: head of link list,name,password,status: info of newroom
Room* add_new_room(Room **head, int id, char *username, Queue *product_list, int number_of_member){
  Room *newroom=creat_newroom(id, username, product_list, number_of_member);
  if ((*head)==NULL){
    (*head)=newroom;
  }
  else{
    Room *top=(*head);
    while((top->next)!=NULL){
      top=top->next;
    }
    top->next=newroom;
  }
   return newroom;

}
Room *add_room(Room **header, Room *room){
  printRoom(room);
  Room * ans=add_new_room(header, room->id, room->username, room->product_list, room->number_of_member);
  return ans;
}

// print all information of a room
void printRoom(Room *room){
  if(room == NULL) return;
  printf("\nRoom ID:%d\ncreater:%s\nparticipant:%d\n", room->id, room->username, room->number_of_member);
  printf("List of item: \n");
  Output(*(room->product_list));
}
//print value in a linklist
//use for debug
//param:head[IN]:head of link list
void print_all_room(Room **head){

  Room *top=(*head);
  while(top!=NULL){
    printRoom(top);
    top=top->next;
  }
}
// search that any node in link list have name
//param: head[IN]:head of link list, name[IN]:name of node want to find
//return : node that match name, return NULL if not found
Room *search_room(Room **head, int id){
 Room *top=(*head);
  if(top==NULL)
    return NULL;
  else{
    while(top!=NULL){
      if(top->id == id){
	return top;
      }
      top=top->next;
    }
    return NULL;
  }
}
//delete a node have name equal to paramater name
//param: head[IN,OUT]:head of link list,head may be change if the first node match the name, name[IN]:name that want to match
//return :void
void delete_room(Room **head, int id){
  Room *top=*head;
  if(top == NULL) return;
  if(top->id == id){
    (*head)=(*head)->next;
     free(top);
    return;
  }
  else{
    Room *next = top;
    while(next!=NULL){
      next=top->next;
      if(next == NULL) return;
      if(next->id == id){
	top->next=next->next;
	free(next);
	return ;
      }
      top=top->next;
    }
  }
}
//free a list to releash memory
void free_room_list(Room **head){
  Room *top=(*head);
  Room *temp=top;
  while(top!=NULL)
    {
      temp=top->next;
      free(top);
      top=temp;
    }
  (*head)=NULL;
}

int count_room(Room *header) {
  int count = 0;
  while(header != NULL){
    count++;
    header = header->next;
  }
  return count;
}
