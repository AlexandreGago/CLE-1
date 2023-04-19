#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include "fifo.h"
#include "sharedRegion.h"
#include "biton.h"

void *worker(void *arg);
void *distributor (void *arg);

int numThreads = 1;
int buffersize = 1000;

int main(int argc, char *argv[]) {


    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    //initialize the shared region and the fifos
    initializeSharedRegion(buffersize, argv[1]);

    pthread_t distributor_thread;
    pthread_t workers[numThreads];
    unsigned int workerId[numThreads];

    //create distributor thread
    if (pthread_create(&distributor_thread, NULL, distributor, NULL) != 0) {
        printf("Error creating distributor thread \n");
        exit(EXIT_FAILURE);
    }

    //create worker threads
    for (int i = 0; i < numThreads; i++) {
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
    for (int i = 0; i < numThreads; i++) {
        if (pthread_join(workers[i], NULL) != 0) {
            printf("Error joining worker thread \n");
            exit(EXIT_FAILURE);
        }
    }
    printf("All threads finished\n");
    fflush(stdout);
    //unite the sorted arrays in one array one crescent and one descrescent at a time


    bool direction = true;
    for (int i = 0; i < numThreads-1; i++) {
        array_t array1 = fifo_pop(getFifoSortedCrescent());
        array_t array2 = fifo_pop(getFifoSortedDecrescent());
        int size = array1.size + array2.size;
        //unite the two arrays
        int *array3 = malloc(sizeof(int) * (size));
        //copy the array1 and array2 in array3
        for (int j = 0; j < array1.size; j++) {
            array3[j] = array1.array[j];
        }
        for (int j = 0; j < array2.size; j++) {
            array3[array1.size + j] = array2.array[j];
        }
        bitonic_sort(array3, size, size, direction);
        //check if the array is sorted crescent or decrescent
        bool crescent = true;
        bool decrescent = true;
        for (int j = 0; j < size - 1; j++) {
            if (array3[j] > array3[j + 1]) {
                crescent = false;
                break;
            }
            if (array3[j] < array3[j + 1]) {
                decrescent = false;
                break;
            }
        }
        //print crescent or decrescent
        if (crescent) {
            printf("Crescent\n");
        } else if (decrescent) {
            printf("Decrescent\n");
        } else {
            printf("Not sorted\n");
        }
        direction = !direction;
        //put the array in the fifo_sorted
        array_t *array4 = malloc(sizeof(array_t));
        array4->array = array3;
        array4->size = array1.size+array2.size;
        array4->direction = direction;
        if (direction) {
            fifo_push(getFifoSortedCrescent(), array4);
        } else {
            fifo_push(getFifoSortedDecrescent(), array4);
        }
        free(array1.array);
        free(array2.array);
    }
    //print the sorted array
    array_t array;
    if(direction){
        array = fifo_pop(getFifoSortedCrescent());
    }
    else{
        array = fifo_pop(getFifoSortedDecrescent());
    }
    //check if the array is sorted crescent order 
    bool crescent = true;
    for (int i = 0; i < array.size - 1; i++) {
        if (array.array[i] < array.array[i + 1]) {
            crescent = false;
            break;
        }
    }
    if (crescent) {
        printf("Sorted DeCrescent\n");
    } else {
        printf("Not sorted\n");
    }
    free(array.array);
    freeSharedRegion();

    return 0;
}

void *worker(void *arg) {
    int *id = (int *)arg;
    // printf("Thread worker %d created \n", *id);
    //while (true) {
    array_t array = fifo_pop(getFifoUnsorted());

    bitonic_sort(array.array, array.size, 2,array.direction);
    if (array.direction) {
        fifo_push(getFifoSortedCrescent(), &array);
    } else {
        fifo_push(getFifoSortedDecrescent(), &array);
    }
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
    //printarray(arr, arr_size);
    bool direction = true;
    for (int i=0;i<numThreads;i++){
        array_t *arr1 = malloc(sizeof(array_t));
        arr1->array = malloc(sizeof(int)*(arr_size/numThreads));
        arr1->size = arr_size/numThreads;
        arr1->direction = direction;
        //copy the elements of arr to arr1
        for (int j=0;j<arr1->size;j++){
            arr1->array[j] = arr[i*arr1->size+j];
        }
            //print the subarrays
        //printf("Subarray %d   \n", i);
        //printarray(arr1->array, arr1->size);
        //put the subarrays in the fifo
        fifo_push(getFifoUnsorted(), arr1); 
        direction = !direction;
    }


    
    // int n_iter = (int)log2(arr_size) + 1;

    // for ( int i = 0; i < n_iter; i++) {
        
    //     int subarray_size = arr_size/ numThreads;


    // }
    //break array into subarrays
    


    return NULL;

}