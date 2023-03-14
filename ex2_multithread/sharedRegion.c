#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>



void initializeSharedRegion(int numThreads, int arraySize) {
    //allocate memory for an array with numThreads elements where 
    //each element is an array of size arraySize
    //initialize each element of the array to 0

    int i, j;
    int **sharedRegion = (int **)malloc(numThreads * sizeof(int *));
    for (i = 0; i < numThreads; i++) {
        sharedRegion[i] = (int *)malloc(arraySize * sizeof(int));
        for (j = 0; j < arraySize; j++) {
            sharedRegion[i][j] = 0;
        }
    }
}