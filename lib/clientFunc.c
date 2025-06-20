#include "../include/clientFunc.h"

//RECEBE ARQUIVO GENERICO
void recebeArquivo(int socket, char *nome, unsigned char *sequencia) {

    //ABRE ARQUIVO PARA ESCRITA EM BINARIO
    FILE *arquivo = fopen(nome, "wb");
    if(!arquivo) {
        perror("ERRO AO CRIAR ARQUIVO");
        exit(1);
    }
    
    int ret;
    unsigned char terminado = 0, liberado = 1, lastAN = 0, sen129 = 0, sequenciaAnt = 126, sequenciaAtu = 0;
    unsigned char *bufferReceive, *corrections;
    pacote_t messageReceive;
    messageReceive.tipo = 126;
    bufferReceive = malloc(MAX_BUFFER);
    corrections = calloc(MAX_DADOS, 1);

    //LOOP DE RECEBIMENTO DE ARQUIVO
    while(!terminado) {
        //SE ESTA LIBERADO PARA RECEBER DADOS
        if(liberado) {
            recv(socket, bufferReceive, MAX_BUFFER, 0);
            ret = checaMensagem(bufferReceive);
            //SE CHECKSUM ESTA CORRETO
            if(ret == 1) {
                //SE HA NECESSIDADE, RESTAURA OS PADROES PROBLEMATICOS
                restaura129(bufferReceive, sen129, corrections);
                recebeMensagem(bufferReceive, &messageReceive);
                sequenciaAtu = messageReceive.sequencia;
                if(sequenciaAnt == 126)
                    sequenciaAnt = (sequenciaAtu + 31) % 32;

                //SE A SEQUENCIA ESTA CORRETA
                if(depoisDe(sequenciaAtu, sequenciaAnt) && sequenciaAnt != sequenciaAtu) {
                    //SE EH DADOS
                    if(messageReceive.tipo == 5) { 
                        sen129 = 0;
                        fwrite(messageReceive.dados, 1, messageReceive.tamanho, arquivo);
                        liberado--;
                    }
                    //SE EH NACK
                    else if(messageReceive.tipo == 1) {
                        if(!lastAN)
                            enviaACK(socket, sequencia);
                        else
                            enviaNACK(socket, sequencia);
                    }
                    //SE EH EOF
                    else if(messageReceive.tipo == 9) {
                        terminado++;
                        enviaACK(socket, sequencia);
                    }
                    //SE EH TIPO 3, PROXIMA MENSAGEM DE DADOS DEVE SER RESTAURADA
                    else if(messageReceive.tipo == 3) {
                        sen129 = 1;
                        liberado--;
                        memcpy(corrections, messageReceive.dados, messageReceive.tamanho);
                    }
                }
                sequenciaAnt = sequenciaAtu;
            }
            //SE CHECKSUM ESTA ERRADO
            else if(ret == -1) {
                enviaNACK(socket, sequencia);
                lastAN = 1;
            }
        }
        //SE NAO ESTA LIBERADO PARA LER O ARQUIVO
        else {
            enviaACK(socket, sequencia);
            lastAN = 0;
            liberado++;
        }
    }
    //LIBERA MEMORIA
    fclose(arquivo);
    free(bufferReceive);
    free(corrections);
}

void encontreiArquivo(int socket, unsigned char *sequencia) {
    enviaACK(socket, sequencia);
    /*
     *
     * RECEBE TAMANHO DE ARQUIVO
     *
     */
    enviaACK(socket, sequencia);
    //recebeArquivo(socket, /*NOME DO ARQUIVO????*/, sequencia);
}

//FUNCAO PARA ENVIO DE DIRECOES
void enviaDirecao(int socket, unsigned char direcao, unsigned char *sequencia, unsigned char *bufferSend) {
    pacote_t mensagem;
    //CRIA MENSAGEM DE DIRECAO
    switch(direcao) {
        case 10:
            criaMensagem(&mensagem, 0, sequencia, 10, NULL);
            break;
        case 11:
            criaMensagem(&mensagem, 0, sequencia, 11, NULL);
            break;
        case 12:
            criaMensagem(&mensagem, 0, sequencia, 12, NULL);
            break;
        case 13:
            criaMensagem(&mensagem, 0, sequencia, 13, NULL);
            break;
        default:
            break;
    }
    //ENVIA
    int tam = encheBuffer(bufferSend, &mensagem, NULL, 0, NULL);
    enviaMensagem(socket, bufferSend, sequencia, tam);
}
