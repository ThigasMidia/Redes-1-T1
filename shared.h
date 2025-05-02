#ifndef __SHARED_H__
#define __SHARED_H__

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MARCADOR_INI 126 // 01111110
#define MAX_DADOS 127    
#define MAX_BUFFER 131   

//STRUCT DE PACOTES PARA ENVIO E RECEBIMENTO PELA REDE
typedef struct pacote_t {
    unsigned char tamanho;
    unsigned char sequencia;
    unsigned char tipo;
    unsigned char dados[MAX_DADOS];
} pacote_t;

//FUNCAO DISPONIBILIZADA PELO TODT
int cria_raw_socket(char* nome_interface_rede);

/*FUNCAO QUE GERA O CHECKSUM ATRAVES DO BUFFER, 
    USADA TANTO PARA O CALCULO INICIAL COMO PARA A CHECAGEM POSTERIOR*/
int checksum(unsigned char *buffer);

//TRANSFORMA UMA STRUCT PACOTE_T EM UM BUFFER PARA ENVIO
int encheBuffer(unsigned char *buffer, pacote_t *mensagem);

//TRANSFORMA UM BUFFER RECEBIDO EM UMA STRUCT PACOTE_T
void recebeMensagem(unsigned char *buffer, pacote_t *mensagem);

#endif
