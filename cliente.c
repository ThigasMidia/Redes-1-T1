#include "shared.h"

int main() {

    //FUNCIONOU!!!!!!!
    int socket = cria_raw_socket("veth0");
    unsigned char *buffer, *dados;
    buffer = malloc(MAX_BUFFER);
    dados = malloc(MAX_BUFFER);
    int fim = 1;
    int cont = 0;
    pacote_t mensagem;
    dados[0] = 113;
    dados[1] = 1;
    criaMensagem(&mensagem, 2, random() % 32, 13, dados);
    int tam = encheBuffer(buffer, &mensagem);
    printf("%d ", tam);
    for(int i = 0; i < tam; i++)
        printf("%d ", buffer[i]);
    printf("\n");

    if(send(socket, buffer, tam, 0)<0) {
        perror("ERRO AO ENVIAR PACOTASSO");
        close(socket);
        exit(1);
    }
    
    while(fim) {
        recv(socket, buffer, MAX_BUFFER, 0);
        int ret = checaMensagem(buffer);
        if(ret == 1) {
            recebeMensagem(buffer, &mensagem);
            printf("RECEBIDO ACK DO SERVIDOR!!!!!!!!!\n");
            enviaACK(buffer, socket, cont);
            cont++;
            fim--;
        }
        else if(ret == 2) {
            printf("MENSAGEM ERRADA!!!!\n");
            enviaACK(buffer, socket, cont);
            cont++;
        }
        else {
            cont++;
            printf("MENSAGEM NAO RECEBIDA\n");
        }
        if(cont == 32) {
            printf("NAO DEU, ABORATANDO\n");
        }
    }

    printf("FOI!!!\n");

    close(socket);

    return 0;
}
