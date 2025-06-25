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
    char comando[64] = "open ";
    
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
                long long comeco = timestamp();
                while((ret != 1) && (timestamp() - comeco <= TIMEOUTMILLIS)) {
                    if (recv(socket, buffer, MAX_BUFFER, 0)) {
                        ret = checaMensagem(buffer);
                        if (ret == 1) {
                            printf("ret1\n");
                            recebeMensagem(buffer, &mensagem);
                            ack = 1;
                        }
                    }
                } 
                printf("%lld\n", timestamp() - comeco);
            }
            
            if(mensagem.tipo == 6 || mensagem.tipo == 7 || mensagem.tipo == 8) {
                movePlayer(&t, move[0]);
                achouTsoro(&t);
                enviaACK(socket, seq);
                recebeArquivo(socket, (char*)mensagem.dados, seq);
                strcat(comando, (char*)mensagem.dados);
                system(comando);
                comando[5] = '\0';
                quantos++;
            }
            else if (mensagem.tipo == 0 || mensagem.tipo == 2) {
                movePlayer(&t, move[0]);
            } 
         }
    }
    
    printf("ACHOU TUDO!!!\n");
    close(socket);
    free(seq);
    free(buffer);
    return 0;
}
