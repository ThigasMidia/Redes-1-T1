#include "shared.h"

int main() {
    int soquete = cria_raw_socket("lo"); // Usa a função cria_raw_socket para criar o socket
    unsigned char *buffer;
    buffer = malloc(MAX_BUFFER);
    ssize_t num_bytes;
    pacote_t mensagem;
    
    //PARECE ESTAR ESCUTANDO NA REDE, MAS PRECISAMOS TESTAR
    while ((num_bytes = recv(soquete, buffer, sizeof(buffer), 0)) > 0) {
        printf("Recebido %zd bytes\n", num_bytes);

        // Processa o pacote recebido
        recebeMensagem(buffer, &mensagem);
        printf("Tamanho: %d, Sequência: %d, Tipo: %d, Dados: %s\n", 
                mensagem.tamanho, mensagem.sequencia, mensagem.tipo, mensagem.dados);
    }

    close(soquete);

    return 0;
}
