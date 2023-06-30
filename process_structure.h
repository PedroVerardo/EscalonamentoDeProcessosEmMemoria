#include <sys/types.h>
#define MAX_NAME_LEN 256
#define QTD_MAX 100

typedef struct {
    int name2;
    // recebemos a duração mas salvaramos como momento final
    int is_running;
    int cont_IO;
    int qtd_execs;
    int qtd_IOs;
    int exec_times[5];
    int IO_time[5];
    int size_in_byts;
    int cont_exec;
    int is_IO_bounded;
    int is_ready;
    // flag para verificar se já escalonamos o programa
    int already_scheduled; 
    pid_t pid;
} Process;