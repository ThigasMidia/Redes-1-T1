#include "./include/shared.h"
#include "./include/serverFunc.h"

int main() {
    srand(time(NULL));
    int socket = cria_raw_socket("veth0");
    unsigned char *buffer;
    buffer = malloc(MAX_BUFFER);
    pacote_t mensagem;
    int fim = 1;
    unsigned char *seq;
    seq = malloc(1);
    *seq = 3;
    while(fim) {
        recv(socket, buffer, MAX_BUFFER, 0);
        int ret = checaMensagem(buffer);
        if(ret == 1) {
            recebeMensagem(buffer, &mensagem);
            if(!mensagem.tipo)
                fim--;
        }
    }
    enviaArquivo(socket, buffer, "sherlock.txt", seq);
    close(socket);
    free(seq);
    return 0;
}
