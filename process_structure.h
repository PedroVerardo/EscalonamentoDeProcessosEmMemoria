#include <sys/types.h>
#define MAX_NAME_LEN 256
#define QTD_MAX 100

typedef struct {
    int operations[50];
    int processName;
    int inMemory;
    int positionInMemory;
    int isBlocked; // se 0, está pronto. se 1, esta bloqueado.
    int currentOperationIndex;
    int size_in_kb;
} Process;