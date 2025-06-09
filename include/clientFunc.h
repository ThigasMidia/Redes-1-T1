#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "shared.h"

//TRATA QUANDO ENCONTRA UM TESOURO
void encontreiArquivo();

//RECEBE ARQUIVO GENERICO
void recebeArquivo(int socket, unsigned char *bufferSend, char *nome, unsigned char *sequencia);

//ENVIA UMA DIRECAO QUE O CLIENTE QUER IR
void enviaDirecao(int socket, unsigned char direcao, unsigned char *sequencia);

#endif
