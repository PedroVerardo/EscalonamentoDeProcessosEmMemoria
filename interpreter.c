#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "data_structures.h"

//get the information
int interpreter(Queue* queue) {

    int number_of_process, nProcessOperations;
    

    FILE *process_file = fopen("exec.txt", "r");
    if (!process_file) {
        printf("Error: fopen failed\n");
        exit(1);
    }

    fscanf(process_file, "%d\n", &number_of_process);
    //printf("%d", number_of_process);
    for(int i = 0; i < number_of_process; i++){
        Process* current_process = (Process*)malloc(sizeof(Process));
        fscanf(process_file, "Processo #%d - %dKb\n", &current_process->processName, &current_process->size_in_kb);
        fscanf(process_file, "%d\n", &nProcessOperations);
        
        for(int j = 0; j < nProcessOperations; j++){
            if(j%2 == 0){
                fscanf(process_file, "exec %d\n", &current_process->operations[j]);  
            } else{
                fscanf(process_file, "io %d\n", &current_process->operations[j]);
            }
        }
        current_process->isBlocked = 0;
        current_process->currentOperationIndex = 0;
        current_process->inMemory = 0;
        enqueue(queue, current_process);
    }

    fclose(process_file);
    return number_of_process;
}

// int main(void) {
//     interpreter(NULL);
//     return 0;
// }