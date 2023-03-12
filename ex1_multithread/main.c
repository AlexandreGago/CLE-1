#include <stdio.h>
#include <stdlib.h>
//include workers
#include <pthread.h>
#include <unistd.h>

#include "sharedRegion.h"
#include "probConst.h"
#include "textProcessing.h"

int numThreads = 4;
int numProducers = 1;

void *worker(void *arg);


/**
 * @brief Main function of the program that counts the number of words and vowels in a text file provided
 * 
 * @param argc the number of arguments
 * @param argv  the arguments
 * @return int 
 */
int main (int argc, char *argv[]){
    //get text files from terminal
    char *files[2];
    files[0] = "dataSet1/text2.txt";
    files[1] = "dataSet1/text3.txt";
    // files[2] = "dataSet1/text1.txt";

    //initialize the shared region
    initializeSharedRegion(files, 2);
    // initializeSharedRegion(argv, argc);

    //create workers
    pthread_t workers[numThreads];
    unsigned int workerId[numThreads];
    
    for( unsigned int i = 0; i < numThreads; i++){
        printf("Creating thread %d\n", i);
        workerId[i] = i;
        pthread_create(&workers[i], NULL, worker, &workerId[i]);

    }
    
    //join workers
    for(unsigned int i = 0; i < numThreads; i++){
        pthread_join(workers[i], NULL);
    }

    //print results
    printFilesData();

    freeSharedRegion();
    
}
/**
 * @brief Worker function that gets data from the shared region and processes it
 * 
 * @param ID ID of the thread
 * @return void* 
 */
void *worker(void *ID){

    //save the id of the thread
    int *id = ID;
    printf("\nThread %d started working \n", *id);

    //create a chunk to work with
    struct Chunk chunk;
    //malloc the buffer
    chunk.data = malloc(CHUNKSIZE);
    chunk.size = 0;
    chunk.nWords = 0;
    chunk.finished = 0;
    chunk.FileId = -1;
    memset(chunk.data, 0, CHUNKSIZE);
    for(int j = 0; j < 6; j++){
        chunk.nVowels[j] = 0;
    }

    //while there are files to process
    while(!allFilesFinished()){

        // printf("Thread %d getting data  \n", *id);
        fflush(stdout);

        //get data from the file
        getData(&chunk);
        //process the chunk
        processChunk(&chunk);
        //save the results in the shared region
        saveResults(&chunk);

        //reset chunk
        chunk.size = 0;
        chunk.nWords = 0;
        chunk.finished = 0;
        chunk.FileId = -1;
        memset(chunk.data, 0, CHUNKSIZE);
        for(int j = 0; j < 6; j++){
            chunk.nVowels[j] = 0;
        }
        // sleep(1);
    }
    //free the chunk data
    free(chunk.data);
    return NULL;
}