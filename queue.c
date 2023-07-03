#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void queue_init(queue *queue) {
    queue->front = -1;
    queue->back = -1;
}

int queue_is_full(queue *queue) {
    return (queue->back == MAX_SIZE - 1);
}

int queue_is_empty(queue *queue) {
    return (queue->front == -1 || queue->front > queue->back);
}

void queue_push(queue *queue, int value) {
    if (queue_is_full(queue)) {
        return;
    }

    if (queue_is_empty(queue)) {
        queue->front = 0;
    }

    queue->back++;
    queue->data[queue->back] = value;
}

int queue_pop(queue *queue) {
    if (queue_is_empty(queue)) {
        return -1;
    }

    int value = queue->data[queue->front];
    queue->front++;

    if (queue->front > queue->back) {
        queue_init(queue);
    }

    return value;
}

int queue_front(queue *queue) {
    if (queue_is_empty(queue)) {
        return -1;
    }

    return queue->data[queue->front];
}
