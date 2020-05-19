#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#define COLUMNS 67
#define ROWS 32

struct termios oldtc;
struct termios newtc;

static int *pos;
static char tecla[128];

/*Cria estruturas termios para armazenar os parametros e
informações do terminal*/

void clearscreen(){
   /*Esta função faz uso das ANSI escape sequences,
   sequências de bytes que ao serem impressas,
   são interpretadas pelo terminal como comandos
   e não como caratéres.*/
   //printf("\033[2J");
   system("clear"); // foi utilizado este com o objetivo de corrigir um bug

}

void cbreak() {
   /*
   Esta função coloca o terminal no modo cbreak ou rare,
   no qual o terminal interpreta um carater de cada vez,
   mas continua a reconhecer combinações de teclas de controlo,
   como Ctrl-C.
   */
   tcgetattr(STDIN_FILENO, &oldtc);
   /*Obtẽm os parâmetros/atributos associados ao
   terminal e coloca-os na struct termios oldtc*/
   newtc = oldtc; //Atribui o valor de oldtc a newtc.
   newtc.c_lflag &= ~(ICANON | ECHO);
   /*Modifica a flag c_lflag que controla o modo do terminal,
   e efetua um bitwise-and com o bitwise-not do bitwise-or das constantes ICANON
   e ECHO, efetivamente definindo o modo não-canónico e a não-ecoação dos carateres
   introduzidos.
   Com o modo canónico desativado, a input do utilizador é dada caratér a carater,
   sem necessidade de delimitadores como newline, entre outras coisas.
   Com ECHO desativado, os carateres introduzidos não são ecoados, ou escritos,
   no ecrã.*/
   tcsetattr(STDIN_FILENO, TCSANOW, &newtc);
   /*Define os atributos do terminal tal como definidos em newtc,
   ou seja, desativa o modo canónico e o eco*/
}

void nocbreak() {
   tcsetattr(STDIN_FILENO, TCSANOW, &oldtc);
   /*Repõe os atributos do terminal para aqueles obtidos no início do programa e guardados em oldtc.*/
   fflush(stdout);
}

int *up(int **maze, int *pos){
   pos[1]++; //soma 1 à posição anterior
   if (maze[pos[1]-1][pos[0]-1] != 1){
      printf("\033[%dA",1); // movimenta para cima
   }
   else{
      pos[1]--; // caso a posição seguinte for uma parede é subtraido 1
   } // à posição atual
   return pos; // retorna a posição atual
}

int *down(int **maze, int *pos){
   pos[1]--;
   if (maze[pos[1]-1][pos[0]-1] != 1){
      printf("\033[%dB",1); // movimenta para baixo
   }
   else{
      pos[1]++;
   }
   return pos;
}

int *right(int **maze, int *pos){
   pos[0]++;
   if (maze[pos[1]-1][pos[0]-1] != 1){
   printf("\033[%dC",1); // movimenta para a direita
   }
   else{
      pos[0]--;
   }
   return pos;
}

int *left(int **maze, int *pos){
   pos[0]--;
   if (maze[pos[1]-1][pos[0]-1] != 1){
      printf("\033[%dD",1); // movimenta para a esquerda
   }
   else{
      pos[0]++;
   }
   return pos;
}

void firstMode(int **maze, char tecla){ // 4 teclas.

      if(tecla==119 || tecla == 87){  // Estes números corresponde a caracteres da tabela ASCII
            pos = up(maze, pos);
      }

      if(tecla==100 || tecla == 68){
            pos = right(maze, pos);
      }

      if(tecla==97 || tecla == 65){
            pos = left(maze, pos);
      }
}

void secondMode(int **maze, char tecla){ // 4 teclas.

     if(tecla==119 || tecla == 87){
           pos = up(maze, pos);
     }

     if(tecla==100 || tecla == 68){
           pos = right(maze, pos);
     }

     if(tecla==97 || tecla == 65){
           pos = left(maze, pos);
     }

     if(tecla==115 || tecla == 83){
           pos = down(maze, pos);
     }


}

int mapa(int **maze, int mode){
   pos = malloc(sizeof(int*) * 2);

   for(int i=ROWS;i>=0;i--){ // calcula a posição do S (posição inicial)
      for(int j = 0; j < COLUMNS; j++){
         if (maze[i][j] == 2){
            pos[0] = i+2;
            pos[1] = j;
         }
      }
   }
   printf("\033[%d;%dH",ROWS+2-pos[1],pos[0]); //coloca o cursor na posição do S

     if (maze[pos[1]-1][pos[0]-1] == 3){ //Se chegar à posição do G o jogo acaba
        printf("\033[34;0HYOU WIN!");
     }

}
