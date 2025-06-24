#include "./include/shared.h"
#include "./include/front.h"
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
    *seq = 0;
    int ret;
    tesouro *tes;
    tes = malloc(sizeof(tesouro) * 8);
    criaTesouros(tes);
    for(int i = 0; i < 8; i++)
        printf("%s       %d/%d\n", tes[i].arquivo, tes[i].x, tes[i].y);
    Tabuleiro tabuleiro;
    iniciaTabuleiro(&tabuleiro);
    
    while(fim) {
        recv(socket, buffer, MAX_BUFFER, 0);
        ret = checaMensagem(buffer);
        if(ret == 1) {
            recebeMensagem(buffer, &mensagem);
            if(!mensagem.tipo)
                fim--;
        }
    }
    while(1) {
        recv(socket, buffer, MAX_BUFFER, 0);
        ret = checaMensagem(buffer);
        if(ret == 1) {
            recebeMensagem(buffer, &mensagem);
            interpretaDirecao(socket, mensagem, seq, &tabuleiro, tes, buffer);
        } else if (ret == -1) {
            enviaNACK(socket, seq);
        }
    }
    close(socket);
    free(seq);
    free(buffer);
    return 0;
};
