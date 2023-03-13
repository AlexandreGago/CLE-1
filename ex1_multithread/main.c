#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "sharedRegion.h"
#include "probConst.h"
#include "textProcessing.h"

void *worker(void *arg);


/**
 * @brief Main function of the program that counts the number of words and vowels in a text file provided
 * 
 * @param argc the number of arguments
 * @param argv  the arguments
 * @return int 
 */
int main(int argc, char *argv[]) {
    //get text files from terminal
    char *files[argc-1];
    //read files from terminal
    for (int i = 1; i < argc; i++) {
        files[i-1] = argv[i];
        printf("\nFile %d: %s, ", i, files[i-1]);
    }
    // files[0] = "dataSet1/text2.txt";
    // files[1] = "dataSet1/test6.txt";

    //initialize the shared region
    initializeSharedRegion(files, argc-1);
    // initializeSharedRegion(argv, argc);

    //create workers
    pthread_t workers[maxThreads];
    unsigned int workerId[maxThreads];

    for (unsigned int i = 0; i < maxThreads; i++) {
        printf("Creating thread %d\n", i);
        workerId[i] = i;
        if (pthread_create(&workers[i], NULL, worker, &workerId[i]) != 0) {
            printf("Error creating thread %d \n", i);
            exit(1);
        }

    }

    //join workers
    for (unsigned int i = 0; i < maxThreads; i++) {
        if (pthread_join(workers[i], NULL) != 0) {
            printf("Error joining thread %d \n", i);
            exit(1);
        }
    }

    //print results
    printFilesData();
    printf("Done \n");
    fflush(stdout);
    freeSharedRegion();
    printf("Done2 \n");
    fflush(stdout);
}

/**
 * @brief Worker function that gets data from the shared region and processes it
 * 
 * @param ID ID of the thread
 * @return void* 
 */
void *worker(void *ID) {

    //save the id of the thread
    int *id = ID;

    //create a chunk to work with
    struct Chunk chunk;
    //malloc the buffer
    if ((chunk.data = malloc(CHUNKSIZE)) == NULL) {
        printf("Error allocating memory in thread %d, exiting thread.\n", *id);
        return NULL;
    }
    chunk.size = 0;
    chunk.nWords = 0;
    chunk.FileId = -1;
    if (memset(chunk.data, 0, CHUNKSIZE) == NULL) {
        printf("Error clearing memory in thread %d ,exiting thread.\n", *id);
        return NULL;
    }
    for (int j = 0; j < 6; j++) {
        chunk.nVowels[j] = 0;
    }

    //while there are files to process
    while (!allFilesFinished()) {

        // printf("Thread %d getting data  \n", *id);

        //get data from the file
        if(!getData(&chunk)){
            printf("Error getting data in thread %d ,exiting thread.\n", *id);
            return NULL;
        }

        //process the chunk
        if(!processChunk(&chunk)){
            printf("Error processing chunk in thread %d ,exiting thread.\n", *id);
            SignalCorruptFile(chunk.FileId);
            return NULL;
        }
        else{
            printf("Thread %d processed chunk of file %d of size %d name \n", *id, chunk.FileId, chunk.size);
            fflush(stdout);
        }

        //save the results in the shared region
        if(!saveResults(&chunk)){
            printf("Error saving results in thread %d ,exiting thread.\n", *id);
            return NULL;
        }

        //reset chunk
        chunk.size = 0;
        chunk.nWords = 0;
        chunk.FileId = -1;
        if (memset(chunk.data, 0, CHUNKSIZE) == NULL) {
            printf("Error clearing memory in thread %d ,exiting thread.\n", *id);
            return NULL;
        }
        for (int j = 0; j < 6; j++) {
            chunk.nVowels[j] = 0;
        }
        // sleep(1);
    }
    //free the chunk data
    free(chunk.data);
    return NULL;
}