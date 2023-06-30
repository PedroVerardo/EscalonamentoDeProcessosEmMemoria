#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "data_structures.h"

//get the information
int interpreter(Queue* queue) {

    int number_of_process, qtd_info;
    

    FILE *process_file = fopen("exec.txt", "r");
    if (!process_file) {
        printf("Error: fopen failed\n");
        exit(1);
    }

    fscanf(process_file, "%d\n", &number_of_process);
    //printf("%d", number_of_process);
    for(int i = 0; i < number_of_process; i++){
        Process* current_process = (Process*)malloc(sizeof(Process));
        fscanf(process_file, "Processo #%d – %dKb\n", &current_process->name2, &current_process->size_in_byts);
        fscanf(process_file, "%d\n", &qtd_info);
        current_process->qtd_IOs = 0;
        current_process->qtd_execs = 0;
        current_process->cont_exec = 0;
        current_process->cont_IO = 0;
        
        for(int j = 0; j < qtd_info; j++){
            if(j%2 == 0){
                fscanf(process_file, "exec %d\n", &current_process->exec_times[j/2]);
                current_process->qtd_execs += 1;
            }
            else{
                fscanf(process_file, "io %d\n", &current_process->IO_time[j/2]);
                current_process->is_IO_bounded = 1;
                current_process->qtd_IOs += 1;
            }
        }
        current_process->is_ready = 1;
        enqueue(queue, current_process);
        //sleep(1);
    }

    fclose(process_file);
    return number_of_process;
}

void print(Process* p, int k){
    for(int i = 0; i < p->qtd_execs; i++){
        if(k == 1){
            printf("%d ", p->IO_time[i]);
        }
        else{
            printf("%d ", p->exec_times[i]);
        }
    }
    printf("\n");
}

// int main(void){
//     Process* current_process = (Process*)malloc(sizeof(Process));
//     interpreter(current_process);
//     print(current_process, 1);
//     //printf("Name: %d\n",current_process->size_in_byts);
// }