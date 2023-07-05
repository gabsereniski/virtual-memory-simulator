/*
    Estrutura de fila em vetor circular
    Autores: Gabriela Paola Sereniski
             João Victor Salvi Silva
*/

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void queue_init(queue *queue)
{
    queue->front = 0;
    queue->back = 0;
    queue->is_full = 0;
}

int queue_is_empty(queue *queue) 
{
    return (queue->front == queue->back && !queue->is_full);
}

int queue_is_full(queue *queue) 
{
    return queue->is_full;
}

void queue_push(queue *queue, int value) 
{
    if(queue_is_full(queue)) return;

    queue->data[queue->back] = value;
    queue->back = (queue->back + 1) % MAX_SIZE;

    if (queue->back == queue->front)
        queue->is_full = 1;
}

int queue_pop(queue *queue) 
{
    if (queue_is_empty(queue)) return -1;

    int value = queue->data[queue->front];
    queue->front = (queue->front + 1) % MAX_SIZE;
    queue->is_full = 0;

    return value;
}

int queue_front(queue *queue)
{
    if (queue_is_empty(queue)) return -1;
    return queue->data[queue->front];
}
