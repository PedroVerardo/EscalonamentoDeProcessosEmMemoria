#include <stdio.h>
#include <stdlib.h>
#include "data_structures.h"

void initialize(Queue *queue) {
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
}

int is_empty(Queue *queue) {
    return queue->size == 0;
}

int is_full(Queue *queue) {
    return queue->size == MAX_PROCESS_COUNT;
}

void enqueue(Queue *queue, Process* value) {
    if (is_full(queue)) {
        printf("Error: Queue is full\n");
        exit(1);
    }
    queue->arr[queue->rear] = value;
    queue->rear = (queue->rear + 1) % MAX_PROCESS_COUNT;
    queue->size++;
}

Process* dequeue(Queue *queue) {
    if (is_empty(queue)) {
        printf("Error: Queue is empty\n");
        exit(1);
    }
    Process* value = queue->arr[queue->front];
    queue->front = (queue->front + 1) % MAX_PROCESS_COUNT;
    queue->size--;
    return value;
}

void printQueue(Queue *queue) {
    if (is_empty(queue)) {
        printf("Queue is empty\n");
        return;
    }

    printf("Queue elements:\n");

    int count = queue->size;
    int i = queue->front;

    while (count > 0) {
        printf("Element %d: %s\n", i, queue->arr[i]->name);
        i = (i + 1) % MAX_PROCESS_COUNT;
        count--;
    }
}


// int main() {
//     Queue my_queue;
//     initialize(&my_queue);

//     Process p1 = {"teste", 0, 0, 0};
//     Process p2 = {"teste2", 0, 0, 0};
//     Process p3 = {"teste3", 0, 0, 0};

//     // Enqueue elements
//     enqueue(&my_queue, &p1);
//     enqueue(&my_queue, &p2);
//     enqueue(&my_queue, &p3);

//     // Dequeue elements and print their values
//     printf("Dequeued value: %s\n", dequeue(&my_queue)->name);
//     printf("Dequeued value: %s\n", dequeue(&my_queue)->name);
//     printf("Dequeued value: %s\n", dequeue(&my_queue)->name);

//     return 0;
// }
