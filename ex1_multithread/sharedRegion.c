#include "sharedRegion.h"
#include "textProcessing.h"

struct FileData *filesData;
int totalFiles = 0;
pthread_mutex_t lockGetData, lockSaveData;

void initializeSharedRegion(char **files, int numFiles) {
    //allocate memory for the shared region
    filesData = (struct FileData *) malloc(numFiles * sizeof(struct FileData));
    totalFiles = numFiles;

    //initialize mutex locks for the shared region
    if (pthread_mutex_init(&lockGetData, NULL)) {
        printf("Error initializing mutex \n");
        exit(1);
    }
    if (pthread_mutex_init(&lockSaveData, NULL)) {
        printf("Error initializing mutex \n");
        exit(1);
    }

    //initialize the shared region with the data of the files to process
    for (int i = 0; i < numFiles; i++) {
        filesData[i].id = i;
        filesData[i].name = files[i];
        filesData[i].finished = 0;
        FILE *fp = fopen(files[i], "r");
        if (fp == NULL) {
            printf("Error opening file '%s' for reading \n", files[i]);
            filesData[i].finished = 1;
        }
        filesData[i].file = fopen(files[i], "r");
        filesData[i].nWords = 0;
        for (int j = 0; j < 6; j++) {
            filesData[i].nVowels[j] = 0;
        }
    }
}

void printFilesData() {
    for (int i = 0; i < totalFiles; i++) {
        printf("File: %s\n", filesData[i].name);
        printf("Number of words: %d\n", filesData[i].nWords);
        printf("Number of vowels: ");
        for (int j = 0; j < 6; j++) {
            printf("%d ", filesData[i].nVowels[j]);
        }
        printf("\n");
    }
}

void freeSharedRegion() {
    for (int i = 0; i < totalFiles; i++) {
        fclose(filesData[i].file);
    }
    free(filesData);
    if (pthread_mutex_destroy(&lockGetData)) {
        printf("Error destroying mutex lockGetData \n");
        exit(1);
    }
    if (pthread_mutex_destroy(&lockSaveData)) {
        printf("Error destroying mutex lockSaveData \n");
        exit(1);
    }
}

int allFilesFinished() {
    for (int i = 0; i < totalFiles; i++) {
        if (filesData[i].finished == 0) {
            return 0;
        }
    }
    return 1;
}

void getData(struct Chunk *fileChunk) {
    //lock the shared region to get data
    if (pthread_mutex_lock(&lockGetData)) {
        printf("Error locking mutex \n");
        exit(1);
    }
    //fills the chunk with the data from the file
    //if the file is finished, chunk->finished = 1

    for (int i = 0; i < totalFiles; i++) {
        if (filesData[i].finished == 0) {
            //set the chunk file id to the file id
            fileChunk->FileId = filesData[i].id;

            //this will read file to chunk and return the number of bytes read,
            //not cutting words in half
            //will also close the file if it is finished
            int n = readToChunk(&filesData[i], fileChunk);

            fileChunk->size = n;
            break;
        }
    }
    //unlock the shared region to get data
    if (pthread_mutex_unlock(&lockGetData)) {
        printf("Error unlocking mutex \n");
        exit(1);
    }
}

void saveResults(struct Chunk *chunk) {
    //lock the shared region to save data
    if (pthread_mutex_lock(&lockSaveData)) {
        printf("Error locking mutex \n");
        exit(1);
    }
    //sets the data from the chunk to the file
    //if the file is finished, chunk->finished = 1
    // printf("ChunkFileID: %d\n", chunk->FileId);
    // printf("ChunkSize: %d\n", chunk->size);
    // printf("ChunkFinished: %d\n", chunk->finished);
    // printf("ChunkData: %s\n", chunk->data);
    // fflush(stdout);


    filesData[chunk->FileId].nWords += chunk->nWords;

    for (int i = 0; i < 6; i++) {
        filesData[chunk->FileId].nVowels[i] += chunk->nVowels[i];
    }
    //unlock the shared region to save data
    if (pthread_mutex_unlock(&lockSaveData)) {
        printf("Error unlocking mutex \n");
        exit(1);
    }
}

//!############ TESTING #########################

// int main(int argc, char *argv[]) {
//     //get text files from terminal
//     char *files[1];
//     files[0] = "dataSet1/asd.txt";
//     // files[1] = "dataSet1/text0.txt";
//     // files[2] = "dataSet1/text1.txt";
//     //initialize the shared region

//     initializeSharedRegion(files, 1);

//     // for(int i = 0; i < 3; i++){
//     //     printf("FileId: %d\n", filesData[i].id);
//     //     printf("FileName: %s\n", filesData[i].name);
//     //     printf("FileName: %p\n", filesData[i].file);
//         // printf("finished: %d\n", filesData[i].finished);
//         // printf("nWords: %d\n", filesData[i].nWords);
//         // for(int j = 0; j < 5; j++){
//         //     printf("nVowels[%d]: %d\n", j, filesData[i].nVowels[j]);
//         // }
//     // }
//     struct Chunk chunk;
//     //malloc the buffer
//     chunk.data = malloc(CHUNKSIZE);
//     chunk.size = 0;
//     chunk.nWords = 0;
//     chunk.finished = 0;
//     chunk.FileId = -1;
//     memset(chunk.data, 0, CHUNKSIZE);
//     for(int j = 0; j < 6; j++){
//         chunk.nVowels[j] = 0;
//     }
//     //get data from the file
//     for (int i = 0; i < 1; i++){

//         getData(&chunk);
//         processChunk(&chunk);

//         saveResults(&chunk);
//         //reset chunk
//         chunk.size = 0;
//         chunk.nWords = 0;
//         chunk.finished = 0;
//         chunk.FileId = -1;
//         memset(chunk.data, 0, CHUNKSIZE);
//         for(int j = 0; j < 6; j++){
//             chunk.nVowels[j] = 0;
//         }
//     }

//     // //print chunk
//     // printf("ChunkData: %s\n", chunk.data);
//     // printf("ChunkSize: %d\n", chunk.size);
//     // printf("ChunkFileID: %d\n", chunk.FileId);
//     // printf("ChunkFinished: %d\n", chunk.finished);
//     // printf("ChunknWords: %d\n", chunk.nWords);
//     // for(int j = 0; j < 6; j++){
//     //     printf("ChunknVowels[%d]: %d\n", j, chunk.nVowels[j]);
//     // }
//     // fflush(stdout);
//     //print file data
//     for(int i = 0; i < totalFiles; i++){
//         printf("FileId: %d\n", filesData[i].id);
//         printf("FileName: %s\n", filesData[i].name);
//         printf("FileName: %p\n", filesData[i].file);
//         printf("finished: %d\n", filesData[i].finished);
//         printf("nWords: %d\n", filesData[i].nWords);
//         for(int j = 0; j < 6; j++){
//             printf("nVowels[%d]: %d\n", j, filesData[i].nVowels[j]);
//         }
//     }

//     return 0;
// }
