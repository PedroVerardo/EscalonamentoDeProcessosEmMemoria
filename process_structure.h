#include <sys/types.h>
#define MAX_NAME_LEN 256
#define QTD_MAX 100

typedef struct {
    int name2;          // the name of the process
    // recebemos a duração mas salvaramos como momento final
    int is_running;     // verify if is the process in the runtime
    int qtd_execs;      // number of execs in one process
    int qtd_IOs;        // number of IO's in one process
    int exec_times[5];  // the time value of exec's
    int IO_time[5];     // the time value of IO's
    int cont_exec;      // counter of execs for one process
    int cont_IO;        // counter of IO for one process
    int is_IO_bounded;
    int is_ready;       // if is in the reary queue
    int size_in_byts;   // tam of the process
} Process;