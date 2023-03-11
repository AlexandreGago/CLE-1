#include "sharedRegion.h"
#include "textProcessing.h"

struct FileData *filesData;
int totalFiles = 0;

void initializeSharedRegion(char** files, int numFiles){
    //allocate memory for the shared region
    filesData = (struct FileData *)malloc(numFiles * sizeof(struct FileData));
    totalFiles = numFiles;
    //initialize the shared region
    for(int i = 0; i < numFiles; i++){
        filesData[i].id = i;
        filesData[i].name = files[i];
        FILE *fp = fopen(files[i], "r");
        if (fp == NULL) {
            printf("Error opening file '%s' for reading \n", files[i]);
            exit(1);
        }
        filesData[i].file = fopen(files[i], "r");
        filesData[i].finished = 0;
        filesData[i].nWords = 0;
        for(int j = 0; j < 6; j++){
            filesData[i].nVowels[j] = 0;
        }
    }
}

void getData(struct Chunk *fileChunk){
  //fills the chunk with the data from the file
  //if the file is finished, chunk->finished = 1
  //! change to argc
  for(int i = 0; i < totalFiles; i++){
    if(filesData[i].finished == 0){
      //set the chunk file id to the file id
      fileChunk->FileId = filesData[i].id;

      // read the file
      // while((c = fgetc(filesData[i].file)) != EOF){
      //!ver melhor
      int n = fread(fileChunk->data, 1, CHUNKSIZE, filesData[i].file);
      if (n <0){
        printf("Error reading file");
        }
      fileChunk->size = n;
      //check if the file is finished
      if(feof(filesData[i].file)){
        filesData[i].finished = 1;
        fileChunk->finished = 1;
      }
      break;
      }
    }
  // }
  // printf("\nChunkFileID: %d\n", fileChunk->FileId);
  // printf("ChunkSize: %d\n", fileChunk->size);
  // printf("ChunkFinished: %d\n", fileChunk->finished);
  // printf("ChunkData: %s\n", fileChunk->data);
  // fflush(stdout);
}

void saveResults(struct Chunk *chunk){
  //sets the data from the chunk to the file
  //if the file is finished, chunk->finished = 1
  // printf("ChunkFileID: %d\n", chunk->FileId);
  // printf("ChunkSize: %d\n", chunk->size);
  // printf("ChunkFinished: %d\n", chunk->finished);
  printf("ChunkData: %s\n", chunk->data);
  fflush(stdout);
  printf("teste5");
  fflush(stdout);

  filesData[chunk->FileId].nWords += chunk->nWords;
  printf("teste6");
  fflush(stdout);


  for(int i = 0; i < 6; i++){
    filesData[chunk->FileId].nVowels[i] += chunk->nVowels[i];
  }
}

//!############ TESTING #########################

int main(int argc, char *argv[]) {
    //get text files from terminal
    char *files[2];
    files[0] = "dataSet1/asd.txt";
    files[1] = "dataSet1/text0.txt";
    // files[2] = "dataSet1/text1.txt";
    //initialize the shared region

    initializeSharedRegion(files, 2);

    // for(int i = 0; i < 3; i++){
    //     printf("FileId: %d\n", filesData[i].id);
    //     printf("FileName: %s\n", filesData[i].name);
    //     printf("FileName: %p\n", filesData[i].file);
        // printf("finished: %d\n", filesData[i].finished);
        // printf("nWords: %d\n", filesData[i].nWords);
        // for(int j = 0; j < 5; j++){
        //     printf("nVowels[%d]: %d\n", j, filesData[i].nVowels[j]);
        // }
    // }
    struct Chunk chunk;
    //malloc the buffer
    chunk.data = malloc(CHUNKSIZE);
    chunk.size = 0;
    chunk.nWords = 0;
    chunk.finished = 0;
    chunk.FileId = -1;
    memset(chunk.data, 0, CHUNKSIZE);
    for(int j = 0; j < 6; j++){
        chunk.nVowels[j] = 0;
    }
    //get data from the file
    for (int i = 0; i < 10; i++){
        printf("teste1");
  fflush(stdout);

        getData(&chunk);
        printf("teste2");
  fflush(stdout);


        processChunk(&chunk);
        printf("teste3");
  fflush(stdout);

        saveResults(&chunk);
        printf("teste4");
  fflush(stdout);


        //reset chunk
        chunk.size = 0;
        chunk.nWords = 0;
        chunk.finished = 0;
        chunk.FileId = -1;
        memset(chunk.data, 0, CHUNKSIZE);
        for(int j = 0; j < 6; j++){
            chunk.nVowels[j] = 0;
        }
    }

    // //print chunk
    // printf("ChunkData: %s\n", chunk.data);
    // printf("ChunkSize: %d\n", chunk.size);
    // printf("ChunkFileID: %d\n", chunk.FileId);
    // printf("ChunkFinished: %d\n", chunk.finished);
    // printf("ChunknWords: %d\n", chunk.nWords);
    // for(int j = 0; j < 6; j++){
    //     printf("ChunknVowels[%d]: %d\n", j, chunk.nVowels[j]);
    // }
    // fflush(stdout);
    //print file data
    for(int i = 0; i < totalFiles; i++){
        printf("FileId: %d\n", filesData[i].id);
        printf("FileName: %s\n", filesData[i].name);
        printf("FileName: %p\n", filesData[i].file);
        printf("finished: %d\n", filesData[i].finished);
        printf("nWords: %d\n", filesData[i].nWords);
        for(int j = 0; j < 6; j++){
            printf("nVowels[%d]: %d\n", j, filesData[i].nVowels[j]);
        }
    }

    return 0;
}
