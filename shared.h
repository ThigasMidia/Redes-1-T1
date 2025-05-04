#ifndef __SHARED_H__
#define __SHARED_H__

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#define MARCADOR_INI 126 //01111110
#define MAX_DADOS 127   
#define MAX_BUFFER 131 //127 + MARCADOR(1) + CHECKSUM(1) + TAM/SEQ/TIPO(2)

//STRUCT DE PACOTES PARA ENVIO E RECEBIMENTO PELA REDE
typedef struct pacote_t {
    unsigned char tamanho;
    unsigned char sequencia;
    unsigned char tipo;
    unsigned char dados[MAX_DADOS];
} pacote_t;

//FUNCAO DISPONIBILIZADA PELO TODT
int cria_raw_socket(char* nome_interface_rede);

//
void criaMensagem(pacote_t *mensagem, unsigned char tamanho, unsigned char sequencia, unsigned char tipo);

/*FUNCAO QUE GERA O CHECKSUM ATRAVES DO BUFFER, 
    USADA TANTO PARA O CALCULO INICIAL COMO PARA A CHECAGEM POSTERIOR*/
int checksum(unsigned char *buffer);

//TRANSFORMA UMA STRUCT PACOTE_T EM UM BUFFER PARA ENVIO
int encheBuffer(unsigned char *buffer, pacote_t *mensagem);

//TRANSFORMA UM BUFFER RECEBIDO EM UMA STRUCT PACOTE_T
void recebeMensagem(unsigned char *buffer, pacote_t *mensagem);

//FAZ CHECKSUM DA MENSAGEM RECEBIDA
int checaMensagem(unsigned char *buffer);

//FAZ UM BOLO DE LARANJA MT GOSTOSO
void enviaACK(unsigned char *buffer, int socket, unsigned char sequencia);

//FAZ UM BOLO DE LIMAO MT GOSTOSO
void enviaNACK(unsigned char *buffer, int socket, unsigned char sequencia);

#endif
