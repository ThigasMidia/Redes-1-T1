#include "../include/serverFunc.h"

void enviaArquivo(int socket, unsigned char *bufferSend, char *nome, unsigned char *sequencia) {
    FILE *arquivo = fopen(nome, "rb");
    if(!arquivo) {
        perror("ERRO AO CRIAR ARQUIVO");
        exit(1);
    }
    unsigned char liberado = 1, terminado = 0, sequenciaAnt = 126, sequenciaAtu = 0;
    int bytes = 0, ret;
    pacote_t messageSend, messageReceive;
    unsigned char *bufferReceive, *dados;
    bufferReceive = malloc(MAX_BUFFER);
    dados = malloc(MAX_DADOS);
    while(!terminado) {
        if(liberado) {
            if((bytes = fread(dados, 1, MAX_DADOS, arquivo)) > 0) {
                criaMensagem(&messageSend, bytes, sequencia, 5, dados);
                enviaMensagem(socket, bufferSend, &messageSend, sequencia);
                liberado--;
            }
            else 
                terminado++;
        }
        else {
            recv(socket, bufferReceive, MAX_BUFFER, 0);
            ret = checaMensagem(bufferReceive);
            if(ret == 1) {
                recebeMensagem(bufferReceive, &messageReceive);
                sequenciaAtu = messageReceive.sequencia;
                if(sequenciaAnt == 126)
                    sequenciaAnt = (sequenciaAtu + 31) % 32;
                if(depoisDe(sequenciaAtu, sequenciaAnt)) {
                    if(!messageReceive.tipo)
                        liberado++;
                    else if(messageReceive.tipo == 1) {
                        decrementaSequencia(sequencia);
                        enviaMensagem(socket, bufferSend, &messageSend, sequencia);
                    }
                }
            }
            else if(ret == -1) 
                enviaNACK(socket, sequencia);
            sequenciaAnt = sequenciaAtu;
        }
    }

    enviaEOF(socket, sequencia);
    while(terminado) {
        recv(socket, bufferReceive, MAX_BUFFER, 0);
        ret = checaMensagem(bufferReceive);
        if(ret == 1) {
            recebeMensagem(bufferReceive, &messageReceive);
            if(depoisDe(sequenciaAtu, sequenciaAnt)) {
                if(!messageReceive.tipo)
                    terminado--;
                else if(messageReceive.tipo == 1) {
                    decrementaSequencia(sequencia);
                    enviaEOF(socket, sequencia);
                }
            }
        }
        else if(ret == -1)
            enviaNACK(socket, sequencia);
    }
    printf("TERMINOU DE ENVIAR!!!!!!!\n");
    free(dados);
    free(bufferReceive);
}

//
void enviaEOF(int socket, unsigned char *sequencia) {
    pacote_t mensagem;
    unsigned char *buffer;
    buffer = malloc(MAX_BUFFER);
    criaMensagem(&mensagem, 0, sequencia, 9, NULL);
    enviaMensagem(socket, buffer, &mensagem, sequencia);
    printf("%d\n", buffer[2] & 0x0F);
    free(buffer);
}

tesouro aleatorizaTesouro() {
    tesouro t;
    t.x = random() % 8;
    t.y = random() % 8;
    return t;
}

int comparaTesouros(tesouro t1, tesouro t2) {
    if(t1.x != t2.x || t1.y != t2.y)
        return 1;
    return 0;
}

void criaTesouros(tesouro *t) {
    tesouro tes;
    int tam = 0, comp, i;

    while(tam < 8) {
        comp = 1;
        i = 0;
        tes = aleatorizaTesouro();
        printf("TESTA TES: %dx%d\n", tes.x, tes.y);
        while(comp && i < tam) {
            if(!comparaTesouros(tes, t[i])) {
                comp--;
                printf("REPETIU!\n");
            }
            i++;
        }
        if(comp) {
            t[tam] = tes;
            tam++;
        }
    }
}
