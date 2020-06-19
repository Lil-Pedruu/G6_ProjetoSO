#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "reading.h"
#include "moving.h"

//Sending an Internet Domain Datagram

/*
* Here I send a datagram to a receiver whose name I get from the
* command line arguments. The form of the command line is:
* dgramsend hostname portnumber
*/
void main(int argc, char* argv[])
{
  int sock;
  struct sockaddr_in name;
  struct addrinfo *hp,hints;

  /* Create socket on which to send. */
  sock = socket(AF_INET,SOCK_DGRAM, 0);
  if (sock == -1) {
    perror("opening datagram socket");
    exit(1);
  }
  /*
  * Construct name, with no wildcards, of the socket to ‘‘send’’
  * to. gethostbyname returns a structure including the network
  * address of the specified host. The port number is taken from
  * the command line.
  */
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;

  int res = getaddrinfo(argv[1],argv[2],&hints,&hp);
  if (res != 0) {
    fprintf(stderr, "%s: getting address error\n", argv[1]);
    exit(2);
  }

  cbreak();
  printf("Prima 'p' para encerrar o Servidor.\n");

  /* Send message to Server. */
  char tecla[128];
  char c;

  c = getchar();
  tecla[0 ]= c;

  /* fica a enviar e a receber datagramas enquanto não é escrito 'p' */
  while (tecla != "p"){
    #define DATA tecla
    if (sendto(sock, DATA, sizeof DATA , 0, hp->ai_addr, hp->ai_addrlen) == -1)
    perror("sending datagram message");

    c = getchar();
    tecla[0] = c;

    // Receiving a message from the Server.
    int n,len;
    char buffer[1024];
    struct sockaddr_in server;

    n = recvfrom(sock, (char *)buffer, 1024,
                  MSG_WAITALL, (struct sockaddr *) &server, // jp->ai_addr server???
                  &len);

    buffer[n] = '\0';
    printf("\n\n// Server: ");

    int i;
    for(i = 0; i < sizeof(buffer); i++)
      printf("%c", buffer[i]);
    }

  close(sock);
  exit(0 );
}
