#include "./include/shared.h"
#include "./include/serverFunc.h"

int main() {
    srand(time(NULL));
    int socket = cria_raw_socket("veth0");
    unsigned char *buffer;
    buffer = malloc(MAX_BUFFER);
    pacote_t mensagem;
    mensagem.tipo = 5;
    mensagem.sequencia = 5;
    mensagem.tamanho = 16;
    for(int i = 0; i < 16; i = i + 4) {
        mensagem.dados[i] = i;
        mensagem.dados[i+1] = i+1;
        mensagem.dados[i+2] = i+2;
        mensagem.dados[i+3] = i+3;
    }
    int fim = 1;
    unsigned char *seq;
    seq = malloc(1);
    *seq = 0;
    /*while(fim) {
        recv(socket, buffer, MAX_BUFFER, 0);
        int ret = checaMensagem(buffer);
        if(ret == 1) {
            recebeMensagem(buffer, &mensagem);
            if(!mensagem.tipo)
                fim--;
        }
    }*/
    enviaMensagem(socket, buffer, &mensagem, seq);    
    mensagem.dados[0] = 126;
    mensagem.dados[1] = 126; 
    mensagem.dados[2] = 134; 
    mensagem.dados[3] = 163; 
    for(int i = 4; i < 16; i = i + 4) {
        printf("%d\n", i);
        mensagem.dados[i] = 129;
        mensagem.dados[i+1] = 0;
        mensagem.dados[i+2] = 0;
        mensagem.dados[i+3] = 1;
    }
    for(int i = 0; i < 16; i++)
        printf("%d ", mensagem.dados[i]);
    printf("\n");
    enviaMensagem(socket, buffer, &mensagem, seq);
    //enviaArquivo(socket, buffer, "miku.mp4", seq);
    close(socket);
    free(seq);
    free(buffer);
    return 0;
}
