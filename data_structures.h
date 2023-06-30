#include "process_structure.h"
#define MAX_PROCESS_COUNT 20

// lista ordenada
typedef struct Node {
    Process *process;
    struct Node *next;
} Node;

Node* initialize_list();
Node* insert_ordered(Node **head, Process *new_process);
void print_list(Node *head);
Node* get_next_process(Node* head, Node* element);


// Fila
typedef struct Queue {
    Process* arr[MAX_PROCESS_COUNT];
    int front;
    int rear;
    int size;
} Queue;

void initialize(Queue *queue);
int is_empty(Queue *queue);
int is_full(Queue *queue);
void enqueue(Queue *queue, Process* value);
Process* dequeue(Queue *queue);
void printQueue(Queue *queue);


// escalonador
void scheduler(Process* process_data);

// interpretador
int interpreter(Queue* list);