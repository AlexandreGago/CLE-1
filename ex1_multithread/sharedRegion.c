#include "sharedRegion.h"

struct FileData *filesData;

void initializeSharedRegion(char* files, int numFiles){
    //allocate memory for the shared region
    filesData = (struct FileData *)malloc(numFiles * sizeof(struct FileData));
    //initialize the shared region
    for(int i = 0; i < numFiles; i++){
        filesData[i].id = i;
        filesData[i].name = files[i];
        filesData[i].file = fopen(files[i], "r");
        filesData[i].finished = 0;
        filesData[i].nWords = 0;
        for(int j = 0; j < 5; j++){
            filesData[i].nVowels[j] = 0;
        }
    }
}

void getData(struct Chunk *chunk){
  //fills the chunk with the data from the file
  //if the file is finished, chunk->finished = 1
}
void saveResults(struct Chunk *chunk){
  //sets the data from the chunk to the file
  //if the file is finished, chunk->finished = 1
}
