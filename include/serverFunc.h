#ifndef __SERVER_H__
#define __SERVER_H__

#include "shared.h"

//TIPO TESOURO QUE GUARDA AS COORDENADAS
typedef struct tesouro {
    unsigned char x;
    unsigned char y;
} tesouro;

//ENVIA ARQUIVO GENERICO
void enviaArquivo(int socket, unsigned char *bufferSend, char *nome, unsigned char *sequencia);

//ENVIA MENSAGEM TIPO 9
void enviaEOF(int socket, unsigned char *sequencia);

//ALEATORIZA POSICAO DO TESOURO
tesouro aleatorizaTesouro();

//COMPARA 2 TESOUROS 
int comparaTesouros(tesouro t1, tesouro t2);

//FUNCAO DE CRIACAO DE TESOUROS EM POSICOES ALEATORIAS, MAS DIFERENTES
void criaTesouros(tesouro *t);

#endif
