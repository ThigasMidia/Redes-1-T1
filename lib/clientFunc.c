#include "../include/clientFunc.h"

void recebeArquivo(int socket, unsigned char *bufferSend, char *nome, unsigned char *sequencia) {
    FILE *arquivo = fopen(nome, "wb");
    if(!arquivo) {
        perror("ERRO AO CRIAR ARQUIVO");
        exit(1);
    }

    int ret;
    unsigned char terminado = 0, liberado = 1, lastAN = 0, sequenciaAnt = 126, sequenciaAtu = 0;
    unsigned char *bufferReceive;
    pacote_t messageSend, messageReceive;
    messageReceive.tipo = 126;
    bufferReceive = malloc(MAX_BUFFER);
    messageSend.tipo = 0;
    int cont = 0;
    while(!terminado) {
        cont++;
        if(liberado) {
            recv(socket, bufferReceive, MAX_BUFFER, 0);
            ret = checaMensagem(bufferReceive);
            if(ret == 1) {
                printf("ACEITOU!!!!!\n");
                recebeMensagem(bufferReceive, &messageReceive);
                sequenciaAtu = messageReceive.sequencia;
                if(sequenciaAnt == 126)
                    sequenciaAnt = (sequenciaAtu + 31) % 32;
                if(depoisDe(sequenciaAtu, sequenciaAnt) /*&& sequenciaAnt != sequenciaAtu*/) {
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
                else
                    printf("ERRO NA SEQUENCIA!!!!!!!!!  %d/%d\n", sequenciaAtu, sequenciaAnt);
                sequenciaAnt = sequenciaAtu;
            }
            else if(ret == -1) {
                printf("BUFFER!!!!!!!!!\n");
                printf("SEQUENCIA!!!!!!!!!  %d/%d\n", sequenciaAtu, sequenciaAnt);
                for(int i = 0; i < MAX_BUFFER; i++)
                    printf("%d ", bufferReceive[i]);
                printf("\n");
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
    free(bufferReceive);
}
