#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "fifo.h"

fifo_t *fifo_unsorted;
fifo_t *fifo_sorted;
//string to be used for the name of the file
char *fileName;

void initializeSharedRegion(int buffersize, char *filename) {
    //allocate memory for an array with numThreads elements where 
    //initialize the fifo_unsorted
    fifo_unsorted = malloc(sizeof(fifo_t));
    fifo_sorted = malloc(sizeof(fifo_t));
    fifo_init(fifo_unsorted, buffersize);
    fifo_init(fifo_sorted, buffersize);
    fileName = (char *)malloc(strlen(filename) + 1);
    strcpy(fileName, filename);
    
}

char *getFileName() {
    return fileName;
}
fifo_t *getFifoUnsorted() {
    return fifo_unsorted;
}
fifo_t *getFifoSorted() {
    return fifo_sorted;
}

void freeSharedRegion() {
    //free the memory allocated for the shared region
    //free the fifos
    fifo_destroy(fifo_unsorted);
    fifo_destroy(fifo_sorted);
    free(fileName);
    free(fifo_unsorted);
    free(fifo_sorted);
    
}