/**
 * @file main.c
 * @brief Main file for the second problem.
 * 
 * This program reads an array from a file and sorts it using a multi-threaded
 * merge sort algorithm. The number of threads to be used can be specified as a
 * command line argument.
 * @author Bernardo Kaluza
 * @author Alexandre Gago
 */
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

/**
 * @brief worker life cycle 
 */
void *worker(void *arg);

/**
 * @brief distributor life cycle 
 */
void *distributor (void *arg);

/**
 * @brief The number of worker threads to be used for sorting. Default is MAXTHREADS.
 */
int nThreads = MAXTHREADS;
/**
 * @brief The size of the FIFO buffers to be used. Default is FIFO_SIZE.
 */
int fifoSize = FIFO_SIZE;

/**
 * @brief A pthread_barrier_t variable used to synchronize worker threads.
 */
pthread_barrier_t barrier;

/**
 * @brief A struct containing the arguments for worker threads.
 */
typedef struct {
    int workerId;//id of the worker thread
    int nThreads;   //number of worker threads
    fifo_t* fifo_unsorted;  //fifo containing the unsorted arrays
    fifo_t* fifo_sorted;    //fifo containing the sorted arrays

} WorkerArgs;

/**
 * @brief Prints the usage of the program.
 * 
 * @param programName The name of the program as a char pointer.
 */
void printUsage(char *programName) {
    printf("Usage: %s [options] <file>\n", programName);
    printf("Options:\n");
    printf(" -t <n> : number of threads (default is %d)\n", MAXTHREADS);
    printf(" -h : print this help message\n");
}

/**
 * @brief The main function of the multi-threaded merge sort program.
 * 
 * Initializes the shared region, creates the distributor and worker threads,
 * and waits for them to finish. Finally, it validates if the final array is sorted.
 * @param argc The number of command line arguments.
 * @param argv An array of command line argument strings.
 * @return Returns 0 if the program runs successfully, and 1 if there's an error.
 */
int main(int argc, char *argv[]) {
    
    //used to measure the time
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
    //get the file name
    char *fileName = argv[optind];

    //start the timer
    if (clock_gettime(CLOCK_MONOTONIC, &start_time) == -1) {
        perror("clock_gettime");
        return 1;
    }

    //initialize the shared regionr
    if(initializeSharedRegion(fifoSize,fileName)){
        printf("Error initializing shared region\n");
        exit(EXIT_FAILURE);
    }
    //initialize the barrier
    if (pthread_barrier_init(&barrier, NULL, nThreads) != 0) {
        perror("pthread_barrier_init");
        return EXIT_FAILURE;
    }

    //create the distributor and worker threads
    pthread_t distributor_thread;
    pthread_t workers[nThreads];
    
    //array of arguments for the worker threads
    WorkerArgs args[nThreads];

    //create distributor thread
    if (pthread_create(&distributor_thread, NULL, distributor, NULL) != 0) {
        printf("Error creating distributor thread \n");
        exit(EXIT_FAILURE);
    }
    //create worker threads
    for (int i = 0; i < nThreads; i++) {
        //initialize the arguments for the worker threads
        args[i].workerId = i;
        args[i].nThreads = nThreads;
        args[i].fifo_unsorted = getFifoUnsorted();
        args[i].fifo_sorted = getFifoSorted();
        //create the worker thread
        if (pthread_create(&workers[i], NULL, worker, &args[i]) != 0) {
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

    //pop the last sorted array from the sorted fifo
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

    //free the shared region
    if(freeSharedRegion()){
        printf("Error freeing shared region\n");
        exit(EXIT_FAILURE);
    }

    //stop the timer
    if (clock_gettime(CLOCK_MONOTONIC, &end_time) == -1) {
        perror("clock_gettime");
        return 1;
    }
    //calculate the time elapsed
    double elapsed_time = (double) (end_time.tv_sec - start_time.tv_sec) + (double) (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("Time elapsed: %f seconds\n", elapsed_time);
    printf("All threads finished\n");
    return 0;
}

/**
 * @brief The worker function for the merge sort algorithm.
 * 
 * @details This function is the life cycle of a worker thread. It gets an unsorted array from the unsorted fifo and sorts it using the merge sort algorithm.
 * Then it pushes the sorted array in the sorted fifo. After all threads have finished sorting, they merge the sorted subarrays back together.
 * The id of the thread determines at each step if it should merge 2 of the arrays.
 * The process ends when the original array is sorted and merged back together.
 * @param arg A pointer to the WorkerArgs structure.
 * @return NULL
 */
void *worker(void *arg) {
    //get the arguments 
    WorkerArgs *args = (WorkerArgs *) arg;
    int *id = &args->workerId;
    int nThreads = args->nThreads;

    
    printf("Thread worker %d created \n", *id);
    //get an array from the unsorted fifo 
    array_t array = fifo_pop(args->fifo_unsorted);
    //sort the array
    mergeSortItr(array.array, array.size);
    //push the sorted array in the sorted fifo
    fifo_push(args->fifo_sorted, &array);
    
    //wait for all threads to finish sorting
    pthread_barrier_wait(&barrier);
    
    //if there is only one thread, the array is sorted
    if (nThreads == 1) {
        return NULL;
    }
    else {
        //merge the sorted arrays
        array_t array3;
        //this is halved at each step, it represents the number of merges that need to be done
        nThreads = nThreads / 2;
        
        //while there is still merging to do
        while (nThreads > 0)
        {
            //the id of the thread determines if it should merge 2 of the arrays
            if (*id <= nThreads-1) {
                array_t array1 = fifo_pop(args->fifo_sorted);
                array_t array2 = fifo_pop(args->fifo_sorted);
                //unite the two arrays
                array3.array = malloc(sizeof(int)*(array1.size+array2.size));
                array3.size = array1.size+array2.size;
                //copy the array1 and array2 in array3
                for(int j=0;j<array1.size;j++){
                    array3.array[j] = array1.array[j];
                }
                for(int j=0;j<array2.size;j++){
                    array3.array[array1.size+j] = array2.array[j];
                }
                //merge the two arrays
                mergeItr(array3.array,0,array1.size-1,array1.size+array2.size-1);
                free(array1.array);
                free(array2.array);
            }
            // wait for all threads to finish merging
            pthread_barrier_wait(&barrier);
            //if the thread merged 2 arrays, push the merged array in the sorted fifo
            if (*id <= nThreads-1) {
                fifo_push(args->fifo_sorted, &array3);
            }
            //halve the number of merges that need to be done in the next step
            nThreads = nThreads / 2;
            
        }
        return NULL;
        
    }
}

/**
 * @brief The distributor function that reads the input file and distributes the data to worker threads.
 * 
 * @param arg 
 * @return NULL
 */
void *distributor (void *arg){
    
    printf("Thread distributor created \n");
    char *filename = getFileName();
    FILE *f;
    int *arr;
    int arr_size;

    //open the file
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
    //read to the array
    if(fread(arr, sizeof(int), arr_size, f)<=0){
        printf("Error reading file %s   \n", filename);
        exit(EXIT_FAILURE);
    };
    fclose(f);
    //divide the array in nThreads subarrays and put them in the fifo
    for (int i=0;i<nThreads;i++){
        //allocate memory for the subarray
        array_t *arr1 = malloc(sizeof(array_t));
        arr1->array = malloc(sizeof(int)*(arr_size/nThreads));
        arr1->size = arr_size/nThreads;
        //copy the elements of arr to arr1
        for (int j=0;j<arr1->size;j++){
            arr1->array[j] = arr[i*arr1->size+j];
        }
        //put the subarrays in the fifo
        fifo_push(getFifoUnsorted(), arr1);      
    }
    free(arr);
    return NULL;
}