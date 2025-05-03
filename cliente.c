#include "shared.h"

int main() {

    //FUNCIONOU!!!!!!!
    int socket = cria_raw_socket("lo");
    unsigned char *buffer;
    buffer = malloc(MAX_BUFFER);
    pacote_t mensagem;
    mensagem.tamanho = 10;
    mensagem.sequencia = 25;
    mensagem.tipo = 13;
    mensagem.dados[0] = 113;
    mensagem.dados[1] = 1;
    mensagem.dados[2] = 2;
    mensagem.dados[3] = 3;
    mensagem.dados[4] = 4;
    mensagem.dados[5] = 5;
    mensagem.dados[6] = 6;
    mensagem.dados[7] = 7;
    mensagem.dados[8] = 8;
    mensagem.dados[9] = 9;
    int tam = encheBuffer(buffer, &mensagem);
    printf("%d ", tam);
    for(int i = 0; i < tam; i++)
        printf("%d ", buffer[i]);
    printf("\n");
    if(send(socket, buffer, tam+1, 0)<0) {
        perror("ERRO AO ENVIAR PACOTASSO");
        close(socket);
        exit(1);
    }

    printf("FOI!!!\n");

    close(socket);

    return 0;
}
