#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "fifo.h"

fifo_t *fifo_unsorted;
fifo_t *fifo_sorted;
//string to be used for the name of the file
char *fileName;

void initializeSharedRegion(int buffersize, char *filename) {
    //allocate memory for an array with numThreads elements where 
    //initialize the fifo_unsorted
    fifo_unsorted = malloc(sizeof(fifo_t));
    fifo_sorted = malloc(sizeof(fifo_t));
    fifo_init(fifo_unsorted, buffersize);
    fifo_init(fifo_sorted, buffersize);
    fileName = (char *)malloc(strlen(filename) + 1);
    strcpy(fileName, filename);
    
}
void printarray(int *array, int size) {
    int i;
    for (i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

char *getFileName() {
    return fileName;
}
fifo_t *getFifoUnsorted() {
    return fifo_unsorted;
}
fifo_t *getFifoSorted() {
    return fifo_sorted;
}

void freeSharedRegion() {
    //free the memory allocated for the shared region
    //free the fifos
    fifo_destroy(fifo_unsorted);
    fifo_destroy(fifo_sorted);
    free(fileName);
    
}

// int main (int argc, char *argv[]) {
//     int numThreads = 1;
//     int arraySize = 10;

//     // fifos

//     fifo_unsorted = malloc(sizeof(fifo_t));
//     fifo_sorted = malloc(sizeof(fifo_t));
    

//     //initialize the shared region and the fifos
//     initializeSharedRegion(numThreads, arraySize,fifo_unsorted,fifo_sorted);

//     //test fifo_unsorted
//     array_t *array1 = malloc(sizeof(array_t));
//     array1->array = malloc(sizeof(int)*arraySize);
//     array1->size = arraySize;
//     for (int i = 0; i < arraySize; i++) {
//         array1->array[i] = i;
//     }
    
//     array_t *array2 = malloc(sizeof(array_t));
//     array2->array = malloc(sizeof(int)*5);
//     array2->size = 5;
//     for (int i = 0; i < 5; i++) {
//         array2->array[i] = i+10;
//     }
    
//     array_t *array5 = malloc(sizeof(array_t));
//     array5->array = malloc(sizeof(int)*9);
//     array5->size = 9;
//     for (int i = 0; i < 9; i++) {
//         array5->array[i] = i+20;
//     }

//     fifo_push(fifo_unsorted, array1);
//     fifo_push(fifo_unsorted, array2);
    
    
//     array_t array3 = fifo_pop(fifo_unsorted);
//     printarray(array3.array, array3.size);
    
//     fifo_push(fifo_unsorted, array5);

//     array_t array4 = fifo_pop(fifo_unsorted);
//     printarray(array4.array, array4.size);

//     array_t array6 = fifo_pop(fifo_unsorted);
//     printarray(array6.array, array6.size);

//     free(array1->array);
//     free(array1);
//     free(array2->array);
//     free(array2);
//     free(array5->array);
//     free(array5);
//     free(fifo_unsorted);
//     free(fifo_sorted);
//     free(array3.array);
//     free(array4.array);
//     free(array6.array);


//     return 1;
// }
