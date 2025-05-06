#include "shared.h"

typedef struct tesouro {
    unsigned char x;
    unsigned char y;
} tesouro;

tesouro aleatorizaTesouro();

int comparaTesouros(tesouro t1, tesouro t2);

void criaTesouros(tesouro *t);
