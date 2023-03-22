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
        fifo->buffer[i].direction = -1;
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
    //print the array to make sure it is correct
    // printf("\narray: ");
    // for (int i = 0; i < array->size; i++) {
    //     printf("%d ", array->array[i]);
    // }
    // printf("size: %d direction: %d \n", array->size, array->direction);
    fflush(stdout);
    fifo->buffer[fifo->end].array = malloc(array->size * sizeof(int));

    memcpy(fifo->buffer[fifo->end].array, array->array, array->size  * sizeof(int));

    fifo->buffer[fifo->end].size = array->size;
    fifo->buffer[fifo->end].direction = array->direction;
    fifo->end = (fifo->end + 1) % fifo->buf_size;

    fifo->count++;

    pthread_cond_signal(&fifo->not_empty);
    pthread_mutex_unlock(&fifo->lock);
}

array_t fifo_pop(fifo_t *fifo) {

    pthread_mutex_lock(&fifo->lock);
    while (fifo->count == 0) {
        pthread_cond_wait(&fifo->not_empty, &fifo->lock);
    }
    array_t array;
    array.array = fifo->buffer[fifo->start].array;
    array.size = fifo->buffer[fifo->start].size;
    array.direction = fifo->buffer[fifo->start].direction;
    //print the array in the buffer to make sure it is correct

    //!CHECK IF THE ARRAY NEEDS FREE OR IF NULL IS OK
    fifo->buffer[fifo->start].array = NULL;
    fifo->buffer[fifo->start].size = 0;
    fifo->buffer[fifo->start].direction = -1;
    // free(fifo->buffer[fifo->start].array);

    // set the start to the next element in the buffer
    fifo->start = (fifo->start + 1) % fifo->buf_size;
    // decrement the element count of the fifo
    fifo->count--;
    
    pthread_cond_signal(&fifo->not_full);
    pthread_mutex_unlock(&fifo->lock);

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