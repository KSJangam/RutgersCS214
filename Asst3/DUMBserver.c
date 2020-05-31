/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <unistd.h>

#include <stdlib.h>

#include <string.h>

#include <time.h>

pthread_mutex_t lock;
//make a struct to pass parameters to the thread function
struct myargs_t {
  int socket;
  // struct box** boxes;
  struct sockaddr_in cli_addr;  
};

//make our message box struct
struct message {
  struct message * next;
  char * message;
  int length;
};

struct box {
  struct message * head;
  struct message * tail;
  int size;
  char * name;
  int open;
};

void error(char * msg) {
  perror(msg);
  exit(1);
}
struct box* boxes;
void enqueue(struct message* m, struct box* b){
  // printf("beofre enqueue: %d\n",b->size);
  if(b->size==0){
    b->head=m;
    b->tail=m;
    b->size=b->size + 1;
  }
  else if(b->size>0){
    b->tail->next=m;
    b->tail=b->tail->next;
    if(b->size==1){
      b->head->next=b->tail;
    }
    b->size=b->size+1;
  }
  //printf("after enqueue: %d\n",b->size);
}

struct message* dequeue(struct box* b){
  struct message* temp=(struct message*)(malloc(sizeof(struct message)));
  //printf("before dequeue: %d\n",b->size);
  if(b->size==0){
    // printf("in last dequeue\n");
    // struct message* temp;
    temp->length=-2;
    //temp->message=(char*)(malloc(256*sizeof(char)));
    //bzero(temp->message,256);
    //printf("after length\n");
    //return temp;
  }
  else if(b->size==1){
    //struct message* temp=b->head;
    // struct message* temp;
    temp->length=b->head->length;
    temp->message=(char*)(malloc(256*sizeof(char)));
    bzero(temp->message,256);
    strcpy(temp->message,b->head->message);
    b->head=NULL;
    b->tail=NULL;
    b->size=0;
    //return temp;
  }
  else{
    // struct message* temp=b->head;
    // struct message* temp;
    temp->length=b->head->length;
    temp->message=(char*)(malloc(256*sizeof(char)));
    bzero(temp->message,256);
    strcpy(temp->message,b->head->message);
    b->head=b->head->next;
    b->size=b->size-1;
    // return temp;
  }
  // printf("after dequeue: %d\n",b->size);
  return temp;

}



void serverPrint(int err, time_t rawtime, struct tm* info, char* timestr, char* ip, char* msg){
time(&rawtime );
info = localtime( &rawtime );
strftime(timestr,80,"%H%M %d %b", info);
if(err==0)
fprintf(stdout, "%s %s %s\n",timestr,ip,msg);
else if(err==1)
fprintf(stderr, "%s %s %s\n",timestr,ip,msg);
}



void* clientThreading(void* args){
//vars
  struct myargs_t* arg = (struct myargs_t*)args;

  // pthread_detach(pthread_self());
 int newsockfd=arg->socket;
 // printf("threading socket: %d\n",newsockfd);
 // struct box* boxes=*(arg->boxes);
 struct sockaddr_in cli_addr=arg->cli_addr;
 char * buffer;
 int check = -1;
 int n=0;
 time_t rawtime;
  struct tm *info;
  char* timestr=(char*)(malloc(80*sizeof(char)));
  int limit=200;

 //char ip[INET_ADDRSTRLEN];
  char* ip=(char*)(malloc(INET_ADDRSTRLEN*sizeof(char)));
  bzero(ip,INET_ADDRSTRLEN);
  bzero(timestr,80);
  struct sockaddr_in* v4addr=(struct sockaddr_in*)&cli_addr;
  struct in_addr ipaddr=v4addr->sin_addr;
  inet_ntop(AF_INET,&ipaddr,ip,INET_ADDRSTRLEN);

  int emptySpot = -1;
  int openIndex = -1;
  //struct box * boxes = (struct box * ) malloc(100 * sizeof(struct box));

 

  int c = 0;
  //int check=0;
  int i = 0;
  int error = 0;
  int found = 0;
  //char inp[256];
  //char inp2[256];
  //char store[256];
  char * inp;
  char * inp2;
  char * store;
  char * tempstore;
  inp = (char * )(malloc(256 * sizeof(char)));
  inp2 = (char * )(malloc(256 * sizeof(char)));
  store = (char * )(malloc(256 * sizeof(char)));
  tempstore= (char * )(malloc(256 * sizeof(char)));
  buffer = (char * )(malloc(256 * sizeof(char)));
  bzero(inp, 256);
  bzero(inp2, 256);
  bzero(store, 256);
  bzero(tempstore,256);
  bzero(buffer, 256);
  
  serverPrint(0,rawtime, info,timestr,ip,"connected");
  // zero out the char buffer to receive a client message
  bzero(buffer, 256);
  // try to read from the client socket
  check = read(newsockfd, buffer, 255);
  // if the read from the client blew up, complain and exit
   if (check < 0) {
    perror("ERROR: problem when reading ");
    pthread_exit(1);
   }
  // try to write to the client socket
  n = write(newsockfd, "HELLO DUMBv0 ready!", 19);
  serverPrint(0,rawtime, info,timestr,ip,"HELLO");
  // if the write to the client below up, complain and exit
  if (n < 0) {
    perror("ERROR: problem writing");
    pthread_exit(1);
  }

  
  //printf("client ip: %s\n",ip);
  while (c != -1) {
    // pthread_mutex_lock(&lock);
    error = 0;
    emptySpot = -1;
    found = -1;
    // if the connection blew up for some reason, complain and exit
    if (newsockfd < 0) {
      perror("ERROR: problem when accepting client");
      pthread_exit(1);
    }
    // zero out the char buffer to receive a client message
    bzero(buffer, 256);
    // try to read from the client socket
    // printf("waiting for client input...\n");
    check = read(newsockfd, buffer, 255);
    // printf("The client message: %s\n",buffer);
    // if the read from the client blew up, complain and exit
    if (check < 0) {
      perror("ERROR: problem when reading ");
      pthread_exit(1);
    }
    // check=write(newsockfd,"sync",4);
    // if(check<0) {
    //  perror("ERROR: problem when writing");
    //  exit(1);
    // }
    
    if (strcmp("GDBYE", buffer) == 0) {
  serverPrint(0,rawtime, info,timestr,ip,"GDBYE");

      // do GDBYE STUFF
      //if open then close
      if (openIndex >= 0) {
        boxes[openIndex].open = 0;
        openIndex = -1;
      }
      close(newsockfd);
      serverPrint(0,rawtime, info,timestr,ip,"disconnected");
      // break;
      c=-1;
      pthread_exit(1);
    } else if (strncmp("CREAT", buffer, 5) == 0) {
      //CREAT arg0
      pthread_mutex_lock(&lock);
      error=0;
      //printf("in creat\n");
      serverPrint(0,rawtime, info,timestr,ip,"CREAT");
      bzero(inp, 256);
      for (i = 0; i < limit; i++) {
        if (boxes[i].name[0] == '*') {
          emptySpot = i;
          break;
        }
      }
      //printf("the empty spot is %d\n",emptySpot);
      //buffer is size 256, may need to rewrite to go to null terminator
      for (i = 6; i < strlen(buffer); i++) {
        inp[i - 6] = buffer[i];
      }
      //printf("The name %s\n",inp);
      if ((strlen(inp) < 5 || strlen(inp) > 25) || ((inp[0] < 'A' || inp[0] > 'Z') && (inp[0] < 'a' || inp[0] > 'z'))) {
	//printf("bad format\n");
	error = 1;
        //send an error back to the client
        check = write(newsockfd, "ER:WHAT?", 8);
        serverPrint(1,rawtime, info,timestr,ip,"ER:WHAT?");
        // if the write to the client below up, complain and exit
        if (check < 0) {
          perror("ERROR: problem writing");
          pthread_exit(1);
        }
      }
      if (error != 1) {
	
        //loop through the boxes and check  if it exists
        for (i = 0; i < limit; i++) {
          if (strcmp(boxes[i].name, inp) == 0) {
            //send an error back to the client
	   // printf("inside if statement\n");
	    error = 1;
            check = write(newsockfd, "ER:EXIST", 8);
            serverPrint(1,rawtime, info,timestr,ip,"ER:EXIST");
            // if the write to the client below up, complain and exit
            if (check < 0) {
              perror("ERROR: problem writing");
              pthread_exit(1);
            }
            error = 1;
          }
        }
      }
      if (error == 0) {
        strcpy(boxes[emptySpot].name, inp);
	//printf("in proper spot and error: %d\n",error);
        check = write(newsockfd, "OK!", 3);
        // if the write to the client below up, complain and exit
        if (check < 0) {
          perror("ERROR: problem writing");
          pthread_exit(1);
        }

      }
      bzero(buffer,256);
      pthread_mutex_unlock(&lock);
    } else if (strncmp("DELBX", buffer, 5) == 0) {
      //DELBX arg0
      pthread_mutex_lock(&lock);
      serverPrint(0,rawtime, info,timestr,ip,"DELBX");
      bzero(inp, 256);
      //buffer is size 256, may need to rewrite to go to null terminator
      for (i = 6; i < strlen(buffer); i++) {
        inp[i - 6] = buffer[i];
      }
      if ((strlen(inp) < 5 || strlen(inp) > 25) || ((inp[0] < 'A' || inp[0] > 'Z') && (inp[0] < 'a' || inp[0] > 'z'))) {
        error = 1;
        //send an error back to the client
        check = write(newsockfd, "ER:WHAT?", 8);
        serverPrint(1,rawtime, info,timestr,ip,"ER:WHAT?");
        // if the write to the client below up, complain and exit
        if (check < 0) {
          perror("ERROR: problem writing");
          pthread_exit(1);
        }
      }
      
      if (error != 1) {
        for (i = 0; i < limit; i++) {
          if (strcmp(boxes[i].name, inp) == 0) {
	    found=1;
	    if (boxes[i].open == 1) {
              error = 1;
              //send an error back to the client
              check = write(newsockfd, "ER:OPEND", 8);
              serverPrint(1,rawtime, info,timestr,ip,"ER:OPEND");
              // if the write to the client below up, complain and exit
              if (check < 0) {
                perror("ERROR: problem writing");
                pthread_exit(1);
              }
            } else if (boxes[i].size > 0) {
              error = 1;
              //send an error back to the client
              check = write(newsockfd, "ER:NOTMT", 8);
              serverPrint(1,rawtime, info,timestr,ip,"ER:NOTMT");
              // if the write to the client blow up, complain and exit
              if (check < 0) {
                perror("ERROR: problem writing");
                pthread_exit(1);
              }
            } else {
              boxes[i].name[0] = '*';
              //send an ok back to the client
	      //printf("MODE DELETE\n");
              check = write(newsockfd, "OK!", 3);
              // if the write to the client blow up, complain and exit
              if (check < 0) {
                perror("ERROR: problem writing");
                pthread_exit(1);
              }
	      
            }

          }

        }
	if(found<=0 && error!=1){
	  check=write(newsockfd,"ER:NEXST",8);
	  serverPrint(1,rawtime, info,timestr,ip,"ER:NEXST");
	  if(check<0){
	    perror("ERROR: problem writing");
	    pthread_exit(1);
	  }
	}
      }
      pthread_mutex_unlock(&lock);
    } else if (strncmp("OPNBX", buffer, 5) == 0) {
      //OPNBX arg0
      serverPrint(0,rawtime, info,timestr,ip,"OPNBX");
      bzero(inp, 256);
      if(openIndex>=0){
	error=1;
	check = write(newsockfd, "ER:OPNEX", 8);
	serverPrint(1,rawtime,info,timestr,ip,"ER:OPNEX");
      }
      //buffer is size 256, may need to rewrite to go to null terminator
      for (i = 6; i < strlen(buffer); i++) {
        inp[i - 6] = buffer[i];
      }
      if (error!=1 && ((strlen(inp) < 5 || strlen(inp) > 25) || ((inp[0] < 'A' || inp[0] > 'Z') && (inp[0] < 'a' || inp[0] > 'z')))) {
        error = 1;
        //send an error back to the client
        check = write(newsockfd, "ER:WHAT?", 8);
        serverPrint(1,rawtime, info,timestr,ip,"ER:WHAT?");
        // if the write to the client blow up, complain and exit
        if (check < 0) {
          perror("ERROR: problem writing");
          pthread_exit(1);
        }
      }

      if (error != 1) {
        for (i = 0; i < limit; i++) {
          if (strcmp(boxes[i].name, inp) == 0) {
	    //printf("names are the same\n");
	    if (boxes[i].open == 1) {
              error = 1;
              //send an error back to the client
              check = write(newsockfd, "ER:OPEND", 8);
              serverPrint(1,rawtime, info,timestr,ip,"ER:OPEND");
              // if the write to the client blow up, complain and exit
              if (check < 0) {
                perror("ERROR: problem writing");
                pthread_exit(1);
              }
              found = 1;
            } else if (boxes[i].open == 0) {
              boxes[i].open = 1;
              openIndex = i;
              //send an error back to the client
	      //printf("MODE OPEN\n");
              check = write(newsockfd, "OK!", 3);
              // if the write to the client blow up, complain and exit
              if (check < 0) {
                perror("ERROR: problem writing");
                pthread_exit(1);
              }
              found = 1;
            }
          }
        }
        if (found <= 0 && error != 1) {
          error = 1;
          //send an error back to the client
          check = write(newsockfd, "ER:NEXST", 8);
          serverPrint(1,rawtime, info,timestr,ip,"ER:NEXST");
          // if the write to the client blow up, complain and exit
          if (check < 0) {
            perror("ERROR: problem writing");
            pthread_exit(1);
          }
        }
      }
    } else if (strncmp("CLSBX", buffer, 5) == 0) {
      //CLSBX arg0
      serverPrint(0,rawtime, info,timestr,ip,"CLSBX");
      bzero(inp, 256);
      //buffer is size 256, may need to rewrite to go to null terminator
      for (i = 6; i < strlen(buffer); i++) {
        inp[i - 6] = buffer[i];
      }
      if ((strlen(inp) < 5 || strlen(inp) > 25) || ((inp[0] < 'A' || inp[0] > 'Z') && (inp[0] < 'a' || inp[0] > 'z'))) {
        error = 1;
        //send an error back to the client
        check = write(newsockfd, "ER:WHAT?", 8);
        serverPrint(1,rawtime, info,timestr,ip,"ER:WHAT?");
        // if the write to the client blow up, complain and exit
        if (check < 0) {
          perror("ERROR: problem writing");
          pthread_exit(1);
        }
      }

      if (error != 1 && openIndex>=0) {
        
          if (strcmp(boxes[openIndex].name, inp) == 0) {
            if (boxes[openIndex].open == 0) {
              error = 1;
              //send an error back to the client
              check = write(newsockfd, "ER:NOOPN", 8);
              serverPrint(1,rawtime, info,timestr,ip,"ER:NOOPN");
              // if the write to the client blow up, complain and exit
              if (check < 0) {
                perror("ERROR: problem writing");
                pthread_exit(1);
              }
              found = 1;
            } else if (boxes[openIndex].open == 1) {
              boxes[openIndex].open = 0;
              openIndex = -1;
              //send an error back to the client
	      //printf("MODE CLOSE\n");
              check = write(newsockfd, "OK!", 3);
              // if the write to the client blow up, complain and exit
              if (check < 0) {
                perror("ERROR: problem writing");
                pthread_exit(1);
              }
              found = 1;
            }
          }
        
        else if(error!=1){
          error = 1;
          //send an error back to the client
          check = write(newsockfd, "ER:OPEND", 8);
          serverPrint(1,rawtime, info,timestr,ip,"ER:OPEND");
          // if the write to the client blow up, complain and exit
          if (check < 0) {
            perror("ERROR: problem writing");
           pthread_exit(1);
          }
        }
      }
      else if(error!=1){
          error = 1;
          //send an error back to the client
          check = write(newsockfd, "ER:NOOPN", 8);
          serverPrint(1,rawtime, info,timestr,ip,"ER:NOOPN");
          // if the write to the client blow up, complain and exit
          if (check < 0) {
              perror("ERROR: problem writing");
              pthread_exit(1);
          }
      }
    } else if (strncmp("PUTMG", buffer, 5) == 0) {
      //PUTMG!arg0!msg
      serverPrint(0,rawtime, info,timestr,ip,"PUT");
      int ind = 0;

      //GET THE ARG0 AND THE MSG
      //arg0 is inp
      //msg is in inp2
      bzero(inp, 256);
      bzero(inp2, 256);
      bzero(store, 256);
      //buffer is size 256, may need to rewrite to go to null terminator
      // printf("before segfault\n");
      for (i = 6; i < strlen(buffer); i++) {
        store[i - 6] = buffer[i];
      }
      //printf("store: %s\n",store);
      //now store has arg0!msg
      for (i = 0; i < strlen(store); i++) {
        if (store[i] == '!') {
          ind = i;
        }
      }
      //printf("in the middle of segfault\n");
      for (i = 0; i < ind; i++) {
        inp[i] = store[i];
      }
      //now inp has arg0
      //printf("inp: %s\n",inp);
      for (i = ind+1; i < strlen(store); i++) {
        inp2[i - (ind+1)] = store[i];
      }
      //now inp2 has msg
      // printf("after segfault\n");
      //printf("inp2: %s\n",inp2);
      if (openIndex < 0) {
        error = 1;
        //send an error back to the client
        check = write(newsockfd, "ER:NOOPN", 8);
        serverPrint(1,rawtime, info,timestr,ip,"ER:NOOPN");
        // if the write to the client blow up, complain and exit
        if (check < 0) {
          perror("ERROR: problem writing");
          pthread_exit(1);
        }

      } else if (openIndex >= 0 && error!=1) {
	  // printf("beforesegfault\n");
	  // boxes[openIndex].head->message = inp2;
	  struct message* msgtemp=(struct message*)(malloc(sizeof(struct message)));
	  //msgtemp->message=inp2;
	  msgtemp->message=(char*)(malloc(256*sizeof(char)));
	  bzero(msgtemp->message,256);
	  strcpy(msgtemp->message,inp2);
	  msgtemp->length=atoi(inp);
	  msgtemp->next=NULL;
	  enqueue(msgtemp,&boxes[openIndex]);
	  
	  // boxes[openIndex].size = 1;
	  bzero(tempstore,256);
	  // printf("middlesegfault\n");
	  tempstore[0]='O';
	  tempstore[1]='K';
	  tempstore[2]='!';
	  //store[3]=' ';
	  // printf("store before cat: %c\n",tempstore[1]);
	  strcat(tempstore,inp2);
	  // printf("aftersegfault\n");
          //send an error back to the client
	  // printf("store: %s\n",tempstore);
          check = write(newsockfd, tempstore, strlen(tempstore));
	  
          // if the write to the client blow up, complain and exit
          if (check < 0) {
            perror("ERROR: problem writing");
            pthread_exit(1);
          }

         

      }

    } else if (strncmp("NXTMG", buffer, 5) == 0) {
      //NXTMG
	serverPrint(0,rawtime, info,timestr,ip,"NXTMG");
      if (openIndex < 0) {
        error = 1;
        //send an error back to the client
        check = write(newsockfd, "ER:NOOPN", 8);
        serverPrint(1,rawtime, info,timestr,ip,"ER:NOOPN");
        // if the write to the client blow up, complain and exit
        if (check < 0) {
          perror("ERROR: problem writing");
          pthread_exit(1);
        }
      } else if (openIndex >= 0) {
	//printf("before dequeuecall\n");
        struct message* msg=dequeue(&boxes[openIndex]);
	//printf("afterdequeue\n");
	if (msg->length<0 && error!=1) {
          error = 1;
	  //printf("inside the <0 if \n");
          //send an error back to the client
          check = write(newsockfd, "ER:EMPTY", 8);
          serverPrint(1,rawtime, info,timestr,ip,"ER:EMPTY");
          // if the write to the client blow up, complain and exit
          if (check < 0) {
            perror("ERROR: problem writing");
            pthread_exit(1);
          }

        }
	if(error!=1){
          //send an error back to the client
          bzero(inp, 256);
          //itoa(boxes[openIndex].head->length,inp,10);
          sprintf(inp, "%d", msg->length);
	  //printf("MODE NEXT\n");
	  bzero(tempstore,256);
	  tempstore[0]='O';
	  tempstore[1]='K';
	  tempstore[2]='!';
	  strcat(inp,"!");
	  strcat(tempstore,inp);
	  strcat(tempstore,msg->message);
          check = write(newsockfd, tempstore, strlen(tempstore));
          // if the write to the client blow up, complain and exit
          if (check < 0) {
            perror("ERROR: problem writing");
            pthread_exit(1);
          }
	}
      }

    }
    bzero(buffer, 256);
    // pthread_mutex_unlock(&lock);
  } //while loop
  return 0;

}


int main(int argc, char * argv[]) {

  char * buffer;
  int check = -1;
  int n;
  int sockfd, newsockfd, portno, clilen;
  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;

  // file descriptor for our server socket
  // file descriptor for a client socket
  // server port to connect to
  // utility variable - size of clientAddressInfo below
  // utility variable - for monitoring reading/writing from/to the socket
  // char array to store data going to and coming from the socket

  // Super-special secret C struct that holds address info for building our server socket
  // Super-special secret C struct that holds address info about our client socket

  //important variables
  //struct box* boxes =(struct box*)malloc(100*sizeof(struct box));
  int emptySpot = -1;
  int openIndex = -1;
  // struct box * boxes = (struct box * ) malloc(100 * sizeof(struct box));
  /*
  time_t rawtime;
  struct tm *info;
  char* timestr=(char*)(malloc(80*sizeof(char)));
  */

  // int c = 0;
  //int check=0;
  /* 
     int i = 0;
  int error = 0;
  int found = 0;
  //char inp[256];
  //char inp2[256];
  //char store[256];
  char * inp;
  char * inp2;
  char * store;
  char * tempstore;
  inp = (char * )(malloc(256 * sizeof(char)));
  inp2 = (char * )(malloc(256 * sizeof(char)));
  store = (char * )(malloc(256 * sizeof(char)));
  tempstore= (char * )(malloc(256 * sizeof(char)));
  buffer = (char * )(malloc(256 * sizeof(char)));
  bzero(inp, 256);
  bzero(inp2, 256);
  bzero(store, 256);
  bzero(tempstore,256);
  bzero(buffer, 256);
  */
  // If the user didn't enter enough arguments, complain and exit
  if (argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }

  /** If the user gave enough arguments, try to use them to get a port number and address **/

  // convert the text representation of the port number given by the user to an int 
  portno = atoi(argv[1]);

  // try to build a socket .. if it doesn't work, complain and exit

  //sockfd = socket(domain, type, protocol)
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  //if the socketfd is negative then exit
  if (sockfd < 0) {
    perror("ERROR: problem when opening socket");
    exit(1);
  }

  /** We now have the port to build our server socket on .. time to set up the address struct **/

  // zero out the socket address info struct .. always initialize!

  //bzero(&serv_addr, sizeof(serv_addr));
  //cast as char array???????
  bzero((char * ) & serv_addr, sizeof(serv_addr));

  // set the remote port .. translate from a 'normal' int to a super-special 'network-port-int'
  serv_addr.sin_port = htons(portno);

  // set a flag to indicate the type of network address we'll be using  
  serv_addr.sin_family = AF_INET;

  // set a flag to indicate the type of network address we'll be willing to accept connections from
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  /** We have an address struct and a socket .. time to build up the server socket **/

  // bind the server socket to a specific local port, so the client has a target to connect to    

  //if it is less than 0 then exit
  if (bind(sockfd, (struct sockaddr * ) & serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR: problem when binding server socket to local port");
    exit(1);
  }

  // set up the server socket to listen for client connections
  //10 is the number of max connections
  listen(sockfd, 100);

  // determine the size of a clientAddressInfo struct
  int limit=200;
  int i=0;
  clilen = sizeof(cli_addr);
  boxes = (struct box * ) malloc(limit * sizeof(struct box));
  for (i = 0; i < limit; i++) {
    struct box temp;
    temp.head = NULL;
    temp.tail = NULL;
    temp.size = 0;
    temp.name = (char * )(malloc(256 * sizeof(char)));
    bzero(temp.name, 256);
    //check this
    temp.name[0] = '*';
    temp.open = 0;
    boxes[i] = temp;
  }
 
   pthread_t* tids= (pthread_t*)malloc(sizeof(pthread_t)*100);
   // struct  myargs_t* args= (struct myargs_t*)(malloc(sizeof(struct myargs_t)*100));
    struct myargs_t args;
  int index=0;
  int c=0;
  // printf("before while loop\n");
  pthread_mutex_init(&lock,NULL);
  while(1){
    newsockfd = accept(sockfd, (struct sockaddr * ) & cli_addr, & clilen);
    // block until a client connects, when it does, create a client socket
  // if the connection blew up for some reason, complain and exit
  if (newsockfd < 0) {
    perror("ERROR: problem when accepting client");
    // exit(1);
    return 1;
  }
  // printf("main socket: %d\n",newsockfd);
  // printf("before args assignment\n");
  args.socket=newsockfd;
  // args.boxes=&boxes;
  args.cli_addr=cli_addr;
  // printf("args socekt: %d\n",args.socket);
  //printf("before new thread\n");
  pthread_create(&tids[index],NULL,clientThreading,&args);
  pthread_detach(tids[index]);
  /** If we're here, a client tried to connect **/
    index=index+1;
   }
  pthread_mutex_destroy(&lock);
  return 0;
}

