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
    
    struct timeval timeout = { .tv_sec = 0, .tv_usec = 0 };
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout, sizeof(timeout));
    
    while(fim) {
        recv(socket, buffer, MAX_BUFFER, 0);
        ret = checaMensagem(buffer);
        if(ret == 1) {
            recebeMensagem(buffer, &mensagem);
            if(!mensagem.tipo)
                fim--;
        }
    }
    while(!achouTudo(tes)) {
        recv(socket, buffer, MAX_BUFFER, 0);
        ret = checaMensagem(buffer);
        if(ret == 1) {
            recebeMensagem(buffer, &mensagem);
            if(mensagem.tipo == 10 || mensagem.tipo == 11 || mensagem.tipo == 12 || mensagem.tipo == 13)
                interpretaDirecao(socket, mensagem, seq, &tabuleiro, tes, buffer);
        } else if (ret == -1) {
            enviaNACK(socket, seq);
        }
    }
    printf("FIM!!!\n");
    close(socket);
    free(seq);
    free(buffer);
    return 0;
};
