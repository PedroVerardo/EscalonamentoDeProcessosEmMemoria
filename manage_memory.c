#include "fits.h"
#include "data_structures.h"
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "partition.h"

int second_reference;
char vizualize_arr[16][2];
Queue queue;
Queue blocked_queue;

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
    Process* p = (Process*) process;
    int time = p->operations[p->currentOperationIndex + 1];
    printf("Bloqueando P%d para IO. Tempo de IO: %d\n", p->processName, time);
    sleep(time);
    printf("IO finalizado. Retirando o processo P%d da fila de bloqueados\n", p->processName);
    dequeue(&blocked_queue); // retirando da fila de bloqueados
    if (p->operations[p->currentOperationIndex + 2] == 0) { // processo finalizado
        printf("Processo P%d finalizado\n", p->processName);
        free(process);
    } else {
        printf("Adicionando o processo P%d na fila de prontos\n", p->processName);
        p->currentOperationIndex += 2;
        enqueue(&queue, p);
        p->isBlocked = 0;
    }
    pthread_exit(NULL);
}


void interruptCurrentProcess(Process* currentProcess) {

    pthread_t thread_IO;
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

    } else if (currentProcess->operations[index] != 0) { // exec não finalizado. Colocar na lista de prontos
        enqueue(&queue, currentProcess);
        currentProcess->isBlocked = 0;
    } else if (currentProcess->operations[index + 1] == 0) { // processo finalizado
        printf("O processo P%d foi finalizado\n", currentProcess->processName);
        free(currentProcess);
    }            

}


int main(void){
    short stack = 0;
    int nCurrentProcessTimeSlice = 0;
    int is_running = 0;
    Process* currentProcess;
    struct timeval current_time;
    int current_second;

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
            interruptCurrentProcess(currentProcess);            
        }

        if (is_running && currentProcess->operations[currentProcess->currentOperationIndex] == 0) {
            is_running = 0;
            interruptCurrentProcess(currentProcess);
            nCurrentProcessTimeSlice = 0;
        }


        if(is_empty(&queue) && is_empty(&blocked_queue) && !is_running){
            printf("nao ha mais processos\n");
            break;
        }

        // coloca um novo processo para executar
        if(nCurrentProcessTimeSlice % 4 == 0 && current_second != second_reference){
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
            printf("[CLOCK] segundo atual = %d\n", second_reference % 60);
            current_second = second_reference;
            nCurrentProcessTimeSlice ++;
            // atualizar o operation number
            int index = currentProcess->currentOperationIndex;
            if (currentProcess->operations[index] == 0) {
                // se a operação atual foi finalizada, incrementamos o indice para fazer referência
                // a operação seguinte
                currentProcess->currentOperationIndex += 1;
            } else {
                // caso contrário, decrementamos o número de timeslices restantes da operação
                currentProcess->operations[index] -= 1;
            }
        }         
    }
}

