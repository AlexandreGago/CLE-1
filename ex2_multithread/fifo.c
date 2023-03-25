/**
 * @file fifo.c
 * @author Bernardo Kaluza
 * @author Alexandre Gago
 * @brief Thread-safe FIFO implementation for array_t
 * @date 2023-03-25
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "fifo.h"

/**
 * @brief Initialize the FIFO
 * 
 * @param fifo Pointer to the FIFO
 * @param buffer_size Size of the buffer
 */
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

/**
 * @brief Push an array into the FIFO
 * 
 * @param fifo Pointer to the FIFO
 * @param array Pointer to the array
 */
void fifo_push(fifo_t *fifo, array_t* array) {
    pthread_mutex_lock(&fifo->lock);
    while (fifo->count == fifo->buf_size) {
        pthread_cond_wait(&fifo->not_full, &fifo->lock);
    }
    free(fifo->buffer[fifo->end].array);
    fifo->buffer[fifo->end].array = malloc(array->size * sizeof(int));

    memcpy(fifo->buffer[fifo->end].array, array->array, array->size  * sizeof(int));

    fifo->buffer[fifo->end].size = array->size;
    fifo->end = (fifo->end + 1) % fifo->buf_size;

    fifo->count++;
    
    free(array->array);

    pthread_cond_signal(&fifo->not_empty);
    pthread_mutex_unlock(&fifo->lock);
}

/**
 * @brief Pop the first array from the FIFO
 * 
 * @param fifo Pointer to the FIFO
 * @return array_t the first array in the FIFO
 */
array_t fifo_pop(fifo_t *fifo) {

    pthread_mutex_lock(&fifo->lock);
    while (fifo->count == 0) {
        pthread_cond_wait(&fifo->not_empty, &fifo->lock);
    }
    array_t array;
    array.array = fifo->buffer[fifo->start].array;
    array.size = fifo->buffer[fifo->start].size;

    fifo->buffer[fifo->start].array = NULL;
    // free(fifo->buffer[fifo->start].array);
    fifo->buffer[fifo->start].size = 0;

    // set the start to the next element in the buffer
    fifo->start = (fifo->start + 1) % fifo->buf_size;
    // decrement the element count of the fifo
    fifo->count--;
    
    pthread_cond_signal(&fifo->not_full);
    pthread_mutex_unlock(&fifo->lock);

    return array;
}

/**
 * @brief Frees the FIFO buffer and destroys the mutex and condition variables
 * 
 * @param fifo 
 */
void fifo_destroy(fifo_t *fifo) {
    for (int i = 0; i < fifo->buf_size; i++) {
        free(fifo->buffer[i].array);
    }
    free(fifo->buffer);
    pthread_mutex_destroy(&fifo->lock);
    pthread_cond_destroy(&fifo->not_empty);
    pthread_cond_destroy(&fifo->not_full);
}