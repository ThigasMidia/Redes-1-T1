#ifndef __SERVER_H__
#define __SERVER_H__

#include "shared.h"

typedef struct tesouro {
    unsigned char x;
    unsigned char y;
} tesouro;

void enviaArquivo(int socket, unsigned char *bufferSend, char *nome, unsigned char *sequencia);

//envia mensagem tipo 9
void enviaEOF(int socket, unsigned char *sequencia);

tesouro aleatorizaTesouro();

int comparaTesouros(tesouro t1, tesouro t2);

void criaTesouros(tesouro *t);

#endif
