#define MARCADOR_INICIO 126
#define MAX_DADOS 127

typedef struct pacote_t {
    unsigned char tamanho;
    unsigned char sequencia;
    unsigned char tipo;
    unsigned char dados[MAX_DADOS];
} pacote_t;
