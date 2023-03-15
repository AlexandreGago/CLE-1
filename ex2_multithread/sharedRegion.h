/**
 * @file sharedRegion.h (header file)
 * @brief Text processing functions
 * 
 * @author Bernardo Kaluza 97521
 * @author Alexandre Gago 98123
 */
#include "fifo.h"
#ifndef SHAREDREGION_H
#define SHAREGION_H


void initializeSharedRegion(int numThreads, int arraySize,fifo_t *fifo_unsorted,fifo_t *fifo_sorted);
void printarray(int *array, int size);
void freeSharedRegion(int numThreads, int arraySize);


#endif /* SHAREGION_H */