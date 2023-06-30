#include "fits.h"
#include "data_structures.h"
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
int second_reference;
char vizualize_arr[16][2];

void manager(short* stack){

}  

char covert_int_to_char(int val){
    char zero = '0';
    return zero + val;
}

void inicialize_arr(char* arr[16][2]){
    for(int i = 0; i < 16; i++){
        for(int j = 0; j < 2; j++){
            arr[i][j] = '0';
        }
        
    }
}

void print_arr_char(char* vizualizer_arr[16][2]){
    for(int i = 0; i < 16; i++){
        printf("[");
        for(int j = 0; j < 2; j++){
            printf("%c",vizualizer_arr[i][j]);
        }
        printf("]");
    }
    printf("\n");
}

void vizualizer(char* arr[16][2], Process p, int ini_pos){
    int tam = calculate_size(p.size_in_byts);
    int ini = ini_pos;
    int cont = 0;
    for(int i = 0; i < 16; i++){
        if(i >= ini_pos && i <= ini_pos + tam- 1){
            if(i >=ini_pos + p.size_in_byts){
                arr[i][0] = 'X';
                arr[i][1] = 'X';
                continue;
            }
            arr[i][0] = 'P';
            arr[i][1] = covert_int_to_char(p.name2);
        }
    }

    print_arr_char(arr);
}

int main(void){
    short stack = 0;
    int cont = 0;
    int is_running = 0;
    Queue queue;
    Queue blocked_queue;
    Process* actual;
    struct timeval current_time;
    int current_second;

    initialize(&queue);
    initialize(&blocked_queue);
    inicialize_arr(vizualize_arr);

    int qtd_process = interpreter(&queue);
    

    while(1){
        if(is_empty(&queue) && is_empty(&blocked_queue) && !is_running){
            printf("nao ha mais processos\n");
            break;
        }

        if (gettimeofday(&current_time, NULL) == -1) {
            printf("Erro na verificação do horário");
            exit(1);
        }
        second_reference = current_time.tv_sec % 60;

        //nao funciona
        if(cont%4 == 0 && current_second != second_reference && is_running == 0){
            actual = dequeue(&queue);
            printf("Process name: p%d\n", actual->name2);

            int ini_pos = worst_fit(actual->size_in_byts, &stack);
            printBits(stack);
            if(ini_pos != -1){
                vizualizer(&vizualize_arr, *actual, ini_pos);
            }
            
            is_running = 0;
        }

        if (current_second != second_reference){
            printf("[CLOCK] segundo atual = %d\n", second_reference);
            current_second = second_reference;
            cont ++;
            
        } 

        
        
        
    }
}
