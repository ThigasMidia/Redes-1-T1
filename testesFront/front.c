#include <stdio.h> 
#include <stdlib.h>

int main() {
    char seta = 'w';
    
    //printf("\e[2J\e[H"); // se quiser manter historico
    system("clear"); // se quiser apagar bonitinho
    
    while ( (seta == 'w') | (seta == 'a') | (seta == 's') | (seta == 'd') ) {
        switch (seta) {
            case 'w':
                printf("\033[96mSekai de ichiban ohime-sama\n\033[0m");
                break;
            case 'a':
                printf("\033[30;46msou iu atsukai kokoroete yo nem\n\033[0m");
                break;
            case 's':
                printf("\033[94;40mSono ichi itsumo to chigau kamigata ni ki ga tsuku koto\n\033[0m");
                break;
            case 'd':
                printf("\033[30;44msono ni chanto kutsu made miru koto\n\033[0m");
                break;
            default:
                break;
        } 
        seta = getchar();
        getchar();
        
        //printf("\e[2J\e[H"); 
        system("clear"); 
    }
    
    return 0;
}
