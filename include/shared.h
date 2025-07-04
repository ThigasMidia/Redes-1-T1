#ifndef __SHARED_H__
#define __SHARED_H__

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <net/if.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define MARCADOR_INI 126 //01111110
#define MAX_DADOS 127   
#define MAX_BUFFER 131 //127 + MARCADOR(1) + CHECKSUM(1) + TAM/SEQ/TIPO(2)

#define TIMEOUTMILLIS 500

//STRUCT DE PACOTES PARA ENVIO E RECEBIMENTO PELA REDE
typedef struct pacote_t {
    unsigned char tamanho;
    unsigned char sequencia;
    unsigned char tipo;
    unsigned char dados[MAX_DADOS];
} pacote_t;

typedef struct {

    int x;
    int y;

} Player;

typedef struct {

   Player p;
   char grid[8][8]; 

} Tabuleiro;

//FUNCAO DISPONIBILIZADA PELO TODT
int cria_raw_socket(char* nome_interface_rede);

//
void criaMensagem(pacote_t *mensagem, unsigned char tamanho, unsigned char *sequencia, unsigned char tipo, unsigned char *dados);

/*FUNCAO QUE GERA O CHECKSUM ATRAVES DO BUFFER, 
    USADA TANTO PARA O CALCULO INICIAL COMO PARA A CHECAGEM POSTERIOR*/
int checksum(unsigned char *buffer);

//TRANSFORMA UMA STRUCT PACOTE_T EM UM BUFFER PARA ENVIO
int encheBuffer(unsigned char *buffer, pacote_t *mensagem, int *tam129, int preciso, unsigned char *corrections);

//TRANSFORMA UM BUFFER RECEBIDO EM UMA STRUCT PACOTE_T
void recebeMensagem(unsigned char *buffer, pacote_t *mensagem);

//FAZ CHECKSUM DA MENSAGEM RECEBIDA
int checaMensagem(unsigned char *buffer);

//FAZ UMA CUCA MT GOSTOSA
void enviaMensagem(int socket, unsigned char *buffer, unsigned char *sequencia, int tam);

//FAZ UM BOLO DE LARANJA MT GOSTOSO
void enviaACK(int socket, unsigned char *sequencia);

//FAZ UM BOLO DE LIMAO MT GOSTOSO
void enviaNACK(int socket, unsigned char *sequencia);

//
int corrige129(unsigned char *buffer, int preciso, unsigned char *corrections);

//
void restaura129(unsigned char *buffer, int preciso, unsigned char *corrections);

//RETORNA 1 SE SEQUENCIA1 VEM DEPOIS DE SEQUENCIA2, RETORNA 0 CASO CONTRARIO
int depoisDe(unsigned char sequencia1, unsigned char sequencia2);

//
void incrementaSequencia(unsigned char *sequencia);

//
void decrementaSequencia(unsigned char *sequencia);

//
long long timestamp();

#endif
