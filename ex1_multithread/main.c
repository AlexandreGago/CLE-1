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

#include "sharedRegion.h"
#include "probConst.h"
#include "textProcessing.h"

#include <time.h>

void *worker(void *arg);

/**
 * @brief Main function of the program that counts the number of words and vowels in a text file provided
 * 
 * @param argc the number of arguments
 * @param argv  the arguments
 * @return int 
 */
int main(int argc, char *argv[]) {

    int nThreads = maxThreads;
    int opt;
    //time the total timeof the program
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    do
    {
        switch (opt = getopt(argc, argv, "t:h"))
        {
        case 't':
            nThreads = atoi(optarg);
            if (nThreads <= 0) {
                printf("Invalid number of threads\n");
                exit(1);
            }
            if (nThreads > maxThreads) {
                printf("Number of threads is too large, max is %d\n", maxThreads);
                exit(1);
            }
            break;
        case 'h':
            printf("Usage: %s [-t nThreads] file1 file2 file3 ...\n", argv[0]);
            exit(0);

        case '?':
            printf("Invalid option\n");
            printf("Usage: %s [-t nThreads] file1 file2 file3 ...\n", argv[0]);
            exit(1);

        }
    } while (opt != -1);
    
    //files are provided after the options
    if (argc - optind < 1) {
        printf("No files provided\n");
        printf("Usage: %s [-t nThreads] file1 file2 file3 ...\n", argv[0]);
        exit(1);
    }
    char *files[argc-optind];
    for (int i = optind; i < argc; i++) {
        files[i-optind] = argv[i];
    }

    //initialize the shared region
    initializeSharedRegion(files, argc-optind);

    //create workers
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

    //print results
    printFilesData();

    // free shared region
    freeSharedRegion();
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("\nTotal time: %f seconds\n", cpu_time_used);

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