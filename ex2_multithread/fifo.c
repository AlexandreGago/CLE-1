#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "fifo.h"


void fifo_init(fifo_t *fifo, int buffer_size) {
    fifo->buffer = malloc(buffer_size * sizeof(array_t));
    for (int i = 0; i < buffer_size; i++) {
        fifo->buffer[i].array = NULL;
        fifo->buffer[i].size = 0;
    }
    fifo->start = 0;
    fifo->end = 0;
    fifo->count = 0;
    fifo->buf_size = buffer_size;
    
    pthread_mutex_init(&fifo->lock, NULL);
    pthread_cond_init(&fifo->not_empty, NULL);
    pthread_cond_init(&fifo->not_full, NULL);
}


void fifo_push(fifo_t *fifo, array_t* array) {
    pthread_mutex_lock(&fifo->lock);
    while (fifo->count == fifo->buf_size) {
        pthread_cond_wait(&fifo->not_full, &fifo->lock);
    }
    fifo->buffer[fifo->end].array = malloc(array->size * sizeof(int));

    memcpy(fifo->buffer[fifo->end].array, array->array, array->size  * sizeof(int));

    fifo->buffer[fifo->end].size = array->size;
    fifo->end = (fifo->end + 1) % fifo->buf_size;

    fifo->count++;

    pthread_mutex_unlock(&fifo->lock);
    pthread_cond_signal(&fifo->not_empty);
}

array_t fifo_pop(fifo_t *fifo) {

    pthread_mutex_lock(&fifo->lock);
    while (fifo->count == 0) {
        pthread_cond_wait(&fifo->not_empty, &fifo->lock);
    }
    array_t array;
    array.array = fifo->buffer[fifo->start].array;
    array.size = fifo->buffer[fifo->start].size;
    //print the array in the buffer to make sure it is correct

    fifo->buffer[fifo->start].array = NULL;
    fifo->buffer[fifo->start].size = 0;

    fifo->start = (fifo->start + 1) % fifo->buf_size;
    fifo->count--;
    
    pthread_mutex_unlock(&fifo->lock);
    pthread_cond_signal(&fifo->not_full);

    return array;
}


void fifo_destroy(fifo_t *fifo) {
    for (int i = 0; i < fifo->buf_size; i++) {
        free(fifo->buffer[i].array);
    }
    free(fifo->buffer);
    pthread_mutex_destroy(&fifo->lock);
    pthread_cond_destroy(&fifo->not_empty);
    pthread_cond_destroy(&fifo->not_full);
}