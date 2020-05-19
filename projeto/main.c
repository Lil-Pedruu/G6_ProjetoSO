#include <stdio.h>
#include "moving.h"
#include "reading.h"

int main(int argc, char* argv[]) {
   int **maze;

   clearscreen(); //Limpar o ecrã
   cbreak(); //Colocar o terminal no modo cbreak ou rare
   maze = ler(argc, argv); //retorna a matriz
   mapa(maze); // inicia o movimento do cursor utilizando a matriz

   nocbreak(); //Por o terminal de volta no modo cooked
   clearscreen();

   return 0;
}
