#ifndef __SERVER_H__
#define __SERVER_H__

#include "shared.h"

//TIPO TESOURO QUE GUARDA AS COORDENADAS
typedef struct tesouro {
    unsigned char x;
    unsigned char y;
    unsigned char *arquivo;
    unsigned char encontrado;
} tesouro;

//LE TAMANHO DO ARQUIVO
int checaTamanhoArquivo(int socket, unsigned char *sequencia, unsigned char *bufferSend, unsigned char *bufferReceive, char *nomearquivo);

//ENVIA MENSAGEM TIPO 9
void enviaEOF(int socket, unsigned char *sequencia);

//ENVIA ARQUIVO GENERICO
void enviaArquivo(int socket, unsigned char *bufferSend, char *nome, unsigned char *sequencia);

//TRATA QUANDO O CLIENTE ENCONTROU O ARQUIVO
void encontrouArquivo(int socket, unsigned char *nomearquivo, unsigned char *sequencia, unsigned char *buffer);

//INTERPRETA DIRECAO ENVIADA PELO CLIENTE, INCLUINDO SE ENCONTROU UM ARQUIVO OU NAO
int interpretaDirecao(int socket, pacote_t direcao, unsigned char *sequencia, Tabuleiro *t, tesouro *tes, unsigned char *bufferSend);

//CHECA SE PLAYER ESTA EM UM TESOURO
int checaSeEncontrou(Player p, tesouro *t);

//ENVIA TIPO DO ARQUIVO ENCONTRADO
void encontrouArquivo(int socket, unsigned char *nomearquivo, unsigned char *sequencia, unsigned char *buffer);

//ALEATORIZA POSICAO DO TESOURO
tesouro aleatorizaTesouro();

//COMPARA 2 TESOUROS 
int comparaTesouros(tesouro t1, tesouro t2);

//FUNCAO DE CRIACAO DE TESOUROS EM POSICOES ALEATORIAS, MAS DIFERENTES
void criaTesouros(tesouro *t);

//FUNCAO QUE CHECA SE TODOS OS TESOUROS JA FORAM ENCONTRADOS
int achouTudo (tesouro* tes);

#endif
