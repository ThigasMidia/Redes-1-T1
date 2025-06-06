#include "../include/serverFunc.h"

//ENVIA ARQUIVO GENERICO 
void enviaArquivo(int socket, unsigned char *bufferSend, char *nome, unsigned char *sequencia) {

    //ABRE ARQUIVO PARA LEITURA EM BINARIO
    FILE *arquivo = fopen(nome, "rb");
    if(!arquivo) {
        perror("ERRO AO CRIAR ARQUIVO");
        exit(1);
    }
    
    unsigned char liberado = 1, terminado = 0, errorM = 0, sequenciaAnt = 126, sequenciaAtu = 0;
    int bytes = 0, ret, tam, tam129;

    //MENSAGEM ENVIADA E MENSAGEM RECEBIDA
    pacote_t messageSend, messageReceive;

    //BUFFER DE RECEBIMENTO E DADOS LIDOS
    unsigned char *bufferReceive, *dados, *corrections;
    bufferReceive = malloc(MAX_BUFFER);
    dados = malloc(MAX_DADOS);
    corrections = calloc(MAX_DADOS, 1);

    messageSend.tipo = 0;
    tam129 = 0;
    
    //LOOP DE ENVIO DE ARQUIVO
    while(!terminado) {
        //SE ESTA LIBERADO PARA LER O ARQUIVO
        if(liberado) {
            //SE A MENSAGEM ANTERIOR FOI DO TIPO 3 (NECESSIDADE DE CORRIGIR A PROXIMA MENSAGEM)
            if(errorM) {
                criaMensagem(&messageSend, bytes, sequencia, 5, dados);
                tam = encheBuffer(bufferSend, &messageSend, &tam129, 1, corrections);
                enviaMensagem(socket, bufferSend, sequencia, tam);
                errorM--;
                liberado--;
            }
            
            //LOOP NORMAL DE LEITURA DE ARQUIVO
            else if((bytes = fread(dados, 1, MAX_DADOS, arquivo)) > 0) {
                criaMensagem(&messageSend, bytes, sequencia, 5, dados);
                tam = encheBuffer(bufferSend, &messageSend, &tam129, 1, corrections);

                //SE EXISTE ALGUM PADRAO PROBLEMATICO NA MENSAGEM
                if(tam129) {
                    criaMensagem(&messageSend, tam129, sequencia, 3, corrections);
                    tam = encheBuffer(bufferSend, &messageSend, &tam129, 0, corrections);
                    errorM++;
                }
                enviaMensagem(socket, bufferSend, sequencia, tam);
                liberado--;
            }
            //TERMINOU DE LER O ARQUIVO
            else { 
                terminado++;
            }
        }
        //SE NAO ESTA LIBERADO PARA LER O ARQUIVO
        else {
            recv(socket, bufferReceive, MAX_BUFFER, 0);
            ret = checaMensagem(bufferReceive);
            //SE CHECKSUM ESTA CORRETO
            if(ret == 1) {
                recebeMensagem(bufferReceive, &messageReceive);
                sequenciaAtu = messageReceive.sequencia;
                if(sequenciaAnt == 126)
                    sequenciaAnt = (sequenciaAtu + 31) % 32;

                //SE SEQUENCIA ESTA CERTA
                if(depoisDe(sequenciaAtu, sequenciaAnt) && sequenciaAnt != sequenciaAtu) {
                    //SE RECEBEU ACK
                    if(!messageReceive.tipo) {
                        liberado++;
                    }
                    //SE RECEBEU NACK
                    else if(messageReceive.tipo == 1) {
                        criaMensagem(&messageSend, bytes, sequencia, 5, dados);
                        tam = encheBuffer(bufferSend, &messageSend, &tam129, 0, corrections);
                        enviaMensagem(socket, bufferSend, sequencia, tam);
                    }
                }
            }

            //SE CHECKSUM ESTA ERRADO
            else if(ret == -1) 
                enviaNACK(socket, sequencia);
            
            sequenciaAnt = sequenciaAtu;
        }
    }
    
    //ENVIA EOF
    enviaEOF(socket, sequencia);

    //ESPERA PARA RECEBER ACK DO EOF
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

    //LIBERA MEMORIA
    free(dados);
    free(bufferReceive);
    free(corrections);
}

//FUNCAO PARA ENVIO DE EOF
void enviaEOF(int socket, unsigned char *sequencia) {
    pacote_t mensagem;
    unsigned char *buffer;
    buffer = malloc(MAX_BUFFER);
    criaMensagem(&mensagem, 0, sequencia, 9, NULL);
    int tam = encheBuffer(buffer, &mensagem, NULL, 0, NULL);
    enviaMensagem(socket, buffer, sequencia, tam);
    free(buffer);
}

//ALEATORIZA UMA POSICAO
tesouro aleatorizaTesouro() {
    tesouro t;
    t.x = random() % 8;
    t.y = random() % 8;
    return t;
}

//COMPARA PARA SABER SE 2 TESOUROS POSSUEM A MESMA COORDENADA
int comparaTesouros(tesouro t1, tesouro t2) {
    if(t1.x != t2.x || t1.y != t2.y)
        return 1;
    return 0;
}

//FUNCAO DE CRIACAO DE TODAS AS POSICOES DE TESOUROS
void criaTesouros(tesouro *t) {
    tesouro tes;
    int tam = 0, comp, i;

    while(tam < 8) {
        comp = 1;
        i = 0;
        tes = aleatorizaTesouro();
        //LOOP DE COMPARACAO DE TODAS AS POSICOES DO VETOR
        while(comp && i < tam) {
            if(!comparaTesouros(tes, t[i])) 
                comp--;
            i++;
        }
        //COLOCA NO VETOR
        if(comp) {
            t[tam] = tes;
            tam++;
        }
    }
}
