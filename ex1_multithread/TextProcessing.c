/**
 * @file TextProcessing.c
 * @author Bernardo Kaluza 97521
 * @author Alexandre Gago 98123
 * @brief Problem name: Text Processing in Portuguese
 * @date 2023-03-10
 * @copyright Copyright (c) 2023
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//! TEMPORATy
struct Chunk
{
    int FileId;
    int finished;
    int size;
    unsigned char *data;
    
    int nWords;
    int nVowels[5];
};

//receives a pointer to the buffer
int getChar(unsigned char *buffer, int *index){
    unsigned char c = buffer[0];
    (*index)++;
    return c;
}

bool isAlphaNumeric(int c){
    //a character is alphanumeric if it is a letter or a number
    //a character is a letter if it is between 65 and 90 or between 97 and 122
    //a character is a number if it is between 48 and 57
    return ((65 <= c && c <= 90) || (97 <= c && c <= 122) || (48 <= c && c <= 57));
}

/**
 * @brief Performs the processing of a chunk of text
 * It is called by the threads and receives a pointer to a FileData struct
 * The function will process the chunk and count the number of words, and how many words have each vowel
 * 
 * 
 * @param Chunk Struct that contains the data needed to process the chunk
 * It will also be used to store the results of the processing.
 */
void processChunk(struct Chunk *fileChunk){
    //pointer to index of the buffer
    int *buffIndex = 0;

    bool inWord = false;
    int nWords = 0;
    int nVowels[] = {0,0,0,0,0,0};
    bool vowelInWord[] = {false, false, false, false, false, false};

    //loop that will process the chunk
    while (buffIndex <= fileChunk->size) {

        //get the number representation of the char
        int c = getChar(fileChunk->data, &buffIndex);

        
        if (!inWord && isAlphaNumeric(c)) {
            inWord = true;
            nWords++;
        } else if (inWord && isSeparation(c)) {
            inWord = false;
        }

    }
    
}
