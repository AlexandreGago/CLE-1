#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "structs.h"
#include <time.h>

#include "textProcessing.h"

void processcmd(int argc, char* argv[]);
void printFilesData(FileData *filesData, int numFiles);
int SendChunk(FileData *filesData, Chunk *chunk, int worker, int *currentFile);
int allFilesDone();
int initializeDistributor(char **files, int numFiles,FileData *filesData);
int clearChunk(Chunk *chunk);
void parseCommandLine(int argc, char *argv[], char **files);

int chunkSize =4096;
MPI_Request dummyRequest = MPI_REQUEST_NULL;

int main (int argc, char *argv[]){
    int rank;
    int size;
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    MPI_Status status;

    if (rank == 0){//dispacher Non-blocking send and receive
        char **files = (char **) malloc( (argc-optind) * sizeof(char *));
        int currentFile = 0;
        //parse command line
        parseCommandLine(argc, argv, files);

        //Broadcast chunk size
        MPI_Bcast(&chunkSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

        printf ("dispacher initiated \n");
        fflush(stdout);
        //read data from file
        FILE *fp;
        fp = fopen("./dataSet1/teste.txt", "r");
        if (fp == NULL){
            printf("Error opening file  %s \n", "data.txt");
            MPI_Abort(MPI_COMM_WORLD, 1);

        }

        FileData *filesData = (FileData *) malloc( (argc-optind) * sizeof(FileData));
        //initialize the distributor
        if(initializeDistributor(files, argc-optind, filesData) != EXIT_SUCCESS){
            printf("Error initializing the distributor \n");
            MPI_Abort(MPI_COMM_WORLD, 1);

        }
        //{
            //printFilesData(filesData, argc-optind);
            //*DEBUG
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
            //*/
            //}


        Chunk chunk;
        unsigned char data_buffer[chunkSize]; // allocate data_buffer on the stack
        chunk.data = data_buffer; // set the data pointer to point to the stack-allocated buffer
        
        //first thing to do is send a chunk to every worker
        //create an array of requests to keep track of receives
        Chunk chunks[size-1];
        for ( int i = 1; i < size; i++) {
            chunks[i-1].data = NULL;
            if (clearChunk(&chunks[i-1]) != EXIT_SUCCESS){
                printf("Error clearing chunk \n");
                MPI_Abort(MPI_COMM_WORLD, 1);

            }
        }
        int unprocessedChunks =0;
        MPI_Request requests[size-1];
        for (int i = 1; i < size; i++) {
            requests[i-1] = MPI_REQUEST_NULL;
            if (currentFile < argc-optind){
                SendChunk(filesData, &chunk, i, &currentFile);
                MPI_Irecv(&chunks[i-1], sizeof(Chunk), MPI_BYTE, i, 0, MPI_COMM_WORLD, &requests[i-1]);
                unprocessedChunks++;
                fflush(stdout);
            }
        }

        //test each request to see if it has finished
        //TODO files finisehd
        int flag;
        int finished = currentFile >= argc-optind;
        // printf("finished: %d \n", finished);
        // printf("unprocessedChunks: %d \n", unprocessedChunks);
        fflush(stdout);
        while (!finished || unprocessedChunks > 0) { 
            for (int i = 1; i < size; i++) {
                flag = 0;
                //not having this causes undefined behaviour
                if (requests[i-1] != MPI_REQUEST_NULL) {
                    MPI_Test(&requests[i-1], &flag, &status);
        
                    if (flag) {
                        unprocessedChunks--;
                        //a request has finished
                        //save the data
                        filesData[chunks[i-1].FileId].nWords += chunks[i-1].nWords;

                        for (int j = 0; j < 6; j++) {
                            filesData[chunks[i-1].FileId].nVowels[j] += chunks[i-1].nVowels[j];
                        }

                        // //print the received chunk
                        // printf("dispacher received chunk from worker %d \n", i);
                        // printf("chunk.FileId: %d \n", chunks[i-1].FileId);
                        // printf("chunk.nWords: %d \n", chunks[i-1].nWords);
                        // for (int j = 0; j < 6; j++) {
                        //     printf("chunk.nVowels[%d]: %d \n", j, chunks[i-1].nVowels[j]);
                        // }
                        // // printf("chunk.data: %s \n", chunks[i].data);
                        // fflush(stdout);
                        

                        if (clearChunk(&chunks[i-1]) != EXIT_SUCCESS){
                            printf("Error clearing chunk \n");
                            MPI_Abort(MPI_COMM_WORLD, 1);
                        }

                        if (currentFile < argc-optind){
                            //send a new chunk to the worker
                            printf("sending chunk to worker %d \n", i);
                            fflush(stdout);
                            SendChunk(filesData, &chunk, i, &currentFile);
                            requests[i-1] = MPI_REQUEST_NULL;
                            MPI_Irecv(&chunks[i-1], sizeof(Chunk), MPI_BYTE, i, 0, MPI_COMM_WORLD, &requests[i-1]);
                            unprocessedChunks++;
                        }
                        
                        finished = currentFile>=argc-optind;
                        
                    }
                }
            }
        }
        //send message to workers to finish
 
        printf("dispacher finished \n");
        printFilesData(filesData, argc-optind);
        MPI_Abort(MPI_COMM_WORLD, 1);

        // MPI_Wait(&request, MPI_STATUS_IGNORE);
        // while(1){
        //     if(allFilesDone()){
        //         break;
        //     }
        // }
    }

    //change to > 0
    else {//workers use blocking sends and receives
        printf("worker %d initiated\n", rank);
        fflush(stdout);
        //receive chunk size
        MPI_Bcast(&chunkSize, 1, MPI_INT, 0, MPI_COMM_WORLD);



        while (1) {
      
            

            // Declare a buffer to hold the received data
            unsigned char* recv_buffer = NULL;

            // Declare a variable to hold the size of the incoming message
            int incoming_size;



            // Probe the incoming message to get its size
            MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_BYTE, &incoming_size);

            recv_buffer = (unsigned char*)malloc(incoming_size);

            //*Debug
            // printf("Worker %d: Incoming message size: %d\n", rank, incoming_size);
            // printf("Size of Chunk struct: %ld\n", sizeof(Chunk));
            // printf("Size of incoming data: %ld\n", incoming_size - sizeof(Chunk));
            //*/
            
            MPI_Recv(recv_buffer, incoming_size, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


            //?maneira 3 - Stack, 
            Chunk* received_chunk = (Chunk*)recv_buffer;

            // Declare a fixed-size buffer for the data field
            unsigned char data_buffer[chunkSize+1];

            // Copy the data field from the receive buffer to the data buffer
            memcpy(data_buffer, recv_buffer + sizeof(Chunk), received_chunk->size);

            // Null terminate the data buffer
            data_buffer[received_chunk->size] = '\0';

            // Set the data field of the received_chunk to point to the data buffer
            received_chunk->data = data_buffer;


            //Process the chunk
            if (processChunk(received_chunk) != EXIT_SUCCESS) {
                printf("Error processing chunk \n");
                MPI_Abort(MPI_COMM_WORLD, 1);

            }

            //*DEBUG
            // print all chunk fields
            // printf("FileId: %d\n", received_chunk->FileId);
            // printf("size: %d\n", received_chunk->size);
            // printf("nWords: %d\n", received_chunk->nWords);
            // for (int i = 0; i < 6; i++) {
            //     printf("nVowels[%d]: %d\n", i, received_chunk->nVowels[i]);
            // }
            // printf("data: %s\n", received_chunk->data);
            //*/

            //Send the chunk back to the dispatcher
            //only need to send results, not text
            //set data to null
            received_chunk->data = NULL;
            printf( "worker %d finished \n", rank);
            fflush(stdout);
            printf("worker %d sending chunk \n", rank);
            MPI_Send(received_chunk, sizeof(Chunk), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
        }


    }

    MPI_Finalize ();
    return EXIT_SUCCESS;
}

int SendChunk(FileData *filesData, Chunk *chunk, int worker, int* currentFile){

    //clear the chunk
    if  (clearChunk(chunk) != EXIT_SUCCESS){
        printf("Error clearing the chunk \n");
        MPI_Abort(MPI_COMM_WORLD, 1);

    }

    chunk->FileId = filesData[*currentFile].id;
    printf("currentFile %d \n", *currentFile);
    fflush(stdout);
    
    int n;
    if ((n = readToChunk(&filesData[*currentFile], chunk,currentFile)) == 0) {
        printf("Error reading file to chunk \n");
        return -1;
    }
    printf("READ TO CHUNK %d \n", n);


    chunk->size = n;
    // Calculate the size of the buffer needed to hold the Chunk struct
    int buffer_size = sizeof(Chunk) + chunk->size;

    // Allocate a buffer of the appropriate size
    unsigned char* buffer = (unsigned char*)malloc(buffer_size);

    // Copy the Chunk struct into the buffer
    memcpy(buffer, chunk, sizeof(Chunk));
    memcpy(buffer + sizeof(Chunk), chunk->data, chunk->size);
    MPI_Isend(buffer, buffer_size, MPI_BYTE, worker, 0, MPI_COMM_WORLD, &dummyRequest);
    return 0;
}


int clearChunk(Chunk *chunk){
    chunk->size = 0;
    chunk->nWords = 0;
    chunk->FileId = -1;

    
    if (chunk->data != NULL) {
        if (memset(chunk->data, 0, chunkSize) == NULL) {
            printf("Error clearing Chunk\n");
            MPI_Abort(MPI_COMM_WORLD, 1);

        }
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

    //initialize the filesData structure
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

void printFilesData(FileData *filesData, int totalFiles) {
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
    printf("-------------------------------\n");
}
void parseCommandLine(int argc, char *argv[], char **files){
    int opt;

    //*parse command line arguments
    do{
        switch (opt = getopt(argc, argv, "s:h"))
        {
        case 's':
            //only valid values are 4 and 8
            if (atoi(optarg) == 4) {
                chunkSize = 4096;
            } else if (atoi(optarg) == 8) {
                chunkSize = 8192;
            } else {
                printf("Invalid chunk size, valid values are:\n 4 - 4096 bytes\n 8 - 8192 bytes\n");
                // printUsage(argv[0]);r
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
            break;

        case 'h':
            // printUsage(argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);

            break;

        case '?':
            printf("Invalid option\n");
            // printUsage(argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
            break;

        }
    } while (opt != -1);

    //files are provided after the options
    if (argc - optind < 1) {
        printf("No files provided\n");
        // printUsage(argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);

    }
    for (int i = optind; i < argc; i++) {
        files[i-optind] = argv[i];
    }
    //*END parse command line arguments
}

