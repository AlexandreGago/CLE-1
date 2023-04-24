#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// #include "structs.h"
#include <time.h>


void processcmd(int argc, char* argv[]);
int allFilesDone();
int initializeDistributor(char **files, int numFilesa);

MPI_Request dummyRequest = MPI_REQUEST_NULL;

int main (int argc, char *argv[]){
    int rank;
    int size;
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    MPI_Status status;

    int sub_arr_size;
    int *sub_arr;

    //if size isn't 2,3,5 or 9, exit
    if (size != 2 && size != 3 && size != 5 && size != 9){
        printf("This exercice requires exactly 2,3,5 or 9 processes.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0){//dispacher process

        printf ("initializing dispacher \n");
        fflush(stdout);
        //TODO read files from command line
        char *filename = argv[1];
        FILE *f; 
        int *arr;
        int arr_size;

        //open the file
        f = fopen(filename, "rb");
        if (f == NULL) {
            printf("Error opening file %s   \n", filename);
            exit(EXIT_FAILURE);
        }
        //read the number of elements
        if(fread(&arr_size, sizeof(int), 1, f)<=0){
            printf("Error reading file %s   \n", filename); 
            exit(EXIT_FAILURE);
        };
        //allocate memory for the array
        arr = (int *)malloc(arr_size * sizeof(int));
        //read to the array
        if(fread(arr, sizeof(int), arr_size, f)<=0){
            printf("Error reading file %s   \n", filename);
            exit(EXIT_FAILURE);
        };
        fclose(f);
        // printf ("dispacher read %d elements from file %s \n", arr_size, filename);
        //send the size of the sub-array to all workers
        int sub_array_size = arr_size/(size-1);
        MPI_Bcast(&sub_array_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        
        //send the sub-arrays to the workers
        int *sub_array = (int *)malloc(sub_array_size * sizeof(int));
        MPI_Scatter(arr, sub_array_size, MPI_INT, sub_arr, sub_arr_size, MPI_INT, 0, MPI_COMM_WORLD);
        printf("Scattered\n");

        //MPI_GATHER to receive the mergeserted sub-arrays
        int *sorted_sub_array = (int *)malloc(arr_size * sizeof(int));
        MPI_Gather(&sorted_sub_array, sub_array_size, MPI_INT, arr, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);
        printf("Gathered\n");
        fflush(stdout);

        printf("dispacher first stage finished \n");
        printf("arr: ");
        for (int i = 0; i < arr_size; i++) {
            printf("%d ", arr[i]);
        }

        fflush (stdout);

        //merge part
        while (sub_arr_size != arr_size){
            //send merges

            //receive merged
            sub_arr_size = sub_arr_size*2;
        }

        printf("dispacher finished \n");
        MPI_Abort(MPI_COMM_WORLD, 1);

    }

    else {//workers use blocking sends and receives
        int sub_array_size;
        printf("worker %d initiated\n", rank);
        fflush(stdout);

        //! receive the size of the sub-array
        MPI_Bcast(&sub_array_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        // printf("Rank %d received sub-array size: %d\n", rank, sub_array_size);
        //mergeSort part
        int *sub_array = (int *)malloc(sub_array_size * sizeof(int));

        //receive the sub-array
        MPI_Scatter(NULL, sub_array_size, MPI_INT, sub_array, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);

        printf("\nRank %d has sub-array: ", rank);
        for (int i = 0; i < sub_array_size; i++) {
            printf("%d ", sub_array[i]);
        }
        printf("\n");
        fflush(stdout);

        //send the sub-array back
        MPI_Gather(sub_array, sub_array_size, MPI_INT, NULL, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);
        free(sub_array);

        // while(1){//merge part
        //     //! receive the size of the sub-array
        //     MPI_Bcast(&sub_array_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        //     int *sub_array = (int *)malloc(sub_array_size * sizeof(int));
        //     //receive the sub-array
        //     MPI_Scatter(NULL, sub_array_size, MPI_INT, sub_array, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);
        //     //merge
        //     //send the merged sub-array

        //     free(sub_array);
        // }
        printf("\n");

    }

    MPI_Finalize ();
    return EXIT_SUCCESS;
}


int initializeDistributor(char **files, int numFiles) {



    return EXIT_SUCCESS;
}


