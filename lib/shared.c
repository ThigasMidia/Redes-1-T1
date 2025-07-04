#include "../include/shared.h"

long long timestamp() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec*1000 + tp.tv_usec/1000;
}

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
    
    // pro timeout
    struct timeval timeout = { .tv_sec = TIMEOUTMILLIS/1000, .tv_usec = (TIMEOUTMILLIS%1000) * 1000 };
    setsockopt(soquete, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout, sizeof(timeout));
    
    return soquete;
}

//MONTA UMA MENSAGEM SEM DADOS
void criaMensagem(pacote_t *mensagem, unsigned char tamanho, unsigned char *sequencia, unsigned char tipo, unsigned char *dados) {
    
    mensagem->tamanho = tamanho;
    mensagem->sequencia = (*sequencia);
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
int encheBuffer(unsigned char *buffer, pacote_t *mensagem, int *tam129, int preciso, unsigned char *corrections) {
    
    //COLOCA O MARCADOR DE INICIO
    buffer[0] = MARCADOR_INI;
    //7 BITS DE TAMANHO + BIT MAIS SIGNIFICATIVO DE SEQUENCIA
    buffer[1] = (mensagem->tamanho << 1) | (mensagem->sequencia >> 4);
    //4 BITS MENOS SIGNIFICATIVOS DE SEQUENCIA + 4 BITS DE TIPO
    buffer[2] = (mensagem->sequencia << 4) | mensagem->tipo;
    
    //COPIA TODOS OS DADOS A PARTIR DA POSICAO 5 DO BUFFER
    memcpy(&buffer[4], mensagem->dados, mensagem->tamanho); 
    
    if(mensagem->tipo == 5)
        (*tam129) = corrige129(buffer, preciso, corrections);
    //CHECKSUM ARMAZENADO NA POSICAO 4 DO BUFFER
    buffer[3] = checksum(buffer);
    int tam = mensagem->tamanho + 4;
    if(tam < 14) { 
        memset(buffer + tam, 0, 14 - tam);
        tam = 14;
    }
    //DEVOLVE TAMANHO DO BUFFER PARA USO POSTERIOR
    return tam;
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

//FUNCAO DE ENVIO DE MENSAGEM GENERICA. MEXE NA SEQUENCIA
void enviaMensagem(int socket, unsigned char *buffer, unsigned char *sequencia, int tam) {
    if(send(socket, buffer, tam, 0) < 0)
        perror("ERRO AO ENVIAR MENSAGEM");
    incrementaSequencia(sequencia);
}

//FUNCAO DE ENVIO DE ACKS PARA FACILITAR
void enviaACK(int socket, unsigned char *sequencia) {
    pacote_t mensagem;
    unsigned char *buffer;
    buffer = malloc(MAX_BUFFER);
    criaMensagem(&mensagem, 0, sequencia, 0, NULL);
    int tam = encheBuffer(buffer, &mensagem, NULL, 0, NULL);
    enviaMensagem(socket, buffer, sequencia, tam);
    free(buffer);
}

//FUNCAO DE ENVIO DE NACKS PARA FACILITAR
void enviaNACK(int socket, unsigned char *sequencia) {
    pacote_t mensagem;
    unsigned char *buffer;
    buffer = malloc(MAX_BUFFER);
    criaMensagem(&mensagem, 0, sequencia, 1, NULL);
    int tam = encheBuffer(buffer, &mensagem, NULL, 0, NULL);
    enviaMensagem(socket, buffer, sequencia, tam);
    free(buffer);
}

//CORRIGE PADROES DE 129 E 136 NAS MENSAGENS
int corrige129(unsigned char *buffer, int preciso, unsigned char *corrections) {
    int retIt = 0;
    //SE EH NECESSARIO 
    if(preciso) {
        int i, tam = buffer[1] >> 1;
        //CHECA OS DADOS
        for(i = 4; i < tam+3; i++) {
            //SE A PROXIMA EH 0, TROCA POR 255
            if((buffer[i] == 129 || buffer[i] == 136) && buffer[i+1] == 0) {
                corrections[retIt] = i;
                retIt++;
                buffer[i+1] = 255;
            }
            //SE EH 136 E O PROXIMA EH 168, TROCA POR 254
            else if(buffer[i] == 136 && buffer[i+1] == 168) {
                corrections[retIt] = i;
                buffer[i+1] = 254;
                retIt++;
            }
        }
    }
    return retIt;
}

//RESTAURA A MENSAGEM PARA ANTES DA CORRECAO DE 129 E 136
void restaura129(unsigned char *buffer, int preciso, unsigned char *corrections) {
    int tam = buffer[1] >> 1, errorIt = 0;
    //SE EH NECESSARIO
    if(preciso) {
        for(int i = 4; i < tam+3; i++) {
            //TROCA 255 PARA 0
            if((buffer[i] == 129 || buffer[i] == 136) && buffer[i+1] == 255 && corrections[errorIt] == i) {
                buffer[i+1] = 0;
                errorIt++;
            }
            //TROCA 254 PARA 168
            else if(buffer[i] == 136 && buffer[i+1] == 254 && corrections[errorIt] == i) {
                buffer[i+1] = 168;
                errorIt++;
            }
        }
    }
}


//CHECA ORDEM DA SEQUENCIA
int depoisDe(unsigned char sequencia1, unsigned char sequencia2) {

    return (sequencia1 - sequencia2 + 32) % 32 < 16;
}

//AUMENTA SEQUENCIA
void incrementaSequencia(unsigned char *sequencia) {
    (*sequencia) = ((*sequencia) + 1) % 32;
}

//DIMINUI SEQUENCIA
void decrementaSequencia(unsigned char *sequencia) {
    (*sequencia) = ((*sequencia) + 31) % 32;
}
