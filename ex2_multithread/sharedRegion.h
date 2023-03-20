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

/**
 * @brief Initializes the shared region
 * 
 * @param arraySize  Size of the array
 * @param filename  Name of the file
 */
void initializeSharedRegion(int arraySize, char *filename);

/**
 * @brief Prints an array
 * 
 * @param array  Pointer to the array
 * @param size  Size of the array
 */
void printarray(int *array, int size);

/**
 * @brief Frees memory allocated for the shared region
 * 
 */
void freeSharedRegion();

/**
 * @brief Gets the file name
 * 
 * @return char*  Pointer to the file name
 */
char *getFileName();

/**
 * @brief Get the Fifo Unsorted object
 * 
 * @return fifo_t* Pointer to the unsorted fifo
 */
fifo_t *getFifoUnsorted();
/**
 * @brief Get the Fifo Sorted object
 * 
 * @return fifo_t* Pointer to the sorted fifo
 */
fifo_t *getFifoSorted();

#endif /* SHAREGION_H */