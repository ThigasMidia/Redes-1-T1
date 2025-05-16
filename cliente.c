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
    recebeArquivo(socket, buffer, "xereca.mp4", seq);
    printf("FOI!!!\n");
    /*recv(socket, buffer, MAX_BUFFER, 0); 
    for(int i = 0; i < 20; i++)
        printf("%d ", buffer[i]);
    printf("\n");
    recebeMensagem(buffer, &mensagem);
    printf("PRINT PRELIMINAR\n");
    for(int i = 0; i < mensagem.tamanho; i++)
        printf("%d ", mensagem.dados[i]);
    printf("\n");
    recv(socket, buffer, MAX_BUFFER, 0);
    recebeMensagem(buffer, &mensagem);
    for(int i = 0; i < 20; i++)
        printf("%d ", buffer[i]);
    printf("\n");
    printf("PRINT QUE IMPORTA!! %d\n", mensagem.tamanho);
    for(int i = 0; i < mensagem.tamanho; i++)
        printf("%d ", mensagem.dados[i]);
    printf("\n");*/
    close(socket);
    free(seq);
    free(buffer);
    free(dados);
    return 0;
}
