#include "shared.h"
#include "serverFunc.h"

int main() {
    srand(time(NULL));
    int socket = cria_raw_socket("veth1");
    unsigned char *buffer;
    buffer = malloc(MAX_BUFFER);
    pacote_t mensagem;
    int fim = 1;
    int cont = 0;
    tesouro *t;
    t = malloc(sizeof(tesouro)*8);
    criaTesouros(t);
    for(int i = 0; i < 8; i++)
        printf("%dx%d\n", t[i].x, t[i].y);
    
    //TA FUNFANDO!!!!!!
    while (fim) {
        recv(socket, buffer, MAX_BUFFER, 0); 
        int ret = checaMensagem(buffer); 
        if(ret == 1) {
            recebeMensagem(buffer, &mensagem);
            printf("Tipo: %d\n", mensagem.tipo);
            for(int i = 0; i < mensagem.tamanho; i++)
                printf("%d ", mensagem.dados[i]);
                printf("\n");
            enviaACK(buffer, socket, cont);
            cont++;
            fim--;
        }
        else if(ret == -1){
            printf("MENSAGEM ERRADA!!!!\n");
            enviaNACK(buffer, socket, cont);
            cont++;
        }
    }
    printf("\n");

    close(socket);

    return 0;
}
