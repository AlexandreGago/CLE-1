#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include "fifo.h"
#include "sharedRegion.h"
#include "merge.h"

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

    // //put arr in fifo_unsorted
    // array_t *array1 = malloc(sizeof(array_t));
    // array1->array = malloc(sizeof(int)*arr_size);
    // array1->size = arr_size;
    // for (int i = 0; i < arr_size; i++) {
    //     array1->array[i] = arr[i];
    // }

    // fifo_push(fifo_unsorted, array1);
    // //get arr from fifo and call mergeSortItr
    // array_t array2 = fifo_pop(fifo_unsorted);

    // mergeSortItr(array2.array, array2.size);
    
    // fifo_push(fifo_sorted, &array2);

    // array_t array3 = fifo_pop(fifo_sorted);
    
    // //chec if array3 == arr
    // mergeSortItr(arr, arr_size);
    // bool equal = true;
    // for (int i = 0; i < array3.size; i++) {
    //     if (array3.array[i] != arr[i]) {
    //         equal = false;
    //     }
    // }
    // if (equal) {
    //     printf("Array is equal\n");
    // } else {
    //     printf("Array is not equal\n");
    // }
    // check if array3 is sorted in crescent order
    // bool crescent = true;
    // for (int i = 0; i < array3.size-1; i++) {
    //     if (array3.array[i] > array3.array[i+1]) {
    //         crescent = false;
    //     }
    // }
    // if (crescent) {
    //     printf("Array is sorted in crescent order\n");
    // } else {
    //     printf("Array is not sorted in crescent order\n");
    // }
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
    //merge the sorted subarrays
    for(int i=0;i<numThreads-1;i++){
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
        // printf("\n\n");
        free(array1.array);
        free(array2.array);
    }
    array_t array3 = fifo_pop(getFifoSorted());
    printf("array size: %d\n",array3.size);
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
    //printarray(arr, arr_size);

    for (int i=0;i<numThreads;i++){
        array_t *arr1 = malloc(sizeof(array_t));
        arr1->array = malloc(sizeof(int)*(arr_size/numThreads));
        arr1->size = arr_size/numThreads;
        //copy the elements of arr to arr1
        for (int j=0;j<arr1->size;j++){
            arr1->array[j] = arr[i*arr1->size+j];
        }
            //print the subarrays
        //printf("Subarray %d   \n", i);
        //printarray(arr1->array, arr1->size);
        //put the subarrays in the fifo
        fifo_push(getFifoUnsorted(), arr1); 
        
    }


    return NULL;

}