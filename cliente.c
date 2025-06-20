#include "./include/shared.h"
#include "./include/clientFunc.h"
#include "./include/front.h"

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
    
    Tabuleiro t;
    iniciaTabuleiro(&t);
    char move;
        
    printTabuleiro(&t, DOGUI);
    enviaACK(socket, seq);
    unsigned char quantos = 0;
    
    t.grid[2][3] = TSORO;
    // por enquanto, depois agnt muda pra while nn encontrou todos ou sla
    while (quantos != 8) {
        move = getchar();
        getchar();
        movePlayer(&t, move);
        int direct;
        if(move == 'w')
            direct = 11;
        else if (move == 'a')
            direct = 13;
        else if (move == 's')
            direct = 12;
        else
            direct = 10;
        int ret = 0;
        enviaDirecao(socket, direct, seq, buffer);
        while(ret == 0) {
            printf("TA EM 0\n");
            recv(socket, buffer, MAX_BUFFER, 0);
            ret = checaMensagem(buffer);
        }
        if(ret == 1) {
            recebeMensagem(buffer, &mensagem);
            printf("TIPO: %d\n", mensagem.tipo);
            if(mensagem.tipo == 6 || mensagem.tipo == 7 || mensagem.tipo == 8) {
                printf("%s\n", mensagem.dados);
                enviaACK(socket, seq);
                recebeArquivo(socket, mensagem.dados, seq);
                quantos++;
            }
        }
    }
    
    printf("FOI!!!\n");
    close(socket);
    free(seq);
    free(buffer);
    free(dados);
    return 0;
}
