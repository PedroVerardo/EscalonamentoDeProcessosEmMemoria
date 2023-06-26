#include "process_structure.h"
#define MAX_PROCESS_COUNT 20

typedef struct Queue {
    Process* arr[MAX_PROCESS_COUNT];
    int front;
    int rear;
} Queue;

void initialize(Queue *queue);
int is_empty(Queue *queue);
int is_full(Queue *queue);
void enqueue(Queue *queue, Process* value);
Process* dequeue(Queue *queue);
void printQueue(Queue *queue);