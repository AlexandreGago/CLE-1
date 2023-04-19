#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <getopt.h>
#include "probConst.h"

#include "sharedRegion.h"
#include "merge.h"
#include "fifo.h"

void *worker(void *arg);
void *distributor (void *arg);

int nThreads = MAXTHREADS;
int fifoSize = FIFO_SIZE;

void printUsage(char *programName) {
    printf("Usage: %s [options] <file>\n", programName);
    printf("Options:\n");
    printf(" -t <n> : number of threads (default is %d)\n", MAXTHREADS);
    printf(" -h : print this help message\n");
}
int main(int argc, char *argv[]) {

    struct timespec start_time, end_time;
    int opt;

    do
    {
        switch (opt = getopt(argc, argv, "t:h"))
        {
        case 't':
            nThreads = atoi(optarg);
            //nThreads can only be 1,2,4,8
            if (nThreads != 1 && nThreads != 2 && nThreads != 4 && nThreads != 8) {
                printf("Invalid number of threads, valid values are:\n 1 - 1 thread\n 2 - 2 threads\n 4 - 4 threads\n 8 - 8 threads\n");
                printUsage(argv[0]);
                exit(1);
            }
            if (nThreads > MAXTHREADS) {
                printf("Number of threads is too large, max is %d\n", MAXTHREADS);
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
    char *fileName = argv[optind];

    if (clock_gettime(CLOCK_MONOTONIC, &start_time) == -1) {
        perror("clock_gettime");
        return 1;
    }

    //initialize the shared region and the fifos
    initializeSharedRegion(fifoSize,fileName); 
    pthread_t distributor_thread;
    pthread_t workers[nThreads];
    unsigned int workerId[nThreads];

    //create distributor thread
    if (pthread_create(&distributor_thread, NULL, distributor, NULL) != 0) {
        printf("Error creating distributor thread \n");
        exit(EXIT_FAILURE);
    }

    //create worker threads
    for (int i = 0; i < nThreads; i++) {
        workerId[i] = i;
        if (pthread_create(&workers[i], NULL, worker, &workerId[i]) != 0) {
            printf("Error creating worker thread \n");
            exit(EXIT_FAILURE);
        }
    }
    //wait for distributor thread to finish
    if (pthread_join(distributor_thread, NULL) != 0) {
        printf("Error joining distributor thread \n");
        exit(EXIT_FAILURE);
    }

    //wait for worker threads to finish
    for (int i = 0; i < nThreads; i++) {
        if (pthread_join(workers[i], NULL) != 0) {
            printf("Error joining worker thread \n");
            exit(EXIT_FAILURE);
        }
    }
    //merge the sorted subarrays
    for(int i=0;i<nThreads-1;i++){
        // printf("Merge %d\n",i);
        array_t array1 = fifo_pop(getFifoSorted());
        array_t array2 = fifo_pop(getFifoSorted());
        //unite the two arrays
        int* array3 = malloc(sizeof(int)*(array1.size+array2.size));
        //copy the array1 and array2 in array3
        for(int j=0;j<array1.size;j++){
            array3[j] = array1.array[j];
        }
        for(int j=0;j<array2.size;j++){
            array3[array1.size+j] = array2.array[j];
        }
        //merge the two arrays
        mergeItr(array3,0,array1.size-1,array1.size+array2.size-1);
        
        //put the array in the fifo_sorted
        array_t *array4 = malloc(sizeof(array_t));
        array4->array = array3;
        array4->size = array1.size+array2.size;
        fifo_push(getFifoSorted(),array4);
        free(array1.array);
        free(array2.array);
    }



    if (clock_gettime(CLOCK_MONOTONIC, &end_time) == -1) {
        perror("clock_gettime");
        return 1;
    }


    array_t array3 = fifo_pop(getFifoSorted());
    //check if array3 is sorted in crescent order
    bool crescent = true;
    for (int i = 0; i < array3.size-1; i++) {
        if (array3.array[i] > array3.array[i+1]) {
            crescent = false;
        }
    }
    if (crescent) {
        printf("Array is sorted in crescent order\n");
    } else {
        printf("Array is not sorted in crescent order\n");
    }


    freeSharedRegion();
    // free(fifo_unsorted);
    // free(fifo_sorted);

    double elapsed_time = (double) (end_time.tv_sec - start_time.tv_sec) + (double) (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("Time elapsed: %f seconds\n", elapsed_time);
    printf("All threads finished\n");
    return 0;
}

void *worker(void *arg) {
    int *id = (int *)arg;
    printf("Thread worker %d created \n", *id);
    //while (true) {
    array_t array = fifo_pop(getFifoUnsorted());
    mergeSortItr(array.array, array.size);
    fifo_push(getFifoSorted(), &array);
    //}
    return NULL;
}


void *distributor (void *arg){
    
    printf("Thread distributor created \n");
    char *filename = getFileName();
    FILE *f;
    int *arr;
    int arr_size;
    f = fopen(filename, "rb");
    if (f == NULL) {
        printf("Error opening file %s   \n", filename);
        exit(EXIT_FAILURE);
    }
    //read the number of elements
    if(fread(&arr_size, sizeof(int), 1, f)<=0){
        printf("Error reading file %s   \n", filename);
        exit(EXIT_FAILURE);
    };
    //allocate memory for the array
    arr = (int *)malloc(arr_size * sizeof(int));
    //read to the arrayc
    if(fread(arr, sizeof(int), arr_size, f)<=0){
        printf("Error reading file %s   \n", filename);
        exit(EXIT_FAILURE);
    };
    fclose(f);

    //printf("Array read from file %s   \n", filename);

    for (int i=0;i<nThreads;i++){
        array_t *arr1 = malloc(sizeof(array_t));
        arr1->array = malloc(sizeof(int)*(arr_size/nThreads));
        arr1->size = arr_size/nThreads;
        //copy the elements of arr to arr1
        for (int j=0;j<arr1->size;j++){
            arr1->array[j] = arr[i*arr1->size+j];
        }
            //print the subarrays
        //printf("Subarray %d   \n", i);
        //put the subarrays in the fifo
        fifo_push(getFifoUnsorted(), arr1); 
        
    }
    return NULL;

}