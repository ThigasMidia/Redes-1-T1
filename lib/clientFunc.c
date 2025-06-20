#include "../include/clientFunc.h"
#include <sys/statvfs.h>
#include <stdint.h>

//RECEBE TAMANHO DO ARQUIVO E CHECA SE POSSUE ESPAÇO EM DISCO
int recebeTamanho(int socket, unsigned char *sequencia, unsigned char *buffer, char *nomearquivo) {

    struct statvfs fs;
    statvfs(nomearquivo, &fs);
    uint64_t espacoDisco = (uint64_t)fs.f_bsize * fs.f_bavail;
    espacoDisco -= (uint64_t)1000000;

    pacote_t mensagem;

    while(1) {
        recv(socket, buffer, MAX_BUFFER, 0);
        int ret = checaMensagem(buffer);
        if(ret == 1) {
            recebeMensagem(buffer, &mensagem);
            uint32_t tamanho = ((uint32_t)buffer[0] << 24) +
                               ((uint32_t)buffer[1] << 16) +
                               ((uint32_t)buffer[2] << 8)  +
                               ((uint32_t)buffer[3]);


            if(espacoDisco - tamanho <= 0) {
                printf("TESOURO ENCONTRADO, MAS NÃO HÁ ESPAÇO EM DISCO SUFICIENTE!!!!!!!\n");
                criaMensagem(&mensagem, 1, sequencia, 15, (unsigned char *)"1");
                int tam = encheBuffer(buffer, &mensagem, NULL, 0, NULL);
                enviaMensagem(socket, buffer, sequencia, tam);
                return 0;
            }
            else {
                enviaACK(socket, sequencia);
                return 1;
            }
        }

        else if(ret == -1)
           enviaNACK(socket, sequencia);
    }
}

//RECEBE ARQUIVO GENERICO
void recebeArquivo(int socket, char *nome, unsigned char *sequencia) {
    
    unsigned char *bufferReceive = malloc(MAX_BUFFER);
    if(recebeTamanho(socket, sequencia, bufferReceive, nome) == 0) {
        free(bufferReceive);
        return;
    }

    //ABRE ARQUIVO PARA ESCRITA EM BINARIO
    FILE *arquivo = fopen(nome, "wb");
    if(!arquivo) {
        perror("ERRO AO CRIAR ARQUIVO");
        exit(1);
    }
    printf("TESOURO ENCONTRADO, BOM GAROTO!!\n");
    int ret;
    unsigned char terminado = 0, liberado = 1, lastAN = 0, sen129 = 0, sequenciaAnt = 126, sequenciaAtu = 0;
    unsigned char *corrections;
    pacote_t messageReceive;
    messageReceive.tipo = 126;
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
