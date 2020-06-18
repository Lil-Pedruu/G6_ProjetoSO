#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include "reading.h"
#include "moving.h"
/*
* The include file <netinet/in.h> defines sockaddr_in as:
* struct sockaddr_in {
* short sin_family;
* u_short sin_port;
* struct in_addr sin_addr;
* char sin_zero[8];
* };
* This program creates a datagram socket, binds a name to it, then
* reads from the socket.
*/
static int n;
static int mode;
static char buf[1024];
static int **maze;

/* Estrutura com atributos a serem usados pela função recvfrom() */
typedef struct args {
  int sock;
  char* buf;
  int size;
  struct sockaddr *client;
  int len;
  int msg;
}args;

/* Receber a informação do client */
void *receive(void *arg){
  args *args1 = (args *)arg;

  n = recvfrom(args1->sock, args1->buf, args1->size,
                args1->msg, args1->client, &(args1->len));

  return NULL;
}

/* Atualiza a posição do cursor */
void *refresh(void *arg){
  args *args1 = (args *)arg;

  if(mode==1){

    firstMode(maze, buf[0]); // movimenta o cursor
    nocbreak();
  }

  if(mode==2){

    secondMode(maze, buf[0]); // movimenta o cursor
    nocbreak();
  }

/*  if(mode==3){
    autonomous(maze); // Modo de movimento autonomo do cursor/mouse.
    nocbreak();
  } */

  char buffer[1024];

  sendto(args1->sock, (char *)getMazeInfo(maze), args1->size,
                args1->msg, args1->client, args1->len);

  return NULL;
}

// MAIN
void main(int argc, char* argv[]) // Iniciar server com indicação do modo (1, 2 ou 3).
{                                 // Exemplo: ./server 2
  int sock, length;
  struct sockaddr_in name;
  /* Create socket from which to read. */
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == -1) {
    perror("opening datagram socket");
    exit(1);
  }
  /* Create name with wildcards. */
  name.sin_family = AF_INET;
  name.sin_addr.s_addr = INADDR_ANY;
  name.sin_port = ntohs(12345); // Port Number.

  if (bind(sock,(struct sockaddr *)&name, sizeof name) == -1) {
    perror("binding datagram socket");
    exit(1);
  }
  /* Find assigned port value and print it out. */
  length = sizeof(name);
  if (getsockname(sock,(struct sockaddr *) &name, &length) == -1) {
    perror("getting socket name");
    exit(1);
  }


//------------------------------------
  char *_argv[1];
  _argv[0]="ler"; // Argumento para iniciar função ler().

  clearscreen();
  cbreak(); //Colocar o terminal no modo cbreak ou rare
  maze = ler(1, _argv); //retorna a matriz

  /* Modo de jogo. */
  mode = atoi(argv[1]); // converte argumento de (string) para inteiro
  mapa(maze, mode); // inicia o movimento do cursor utilizando a matriz
  printf("^");
  printf("\033[%dD",1); // movimenta para a esquerda
  nocbreak();
//-----------------------------------------

// Ciclo while -> Faz com que o Servidor esteja constantemente à espera de input
//               pelo Cliente e faça a atualização logo de seguida.
  while(buf[0] != 112){ // 112 é o caracter 'p' na tabela ASCII. Usado para terminar o server.
  int len;
  struct sockaddr_in client;
  len = sizeof(client);

  /* Criar estrutura do tipo 'args' e devidos atributos,
     para serem usados na função recvfrom() que vai receber a informação. */
  args args1;
  args1.sock = sock;
  args1.buf = (char *)buf;
  args1.size = 1024;
  args1.msg = MSG_WAITALL;
  args1.client = ( struct sockaddr *) &client;
  args1.len = len;

  /* Criar primeira thread (recebe informação). */
  void *status1;
  pthread_t p1;
  pthread_create(&p1,NULL,receive,&args1); //Read from the socket
  pthread_join(p1,&status1);

  buf[n] = '\0';

  /* Criar segunda thread (atualiza informação). */
  void *status2;
  pthread_t p2;
  pthread_create(&p2,NULL,refresh,&args1);  // Atualiza a posição do cursor
  pthread_join(p2,&status2);              // e envia informação ao Cliente.
}

  printf("Fim do server");
  close(sock);
  exit(0);
}
