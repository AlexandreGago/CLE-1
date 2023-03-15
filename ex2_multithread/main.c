#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include "fifo.h"
#include "sharedRegion.h"
#include "merge.h"

void *worker(void *arg);
void *distributor (void *arg);

int numThreads = 1;
int arraySize = 100;

int main(int argc, char *argv[]) {

    FILE *f;
    int *arr;
    int arr_size;
    // int i;
    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }
    f = fopen(argv[1], "rb");
    if (f == NULL) {
        printf("Error opening file %s   \n", argv[1]);
    }
    //read the number of elements
    if(fread(&arr_size, sizeof(int), 1, f)<=0){
        printf("Error reading file %s   \n", argv[1]);
        exit(0);
    };
    //allocate memory for the array
    arr = (int *)malloc(arr_size * sizeof(int));
    //read to the array with malloc
    if(fread(arr, sizeof(int), arr_size, f)<=0){
        printf("Error reading file %s   \n", argv[1]);
        exit(0);
    };

    // fifos
    fifo_t *fifo_unsorted;
    fifo_t *fifo_sorted;
    fifo_unsorted = malloc(sizeof(fifo_t));
    fifo_sorted = malloc(sizeof(fifo_t));
    

    //initialize the shared region and the fifos
    initializeSharedRegion(numThreads, arraySize,fifo_unsorted,fifo_sorted);

    //put arr in fifo_unsorted
    array_t *array1 = malloc(sizeof(array_t));
    array1->array = malloc(sizeof(int)*arr_size);
    array1->size = arr_size;
    for (int i = 0; i < arr_size; i++) {
        array1->array[i] = arr[i];
    }

    fifo_push(fifo_unsorted, array1);
    //get arr from fifo and call mergeSortItr
    array_t array2 = fifo_pop(fifo_unsorted);

    mergeSortItr(array2.array, array2.size);
    
    fifo_push(fifo_sorted, &array2);

    array_t array3 = fifo_pop(fifo_sorted);
    
    //chec if array3 == arr
    mergeSortItr(arr, arr_size);
    bool equal = true;
    for (int i = 0; i < array3.size; i++) {
        if (array3.array[i] != arr[i]) {
            equal = false;
        }
    }
    if (equal) {
        printf("Array is equal\n");
    } else {
        printf("Array is not equal\n");
    }
    // check if array3 is sorted in crescent order
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
        
    return 0;
}

void *worker_func(void *arg) {

//   unsigned int id,val;                                                                    /* produced value */

//   while(1){
//                                                                    /* retrieve a value */
//     }
    return NULL;
}

void *distributor (void *arg){
    
    // printf("Thread distributor created \n");

    return NULL;
}