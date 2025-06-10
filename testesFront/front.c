#include <stdio.h> 
#include <stdlib.h>

#define DOGUI   'M'
#define VAZIO   '.'
#define PASSO   '*'
#define TSORO   'x'

void iniciaTabuleiro (char tabuleiro[8][8]) {
    for (int i = 7; i >= 0; i--) {
        for (int j = 0; j < 8; j++) {
            tabuleiro[i][j] = VAZIO;
        }
    }
    
    tabuleiro[0][0] = DOGUI;
    
    return;
} 

void printTabuleiro (char tabuleiro[8][8]) {
    //printf("\e[2J\e[H"); 
    system("clear");
    
    printf("\033[48;2;214;77;77m                                                                                          ");
    printf("\033[48;2;214;77;77m  \n  ");
    printf("\033[38;2;214;77;77;48;2;232;209;116m       |\\_/|                     __   __   __           __                  __    /  /  "); //1
    printf("\033[48;2;214;77;77m  \n  ");
    printf("\033[48;2;232;209;116m       | \033[38;2;77;115;88m@ @  \033[38;2;214;77;77m Woof!            |__) /  \\ |__)  /\\     /  `  /\\  \\  /  /\\  |__)  /  /   "); //5
    printf("\033[48;2;214;77;77m  \n  ");
    printf("\033[48;2;232;209;116m       |   <>              _    |__) \\__/ |  \\ /~~\\    \\__, /~~\\  \\/  /~~\\ |  \\ .  .    "); //8
    printf("\033[48;2;214;77;77m  \n  ");
    printf("\033[48;2;232;209;116m       |  _/\\------____ ((| |))                                                         "); //1
    printf("\033[48;2;214;77;77m  \n  ");
    printf("\033[48;2;232;209;116m       |               `--' |                                                           ");
    printf("\033[48;2;214;77;77m  \n  ");
    printf("\033[48;2;232;209;116m   ____|_       ___|   |___.'                                                           ");
    printf("\033[48;2;214;77;77m  \n  ");
    printf("\033[48;2;232;209;116m  /_/_____/____/_______|                                                                ");
    printf("\033[48;2;214;77;77m  \n  ");
    printf("\033[48;2;214;77;77m                                                                                          \n");
    
    printf("\033[38;2;77;115;88m");
    
    for (int i = 7; i >= 0; i--) {
        printf("  ");
        printf("\033[48;2;158;214;112m");
        for (int j = 0; j < 8; j++) {
            printf("     %c     ", tabuleiro[i][j]);
        }  
        printf("\033[48;2;214;77;77m  \n"); 
    }
    printf("\033[48;2;214;77;77m                                                                                            \n");
    printf("\033[0m");
    
    return;
}

int main() {
    char seta = 'w';
    char tabuleiro[8][8];
    
    //printf("\e[2J\e[H"); // se quiser manter historico
    system("clear"); // se quiser apagar bonitinho
    iniciaTabuleiro(tabuleiro);
    
    while ( (seta == 'w') | (seta == 'a') | (seta == 's') | (seta == 'd') ) {
        printTabuleiro(tabuleiro);
        seta = getchar();
        getchar();
        
        //printf("\e[2J\e[H"); 
        system("clear"); 
    }
    
    return 0;
}
