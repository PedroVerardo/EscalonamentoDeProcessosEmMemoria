#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

// processo genérico que é executado pelo escalonador
int main(int argc, char *argv[]){

    struct timeval start, current;

    // obtemos as informações passadas pelo escalonador
    char* name = argv[1];
    int is_IO_bounded = atoi(argv[2]);
    int scheduler_pid = atoi(argv[3]);

    // congelamos o processo pois queremos que ele execite efetivamente somente quando 
    // for a escolha do escalonador (e não diretamente com a sua criação)
    if (kill(getpid(), SIGSTOP) == -1) {
        printf("Erro no sinal SIGSTOP");
        exit(1);
    }

    // iniciando efetivamente a execução
    printf("[PROCESSO %s] iniciado execucao\n", name);
    gettimeofday(&start, NULL);
    while(1){
        // obtemos uma referência de tempo e comparamos com a referência obtida acima do loop
        // se a dirença for maior que meio segundo, imprimimos que o processo está executando
        // assim podemos verificar que os sinais de parada e continuação funcionam devidamente
        gettimeofday(&current, NULL);
        double delta = (current.tv_sec - start.tv_sec) + (current.tv_usec - start.tv_usec) / 1000000.0;
        if (delta >= 0.5) {
            printf("[PROCESSO %s] executando\n", name);
            gettimeofday(&start, NULL);
        }

        // se o processo for IO bounded, enviamos um sinal de IO para o escalonador
        // e interrompemos a execução do processo atual
        if (is_IO_bounded) {
            printf("[PROCESSO %s] IO detectado\n", name);
            kill(scheduler_pid, SIGUSR1);
            if (kill(getpid(), SIGSTOP) == -1) {
                printf("Erro no sinal SIGSTOP");
                exit(1);
            }
        }
    }
}