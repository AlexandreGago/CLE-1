#include <mpi.h>

#include <stdio.h>

#include <stdlib.h>

#include <math.h>

#include <unistd.h>

#include <stdbool.h>

#include <time.h>

#include "merge.h"

int main(int argc, char * argv[]) {
    MPI_Comm presentComm, nextComm;
    MPI_Group presentGroup, nextGroup;
    int gMemb[8];
    int rank, nProc, nProcNow, length, nIter;
    int i, j;

    int err = MPI_Init( & argc, & argv);
    if (err != MPI_SUCCESS) {
        printf("Error initializing MPI\n");
        MPI_Abort(MPI_COMM_WORLD, err);
    }

    MPI_Comm_rank(MPI_COMM_WORLD, & rank);
    MPI_Comm_size(MPI_COMM_WORLD, & nProc);

    if (rank == 0) {
        if (nProc != 1 && nProc != 2 && nProc != 4 && nProc != 8) {
            printf("Wrong number of processes!\n Should be 1, 2, 4 or 8\n");
            MPI_Finalize();
            return EXIT_FAILURE;
        }
        if (argc != 2) {
            printf("Wrong number of arguments!\n Usage: mpirun -n %d %s <input file>\n", nProc, argv[0]);
            MPI_Finalize();
            return EXIT_FAILURE;
        }
    }

    double start_time = MPI_Wtime();
    nIter = 32 - __builtin_clz(nProc) ;
    int * sendData;

    if (rank == 0) {
        char * filename = argv[1];
        FILE * f;

        //open the file
        f = fopen(filename, "rb");
        if (f == NULL) {
            perror("Error opening file");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
            return EXIT_FAILURE;
        }
        //read the number of elements
        if (fread( & length, sizeof(int), 1, f) <= 0) {
            perror("Error reading file");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
            return EXIT_FAILURE;
        };

        if (length <= 0) {
            printf("Invalid length of the array: %d\n", length);
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
            return EXIT_FAILURE;
        }

        //allocate memory for the array
        sendData = malloc(length * sizeof(int));
        if (sendData == NULL) {
            perror("Error allocating memory");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
            return EXIT_FAILURE;
        }

        //read to the array
        if (fread(sendData, sizeof(int), length, f) <= 0) {
            perror("Error reading file");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
            return EXIT_FAILURE;
        };
        fclose(f);

        MPI_Bcast( & length, 1, MPI_INT, 0, MPI_COMM_WORLD);

    } else {
        MPI_Bcast( & length, 1, MPI_INT, 0, MPI_COMM_WORLD);
        sendData = NULL;
    }

    int * recData = malloc(length * sizeof(int));
    if (recData == NULL) {
        perror("Error allocating memory");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        return EXIT_FAILURE;
    }

    nProcNow = nProc;
    presentComm = MPI_COMM_WORLD;
    MPI_Comm_group(presentComm, & presentGroup);
    for (i = 0; i < 8; i++)
        gMemb[i] = i;
    for (j = 0; j < nIter; j++) {
        if (j > 0) {
            MPI_Group_incl(presentGroup, nProcNow, gMemb, & nextGroup);
            MPI_Comm_create(presentComm, nextGroup, & nextComm);
            presentGroup = nextGroup;
            presentComm = nextComm;
            if (rank >= nProcNow) {
                free(recData);
                MPI_Finalize();
                return EXIT_SUCCESS;
            }
        }

        MPI_Comm_size(presentComm, & nProc);
        MPI_Scatter(sendData, length / nProcNow, MPI_INT, recData, length / nProcNow, MPI_INT, 0, presentComm);

        if (j == 0)
            mergeSortItr(recData, length / nProcNow);
        else
            mergeItr(recData, 0, (length / nProcNow) / 2 - 1, (length / nProcNow) - 1);

        MPI_Gather(recData, length / nProcNow, MPI_INT, sendData, length / nProcNow, MPI_INT, 0, presentComm);
        nProcNow = nProcNow >> 1;
    }

    //stop the timer
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;

    //check if the array is sorted
    bool sorted = true;
    for (int i = 0; i < length - 1; i++) {
        if (sendData[i] > sendData[i + 1]) {
            sorted = false;
            break;
        }
    }

    if (sorted) {
        printf("Array is sorted\n");
    } else {
        printf("Array is not sorted\n");
    }

    free(sendData);
    free(recData);

    if (rank == 0) {
        printf("Time elapsed: %f seconds\n", elapsed_time);
    }

    MPI_Finalize();

    return EXIT_SUCCESS;
}