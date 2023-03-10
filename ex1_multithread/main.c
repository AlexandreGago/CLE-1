#include <stdio.h>
#include <stdlib.h>
//include threads
#include <pthread.h>
#include "sharedRegion.h"
#include "probConst.h"

int numThreads = 4;
int numProducers = 1;

void getNextChar(char *c, FILE *f);
char processBuffer(unsigned char *buf, int size);
void *worker(void *arg);


/**
 * @brief 
 * 
 * @param argc the number of arguments
 * @param argv  the arguments
 * @return int 
 */
int main (int argc, char *argv[]){
    for (int i = 0; i < numThreads; i++){
        pthread_create(&tid, NULL, worker, (void *) i);
    }


    return 0;
}

void getNextChar(char *c, FILE *f){
}

char processBuffer(unsigned char *buf, int size) {
}

void *worker(void *ID){

    unsigned int id = (unsigned int) ID;
    /* structure containing a chunk of a file and process it saving it to the shared zone */
    //malloc the chunk
    struct Chunk *chunk = malloc(sizeof(struct Chunk));
    //malloc the buffer
    chunk->data = malloc(sizeof(unsigned char) * ChunkSize);
    while(1){
        //get the data from the shared region
        getData(chunk);
        //process the data
        processChunk(chunk);
        //save the data to the shared region
        saveResults(chunk);
        //reset the chunk
        chunk->size = 0;
        chunk->nWords = 0;
        chunk->finished = 0;
        chunk->FileId = -1;
        memset(chunk->data, 0, ChunkSize);

    }
    free(chunk->data);
    free(chunk);
    
    pthread_exit(NULL);
    return 0;
}