#include "shared.h"

int main() {

    //FUNCIONOU!!!!!!!
    int socket = cria_raw_socket("veth0");
    unsigned char *buffer;
    buffer = malloc(MAX_BUFFER);
    pacote_t mensagem;
    mensagem.tamanho = 2;
    mensagem.sequencia = 25;
    mensagem.tipo = 13;
    mensagem.dados[0] = 113;
    mensagem.dados[1] = 1;
    int tam = encheBuffer(buffer, &mensagem);
    printf("%d ", tam);
    for(int i = 0; i < tam; i++)
        printf("%d ", buffer[i]);
    printf("\n");
    if(send(socket, buffer, tam, 0)<0) {
        perror("ERRO AO ENVIAR PACOTASSO");
        close(socket);
        exit(1);
    }
    enviaACK(buffer, socket, 3);
    enviaNACK(buffer, socket, 4);

    printf("FOI!!!\n");

    close(socket);

    return 0;
}
