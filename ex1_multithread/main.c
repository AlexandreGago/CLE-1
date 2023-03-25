/**
 *  \file main.c 
 *
 *  \brief Problem name: Text Processing in Portuguese.
 *
 *  Main program.
 *
 *  \author Alexandre Gago 98123
 *  \author Bernardo Kaluza 97521
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <getopt.h>

#include "sharedRegion.h"
#include "probConst.h"
#include "textProcessing.h"

#include <time.h>

void *worker(void *arg);
int chunkSize = CHUNKSIZE;

/**
 * @brief Main function of the program that counts the number of words and vowels in a text file provided
 * 
 * @param argc the number of arguments
 * @param argv  the arguments
 * @return int 
 */

void printUsage(char name[]) {

    printf("Usage: %s [-t nThreads] [-s chunkSize] file1 file2 file3 ...\n", name);
}

int main(int argc, char *argv[]) {

    int nThreads = MAXTHREADS;
    int opt;

    //time variables
    struct timespec start_time, end_time;

    do
    {
        switch (opt = getopt(argc, argv, "t:s:h"))
        {
        case 't':
            nThreads = atoi(optarg);
            if (nThreads <= 0) {
                printf("Invalid number of threads\n");
                exit(1);
            }
            if (nThreads > MAXTHREADS) {
                printf("Number of threads is too large, max is %d\n", MAXTHREADS);
                exit(1);
            }
            break;
        case 's':
            int argChunkSize = atoi(optarg);
            //only valid values are 4 and 8
            if (argChunkSize == 4) {
                chunkSize = 4096;
            } else if (argChunkSize == 8) {
                chunkSize = 8192;
            } else {
                printf("Invalid chunk size, valid values are:\n 4 - 4096 bytes\n 8 - 8192 bytes\n");
                printUsage(argv[0]);
                exit(1);
            }
            break;

        case 'h':
            printUsage(argv[0]);
            exit(0);
            break;

        case '?':
            printf("Invalid option\n");
            printUsage(argv[0]);
            exit(1);
            break;

        }
    } while (opt != -1);
    
    //files are provided after the options
    if (argc - optind < 1) {
        printf("No files provided\n");
        printUsage(argv[0]);
        exit(1);
    }
    char *files[argc-optind];
    for (int i = optind; i < argc; i++) {
        files[i-optind] = argv[i];
    }

    if (clock_gettime(CLOCK_MONOTONIC, &start_time) == -1) {
        perror("clock_gettime");
        return 1;
    }

    //initialize the shared region
    initializeSharedRegion(files, argc-optind);

    //create workers
    printf("Using %d thread(s)\n", nThreads);
    pthread_t workers[nThreads];
    unsigned int workerId[nThreads];

    for (unsigned int i = 0; i < nThreads; i++) {
        // printf("Creating thread %d\n", i);
        workerId[i] = i;
        if (pthread_create(&workers[i], NULL, worker, &workerId[i]) != 0) {
            printf("Error creating thread %d \n", i);
            exit(1);
        }

    }

    //join workers
    for (unsigned int i = 0; i < nThreads; i++) {
        if (pthread_join(workers[i], NULL) != 0) {
            printf("Error joining thread %d \n", i);
            exit(1);
        }
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end_time) == -1) {
        perror("clock_gettime");
        return 1;
    }
    //print results
    printFilesData();

    // free shared region
    freeSharedRegion();
    
    double elapsed_time = (double) (end_time.tv_sec - start_time.tv_sec) + (double) (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("Time elapsed: %f seconds\n", elapsed_time); 

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
        pthread_exit(NULL);
    }
    chunk.size = 0;
    chunk.nWords = 0;
    chunk.FileId = -1;
    if (memset(chunk.data, 0, CHUNKSIZE) == NULL) {
        printf("Error clearing memory in thread %d ,exiting thread.\n", *id);
        pthread_exit(NULL);
    }
    for (int j = 0; j < 6; j++) {
        chunk.nVowels[j] = 0;
    }

    //while there are files to process
    while (!allFilesFinished()) {

        // printf("Thread %d getting data  \n", *id);

        //get data from the file
        if(getData(&chunk) != 0){
            printf("Error getting data in thread %d ,exiting thread.\n", *id);
            pthread_exit(NULL);
        }

        //process the chunk
        if(processChunk(&chunk) != 0){
            printf("Error processing chunk in thread %d , ignoring file.\n", *id);
            SignalCorruptFile(chunk.FileId);
            
        }
        else {
            //save the results in the shared region
            if(saveResults(&chunk) != 0){
                printf("Error saving results in thread %d ,exiting program.\n", *id);
                exit(1);
            }
        }

        

        //reset chunk
        chunk.size = 0;
        chunk.nWords = 0;
        chunk.FileId = -1;
        if (memset(chunk.data, 0, CHUNKSIZE) == NULL) {
            printf("Error clearing memory in thread %d ,exiting thread.\n", *id);
            free(chunk.data);
            pthread_exit(NULL);
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