#include <sys/types.h>
#define MAX_NAME_LEN 256
#define QTD_MAX 100

typedef struct {
    char name[MAX_NAME_LEN];
    int name2;
    // recebemos a duração mas salvaramos como momento final
    int is_running;
    int cont_exec;
    int cont_IO;
    int is_ready;
    int qtd_execs;
    int qtd_IOs;
    int exec_times[5];
    int is_IO_bounded;
    int IO_time[5];
    int size_in_byts;
    // flag para verificar se já escalonamos o programa
    int already_scheduled; 
    pid_t pid;
} Process;