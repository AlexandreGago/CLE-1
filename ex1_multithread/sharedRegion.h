#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "probConst.h"

/**
 *  \file sharedRegion.h (header file)
 *
 *  \brief Problem name: Text Processing.
 *
 *  Problem simulation parameters.
 *
 *  \author Alexandre Gago  - March 2023
 *  \author Bernardo Kaluza - March 2023
 */
#ifndef SHARED_REGION_H
#define SHARED_REGION_H
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
    int corrupt;

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
    int size;
    unsigned char *data;
    
    int nWords;
    int nVowels[6];
};


/**
 * @brief 
 * Function that serves to initializa the shared region allocating memory for the filesData array
 * @param files Name of the files to process
 * 
 * @param numFiles Total number of the files given
 */
extern void initializeSharedRegion(char** files, int numFiles);
/**
 * @brief 
 * Funtion that serves to print the data of the filesData array
 */
extern void printFilesData();

/**
 * @brief 
 * Funtion that serves to free the memory allocated in the shared region
 */
extern void freeSharedRegion();

/**
 * @brief 
 * Funtion that serves to check if all the files have been processed
 * @return int 
 */
extern int allFilesFinished();

/**
 * @brief get the a chunk of data from a file and store it in the chunk struct
 * 
 * @param chunk 
 */
extern int getData(struct Chunk *chunk);

/**
 * @brief 
 * Save the results of the processing of a chunk in the filesData array
 * @param chunk chunk containing the data to be saved
 */
extern int saveResults(struct Chunk *chunk);

/**
 * @brief 
 * 
 * @param fileId 
 */
extern void SignalCorruptFile(int fileId);

/**
 * @brief 
 * Set a File in the filesData array as finished and corrupt
 * @param FileId 
 * ID of the file to be set as finished and corrupt
 */
void SignalCorruptFile(int FileId);


#endif
