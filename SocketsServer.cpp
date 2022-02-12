#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

void error (char *msg)
{
  perror(msg);
  exit(1);
}

int main(int argc, char **argv)
{
  //DEFINING VARIABLES
  struct sockaddr_in serv_addr, cli_addr;
  int sockfd, newsockfd, portno, clilen;
  char buffer[256];
  int n;
  bool close_connection = false;

  //CHECKS THE ARGUMENT COUNT TO MAKE SURE THE PORT NUMBER IS ENTERED
  if (argc < 2)
  {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }

  //CREATES A SOCKET FILE DESCRIPTOR
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
  {
    error("ERROR opening socket");
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));

  //CONVERTS THE PORT FROM STRING TO INTEGER
  portno = atoi(argv[1]);

  //DEFINING THE SERVER ADDRESS
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(portno);
  serv_addr.sin_addr.s_addr = INADDR_ANY;


  //BINDING THE SOCKET TO THE SPECIFIED IP AND PORT
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
  {
    error("ERROR on binding");
  }

  //LISTENS TO THE NETWORK FOR ANY CONNECTIONS
  listen(sockfd, 5); //ALLOWS AT MOST 5 CONNECTIONS

  clilen = sizeof(cli_addr);

  //ACCEPTS CLIENT SOCKET
  newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *) &clilen);
  if(newsockfd < 0)
  {
    error("ERROR on accept");
  }

  //LOOP ALLOWS MULTIPLE MESSAGES TO BE TRANSMITTED BACK AND FORTH
  while(!close_connection)
  {
    bzero(buffer, 256);

    //READS INPUT FROM THE SOCKET FILE DESCRIPTOR INTO THE BUFFER
    n = read(newsockfd, &buffer, 255);
    if(n < 0)
    {
      error("ERROR reading from socket");
    }

    //COMPARES THE BUFFER TO THE STRING EXIT
    if(strcmp(buffer, "exit\n") == 0) //IF BUFFER == EXIT
    {
      //CLOSES THE CONNECTION AND TERMINATES THE PROGRAM
      close_connection = true;
      n = write(newsockfd, "exit", 4);
      if(n < 0)
      {
        error("ERROR writing to socket");
      }
    }
    //ECHOS THE CLIENT INPUT BACK TO THE CLIENT
    else
    {
      printf("Here is the message: %s\n", buffer);

      n = write(newsockfd, &buffer, 255);
      if(n < 0)
      {
        error("ERROR writing to socket");
      }
    }
  }

  return 0;
}
