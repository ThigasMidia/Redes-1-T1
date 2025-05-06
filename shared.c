#include "shared.h"

int cria_raw_socket(char* nome_interface_rede) {
    // Cria arquivo para o socket sem qualquer protocolo
    int soquete = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (soquete == -1) {
        fprintf(stderr, "Erro ao criar socket: Verifique se você é root!\n");
        exit(-1);
    }
 
    int ifindex = if_nametoindex(nome_interface_rede);
 
    struct sockaddr_ll endereco = {0};
    endereco.sll_family = AF_PACKET;
    endereco.sll_protocol = htons(ETH_P_ALL);
    endereco.sll_ifindex = ifindex;
    // Inicializa socket
    if (bind(soquete, (struct sockaddr*) &endereco, sizeof(endereco)) == -1) {
        fprintf(stderr, "Erro ao fazer bind no socket\n");
        exit(-1);
    }
 
    struct packet_mreq mr = {0};
    mr.mr_ifindex = ifindex;
    mr.mr_type = PACKET_MR_PROMISC;
    // Não joga fora o que identifica como lixo: Modo promíscuo
    if (setsockopt(soquete, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1) {
        fprintf(stderr, "Erro ao fazer setsockopt: "
            "Verifique se a interface de rede foi especificada corretamente.\n");
        exit(-1);
    }
    fprintf(stderr, "CRIOU O SOCKET!\n"); 
    return soquete;
}

//MONTA UMA MENSAGEM SEM DADOS
void criaMensagem(pacote_t *mensagem, unsigned char tamanho, unsigned char sequencia, unsigned char tipo, unsigned char *dados) {
    
    mensagem->tamanho = tamanho;
    mensagem->sequencia = sequencia;
    mensagem->tipo = tipo;
    if(tamanho)
        memcpy(mensagem->dados, dados, tamanho);
}

//CHECKSUM DE 4 EM 4 BITS DE TAMANHO, SEQUENCIA, TIPO E DADOS
int checksum(unsigned char *buffer) {
    int sum = 0, tam;
     
    tam = (buffer[1] >> 1) + 4;

    //PRIMEIROS 4 BITS DE TAMANHO
    sum += buffer[1] >> 4;
    //ULTIMOS 3 BITS DE TAMANHO + PRIMEIRO BIT DE SEQUENCIA
    sum += buffer[1] & 0x0F;
    
    //ULTIMOS 4 BITS DE SEQUENCIA
    sum += buffer[2] >> 4;
    //BITS DE TIPO
    sum += buffer[2] & 0x0F;
    //LOOP PARA BYTES DE DADOS
    for(int i = 4; i < tam; i++) {
        sum += buffer[i] >> 4;
        sum += buffer[i] & 0x0F;
    }
    //MOD 256 PARA RETORNAR APENAS OS 8 BITS MENOS SIGNIFICATIVOS
    return sum % 256;
}

//TRANSFORMA UMA MENSAGEM (TAMANHO, SEQUENCIA, TIPO E DADOS) EM UM BUFFER PARA ENVIO NA REDE
int encheBuffer(unsigned char *buffer, pacote_t *mensagem) {
    
    //COLOCA O MARCADOR DE INICIO
    buffer[0] = MARCADOR_INI;
    //7 BITS DE TAMANHO + BIT MAIS SIGNIFICATIVO DE SEQUENCIA
    buffer[1] = (mensagem->tamanho << 1) | (mensagem->sequencia >> 4);
    //4 BITS MENOS SIGNIFICATIVOS DE SEQUENCIA + 4 BITS DE TIPO
    buffer[2] = (mensagem->sequencia << 4) | mensagem->tipo;
    
    //COPIA TODOS OS DADOS A PARTIR DA POSICAO 5 DO BUFFER
    memcpy(&buffer[4], mensagem->dados, mensagem->tamanho); 
    
    //CHECKSUM ARMAZENADO NA POSICAO 4 DO BUFFER
    buffer[3] = checksum(buffer);
    if(mensagem->tamanho+4 < 14) {
        int tam = mensagem->tamanho;
        for(int i = tam+4; i < 14; i++) {
            buffer[i] = 0;
            mensagem->tamanho++;
        }
    }
    //DEVOLVE TAMANHO DO BUFFER PARA USO POSTERIOR
    return mensagem->tamanho+4;
}

//TRANSFORMA UM BUFFER RECEBIDO EM UMA MENSAGEM
void recebeMensagem(unsigned char *buffer, pacote_t *mensagem) {
    unsigned char *comeco = buffer+4;
    //TAMANHO RECEBE OS 7 BITS MAIS SIGNIFICATIVOS DA SEGUNDA POSICAO DO BUFFER
    mensagem->tamanho = buffer[1] >> 1;
    //SEQUENCIA RECEBE O BIT MAIS SIGNIFICATIVO DE SEQUENCIA NA ULTIMA POSICAO DO BUFFER
    //E SOMA/CONCATENA COM OS 4 BITS MAIS SIGNIFICATIVOS DO OUTRO BYTE
    mensagem->sequencia = ((buffer[1] & 0x01) << 4) + (buffer[2] >> 4);
    //TIPO RECEBE OS 4 BITS MENOS SIGNIFICATIVOS ONDE ESTA O TIPO
    mensagem->tipo = buffer[2] & 0x0F;
     
    //COPIA PARA O CAMPO DE DADOS DA MENSAGEM O QUE ESTA ARMAZENADO NO BUFFER
    memcpy(mensagem->dados, comeco, mensagem->tamanho);

}

//CHECA: SE NAO É MENSAGEM, RETORNA 0. SE A MENSAGEM ESTA CORRETA, RETORNA 1. SE CHECKSUM ERRADO, RETORNA -1
int checaMensagem(unsigned char *buffer) {
    if(buffer[0] == MARCADOR_INI) {
        if(buffer[3] == checksum(buffer))
            return 1;
        return -1;
    }

    return 0;
}

void enviaMensagem(int socket, unsigned char *buffer, pacote_t *mensagem) {
    int tam = encheBuffer(buffer, mensagem);
    if(send(socket, buffer, tam, 0) < 0)
        perror("ERRO AO ENVIAR MENSAGEM");
}

void enviaACK(unsigned char *buffer, int socket, unsigned char *sequencia) {
    pacote_t mensagem;
    criaMensagem(&mensagem, 0, sequencia, 0, NULL);
    enviaMensagem(socket, buffer, &mensagem);
}

void enviaNACK(unsigned char *buffer, int socket, unsigned char *sequencia) {
    pacote_t mensagem;
    criaMensagem(&mensagem, 0, sequencia, 1, NULL);
    enviaMensagem(socket, buffer, &mensagem);

}

//UTILIZEI A MAIN PARA TESTAR TODAS AS FUNCOES ANTERIORES
/*
int main() {
    pacote_t mensagem, teste;
    mensagem.tamanho = 2;
    mensagem.sequencia = 25;
    mensagem.tipo = 13;
    mensagem.dados[0] = 113;
    mensagem.dados[1] = 10;

    unsigned char *buffer;
    buffer = malloc(MAX_BUFFER);

    encheBuffer(buffer, &mensagem);

    printf("%d ", buffer[0]);
    printf("%d ", buffer[1]);
    printf("%d ", buffer[2]);
    //printf("%d ", buffer[3]);
    for(int i = 4; i < mensagem.tamanho+4; i++)
        printf("%d ", buffer[i]);
    printf("\n");
    recebeMensagem(buffer, &teste);
    printf("%d/%d\n", teste.tamanho, mensagem.tamanho);
    printf("%d/%d\n", teste.sequencia, mensagem.sequencia);
    printf("%d/%d\n", teste.tipo, mensagem.tipo);
    for(int i = 0; i < mensagem.tamanho; i++)
        printf("%d/%d\n", teste.dados[i], mensagem.dados[i]);
    free(buffer);
    return 0;
}*/
