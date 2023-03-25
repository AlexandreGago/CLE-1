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
 * @brief Initializes the shared region and the fifos with the specified buffer size and file name.
 * 
 * @param buffersize The size of the FIFO buffers.
 * @param filename The name of the file to be processed.
 */
int initializeSharedRegion(int arraySize, char *filename);



/**
 * @brief Returns the file name of the shared region.
 * 
 * @return The file name as a char pointer.
 */
char *getFileName();

/**
 * @brief Returns the pointer to the fifo_unsorted structure.
 * 
 * @return A pointer to the fifo_unsorted structure.
 */
fifo_t *getFifoUnsorted();
/**
 * @brief Returns the pointer to the fifo_sorted structure.
 * 
 * @return A pointer to the fifo_sorted structure.
 */
fifo_t *getFifoSorted();

/**
 * @brief Frees the memory allocated for the shared region.
 */
int freeSharedRegion();

#endif /* SHAREGION_H */