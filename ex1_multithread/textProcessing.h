/**
 *  \file textProcessing.h (header file)
 *
 * @brief Problem name: Text Processing in Portuguese
 * Text Processing functions, used to count the number of words and vowels in a given chunk of text.
 * Also used to read a chunk from a file.
 *
 *  \author Alexandre Gago  - March 2023
 *  \author Bernardo Kaluza - March 2023
 */


#ifndef TEXT_PROCESSING_H
#define TEXT_PROCESSING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * @brief Get the decimal codepoint of the utf 8 character
 * 
 * @param buffer the buffer of bytes to read from
 * @param index the index of the buffer to start reading on
 * @return int the decimal codepoint of the utf 8 character
 */
int getChar(unsigned char *buffer, int *index);

/**
 * @brief Check if a character is a separation or punctuation character
 * 
 * @param c the decimal codepoint of the character
 * @return true  if the character is a separation or punctuation character
 * @return false if the character is not a separation or punctuation character
 */
bool isSepPunc(int c);

/**
 * @brief Check if a character is a merge character
 * 
 * @param c the decimal codepoint of the character
 * @return true if the character is a merge character
 * @return false if the character is not a merge character
 */
bool isMerge(int c);

/**
 * @brief Check if a character is a vowel and update the vowelInWord and nVowels arrays accordingly
 * @param c  the decimal codepoint of the character
 * @param vowelInWord an array of booleans that indicates if a vowel has already been found in the word
 * @param nVowels an array of integers that indicates the number of vowels found in the chunk
 */
void isVowel(int c, bool *vowelInWord, int *nVowels);

/**
 * @brief Performs the processing of a chunk of text
 * It is called by the threads and receives a pointer to a Chunk struct
 * The function will process the chunk and count the number of words, and how many words have each vowel and update the chunk struct accordingly
 * @param Chunk Struct that contains the data needed to process the chunk
 * It will also be used to store the results of the processing.
 */
int processChunk(struct Chunk *fileChunk);

/**
 * @brief Reads a chunk of data from a file and stores it in a Chunk struct
 * 
 * @param fileData  the struct that contains the file data
 * @param fileChunk the struct that will contain the chunk data
 * @return int 
 */

int readToChunk(struct FileData *fileData, struct Chunk *fileChunk);
#endif /* TEXT_PROCESSING_H */