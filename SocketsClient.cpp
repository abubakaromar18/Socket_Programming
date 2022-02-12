#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


using namespace std;

void error(char *msg)
{
  perror(msg);
  exit(0);
}

int main(int argc, char **argv)
{
  //DEFINING VARIABLES
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  char buffer[256];
  bool close_connection = false;

  //CHECKS THE ARGUMENT COUNT TO MAKE SURE THE HOSTNAME AND PORT NUMBER IS ENTERED
  if(argc < 3)
  {
    fprintf(stderr, "usage %s hostname port\n", argv[0]);
    exit(0);
  }

  //CONVERTS THE PORT FROM STRING TO INTEGER
  portno = atoi(argv[2]);

  //CREATES A SOCKET FILE DESCRIPTOR
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
  {
    error("ERROR opening socket");
  }

  //FINDS HOST USING THE HOSTNAME IN THE ARGUMENT
  server = gethostbyname(argv[1]);
  if(server == NULL)
  {
    fprintf(stderr, "ERROR, no such host");
    exit(0);
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));

  //DEFINING THE SERVER ADDRESS
  serv_addr.sin_family = AF_INET;
  bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portno);

  //CONNECTING THE CLIENT TO THE SERVER
  if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
  {
    error("ERROR connecting");
  }


  //LOOP ALLOWS MULTIPLE MESSAGES TO BE TRANSMITTED BACK AND FORTH
  while(!close_connection)
  {
    printf("Please enter the message (or type exit to terminate connection): ");
    bzero(buffer, 256);
    fgets(buffer, 255, stdin);

    //WRITES THE USER INPUT TO THE SOCKET DESCRIPTOR
    n = write(sockfd, &buffer, strlen(buffer));
    if(n < 0)
    {
      error("ERROR writing to socket");
    }

    bzero(buffer, 256); //CLEAR THE BUFFER

    //READS THE SOCKET DESCRIPTER TO THE BUFFER
    n = read(sockfd, &buffer, 255);
    if(n < 0)
    {
      error("ERROR reading from socket");
    }

    //COMPARES THE BUFFER TO THE STRING EXIT
    if(strcmp(buffer, "exit") == 0) // IF BUFFER CONTENT == EXIT
    {
      //CLOSE CONNECTION AND TERMINATE THE PROGRAM
      close_connection = true;
      printf("Connection Terminated");
    }
    //ELSE WRITE THE CONTENTS SENT FROM THE SERVER
    else
    {
      printf("%s\n", buffer);
    }
  }

  return 0;
}
