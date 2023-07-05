/*
    Estrutura de fila em vetor circular
    Autores: Gabriela Paola Sereniski
             João Victor Salvi Silva
*/

// estrutura de fila para implementação do FIFO

#ifndef QUEUE_H
#define QUEUE_H

#define MAX_SIZE 8192

typedef struct {
    int data[MAX_SIZE];
    int front;
    int back;
    int is_full;
} queue;

void queue_init(queue *queue);
int queue_is_full(queue *queue);
int queue_is_empty(queue *queue);
void queue_push(queue *queue, int value);
int queue_pop(queue *queue);
int queue_front(queue *queue);

#endif
