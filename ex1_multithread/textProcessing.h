/**
 * @file textProcessing.h (header file)
 * @brief Text processing functions
 * 
 * @author Bernardo Kaluza 97521
 * @author Alexandre Gago 98123
 */

#ifndef TEXT_PROCESSING_H
#define TEXT_PROCESSING_H

/**
 * @brief Performs the processing of a chunk of text
 * It is called by the threads and receives a pointer to a Chunk struct
 * The function will process the chunk and count the number of words, and how many words have each vowel and update the chunk struct accordingly
 * @param Chunk Struct that contains the data needed to process the chunk
 * It will also be used to store the results of the processing.
 */
extern void processChunk(struct Chunk *fileChunk);


#endif /* TEXT_PROCESSING_H */