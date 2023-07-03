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
short stack = 0;
Queue inMemory_;
int in_io = 0;

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
    // int ini = ini_pos;
    // int cont = 0;
    if(ini_pos == -1){
        print_arr_char(arr);
        return;
    }

    for(int i = 0; i < 16; i++){
        if(i >= ini_pos && i <= ini_pos + p.size_in_kb- 1){
            arr[i][0] = 'P';
            arr[i][1] = covert_int_to_char(p.processName);
        }
    }

    print_arr_char(arr);
}

void remove_process_vizualizer(char arr[16][2], Process p){
    for(int i = 0; i < 16; i++){
        if(i >= p.positionInMemory && i <= p.positionInMemory + p.size_in_kb- 1){
            arr[i][0] = '0';
            arr[i][1] = '0';
        }
    }
}

void* blockWhileIO(void* process) {
    Process* p = (Process*) process;
    in_io = 1;
    int time = p->operations[p->currentOperationIndex + 1];
    printf("Bloqueando P%d para IO. Tempo de IO: %d\n", p->processName, time);
    sleep(time);
    
    printf("IO finalizado. Retirando o processo P%d da fila de bloqueados\n", p->processName);
    if(!is_empty(&blocked_queue)){
        Process* new = dequeue(&blocked_queue); // retirando da fila de bloqueados
        Process* new2 = new;
        while(new->processName != p->processName){
            enqueue(&blocked_queue,new);
            new = dequeue(&blocked_queue);
            if(new2 == new){
                printf("processo p%d nao esta na fila de bloqueados\n", p->processName);
                break;
            }
        }
    }
    else{
        printf("Fila de bloqueados vazia processo P%d ja esta fora\n", p->processName);
    }

    if (p->operations[p->currentOperationIndex + 2] == 0) { // processo finalizado
        printf("Processo P%d finalizado\n", p->processName);
        deallocate(p->size_in_kb, p->positionInMemory, &stack);
        remove_process_vizualizer(vizualize_arr, *p);
        free(process);
    } else {
        printf("Adicionando o processo P%d na fila de prontos\n", p->processName);
        p->currentOperationIndex += 2;
        enqueue(&queue, p);
        p->isBlocked = 0;
    }
    in_io = 0;
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
        enqueue(&inMemory_,currentProcess);
        currentProcess->isBlocked = 0;
    } else if (currentProcess->operations[index + 1] == 0) { // processo finalizado
        printf("O processo P%d foi finalizado\n", currentProcess->processName);
        deallocate(currentProcess->size_in_kb, currentProcess->positionInMemory, &stack);
        remove_process_vizualizer(vizualize_arr, *currentProcess);
        free(currentProcess);
    }            

}


int main(void){
    int nCurrentProcessTimeSlice = 0;
    int is_running = 0;
    Process* currentProcess;
    Process* blockedProcess;
    struct timeval current_time;
    int current_second;
    int ini_pos;
    

    initialize(&inMemory_);
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


        if(is_empty(&queue) && !is_running && !in_io){
            printf("nao ha mais processos\n");
            break;
        }

        // coloca um novo processo para executar
        if(nCurrentProcessTimeSlice % 4 == 0 && current_second != second_reference && !is_empty(&queue)){
            currentProcess = peek(&queue);

            printf("Process name: p%d\n", currentProcess->processName);
            printf("process time: %d\n",currentProcess->operations[currentProcess->currentOperationIndex]);


            if(!currentProcess->inMemory){
                //int blocked_queue_empty = 0;
                ini_pos = worst_fit(currentProcess->size_in_kb, &stack);
            
                while(ini_pos == -1){
                    // printQueue(&blocked_queue);
                    // printBits(stack);
                    if(!is_empty(&blocked_queue)){
                        blockedProcess = dequeue(&blocked_queue);
                        deallocate(blockedProcess->size_in_kb, blockedProcess->positionInMemory, &stack);
                        blockedProcess->inMemory = 0;
                        remove_process_vizualizer(vizualize_arr, *blockedProcess);
                        ini_pos = worst_fit(currentProcess->size_in_kb, &stack);
                    }
                    else{
                        currentProcess = dequeue(&queue);
                        enqueue(&queue,currentProcess);
                        currentProcess = peek(&queue);
                        if(currentProcess->inMemory){
                            ini_pos = currentProcess->positionInMemory;
                        }
                    }
                }

                dequeue(&queue);
                currentProcess->inMemory = 1;
                currentProcess->positionInMemory = ini_pos;
                vizualizer(vizualize_arr, *currentProcess, ini_pos);
               
            }
            
            else{
                dequeue(&queue);
                vizualizer(vizualize_arr, *currentProcess, -1);
            } 

            printBits(stack);
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

