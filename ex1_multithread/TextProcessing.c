#include <stdio.h>
#include <stdlib.h>
//include threads
#include <pthread.h>
#include "fifo.h"

int numThreads = 4;
int numProducers = 1;

void getNextChar(char *c, FILE *f);
char processBuffer(unsigned char *buf, int size);
void *produce(void *arg);
void *consume(void *arg);

/**
 * @brief 
 * 
 * @param argc the number of arguments
 * @param argv  the arguments
 * @return int 
 */
int main (int argc, char *argv[]){
    //create the worker threads and the buffer and wait for them to finish
    pthread_t *threads = malloc(sizeof(pthread_t) * numThreads);

    for(int i = 0; i < numProducers; i++){
        pthread_create(&threads[i], NULL, produce, NULL);
    }
    for (int i = numProducers; i < numThreads; i++){
        pthread_create(&threads[i], NULL, consume, NULL);
    }

    for(int i = 0; i < numThreads; i++){
        pthread_join(threads[i], NULL);
    }
    return 0;
}

void getNextChar(char *c, FILE *f){
}

char processBuffer(unsigned char *buf, int size) {
}

void *produce(void *arg){
    while(1){
        //if the FIFO is not full
        if (FIFO_isFull() == 0){
            //read 8k bytes from the file
            //process the buffer
            //insert on the FIFO
        }
    }
}

void *consume(void *arg){
    Block *b;
    //copy a block from the FIFO
    *b = getBLock();//getVal()
    //process the block
    getNextChar(b->data, b->size);
    //free the block
    free(b);

}