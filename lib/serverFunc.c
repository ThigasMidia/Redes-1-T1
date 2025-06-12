#include "../include/serverFunc.h"
#include <dirent.h>

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

//INTERPRETA A DIRECAO ENVIADA PELO CLIENTE E ENVIA MENSAGEM CORRESPONDENTE
void interpretaDirecao(int socket, pacote_t direcao, unsigned char *sequencia, Tabuleiro *t, tesouro *tes, unsigned char *bufferSend) {
    int msg = 0;
    switch(direcao.tipo) {
        case 10:
            if(t->p.x != 7) {
                (t->p.x)++;
                msg = 2;
            }
            break;
        case 11:
            if(t->p.y != 7) {
                (t->p.y)++;
                msg = 2;
            }
            break;
        case 12:
            if(t->p.y) {
                (t->p.y)--;
                msg = 2;
            }
            break;
        case 13:
            if(t->p.x) {
                (t->p.x)--;
                msg = 2;
            }
            break;
        default:
            break;
    }
    printf("MENSAGEM: %d       %d/%d\n", msg, t->p.x, t->p.y);
    int posi = checaSeEncontrou(t->p, tes);
    printf("POSI: %d\n", posi);
    if(posi != 8) {
    //LOGICA DE TER ENCONTRADO UM TESOURO
        encontrouArquivo(socket, tes[posi].arquivo, sequencia, bufferSend);
        //LOGICA DE ENVIO DE TAMANHO//
        enviaArquivo(socket, bufferSend, (char *)tes[posi].arquivo, sequencia);
    }
    
    
    else { 
        pacote_t envio;
        criaMensagem(&envio, 0, sequencia, msg, NULL);
        int tam = encheBuffer(bufferSend, &envio, NULL, 0 ,NULL);
        enviaMensagem(socket, bufferSend, sequencia, tam);
    }
}

void encontrouArquivo(int socket, unsigned char *nomearquivo, unsigned char *sequencia, unsigned char *buffer) {
    int tipo;
    pacote_t mensagem;
    unsigned char formato = nomearquivo[3];
    if(formato == 't')
        tipo = 6;
    else if(formato == 'j')
        tipo = 8;
    else
        tipo = 7;

    unsigned char *realNomeArquivo, arquivo[256];
    strncpy(arquivo, nomearquivo + 11, 5);
    realNomeArquivo = arquivo;
    criaMensagem(&mensagem, 5, sequencia, tipo, realNomeArquivo);
    int tam = encheBuffer(buffer, &mensagem, NULL, 0,  NULL);
    enviaMensagem(socket, buffer, sequencia, tam);
    while(tam) {
        recv(socket, buffer, MAX_BUFFER, 0);
        int ret = checaMensagem(buffer);
        if(ret == 1) 
            tam = 0;
        
        else if(ret == -1) 
           enviaNACK(socket, sequencia);
    }
}

//CHECA SE PLAYER ESTA EM UM TESOURO
int checaSeEncontrou(Player p, tesouro *t) {
   int achou = 8, cont = 0;
   printf("POSICOES PLAYER: %d/%d\n", p.x, p.y);
   while(achou == 8 && cont < 8) {
       printf("POSICOES TESOURO: %d/%d      %d\n", t[cont].x, t[cont].y, t[cont].encontrado);
        if(p.y == t[cont].y && p.x == t[cont].x && !t[cont].encontrado) {
            achou = cont;
            t[cont].encontrado = 1;
        }
        cont++;
   }

   return achou;
}

//ALEATORIZA UMA POSICAO
tesouro aleatorizaTesouro() {
    tesouro t;
    t.x = random() % 8;
    t.y = random() % 8;
    while(t.x == 0 && t.y == t.x) {
        t.x = random() % 8;
        t.y = random() % 8;
    }
    return t;
}

//COMPARA PARA SABER SE 2 TESOUROS POSSUEM A MESMA COORDENADA
int comparaTesouros(tesouro t1, tesouro t2) {
    if(t1.x != t2.x || t1.y != t2.y)
        return 1;
    return 0;
}


void leArquivosdeEnvio(char arquivos[8][256]) {
    DIR *diretorio = opendir("./enviados");
    struct dirent *arquivo;
    int tam = 0;
    while((arquivo = readdir(diretorio)) != NULL && tam < 8) {
        if(strcmp(arquivo->d_name, ".") != 0 && strcmp(arquivo->d_name, "..") != 0) {
            strcpy(arquivos[tam], "./enviados/");
            strcat(arquivos[tam], arquivo->d_name);
            tam++;
        }
    }
}

//FUNCAO DE CRIACAO DE TODAS AS POSICOES DE TESOUROS
void criaTesouros(tesouro *t) {
    tesouro tes;
    int tam = 0, comp, i;

    char arquivos[8][256];
    leArquivosdeEnvio(arquivos);

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
            t[tam].arquivo = malloc(256);
            strcpy((char *)t[tam].arquivo, arquivos[tam]);
            t[tam].encontrado = 0;
            tam++;
        }
    }
}
