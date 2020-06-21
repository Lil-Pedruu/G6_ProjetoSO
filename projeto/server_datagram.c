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

// Ferrolhos.
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;

// Variáveis condição.
pthread_cond_t cond_1  = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_2  = PTHREAD_COND_INITIALIZER;

static int c, n, d, mode, sock;
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

  while(1){
  pthread_mutex_lock(&mutex_1);
      while(d!=1){
      pthread_cond_wait(&cond_2, &mutex_1);
      }

      n = recvfrom(args1->sock, args1->buf, args1->size,
                    args1->msg, args1->client, &(args1->len));
     if(buf[0]==112){
        printf("Fim do server.\n");
        close(sock);
        exit(0);
        }
      buf[n] = '\0';

      c=1;
      pthread_cond_signal(&cond_1);
      pthread_mutex_unlock(&mutex_1);
      }
  return NULL;
}

/* Atualiza a posição do cursor */
void *refresh(void *arg){
  args *args1 = (args *)arg;

  while(1){
  pthread_mutex_lock(&mutex_2);
      while(c!=1){
      pthread_cond_wait(&cond_1, &mutex_2);
      }

      if(mode==1){

        firstMode(maze, buf[0]); // movimenta o cursor
        nocbreak();
      }

      if(mode==2){

        secondMode(maze, buf[0]); // movimenta o cursor
        nocbreak();
      }

      /*if(mode==3){
          autonomous(maze); // Modo de movimento autonomo do cursor/mouse.
          nocbreak();
        } */

      sendto(args1->sock, (char *)getMazeInfo(maze), args1->size,
                    args1->msg, args1->client, args1->len);
      buf[0]=0;

      c=0;
      d=1;
      pthread_cond_signal(&cond_2);
      pthread_mutex_unlock(&mutex_2);
  }
  return NULL;
}

// MAIN
void main(int argc, char* argv[]) // Iniciar server com indicação do modo (1 ou 2).
{                                 // Exemplo: ./server 2
  int length;
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


  void *status1, *status2; // Aponta para o retornos das threads.
  pthread_t p1, p2; // Threads.
  d=1;
  pthread_create(&p1,NULL,receive,&args1); // Recebe info do Cliente.
  pthread_create(&p2,NULL,refresh,&args1); // Atualiza e envia info ao Cliente.
  pthread_join(p1,&status1); // Suspende as threads e ajuda a main thread a esperar que as threads p1
  pthread_join(p2,&status2); // e p2 terminem a sua execução, ou seja, só depois
                            // de as threads terminarem é que o código pode seguir.
}
