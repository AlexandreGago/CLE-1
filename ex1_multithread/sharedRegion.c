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

int initializeSharedRegion(char **files, int numFiles) {
    //allocate memory for the shared region
    filesData = (struct FileData *) malloc(numFiles * sizeof(struct FileData));
    if (filesData == NULL) {
        // handle the error
        printf("Error: Unable to allocate memory for the shared region \n");
        return EXIT_FAILURE;
    }
    //save the number of files to process
    totalFiles = numFiles;

    //initialize mutex locks for the shared region
    if (pthread_mutex_init(&lockGetData, NULL)) {
        printf("Error initializing mutex \n");
        return EXIT_FAILURE;
    }
    if (pthread_mutex_init(&lockSaveData, NULL)) {
        printf("Error initializing mutex \n");
        return EXIT_FAILURE;
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
    return EXIT_SUCCESS;
;
}

void printFilesData() {
    //print the shared region data 
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

int freeSharedRegion() {
    //free the shared region
    //close all files
    for (int i = 0; i < totalFiles; i++) {
        if (filesData[i].file != NULL){
            if (fclose(filesData[i].file)) {
                printf("Error closing file %s \n", filesData[i].name);
            }
        }
    }
    free(filesData);
    //destroy mutex locks for the shared region
    if (pthread_mutex_destroy(&lockGetData)) {
        printf("Error destroying mutex lockGetData \n");
        return 0;
    }
    if (pthread_mutex_destroy(&lockSaveData)) {
        printf("Error destroying mutex lockSaveData \n");
        return 0;
    }
    return 1;
}

int allFilesFinished() {
    //check if all files are finished
    //lock the shared region
    if (pthread_mutex_lock(&lockGetData)) {
        printf("Error locking mutex in getData \n");
        exit(1);
    }
    //check if all files are finished
    int allFinished = 1;
    for (int i = 0; i < totalFiles; i++) {
        if (filesData[i].finished == 0) {
            allFinished = 0;
            //the break is needed to avoid a deadlock
            break;
        }
    }
    if (pthread_mutex_unlock(&lockGetData)) {
        printf("Error unlocking mutex \n");
        exit(1);
    }
    return allFinished;
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
    //save the results of the chunk to the shared region
    filesData[chunk->FileId].nWords += chunk->nWords;
    //save the number of vowels
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

void SignalCorruptFile(int FileId){
    //lock the shared region to save data
    if (pthread_mutex_lock(&lockGetData)) {
        printf("Error locking mutex in getData \n");
        exit(1);
    }
    //set the file as corrupt
    filesData[FileId].corrupt = 1;
    //set the file as finished
    filesData[FileId].finished = 1;
    //unlock the shared region to save data
    if (pthread_mutex_unlock(&lockGetData)) {
        printf("Error unlocking mutex \n");
        exit(1);
    }
}