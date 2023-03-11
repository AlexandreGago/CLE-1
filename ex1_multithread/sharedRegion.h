#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "probConst.h"
#include <string.h>

/**
 * @brief 
 * Structure containing id,name and pointer to file and the number of words on the file and the number of words with each vowel
 */
struct FileData
{
    int id;
    char *name;
    FILE *file;
    int finished;

    int nWords;
    int nVowels[6];
};

/**
 * @brief 
 *  Structure containing a chunk of a file 
 */
struct Chunk
{
    int FileId;
    int finished;
    int size;
    unsigned char *data;
    
    int nWords;
    int nVowels[6];
};


/**
 * @brief 
 * 
 * @param files 
 * @param numFiles 
 */
extern void initializeSharedRegion(char** files, int numFiles);

/**
 * @brief Get the Data object
 * 
 * @param chunk 
 */
extern void getData(struct Chunk *chunk);

/**
 * @brief 
 * 
 * @param chunk 
 */
extern void saveResults(struct Chunk *chunk);

