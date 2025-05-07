#include "clientFunc.h"

void recebeArquivo(int socket, unsigned char *bufferSend, char *nome, unsigned char *sequencia) {
    FILE *arquivo = fopen(nome, "wb");
    if(!arquivo) {
        perror("ERRO AO CRIAR ARQUIVO");
        exit(1);
    }
    int ret;
    unsigned char terminado = 0, liberado = 1, lastAN = 0;
    unsigned char *bufferReceive;
    pacote_t messageSend, messageReceive;
    messageReceive.tipo = 126;
    bufferReceive = malloc(MAX_BUFFER);
    printf("CHECANDO\n");
    while(!terminado) {
        if(liberado) {
            recv(socket, bufferReceive, MAX_BUFFER, 0);
            ret = checaMensagem(bufferReceive);
            if(ret == 1) {
                recebeMensagem(bufferReceive, &messageReceive);
                if(messageReceive.tipo == 5) {
                    fwrite(messageReceive.dados, 1, messageReceive.tamanho, arquivo);
                    liberado--;
                }
                else if(messageReceive.tipo == 1) {
                    if(!lastAN)
                        enviaACK(socket, sequencia);
                    else
                        enviaNACK(socket, sequencia);
                }
                else if(messageReceive.tipo == 9) {
                    terminado++;
                    enviaACK(socket, sequencia);
                }
            }
            else if(ret == -1) {
                enviaNACK(socket, sequencia);
                lastAN = 1;
            }
        }
        else {
            enviaACK(socket, sequencia);
            lastAN = 0;
            liberado++;
        }
    }
    printf("TERMINOU DE RECEBER!!!!!!!!\n");
    free(bufferReceive);
}
