#include "serverFunc.h"

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
