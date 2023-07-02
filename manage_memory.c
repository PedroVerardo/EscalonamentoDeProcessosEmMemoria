#include "fits.h"
#include "data_structures.h"
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include "partition.h"
int second_reference;
char vizualize_arr[16][2];

void manager(short* stack){

}  

char covert_int_to_char(int val){
    char zero = '0';
    return zero + val;
}

void inicialize_arr(char arr[16][2]){
    for(int i = 0; i < 16; i++){
        for(int j = 0; j < 2; j++){
            arr[i][j] = '0';
        }
        
    }
}

void print_arr_char(char vizualizer_arr[16][2]){
    for(int i = 0; i < 16; i++){
        printf("[");
        for(int j = 0; j < 2; j++){
            printf("%c",vizualizer_arr[i][j]);
        }
        printf("]");
    }
    printf("\n");
}

void vizualizer(char arr[16][2], Process p, int ini_pos){
    int tam = calculate_size(p.size_in_kb);
    // int ini = ini_pos;
    // int cont = 0;
    for(int i = 0; i < 16; i++){
        if(i >= ini_pos && i <= ini_pos + tam- 1){
            if(i >=ini_pos + p.size_in_kb){
                arr[i][0] = 'X';
                arr[i][1] = 'X';
                continue;
            }
            arr[i][0] = 'P';
            arr[i][1] = covert_int_to_char(p.processName);
        }
    }

    print_arr_char(arr);
}

void* blockWhileIO(void* process) {
    printf("Thread is running\n");
    Process* p = (Process*) process;
    int time = p->operations[p->currentOperationIndex + 1];
    printf("tempo de IO: %d\n", time);
    sleep(time);
    printf("IO finalizado. Adicionando o processo P%d na fila de prontos\n", p->processName);
    pthread_exit(NULL);
}


int main(void){
    short stack = 0;
    int nCurrentProcessTimeSlice = 0;
    int is_running = 0;
    Queue queue;
    Queue blocked_queue;
    Process* currentProcess;
    struct timeval current_time;
    int current_second;
    pthread_t thread_IO;

    initialize(&queue);
    initialize(&blocked_queue);
    inicialize_arr(vizualize_arr);

    // talvez utilizemos o número de processos
    int qtd_process = interpreter(&queue);
    

    while(1){

        if (gettimeofday(&current_time, NULL) == -1) {
            printf("Erro na verificação do horário");
            exit(1);
        }
        second_reference = current_time.tv_sec;

        // passaram-se 4 time slices e algum processo está executando
        if(is_running == 1 && nCurrentProcessTimeSlice % 4 == 0){
            is_running = 0;

            // verificar se a próxima execução é IO
            int index = currentProcess->currentOperationIndex;
            if (currentProcess->operations[index] == 0 && currentProcess->operations[index + 1] != 0) {
                // proxima operação é um IO
                // coloca na lista de bloqueados
                currentProcess->isBlocked = 1;
                enqueue(&blocked_queue, currentProcess);

                // Abre thread que verifica o tempo do IO
                // int IO_time = currentProcess->operations[index + 1];
                if (pthread_create(&thread_IO, NULL, blockWhileIO, currentProcess) != 0) {
                    printf("Erro na criacao da thread");
                    exit(EXIT_FAILURE);
                }

            }

            // se sim, .  e da wait nesse tempo.
            // se não, coloca na fila de prontos.
            
        }

        // verificar se currentProcess->operations[index] == 0
        // se sim, currentProcess->operations[index] += 1

        if(is_empty(&queue) && is_empty(&blocked_queue) && !is_running){
            printf("nao ha mais processos\n");
            break;
        }

        // coloca um novo processo para executar
        if(nCurrentProcessTimeSlice%4 == 0 && current_second != second_reference){
            currentProcess = dequeue(&queue);

            printf("Process name: p%d\n", currentProcess->processName);
            printf("process time: %d\n",currentProcess->operations[currentProcess->currentOperationIndex]);

            int ini_pos = worst_fit(currentProcess->size_in_kb, &stack);
            printBits(stack);
            
            if(ini_pos != -1){
                
            }
            vizualizer(vizualize_arr, *currentProcess, ini_pos);
            is_running = 1;
        }

        if (current_second != second_reference){
            printf("[CLOCK] segundo atual = %d\n", second_reference);
            current_second = second_reference;
            nCurrentProcessTimeSlice ++;
            // atualizar o operation number
            int index = currentProcess->currentOperationIndex;
            currentProcess->operations[index] -= 1;
        } 

        
        
        
    }
}

