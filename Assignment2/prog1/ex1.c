#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "structs.h"
#include <time.h>

#include "textProcessing.h"

void processcmd(int argc, char* argv[]);

int allFilesDone();
int initializeDistributor(char **files, int numFiles,FileData *filesData);
int clearChunk(Chunk *chunk);

int chunkSize = 4096;

int main (int argc, char *argv[]){
    int rank;
    int size;
    int opt;
    int chunkSize;
    char data[ChunkSize];

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    MPI_Status status;

    MPI_Request request;

    if (rank == 0){//dispacher Non-blocking send and receive

        //parse command line arguments
        do
        {
            switch (opt = getopt(argc, argv, "s:h"))
            {
            case 's':
                int argChunkSize = atoi(optarg);
                //only valid values are 4 and 8
                if (argChunkSize == 4) {
                    chunkSize = 4096;
                } else if (argChunkSize == 8) {
                    chunkSize = 8192;
                } else {
                    printf("Invalid chunk size, valid values are:\n 4 - 4096 bytes\n 8 - 8192 bytes\n");
                    // printUsage(argv[0]);r
                    exit(1);
                }
                break;

            case 'h':
                // printUsage(argv[0]);
                exit(0);
                break;

            case '?':
                printf("Invalid option\n");
                // printUsage(argv[0]);
                exit(1);
                break;

            }
        } while (opt != -1);

        //files are provided after the options
        if (argc - optind < 1) {
            printf("No files provided\n");
            // printUsage(argv[0]);
            exit(1);
        }
        char *files[argc-optind];
        for (int i = optind; i < argc; i++) {
            files[i-optind] = argv[i];
        }
        //END parse command line arguments


        printf ("dispacher initiated \n");
        //read data from file
        FILE *fp;
        fp = fopen("./dataSet1/teste.txt", "r");
        if (fp == NULL){
            printf("Error opening file  %s \n", "data.txt");
            exit(1);
        }

        FileData *filesData = (FileData *) malloc( (argc-optind) * sizeof(FileData));
        //initialize the distributor
        if(initializeDistributor(files, argc-optind,filesData) != EXIT_SUCCESS){
            printf("Error initializing the distributor \n");
            exit(1);
        }
        //print filesdata fields
        // for (int i = 0; i < argc-optind; i++) {
        //     printf("id: %d\n", filesData[i].id);
        //     printf("name: %s\n", filesData[i].name);
        //     printf("finished: %d\n", filesData[i].finished);
        //     printf("corrupt: %d\n", filesData[i].corrupt);
        //     printf("file: %p\n", filesData[i].file);
        //     printf("nWords: %d\n", filesData[i].nWords);
        //     for (int j = 0; j < 6; j++) {
        //         printf("nVowels[%d]: %d\n", j, filesData[i].nVowels[j]);
        //     }
        // }
        //send data to workers

        Chunk chunk;
        //malloc the buffer
        if ((chunk.data = malloc(chunkSize)) == NULL) {
            printf("Error allocating memory for Chunk\n");
            exit(EXIT_FAILURE);
        }
        if (memset(chunk.data, 0, chunkSize) == NULL) {
            printf("Error clearing memory in Chunk\n");
            exit(EXIT_FAILURE);
        }

        for (int i = 1; i < 2; i++){
            //!cut this into functions
            //clear the chunk
            if  (clearChunk(&chunk) != EXIT_SUCCESS){
                printf("Error clearing the chunk \n");
                exit(1);
            }
            chunk.FileId = filesData[0].id;
            int n;
            if ((n = readToChunk(&filesData[0], &chunk))== 0) {
                printf("Error reading file to chunk \n");
                return -1;
            }

            chunk.size = n;

            // Calculate the size of the buffer needed to hold the Chunk struct
            int buffer_size = sizeof(Chunk) + chunk.size;

            // Allocate a buffer of the appropriate size
            unsigned char* buffer = (unsigned char*)malloc(buffer_size);

            // Copy the Chunk struct into the buffer
            memcpy(buffer, &chunk, sizeof(Chunk));
            memcpy(buffer + sizeof(Chunk), chunk.data, chunk.size);


            MPI_Send(buffer, buffer_size, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
        }
        // MPI_Wait(&request, MPI_STATUS_IGNORE);
        // while(1){
        //     if(allFilesDone()){
        //         break;
        //     }
        // }
    }

    //change to > 0
    else if (rank ==1){//workers use blocking sends and receives
        printf("worker %d initiated\n", rank);

        // Declare a buffer to hold the received data
        unsigned char* recv_buffer = NULL;

        // Declare a variable to hold the size of the incoming message
        int incoming_size;

        // Probe the incoming message to get its size
        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_BYTE, &incoming_size);

        recv_buffer = (unsigned char*)malloc(incoming_size);

        MPI_Recv(recv_buffer, incoming_size, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //?maneira 1
        // Chunk received_chunk;
        // int header_size = sizeof(Chunk);
        // memcpy(&received_chunk, recv_buffer, header_size);

        // // Allocate memory for the data field and copy its contents
        // received_chunk.data = (unsigned char*)malloc(received_chunk.size);
        // memcpy(received_chunk.data, recv_buffer + header_size, received_chunk.size);

        //?maneira 2
        Chunk* received_chunk = (Chunk*)recv_buffer;
        // Allocate a separate buffer for the data field
        unsigned char* data_buffer = (unsigned char*)malloc(received_chunk->size);

        // Copy the data field from the receive buffer to the data buffer
        memcpy(data_buffer, recv_buffer + sizeof(Chunk), received_chunk->size);

        // Set the data field of the received_chunk to point to the data buffer
        received_chunk->data = data_buffer;

        //print all chunk fields
        printf("FileId: %d\n", received_chunk->FileId);
        printf("size: %d\n", received_chunk->size);
        printf("nWords: %d\n", received_chunk->nWords);
        for (int i = 0; i < 6; i++) {
            printf("nVowels[%d]: %d\n", i, received_chunk->nVowels[i]);
        }
        printf("data: %s\n", received_chunk->data);

    }

    MPI_Finalize ();
    return EXIT_SUCCESS;
}

int allFilesDone(){
    return 0;
}
int clearChunk(Chunk *chunk){
    chunk->size = 0;
    chunk->nWords = 0;
    chunk->FileId = -1;
    if (memset(chunk->data, 0, chunkSize) == NULL) {
        printf("Error clearing Chunk\n");
        exit(EXIT_FAILURE);
    }
    for (int j = 0; j < 6; j++) {
        chunk->nVowels[j] = 0;
    }
    return EXIT_SUCCESS;
}
int initializeDistributor(char **files, int numFiles, FileData *filesData) {

    if (filesData == NULL) {
        // handle the error
        printf("Error: Unable to allocate memory for the shared region \n");
        return EXIT_FAILURE;
    }
     //initialize the memory
    memset(filesData, 0, numFiles * sizeof(FileData));

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
}