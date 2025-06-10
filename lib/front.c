#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "front.h"

void iniciaTabuleiro (Tabuleiro* t) {
    for (int i = 7; i >= 0; i--) {
        for (int j = 0; j < 8; j++) {
            t->grid[i][j] = VAZIO;
        }
    }
    
    t->p.x = 0;
    t->p.y = 0;
    
    return;
} 

void printTabuleiro (Tabuleiro* t, char player) {
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
            if (t->grid[i][j] != TSORO) {
                if (i == t->p.y && j == t->p.x) {
                    printf("\033[48;2;158;214;112m     %c     ", player);
                } else {
                    printf("\033[48;2;158;214;112m     %c     ", t->grid[i][j]);
                }
            } else {
                if (i == t->p.y && j == t->p.x) {
                    printf("\033[48;2;158;214;112m    \033[48;2;232;209;116m %c \033[48;2;158;214;112m    ", player);
                } else {
                    printf("\033[48;2;158;214;112m    \033[48;2;232;209;116m %c \033[48;2;158;214;112m    ", t->grid[i][j]);
                }
            }
        }  
        printf("\033[48;2;214;77;77m  \n"); 
    }
    printf("\033[48;2;214;77;77m                                                                                            \n");
    printf("\033[0m \n");
    
    return;
}

void movePlayer (Tabuleiro* t, int direcao) {
    switch (direcao) {
        case L:
            if (t->p.x == 0) {
                printTabuleiro(t, ERRO);
                sleep(1);
                printTabuleiro(t, DOGUI);
                return;
            }
            
            if (t->grid[t->p.y][t->p.x] != TSORO)
                t->grid[t->p.y][t->p.x] = PASSO;
            t->p.x--; 
            printTabuleiro(t, DOGUI);
            
            break;
        
        case R:
            if (t->p.x == 7) {
                printTabuleiro(t, ERRO);
                sleep(1);
                printTabuleiro(t, DOGUI);
                return;
            }
            
            if (t->grid[t->p.y][t->p.x] != TSORO)
                t->grid[t->p.y][t->p.x] = PASSO;
            t->p.x++;
            printTabuleiro(t, DOGUI);
            
            break;
        
        case U:
            if (t->p.y == 7) {
                printTabuleiro(t, ERRO);
                sleep(1);
                printTabuleiro(t, DOGUI);
                return;
            }
            
            if (t->grid[t->p.y][t->p.x] != TSORO)
                t->grid[t->p.y][t->p.x] = PASSO;
            t->p.y++; 
            printTabuleiro(t, DOGUI);
            
            break;
        
        case D:
            if (t->p.y == 0) {
                printTabuleiro(t, ERRO);
                sleep(1);
                printTabuleiro(t, DOGUI);
                return;
            }
            
            if (t->grid[t->p.y][t->p.x] != TSORO)
                t->grid[t->p.y][t->p.x] = PASSO;
            t->p.y--;
            printTabuleiro(t, DOGUI);
            
            break;
        
        default:
            printTabuleiro(t, ERRO);
            sleep(1);
            printTabuleiro(t, DOGUI);
            break;
    }
    
    return;
}
