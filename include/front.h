#ifndef __FRONT_H__
#define __FRONT_H__

#define DOGUI   'M'
#define VAZIO   '.'
#define PASSO   '*'
#define TSORO   'x'
#define ERRO    '?'

#define L       'a'
#define R       'd'
#define U       'w'
#define D       's'

typedef struct {

    int x;
    int y;

} Player;

typedef struct {

   Player p;
   char grid[8][8]; 

} Tabuleiro;

// Preenche tabuleiro com VAZIO e posisciona DOGUI (player) em (0,0)
void iniciaTabuleiro (Tabuleiro* t);

// Imprime o tabuleiro no stdin
void printTabuleiro (Tabuleiro* t, char player);

// Move o player
void movePlayer (Tabuleiro* t, int direcao);

#endif
