#include "user_database.h"
Node **database_link_list;
void load_data_base(){
  database_link_list=create_link_list();
  FILE *fin=(FILE *)fopen("account.txt","r");
  if(fin==NULL){
    printf("Don't exist file contain database\n");
    exit(1);
    return ;
  }
  char name[MAXLENNAME],password[MAXLENPASSWORD];
  int status=-1;
  while(fscanf(fin,"%s%*c%s%*c%d%*c",name,password,&status),!feof(fin)){
    
    add_new_node(database_link_list,strdup(name),strdup(password),status);
    //printf("%s,%s,%d\n",name,password,status);
  }
  fclose(fin);
  // return database_link_list;
}
//export data from database_link_list to account.txt
void export_to_data_file(){
  Node *top=(*database_link_list);
  FILE* fout=fopen("account.txt","w");
  while(top!=NULL){
    fprintf(fout,"%s %s %d\n",top->name,top->password,top->status);
    top=top->next;
  }
  fclose(fout);
}
//check all character in string s is number or not 

// find user info in database
//user: [out]point to save the user info
//name: [in] user id
//return value: -1 if not found user id,-2 if account is block,-3 if account is already sign-in in at least one process
int search_userid(Node**user,char *name){

  Node *searchresult=search(database_link_list,name);
  if (searchresult==NULL){
    *user=NULL;
    return -1;
  }
  else if (searchresult->status==0){
    *user=NULL;
      return -2;
  }
  // check if account is already sign-in yet or not

  if (searchresult!=NULL && searchresult->islogin==1){
    *user=NULL;
    return -3;
  }
  *user=searchresult;
}