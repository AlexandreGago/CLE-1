#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>
/**
 * @brief 
 * Structure containing id,name and pointer to file and the number of words on the file and the number of words with each vowel
 */
typedef struct {
    int id;
    char *name;
    FILE *file;
    int finished;
    int corrupt;

    int nWords;
    int nVowels[6];
}FileData;

/**
 * @brief 
 *  Structure containing a chunk of a file, the number of words on the chunk and the number of words with each vowel 
 */
typedef struct{
    int FileId;
    int size;
    unsigned char *data;
    
    int nWords;
    int nVowels[6];
}Chunk;


#endif  // STRUCTS_H
