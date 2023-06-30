#include "fits.h"
#include "data_structures.h"
#include <stdio.h>

void manager(short* stack){

}  

int main(void){
    short stack = 0;
    Queue queue;
    Queue blocked_queue;
    initialize(&queue);
    initialize(&blocked_queue);

    int qtd_process = interpreter(&queue);

    // while(1){
    //     int is_running = 0;
    //     if(is_empty(&queue) && is_empty(&blocked_queue) && !is_running){
    //         printf("acabou os processos");
    //         break;
    //     }
    // }
}
