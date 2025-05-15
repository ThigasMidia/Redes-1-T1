#include "../include/serverFunc.h"

void enviaArquivo(int socket, unsigned char *bufferSend, char *nome, unsigned char *sequencia) {
    FILE *arquivo = fopen(nome, "rb");
    if(!arquivo) {
        perror("ERRO AO CRIAR ARQUIVO");
        exit(1);
    }

    unsigned char liberado = 1, terminado = 0, errorM = 0, sequenciaAnt = 126, sequenciaAtu = 0;
    int bytes = 0, ret, tam, tam129;
    pacote_t messageSend, messageReceive;
    unsigned char *bufferReceive, *dados, *erros;
    bufferReceive = malloc(MAX_BUFFER);
    dados = malloc(MAX_DADOS);
    erros = malloc(MAX_DADOS);
    messageSend.tipo = 0;
    int cont = 0;
    tam129 = 256;

    while(!terminado) {
        cont++;
        //printf("TIPO/LIBERADO/CONT   %d/%d/%d\n", messageSend.tipo, liberado, cont);
        if(liberado) {
            if(errorM) {
                criaMensagem(&messageSend, bytes, sequencia, 5, dados);
                tam = encheBuffer(bufferSend, &messageSend, &tam129, erros, 0);
                enviaMensagem(socket, bufferSend, sequencia, tam);
                errorM--;
                //enterToContinue();
            }

            else if((bytes = fread(dados, 1, MAX_DADOS, arquivo)) > 0) {
                criaMensagem(&messageSend, bytes, sequencia, 5, dados);
                tam = encheBuffer(bufferSend, &messageSend, &tam129, erros, 1);
                if(tam129) { 
                    criaMensagem(&messageSend, 0, sequencia, 3, NULL);
                    tam = encheBuffer(bufferSend, &messageSend, &tam129, erros, 0);
                    errorM++;
                }
                enviaMensagem(socket, bufferSend, sequencia, tam);
                liberado--;
            }
            else { 
                terminado++;
            }
        }
        else {
            recv(socket, bufferReceive, MAX_BUFFER, 0);
            ret = checaMensagem(bufferReceive);
            if(ret == 1) {
                recebeMensagem(bufferReceive, &messageReceive);
                sequenciaAtu = messageReceive.sequencia;
                //printf("ACEITOU!!!!!!!!!!!!!!!!!!!!!!!\n");
                if(sequenciaAnt == 126)
                    sequenciaAnt = (sequenciaAtu + 31) % 32;
                if(depoisDe(sequenciaAtu, sequenciaAnt) && sequenciaAnt != sequenciaAtu) {
                    if(!messageReceive.tipo) {
                        liberado++;
                        tam129 = 256;
                    }
                    else if(messageReceive.tipo == 1) {
                        criaMensagem(&messageSend, bytes, sequencia, 5, dados);
                        tam = encheBuffer(bufferSend, &messageSend, &tam129, erros, 0);
                        enviaMensagem(socket, bufferSend, sequencia, tam);
                    }
                }
            }
            else if(ret == -1) { 
                enviaNACK(socket, sequencia);
            }
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
    free(dados);
    free(bufferReceive);
    free(erros);
}

void enterToContinue() {
    char enter[11];
    printf("ENTER\n");
    scanf("%[^\n]", enter);
    getchar();
}

//
void enviaEOF(int socket, unsigned char *sequencia) {
    pacote_t mensagem;
    unsigned char *buffer;
    buffer = malloc(MAX_BUFFER);
    criaMensagem(&mensagem, 0, sequencia, 9, NULL);
    int tam = encheBuffer(buffer, &mensagem, NULL, NULL, 0);
    enviaMensagem(socket, buffer, sequencia, tam);
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
