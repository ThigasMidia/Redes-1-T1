#include "shared.h"

int main() {

    //TENTATIVA FRUSTRADA DE TENTAR ENVIAR MENSAGEM EM LOOPBACK (NAO ROLOU D:)
    int socket = cria_raw_socket("lo");
    unsigned char *buffer;
    buffer = malloc(MAX_BUFFER);
    pacote_t mensagem;
    mensagem.tamanho = 2;
    mensagem.sequencia = 25;
    mensagem.tipo = 13;
    mensagem.dados[0] = 113;
    mensagem.dados[1] = 10;
    int tam = encheBuffer(buffer, &mensagem);

    if(send(socket, buffer, tam, 0) == -1) {
        perror("ERRO AO ENVIAR PACOTASSO");
        close(socket);
        exit(1);
    }

    printf("FOI!!!\n");

    close(socket);

    return 0;
}
