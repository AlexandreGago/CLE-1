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


void initializeSharedRegion(int arraySize, char *filename);
void printarray(int *array, int size);
void freeSharedRegion();
char *getFileName();
fifo_t *getFifoUnsorted();
fifo_t *getFifoSorted();

#endif /* SHAREGION_H */