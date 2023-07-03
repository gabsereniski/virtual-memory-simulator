#ifndef QUEUE_H
#define QUEUE_H

#define MAX_SIZE 512

typedef struct {
    int data[MAX_SIZE];
    int front;
    int back;
} queue;

void queue_init(queue *queue);
int queue_is_full(queue *queue);
int queue_is_empty(queue *queue);
void queue_push(queue *queue, int value);
int queue_pop(queue *queue);
int queue_front(queue *queue);

#endif
