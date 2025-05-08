#include "./include/shared.h"
#include "./include/clientFunc.h"

int main() {

    //FUNCIONOU!!!!!!!
    int socket = cria_raw_socket("veth1");
    unsigned char *buffer, *dados;
    buffer = malloc(MAX_BUFFER);
    dados = malloc(MAX_BUFFER);
    pacote_t mensagem;
    int fim = 1;
    unsigned char *seq;
    seq = malloc(1);
    *seq = 0;

    enviaACK(socket, seq);
    recebeArquivo(socket, buffer, "xereca.txt", seq);
    printf("FOI!!!\n");

    close(socket);
    free(seq);
    return 0;
}
