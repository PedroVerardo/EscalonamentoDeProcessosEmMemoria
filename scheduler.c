#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include "data_structures.h"

// definindo as estruturas de dados que represetam as filas de prontos
Queue roundRobin;
Node* realTime;

// guardamos um ponteiro para o próximo processo realtime que será executado
// ex: p1 tem inicio em 9 e p2 em 23
// se estamos no segundo 15, next_realTime = &p2
Node* next_realTime;

// guardamos umponteiro para o processo que esta executando no momento corrente
Process* current_process;

// referência para sabermos o segundo atual dentro do cilco 0 - 59
int second_reference;

// definição de funções
void runNextRealTime();
void runNextRoundRobin(int second_reference);

// quando recebemos um novo processo realtime, devemos verificar se 
// ele deverá ser executado antes do next_realTime guardado
// A função abaixo retorna qual deve ser o próximo real time a ser executado
Node* get_closer(int time_reference, Node* p1, Node* p2) {
    
    // o p2 é o recém inserido
    // o p1 é o next e pode ser null
    if (!p1)
        return p2;

    int n1 = p1->process->initial_time;
    int n2 = p2->process->initial_time;
    int t = time_reference;

    if (t > n1)
        n1 += 60;
    
    if (t > n2)
        n2 += 60;

    int diff1 = n1 - t;
    int diff2 = n2 - t;

    if (diff1 >= diff2)
        return p2;

    return p1;
}



// precisamos verificar se um novo processo realtime é compatível com os demais
// a função indica se existe algum tempo conflitante entre eles
int possibleToScheduled(Node* head, Process real_time_process){
    if(head == NULL){
        return 1;
    }
    Node* current = head;
    int time_ini;
    int time_end;

    while(current != NULL){
        time_ini = current->process->initial_time;
        time_end = current->process->end_time;

        if(real_time_process.initial_time >= time_ini
            && real_time_process.initial_time < time_end){
            return 0;
        }

        if(real_time_process.end_time >= time_ini
            && real_time_process.end_time < time_end){
            return 0;
        }

        if(real_time_process.initial_time < time_ini
            && real_time_process.end_time > time_end){
            return 0;
        }
        current = current->next;
    }
    return 1;
}

// vamos criar uma cópia do processo passado na memória compartilhada
// para que o escalonadro possa armazenar os dados dos processos corretamente
Process* createCopy(Process* original) {

    Process* copy = (Process*)malloc(sizeof(Process));

    // Copiando as informações do processo original
    if (copy != NULL) {
        strcpy(copy->name, original->name);
        copy->initial_time = original->initial_time;
        copy->end_time = original->end_time;
        copy->is_running = original->is_running;
        copy->is_ready = original->is_ready;
        copy->is_IO_bounded = original->is_IO_bounded;
        copy->already_scheduled = original->already_scheduled;
        copy->pid = original->pid;
    }

    return copy;
}

// a rotina abaixo define se um novo processo deve ser executado e
// em caso positivo, define e inicia a execução do novo processo
void updateRunningProcess(int second_reference) {
    if (!current_process) {
        if (next_realTime) {
            if (next_realTime->process->initial_time == second_reference) {
                // se temos um real time para começar
                runNextRealTime();
                return;
            }
        }
        if (!is_empty(&roundRobin)) {
            // se temos algum round robin para começar
            runNextRoundRobin(second_reference); 
        }


        return;     
    }
        
    // se chegou estamos no final do time slice do processo atual
     // precisamos escolher o novo processo a ser executado
    if (current_process->end_time == second_reference) {
       
        // stop o current process
        if (kill(current_process->pid, SIGSTOP) == -1) {
            printf("Erro no sinal SIGSTOP");
            exit(1);
        }

        if (current_process->initial_time == -1) {
            // se for um round robin, volta para o fim da fila
            enqueue(&roundRobin, current_process);
        }

        current_process = NULL;

        if (!next_realTime) {
            // se não temos realtimes na fila de prontos, executamos um round robin
            runNextRoundRobin(second_reference);
            return;
        }

        if (next_realTime->process->initial_time == second_reference) {
            // se temos um real time para começar, o executamos
            runNextRealTime();
        } else {
            // se não, executamos o próximo round robin
            runNextRoundRobin(second_reference);
        }
    }
}

void runNextRealTime() {
    // rodar o next_realtime
    printf("[ESCALONADOR] executando o processo %s (real time)\n", next_realTime->process->name);
    if (kill(next_realTime->process->pid, SIGCONT) == -1) {
        printf("Erro no sinal SIGCONT");
        exit(1);
    }

    // Atulizar o next_realtime e o processo corrente
    current_process = next_realTime->process;
    next_realTime = get_next_process(realTime, next_realTime);
}

void runNextRoundRobin(int second_reference) {

    if (is_empty(&roundRobin))
        return;
    // rodar proximo da fila roundrobin
    Process* first_roundRobin = dequeue(&roundRobin);
    printf("[ESCALONADOR] executando o processo %s (round robin)\n", first_roundRobin->name);
    if (kill(first_roundRobin->pid, SIGCONT) == -1) {
        printf("Erro no sinal SIGCONT");
        exit(1);
    }

    // o round robin tem um minuto para executar
    first_roundRobin->end_time = (second_reference + 1) % 60;
    current_process = first_roundRobin;
}

// a rotina implementa os tratamentos necessários quando um novo processo é recebido
void schedule(Process *process_data, pid_t pid, int second_reference) {

    process_data->pid = pid;

    // informamos ao escalonador que ela já foi tratada
    process_data->already_scheduled = 1;

    // criamos uma cópia a ser armazenada pelo escalonador
    Process* new_process = createCopy(process_data);

    if (new_process->initial_time == -1) { // se round robin, adicionamos na fila de prontos
        printf("[ESCALONADOR] alocando %s na fila de prontos round robin\n", process_data->name);
        enqueue(&roundRobin, new_process);

    } else { // se real time, verificamos os conflitos
        if(!possibleToScheduled(realTime, *process_data)){
            printf("[ESCALONADOR] [TEMPOS INCOMPATIVEIS] nao foi possivel alocar %s na fila de prontos real time\n", process_data->name);
        }
        else{ // se não há conflitos, inserimos na fila de prontos do realtime e atualizamos o próximo a executar
            printf("[ESCALONADOR] alocando %s na fila de prontos real time\n", process_data->name);    
            Node* inserted = insert_ordered(&realTime, new_process);
            next_realTime = get_closer(second_reference, next_realTime, inserted);
        }
    }
}

// a rotina é executada em um thread que o simula a presença do IO
void* wait_for_IO(void *IO_process) {
    sleep(3);
    // após aguardar o tempo de IO, botamos o processo de volta na fila
    Process* ready_process = (Process*) IO_process;
    printf("[ESCALONADOR] IO do processo %s finalizado\n", ready_process->name);
    printf("[ESCALONADOR] adicionando o processo %s na fila de prontos\n", ready_process->name);
    enqueue(&roundRobin, ready_process);
    return NULL;
}

// o handler é acionado quando o processo em excução reconhece a presença de um IO
void handler(int signum){
    pthread_t thread_id;

    // criamos um thread para simular o IO
    if (pthread_create(&thread_id, NULL, &wait_for_IO, current_process) != 0) {
        printf("Failed to create thread\n");
        return;
    }

    // retiramos o processo em IO do processo corrente e atualizamos o processo a ser executado
    current_process = NULL;
    updateRunningProcess(second_reference);
}

// BLOCO PRINCIPAL: ESCALONADOR
void scheduler(Process* process_data) {
    pid_t pid;
    int scheduler_pid = getpid();
    struct timeval current_time;
    int current_second;

    // inicializamos as filas de prontos
    initialize(&roundRobin);
    realTime = initialize_list();

    // definimos o handler para o sinal de IO (recebido diretamente dos processos)
    signal(SIGUSR1, handler);

    // loop contínuo
    while (1) {

        // verificamos o tempo a cada iteração
        // se o segundo atual mudar, atualizamos o clock
        if (gettimeofday(&current_time, NULL) == -1) {
            printf("Erro na verificação do horário");
            exit(1);
        }
        second_reference = current_time.tv_sec % 60;

        if (current_second != second_reference){
            printf("[CLOCK] segundo atual = %d\n", second_reference);
            current_second = second_reference;
        } 

        // se o programa ainda não tiver sido tratado pelo escalonador
        // criamos um novo processo para executar o programa
        if (!process_data->already_scheduled) {
            pid = fork();
            if (pid == -1) {
                printf("Erro na criação de novo processo");
                exit(1);
            } else if (pid > 0) { // escalonador
                // tratamentos preliminares do processo
                schedule(process_data, pid, second_reference);

                // se for o primeiro processo de todos
                if (!current_process && !next_realTime) {     
                    if (process_data->initial_time == -1) {
                        // caso seja um round robin
                        runNextRoundRobin(second_reference);
                    } else {
                        // caso seja um real time
                        next_realTime = realTime;
                    }
                }
                
            } else { // novo processo
                // O novo processo também cria uma cópia para obter um referência de suas informações
                Process* self_information = createCopy(process_data);

                // criamos buffers para passar inteiro como parâmetros via argv
                char buffer_IO[10];
                char buffer_pid[10];

                // convertemos os inteiros para strings por meio dos buffers
                sprintf(buffer_IO, "%d", self_information->is_IO_bounded);
                sprintf(buffer_pid, "%d", scheduler_pid);

                // definimos os parâmetros para executar o programa externo
                char *programPath = "./new_process"; 
                char *args[] = {programPath, self_information->name, buffer_IO, buffer_pid, NULL};
                execvp(programPath, args);
            }
                
        }

        // se estiver no momento de atulizar o processo em execução,
        // realizamos a atualização
        updateRunningProcess(second_reference);
    }

}
