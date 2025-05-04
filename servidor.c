#include "shared.h"

int main() {
    int soquete = cria_raw_socket("veth1");
    unsigned char *buffer;
    buffer = malloc(MAX_BUFFER);
    ssize_t num_bytes;
    pacote_t mensagem;
    
    //TA FUNFANDO!!!!!!
    while ((num_bytes = recv(soquete, buffer, MAX_BUFFER, 0)) > 0) {
        printf("Recebido %zd bytes\n", num_bytes);
        
        if(checaMensagem(buffer))
            recebeMensagem(buffer, &mensagem);
        else
            printf("MENSAGEM ERRADA!!!!\n");

        printf("Tamanho: %d, Sequência: %d, Tipo: %d, Dados: ", 
                mensagem.tamanho, mensagem.sequencia, mensagem.tipo);
        for(int i = 0; i < mensagem.tamanho; i++)
            printf("%d ", mensagem.dados[i]);
        printf("\n");
    }
    printf("\n");

    close(soquete);

    return 0;
}
