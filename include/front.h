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

#include "shared.h"

// Preenche tabuleiro com VAZIO e posisciona DOGUI (player) em (0,0)
void iniciaTabuleiro (Tabuleiro* t);

// Imprime o tabuleiro no stdin
void printTabuleiro (Tabuleiro* t, char player);

// Move o player
void movePlayer (Tabuleiro* t, int direcao);

#endif
