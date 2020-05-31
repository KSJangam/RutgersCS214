#include <stdio.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <netdb.h>

#include <unistd.h>

#include <stdlib.h>

#include <string.h>


void error(char * msg) {
  perror(msg);
  exit(0);
}

int main(int argc, char * argv[]) {
  // Declare initial vars

  //char buffer[256];
  int check = -1;
  int sockfd, portno;
  struct sockaddr_in serv_addr;
  struct hostent * info;

  //variable to check when to exit the loop
  int c = 0;
  //int check=0;
  //command string
  char * str;
  //input string
  char * inp;
  //input2 string
  char * inp2;
  //string to be sent to server
  char * sent;
  //message read from server
  char * buffer;
  str = (char * )(malloc(256 * sizeof(char)));
  inp = (char * )(malloc(256 * sizeof(char)));
  inp2 = (char * )(malloc(256 * sizeof(char)));
  sent = (char * )(malloc(256 * sizeof(char)));
  //printf("Success on connecting!\n");
  buffer = (char * )(malloc(256 * sizeof(char)));
  bzero(str, 256);
  bzero(buffer, 256);
  bzero(inp2, 256);
  bzero(inp, 256);
  bzero(sent, 256);

  // file descriptor for our socket
  // server port to connect to
  // utility variable - for monitoring reading/writing from/to the socket
  // char array to store data going to and coming from the server
  // Super-special secret C struct that holds address info for building our socket
  // Super-special secret C struct that holds info about a machine's address

  // If the user didn't enter enough arguments, complain and exit	
  if (argc < 3) {
    fprintf(stderr, "ERROR: not enough input\n");
    exit(1);
  }

  /** If the user gave enough arguments, try to use them to get a port number and address **/

  // convert the text representation of the port number given by the user to an int
  portno = atoi(argv[2]);

  // look up the IP address that matches up with the name given - the name given might
  //    BE an IP address, which is fine, and store it in the 'serverIPAddress' struct
  info = gethostbyname(argv[1]);

  if (info == NULL) {
    perror("ERROR: problem with given host name");
    exit(1);
  }
  // try to build a socket .. if it doesn't work, complain and exit
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("ERROR: problem when opening socket");
    exit(1);
  }

  // printf("MADE IT\n");
  /** We now have the IP address and port to connect to on the server, we have to get    **/
  /**   that information into C's special address struct for connecting sockets                     **/

  // zero out the socket address info struct .. always initialize!
  bzero((char * ) & serv_addr, sizeof(serv_addr));

  // set a flag to indicate the type of network address we'll be using 
  serv_addr.sin_family = AF_INET;

  // set the remote port .. translate from a 'normal' int to a super-special 'network-port-int'
  serv_addr.sin_port = htons(portno);
  // do a raw copy of the bytes that represent the server's IP address in 
  //   the 'serverIPAddress' struct into our serverIPAddressInfo struct
  bcopy((char * ) info -> h_addr, (char * ) & serv_addr.sin_addr.s_addr, info -> h_length);

  // printf("OVER HERE\n");
  /** We now have a blank socket and a fully parameterized address info struct .. time to connect **/

  // try to connect to the server using our blank socket and the address info struct 
  //   if it doesn't work, complain and exit
  //error checking
  check = connect(sockfd, (struct sockaddr * ) & serv_addr, sizeof(serv_addr));
  printf("CONNECTED\n");
  if (check < 0) {
    perror("ERROR: problem when connecting");
    exit(1);
  }

  /** If we're here, we're connected to the server .. w00t!  **/

  // zero out the message buffer
  bzero(buffer, 256);
  // printf("WAITNING FOR INPUT\n");
  // get a message from the user
  // fgets(buffer, 255, stdin);
  buffer[0]='H';
  buffer[1]='E';
  buffer[2]='L';
  buffer[3]='L';
  buffer[4]='O';
  // try to write it out to the server
   check = write(sockfd, buffer, strlen(buffer));

  // if we couldn't write to the server for some reason, complain and exit
   if (check < 0) {
    perror("ERROR: problem when writing");
    exit(1);
  }

  // sent message to the server, zero the buffer back out to read the server's response
  bzero(buffer, 256);

  // read a message from the server into the buffer
  check = read(sockfd, buffer, 255);
  // if we couldn't read from the server for some reason, complain and exit
  if (check < 0) {
    perror("ERROR: problem when reading");
    exit(1);
  }
  if(strcmp("HELLO DUMBv0 ready!",buffer)!=0){
    c=-1;
  }
  else{
  // print out server's message
  printf("%s\n", buffer);
  // ---------------------------------------------------------------------
  printf("PROGRAM STARTED\n");
  check = 0;
  }
  // zero out the message buffer
  bzero(buffer, 256);
  bzero(sent, 256);
  while (c != -1) {
    //do error checking inside the functions themselves
    bzero(str,256);
    printf("> ");
    scanf("%s", str);
    printf("\n");
    bzero(inp, 256);
    bzero(inp2, 256);
    bzero(buffer, 256);
    bzero(sent, 256);
    
    //checks to see what the input string is

    if (strcmp("quit", str) == 0) {
      //do GOODBYE stuff

      //send GDBYE command to server to shutdown
      check = write(sockfd, "GDBYE", strlen("GDBYE"));

      // if we couldn't write to the server for some reason, complain and exit
      if (check < 0) {
        perror("ERROR: problem when writing");
        exit(1);
      }

      // sent message to the server, zero the buffer back out to read the server's response
      bzero(buffer, 256);
      // read a message from the server into the buffer
      check = read(sockfd, buffer, 255);
      // if we couldn't read from the server for some reason, report success and exit
      if (check <= 0) {
        printf("Successfully quit!\n");
        c = -1;
        exit(1);
      }
      //if something came back
      else if (check > 0) {
        perror("ERROR: problem when attempting to close connection with server. Still exiting...");
        c = -1;
        exit(1);
      }
      c = -1;

    } else if (strcmp("create", str) == 0) {
      printf("Okay, what is the name of the new message box?\n");
      printf("create:> ");
      scanf("%s", inp);
      // printf("\n");
      //do CREATE stuff

      //get the input into DUMB protocol form
      bzero(sent, 256);
      sent[0]='C';
      sent[1]='R';
      sent[2]='E';
      sent[3]='A';
      sent[4]='T';
      sent[5]=' ';
      // printf("SEGFAULT BEFORE\n");
      //sent = strcat("CREAT ", inp);
      strcat(sent,inp);
      // printf("SEGFAULT AFTER\n");
      // try to write it out to the server
      check = write(sockfd, sent, strlen(sent));

      // if we couldn't write to the server for some reason, complain and exit
      if (check < 0) {
        perror("ERROR: problem when writing");
        exit(1);
      }

      // sent message to the server, zero the buffer back out to read the server's response
      bzero(buffer, 256);

      // read a message from the server into the buffer
      check = read(sockfd, buffer, 255);
      // if we couldn't read from the server for some reason, complain and exit
      if (check < 0) {
        perror("ERROR: problem when reading");
        exit(1);
      }

      //check what the message was
      if (strcmp("OK!", buffer)==0) {
        printf("OK!\n");
      } else if (strcmp("ER:EXIST", buffer)==0) {
        printf("ERROR: %s already exists\n", inp);
        //do we need perror?
      } else if (strcmp("ER:WHAT?", buffer)==0) {
        printf("ERROR: user message is broken or malformed\n");
      }
    } else if (strcmp("delete", str) == 0) {
      printf("Okay, what is the name of the message box? to be deleted?\n");
      printf("delete:> ");
      scanf("%s", inp);
      // printf("\n");
      //do DELETE stuff

      //get the input into DUMB protocol form
      bzero(sent, 256);
      
      //sent = strcat("DELBX ", inp);
      sent[0]='D';
      sent[1]='E';
      sent[2]='L';
      sent[3]='B';
      sent[4]='X';
      sent[5]=' ';
      strcat(sent,inp);
      // try to write it out to the server
      check = write(sockfd, sent, strlen(sent));
     
      // if we couldn't write to the server for some reason, complain and exit
      if (check < 0) {
        perror("ERROR: problem when writing");
        exit(1);
      }

      // sent message to the server, zero the buffer back out to read the server's response
      bzero(buffer, 256);

      // read a message from the server into the buffer
      check = read(sockfd, buffer, 255);
      // if we couldn't read from the server for some reason, complain and exit
      if (check < 0) {
        perror("ERROR: problem when reading");
        exit(1);
      }

      //check what the message was
      if (strcmp("OK!", buffer)==0) {
        printf("OK!\n");
      } else if (strcmp("ER:NEXST", buffer)==0) {
        printf("ERROR: %s does not exist as box\n", inp);
        //do we need perror?
      } else if (strcmp("ER:OPEND", buffer)==0) {
        printf("ERROR: box is opened by another user\n");
      } else if (strcmp("ER:NOTMT", buffer)==0) {
        printf("ERROR: box is not empty and still has messages\n");
      } else if (strcmp("ER:WHAT?", buffer)==0) {
        printf("ERROR: user message is broken or malformed\n");
      }
    } else if (strcmp("open", str) == 0) {
      printf("Okay, open which message box?\n");
      printf("open:> ");
      scanf("%s", inp);
      // printf("\n");
      //do OPEN stuff

      //get the input into DUMB protocol form
      bzero(sent, 256);
      // sent = strcat("OPNBX ", inp);
      sent[0]='O';
      sent[1]='P';
      sent[2]='N';
      sent[3]='B';
      sent[4]='X';
      sent[5]=' ';
      strcat(sent,inp);
      //strcat(sent,inp);

      // try to write it out to the server
      check = write(sockfd, sent, strlen(sent));

      // if we couldn't write to the server for some reason, complain and exit
      if (check < 0) {
        perror("ERROR: problem when writing");
        exit(1);
      }

      // sent message to the server, zero the buffer back out to read the server's response
      bzero(buffer, 256);

      // read a message from the server into the buffer
      check = read(sockfd, buffer, 255);
      // if we couldn't read from the server for some reason, complain and exit
      if (check < 0) {
        perror("ERROR: problem when reading");
        exit(1);
      }

      //check what the message was
      if (strcmp("OK!", buffer)==0) {
        printf("OK!\n");
      } else if (strcmp("ER:NEXST", buffer)==0) {
        printf("ERROR: %s does not exist as box\n", inp);
        //do we need perror?
      } else if (strcmp("ER:OPEND", buffer)==0) {
        printf("ERROR: box is opened by another user\n");
      } else if (strcmp("ER:WHAT?", buffer)==0) {
        printf("ERROR: user message is broken or malformed\n");
      } else if (strcmp("ER:OPNEX", buffer)==0){
	printf("ERROR: this client already has a box open\n");
      }

    } else if (strcmp("close", str) == 0) {
      printf("Okay, close which message box?\n");
      printf("close:> ");
      scanf("%s", inp);
      // printf("\n");
      //do OPEN stuff

      //get the input into DUMB protocol form
      bzero(sent, 256);
      //sent = strcat("CLSBX ", inp);
      sent[0]='C';
      sent[1]='L';
      sent[2]='S';
      sent[3]='B';
      sent[4]='X';
      sent[5]=' ';
      strcat(sent,inp);
      // try to write it out to the server
      check = write(sockfd, sent, strlen(sent));

      // if we couldn't write to the server for some reason, complain and exit
      if (check < 0) {
        perror("ERROR: problem when writing");
        exit(1);
      }

      // sent message to the server, zero the buffer back out to read the server's response
      bzero(buffer, 256);

      // read a message from the server into the buffer
      check = read(sockfd, buffer, 255);
      // if we couldn't read from the server for some reason, complain and exit
      if (check < 0) {
        perror("ERROR: problem when reading");
        exit(1);
      }

      //check what the message was
      if (strcmp("OK!", buffer)==0) {
        printf("OK!\n");
      } else if (strcmp("ER:NOOPN", buffer)==0) {
        //if it does not exist it cannot be open so this error applies
        printf("ERROR: client has no open boxes\n");
        //do we need perror?
      } else if (strcmp("ER:WHAT?", buffer)==0) {
        printf("ERROR: user message is broken or malformed\n");
      } else if(strcmp("ER:OPEND",buffer)==0){
	printf("ERROR: box is opened by other user\n");
      }
    } else if (strcmp("next", str) == 0) {
      //printf("Okay, how many bytes is the message?\n");
      //printf("next:> ");
      //scanf( "%s" , inp);
      //printf("\n");
      //printf("What is the message?\n");
      //printf("next:> ");
      //scanf( "%s" , inp2);
      //printf("\n");
      //do NEXT stuff
      // printf("Getting the next message from open box...\n");

      //get the input into DUMB protocol form
      bzero(sent, 256);
      //this might be broken?
      //strcpy(sent, "NXTMG");

      // try to write it out to the server
      check = write(sockfd, "NXTMG", strlen("NXTMG"));

      // if we couldn't write to the server for some reason, complain and exit
      if (check < 0) {
        perror("ERROR: problem when writing");
        exit(1);
      }

      // sent message to the server, zero the buffer back out to read the server's response
      bzero(buffer, 256);

      // read a message from the server into the buffer
      check = read(sockfd, buffer, 255);
      // if we couldn't read from the server for some reason, complain and exit
      if (check < 0) {
        perror("ERROR: problem when reading");
        exit(1);
      }

      //check what the message was
      if (strncmp("OK!", buffer, 3)==0) {
        printf("%s\n", buffer);
      } else if (strcmp("ER:EMPTY", buffer)==0) {
        printf("ERROR: no messages left in box \n");
        //do we need perror?
      } else if (strcmp("ER:NOOPN", buffer)==0) {
        printf("ERROR: client has no open box\n");
      } else if (strcmp("ER:WHAT?", buffer)==0) {
        printf("ERROR: user message is broken or malformed\n");
      }
    } else if (strcmp("put", str) == 0) {
      // printf("Okay, how many bytes is the message?\n");
      // printf("put:> ");
      //  scanf("%s", inp);
      // printf("\n");
      printf("Okay, What is the message?\n");
      printf("put:> ");
      scanf("%s", inp2);
      //printf("\n");
      //do PUT stuff
      sprintf(inp,"%d",strlen(inp2));
      //get the input into DUMB protocol form
      bzero(sent, 256);
      //this might be broken?
      sent[0]='P';
      sent[1]='U';
      sent[2]='T';
      sent[3]='M';
      sent[4]='G';
      sent[5]='!';
      // printf("before segfault\n");
      strcat(inp,"!");
      strcat(sent,inp);
      strcat(sent,inp2);
      // printf("after segfault\n");
      //sent = strcat(strcat("PUTMG!", strcat(inp, "!")), inp2);

      // try to write it out to the server
      check = write(sockfd, sent, strlen(sent));

      // if we couldn't write to the server for some reason, complain and exit
      if (check < 0) {
        perror("ERROR: problem when writing");
        exit(1);
      }

      // sent message to the server, zero the buffer back out to read the server's response
      bzero(buffer, 256);

      // read a message from the server into the buffer
      check = read(sockfd, buffer, 255);
      // if we couldn't read from the server for some reason, complain and exit
      if (check < 0) {
        perror("ERROR: problem when reading");
        exit(1);
      }
      bzero(sent,256);
      sent[0]='O';
      sent[1]='K';
      sent[2]='!';
      strcat(sent,inp2);
      // printf("sent: %s\n",sent);
      //check what the message was
      if (strcmp(sent, buffer)==0) {
        printf("%s\n", buffer);
      } else if (strcmp("ER:NOOPN", buffer)==0) {
        printf("ERROR: client has no open box\n");
      } else if (strcmp("ER:WHAT?", buffer)==0) {
        printf("ERROR: user message is broken or malformed\n");
      }
    } else if (strcmp("help", str) == 0) {
      //print commands
      printf("Possible commands:\n");
      printf("quit\t(which causes: E.1 GDBYE)\n");
      printf("create\t(which causes: E.2 CREAT)\n");
      printf("delete\t(which causes: E.6 DELBX)\n");
      printf("open\t(which causes: E.3 OPNBX)\n");
      printf("close\t(which causes: E.7 CLSBX)\n");
      printf("next\t(which causes: E.4 NXTMG)\n");
      printf("put\t(which causes: E.5 PUTMG)\n");
    } else {
      printf("That is not a command, for a command list enter 'help'.\n");
    }
    printf("\n");
  }
  free(str);
  free(sent);
  free(inp);
  free(inp2);
  free(buffer);
  return 0;

}
