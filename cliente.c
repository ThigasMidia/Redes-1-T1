#include "./include/shared.h"
#include "./include/clientFunc.h"
#include "./include/front.h"

int main() {

    //FUNCIONOU!!!!!!!
    int socket = cria_raw_socket("veth1");
    unsigned char *buffer;
    buffer = malloc(MAX_BUFFER);
    pacote_t mensagem;
    unsigned char *seq;
    seq = malloc(1);
    *seq = 0;
    int ack;
    
    Tabuleiro t;
    iniciaTabuleiro(&t);
    unsigned char move[64];
        
    printTabuleiro(&t, DOGUI);
    enviaACK(socket, seq);
    unsigned char quantos = 0;
    
    // por enquanto, depois agnt muda pra while nn encontrou todos ou sla
    while (quantos != 8) {
        scanf("%s", move);
        int direct;
        if(move[0] == 'w')
            direct = 11;
        else if (move[0] == 'a')
            direct = 13;
        else if (move[0] == 's')
            direct = 12;
        else if (move[0] == 'd')
            direct = 10;
        else {
            direct = 0;
            printf("Direção Inválida\n");
        }
            
        if (direct) {    
            int ret = 0;
            ack = 0;
            while (!ack) {
                enviaDirecao(socket, direct, seq, buffer);
                while(ret == 0) {
                    recv(socket, buffer, MAX_BUFFER, 0);
                    ret = checaMensagem(buffer);
                }
                if (ret == 1) {
                    recebeMensagem(buffer, &mensagem);
                    ack = 1;
                    if (mensagem.tipo == 1) 
                        ack = 0;
                } 
            }
            
            if(mensagem.tipo == 6 || mensagem.tipo == 7 || mensagem.tipo == 8) {
                movePlayer(&t, move[0]);
                enviaACK(socket, seq);
                recebeArquivo(socket, (unsigned char*)mensagem.dados, seq);
                quantos++;
            }
            else if (mensagem.tipo == 0 || mensagem.tipo == 2) {
                movePlayer(&t, move[0]);
            } 
         }
    }
    
    printf("FOI!!!\n");
    close(socket);
    free(seq);
    free(buffer);
    return 0;
}
