
#include "tcp.h"
//print bit-format of a buffer
//tool to debug
void printBits(size_t const size, void const * const ptr)
{
  unsigned char *b = (unsigned char*) ptr;
  unsigned char byte;
  int i, j;

  for (i=size-1;i>=0;i--)
    {
      for (j=7;j>=0;j--)
        {
	  byte = (b[i] >> j) & 1;
	  printf("%u", byte);
        }
    }
  puts("");
}
//receive all bytes from buff with out any loss
int recv_all_byte(int socket,void *buff,int byte_size){
  int total_bytes=0,bytes_received=0;
  while(1){
    bytes_received = recv(socket, buff+total_bytes,byte_size-total_bytes, 0);
    if (bytes_received <= 0){
      return -1;
    }
    else{
      total_bytes+=bytes_received;
      if(total_bytes==byte_size){
	     return 0;
      }
    }
  }//end conversation
}
// receive a integer
int recv_a_int(int socket,int *result){
  int value=0;
  char buff_temp[SOCKET_MAXIMUM_BUFFER];
  if(recv_all_byte(socket,buff_temp,sizeof(int))!=0)
    {
      return -1;
    }

  memcpy(&value,buff_temp,sizeof(int));
  *result=value>>0;
  return 0;
}
// receive a string from socket
int recv_a_string(int socket,char buff[]){
  int len=0;
  if(recv_a_int(socket,&len)==0){
    if(recv_all_byte(socket,buff,len)==0){
    	return 0;
    }else{
    	return -1;
    }
  }
  return -1;
		
}
int recv_a_time_t(int socket,time_t *result){
  time_t value=0;
  char buff_temp[SOCKET_MAXIMUM_BUFFER];
  if(recv_all_byte(socket,buff_temp,sizeof(time_t))!=0)
    {
      return -1;
    }

  memcpy(&value,buff_temp,sizeof(time_t));
  *result=value>>0;
  return 0;
}
// receive a file from socket
int recv_a_file(int socket,char *path){

char path_copy[SOCKET_MAXIMUM_BUFFER];
strcpy(path_copy,path);
  char filename[SOCKET_MAXIMUM_BUFFER];
  int filesize=0;
  //get file name
  if(recv_a_string(socket,filename)!=0){
  	return -1;}
  if(strlen(filename)==0){
  	return -3;
  }
  // get file size:

  if(recv_a_int(socket,&filesize)!=0){
    //loi
    return -1;
  }
  //check if file is exist
  char fullpath[500]="";
  strcat(fullpath,path_copy);
  strcat(fullpath,"/");
  strcat(fullpath,filename);
  if(check_if_a_file_exist(fullpath)==1){
    send_a_string(socket,FILE_EXIST_ERROR);
    return -1;
  }
  
  //receipt file

  send_a_string(socket,OK);
 
  FILE *output_file=(FILE *)fopen(fullpath,"w");
  char buff[SOCKET_MAXIMUM_BUFFER];

  int value=0;
  int total_receipt=0;
  while(1){
    if(recv_a_int(socket,&value)!=0){
      return -1;
    }
    recv_all_byte(socket,buff,value);
    fwrite(buff,1,value,output_file);
    total_receipt+=value;
    if(total_receipt==filesize){
      break;
    }
  }
  fclose(output_file);
  return 0;
}
//send all byte in buff without loss any data
int  send_all_byte(int socket,void *buff,int number_byte_to_send){
  int total_sent_byte=0,bytes_sent=0;
  while(1){
    bytes_sent = send(socket, buff+total_sent_byte, number_byte_to_send - total_sent_byte, 0);
    if(bytes_sent <0){
     return -1;
    }
    else{
      total_sent_byte+=bytes_sent;
      if(total_sent_byte==number_byte_to_send){
		return 0;
      }
    }
  }
	
}
//send a interger number
int send_a_int(int socket,int value){
  char buff[sizeof(int)];
  memcpy(buff,&value,sizeof(int));
  return send_all_byte(socket,buff,sizeof(int));

}
// int send_a_time_t(int socket,time_t value){
//   char buff[sizeof(time_t)];
//   memcpy(buff,&value,sizeof(time_t));
//   return send_all_byte(socket,buff,sizeof(time_t));
// }
// send a string to socket
int  send_a_string(int socket,char *str){
  if(send_a_int(socket,strlen(str)+1)==0){
	  return send_all_byte(socket,str,strlen(str)+1);
	}
	return -1;
}
// to send a file with a socket, filename is file name path
int send_a_file(int socket,char *filename){
	
  int read_from_file_bytes=0;
  int sz;
  char buff[SOCKET_MAXIMUM_BUFFER];

  //send file name
  if(send_a_string(socket,filename)!=0){
  	return -1;}
  // open file
  FILE *file_input=(FILE *)fopen(filename,"rb");
  fseek(file_input, 0L, SEEK_END);
  sz = ftell(file_input);
  fseek(file_input, 0L, SEEK_SET);
  //send file size
  if(send_a_int(socket,sz)!=0){
  	return -1;}

  char message[SOCKET_MAXIMUM_BUFFER];
  if(recv_a_string(socket,message)!=0){
  	return -1;}
  	// receive message that file is exist in server or not

  if(strcmp(message,OK)!=0){
    fclose(file_input);
    return -2;
  }
  //read and send file content
  while(read_from_file_bytes=fread(buff,1,3,file_input),1){
    if(send_a_int(socket,read_from_file_bytes)!=0)
    	return -1;
    if(send_all_byte(socket,buff,read_from_file_bytes)!=0)
    	return -1;
    // printf("%d - %s\n", read_from_file_bytes,buff);
    if(feof(file_input)){
      break;
    }
  }
  fclose(file_input);
  return 0;

}
//check file is exist or not
// return 1 if exist 0 if not
int check_if_a_file_exist(char *filename){
  FILE *a=(FILE *)fopen(filename,"r");
  if(a==NULL){
    return 0;
  }
  fclose(a);
  return 1;

}
