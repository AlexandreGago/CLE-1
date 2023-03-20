/**
 * @file sharedRegion.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-20
 * 
 * 
 */
#include "sharedRegion.h"
#include "textProcessing.h"

struct FileData *filesData;
int totalFiles = 0;
pthread_mutex_t lockGetData, lockSaveData;

void initializeSharedRegion(char **files, int numFiles) {
    //allocate memory for the shared region
    
    filesData = (struct FileData *) malloc(numFiles * sizeof(struct FileData));
    if (filesData == NULL) {
    // handle the error
    printf("Error: Unable to allocate memory for the shared region \n");
    exit(1);
    }
    totalFiles = numFiles;

    //initialize mutex locks for the shared region
    //!change to 1 mutex called lock
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
        filesData[i].corrupt = 0;
        FILE *fp = fopen(files[i], "r");
        if (fp == NULL) {
            // printf("Error opening file '%s' for reading \n", files[i]);
            filesData[i].finished = 1;
            filesData[i].corrupt = 1;
        }
        
        filesData[i].file = fp;
        filesData[i].nWords = 0;
        for (int j = 0; j < 6; j++) {
            filesData[i].nVowels[j] = 0;
        }
    
        
    }
}

void printFilesData() {
    for (int i = 0; i < totalFiles; i++) {
        printf("\n-------------------------------");
        if (filesData[i].corrupt == 1) {
            printf("\nFile: %s\n", filesData[i].name);
            printf("File is invalid/corrupt \n");
        }
        else{
            printf("\nFile: %s\n", filesData[i].name);
            printf("Number of words: %d\n", filesData[i].nWords);
            //print vowels with alignment
            //   a   e   i   o   u   y
            // 123 123 123 123 123 123
            printf("%4c %4c %4c %4c %4c %4c\n", 'a', 'e', 'i', 'o', 'u', 'y');
            printf("%4d %4d %4d %4d %4d %4d\n", filesData[i].nVowels[0], filesData[i].nVowels[1], filesData[i].nVowels[2], filesData[i].nVowels[3], filesData[i].nVowels[4], filesData[i].nVowels[5]);
        }

    }
    printf("-------------------------------");

}

void freeSharedRegion() {

    for (int i = 0; i < totalFiles; i++) {
        // printf("Closing file %s %p a\n", filesData[i].name,filesData[i].file);
        // printf("%p",filesData[i].file);
        fflush(stdout);
        filesData[i].file = fopen(filesData[i].name, "r");
        if (filesData[i].file != NULL){
            if (fclose(filesData[i].file)) {
                printf("Error closing file %s \n", filesData[i].name);
            }
        }
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

//! use mutex
int allFilesFinished() {
    for (int i = 0; i < totalFiles; i++) {
        if (filesData[i].finished == 0) {
            return 0;
        }
    }
    return 1;
}

int getData(struct Chunk *fileChunk) {
    //lock the shared region to get data
    if (pthread_mutex_lock(&lockGetData)) {
        printf("Error locking mutex in getData \n");
        return -1;
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
            int n;
            if ((n = readToChunk(&filesData[i], fileChunk))== 0) {
                printf("Error reading file to chunk \n");
                return -1;
            }

            fileChunk->size = n;
            break;
        }
    }
    //unlock the shared region to get data
    if (pthread_mutex_unlock(&lockGetData)) {
        printf("Error unlocking mutex \n");
        return -1;
    }
    return 0;
}

int saveResults(struct Chunk *chunk) {
    //lock the shared region to save data
    if (pthread_mutex_lock(&lockSaveData)) {
        printf("Error locking mutex in Save Results \n");
        return -1;
    }

    filesData[chunk->FileId].nWords += chunk->nWords;

    for (int i = 0; i < 6; i++) {
        filesData[chunk->FileId].nVowels[i] += chunk->nVowels[i];
    }
    //unlock the shared region to save data
    if (pthread_mutex_unlock(&lockSaveData)) {
        printf("Error unlocking mutex \n");
        return -1;
    }
    return 0;
}

//!use mutex
void SignalCorruptFile(int FileId){
    filesData[FileId].corrupt = 1;
    filesData[FileId].finished = 1;
}