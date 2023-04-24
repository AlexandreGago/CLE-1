#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int size, rank;
    const int array_size = 64;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size != 5) {
        printf("This example requires exactly 5 processes.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0) {
        // Distributor code
        int *sendbuf = (int *)malloc(array_size * sizeof(int));
        for (int i = 0; i < array_size; i++) {
            sendbuf[i] = i;
        }

        int sendcounts[size];
        int displs[size];
        int chunk_size = array_size / (size - 1);

        for (int i = 0; i < size; i++) {
            sendcounts[i] = i == 0 ? 0 : chunk_size;
            displs[i] = i == 0 ? 0 : chunk_size * (i - 1);
        }

        MPI_Scatterv(sendbuf, sendcounts, displs, MPI_INT, MPI_IN_PLACE, 0, MPI_INT, 0, MPI_COMM_WORLD);
        
        MPI_Gatherv(MPI_IN_PLACE, 0, MPI_INT, sendbuf, sendcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

        printf("Modified array after gather:\n");
        for (int i = 0; i < array_size; i++) {
            printf("%d ", sendbuf[i]);
        }
        printf("\n");

        free(sendbuf);
    }
    else {
        // Worker code
        int chunk_size = array_size / (size - 1);
        int *recvbuf = (int *)malloc(chunk_size * sizeof(int));

        MPI_Scatterv(NULL, NULL, NULL, MPI_INT, recvbuf, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

        for (int i = 0; i < chunk_size; i++) {
            recvbuf[i] -= 1;
        }

        MPI_Gatherv(recvbuf, chunk_size, MPI_INT, NULL, NULL, NULL, MPI_INT, 0, MPI_COMM_WORLD);
        free(recvbuf);
    }

    MPI_Finalize();

    return 0;
}