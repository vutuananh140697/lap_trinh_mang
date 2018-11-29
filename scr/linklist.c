#include "linklist.h"
#include <string.h>
//create a link list
//return a new linklist
Node ** create_link_list(){
  Node **ans=(Node **)malloc(sizeof(Node *));
  (*ans)=NULL;
  return ans;
}
//create a new node 
//new node will have soluongdangnhapsai=0,islogin=0
//return new node
Node *creat_newnode(char *name,char *password,int status){
  Node *newnode=(Node *)malloc(sizeof(Node));
  newnode->name=name;
  newnode->password=password;
  newnode->status=status;
  newnode->soluongdangnhapsai=0;
  newnode->islogin=0;
  newnode->next=NULL;
  return newnode;
}
// add new node to link list
//head: head of link list,name,password,status: info of newnode
Node* add_new_node(Node **head,char *name,char *password,int status){
  Node *newnode=creat_newnode(name,password,status);
  if ((*head)==NULL){
    (*head)=newnode;
  }
  else{
    Node *top=(*head);
    while((top->next)!=NULL){
      top=top->next;
    }
    top->next=newnode;
  }
   return newnode;

}
//print value in a linklist
//use for debug
//param:head[IN]:head of link list
void print_all_node(Node **head){

  Node *top=(*head);
  while(top!=NULL){
    printf("name %s,password %s,status %d,\n",top->name,top->password,top->status);
    top=top->next;
  }
}
// search that any node in link list have name
//param: head[IN]:head of link list, name[IN]:name of node want to find
//return : node that match name, return NULL if not found
Node *search(Node **head,char *name){
 Node *top=(*head);
  if(top==NULL)
    return NULL;
  else{
    while(top!=NULL){
      if(strcmp(top->name,name)==0){
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
void delete(Node **head,char *name){
  Node *top=*head;
  if(strcmp(top->name,name)==0){
    (*head)=(*head)->next;
     free(top);
    return;
  }
  else{
    Node *next;
    while(next!=NULL){
      next=top->next;
      if(strcmp(next->name,name)==0){
	top->next=next->next;
	free(next);
	return ;
      }
      top=top->next;
    }
  }
}
//free a list to releash memory
void free_list(Node **head){
  Node *top=(*head);
  Node *temp=top;
  while(top!=NULL)
    {
      temp=top->next;
      free(top);
      top=temp;
    }
  (*head)=NULL;
}

//block a count
//param: head[IN]:head of link list,name[IN]:name of account that want to block
void block_account(Node **head,char *name)
{
  Node *top=(*head);
  while(top!=NULL){
    if(strcmp(top->name,name)==0){
      top->status=0;
    }
    top=top->next;
  }
}
// check if a list have at least one sign-in account
int have_signed_in_account(Node **head){
	Node *top=*head;
	if(top==NULL)
		return 0;
	while(top!=NULL){
		if(top->islogin==1)
			return 1;
		top=top->next;
	}
	return 0;
}
