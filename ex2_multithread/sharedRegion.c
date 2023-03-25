/**
 * @file shared_region.c
 * @brief Functions to manage a shared region used by multiple threads.
 * @author Bernardo Kaluza
 * @author Alexandre Gago
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "fifo.h"

fifo_t *fifo_unsorted;
fifo_t *fifo_sorted;
//string to be used for the name of the file
char *fileName;



/**
 * @brief Initializes the shared region and the fifos with the specified buffer size and file name.
 * 
 * @param buffersize The size of the FIFO buffers.
 * @param filename The name of the file to be processed.
 */
int initializeSharedRegion(int buffersize, char *filename) {
    //allocate memory for an array with numThreads elements where 
    //initialize the fifo_unsorted
    fifo_unsorted = malloc(sizeof(fifo_t));
    fifo_sorted = malloc(sizeof(fifo_t));
    fifo_init(fifo_unsorted, buffersize);
    fifo_init(fifo_sorted, buffersize);
    fileName = (char *)malloc(strlen(filename) + 1);
    strcpy(fileName, filename);
    return 1;
}

/**
 * @brief Returns the file name of the shared region.
 * 
 * @return The file name as a char pointer.
 */
char *getFileName() {
    return fileName;
}

/**
 * @brief Returns the pointer to the fifo_unsorted structure.
 * 
 * @return A pointer to the fifo_unsorted structure.
 */
fifo_t *getFifoUnsorted() {
    return fifo_unsorted;
}

/**
 * @brief Returns the pointer to the fifo_sorted structure.
 * 
 * @return A pointer to the fifo_sorted structure.
 */
fifo_t *getFifoSorted() {
    return fifo_sorted;
}

/**
 * @brief Frees the memory allocated for the shared region.
 */
int freeSharedRegion() {
    //free the memory allocated for the shared region
    //free the fifos
    fifo_destroy(fifo_unsorted);
    fifo_destroy(fifo_sorted);
    free(fileName);
    free(fifo_unsorted);
    free(fifo_sorted);
    return EXIT_SUCCESS;
}