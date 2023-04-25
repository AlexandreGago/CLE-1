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
    if (size != 2 && size != 4 && size != 8 && size != 1){
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
        int sub_array_size = arr_size/(size);
        MPI_Bcast(&sub_array_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&arr_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        
        //send the sub-arrays to the workers
        int *sub_array = (int *)malloc(sub_array_size * sizeof(int));
        MPI_Scatter(arr, sub_array_size, MPI_INT, sub_array, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);

        //!mergeSort part of dispacher
        printf("####################Scattered merge sort #######################\n");
        fflush(stdout);
        //TODO mergeSort()
        //mergeSort(sub_array, 0, sub_array_size-1);

        // printf("Rank 0 has sub_array: ");
        // for (int i = 0; i < sub_array_size; i++) {
        //     printf("%d ", sub_array[i]);
        // }
        // printf("\n");
        // fflush(stdout);

        //MPI_GATHER to receive the mergeserted sub-arrays
        //print sub-arrays
        printf("rank 0 has sub_array: ");
        for (int i = 0; i < sub_array_size; i++) {
            printf("%d ", sub_array[i]);
        }
        fflush(stdout);
        int *sorted_sub_array = (int *)malloc(arr_size * sizeof(int));
        MPI_Gather(sub_array, sub_array_size, MPI_INT, arr, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);
        printf("#################Gathered merge sort#################3\n");
        fflush(stdout);
        // for (int i = 0; i < sub_array_size; i++) {
        //     arr[i] = sub_array[i];
        // }
        printf("dispacher first stage finished \n");
        printf("arr: ");
        for (int i = 0; i < arr_size; i++) {
            printf("%d ", arr[i]);
        }
        printf("\n");
        fflush (stdout);
        free(sub_array);
        free(sorted_sub_array);


        //merge part
        while (sub_array_size < arr_size){
            sub_array_size = sub_array_size*2;
            printf ("sidpacher sub_array_size: %d\n", sub_array_size);
            fflush(stdout);
            //send the sub-arrays to the workers
            int *sub_array = (int *)malloc(sub_array_size * sizeof(int));
            MPI_Scatter(arr, sub_array_size, MPI_INT, sub_array, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);

            //!mergeSort part of dispacher
            printf("#####################Scattered %d #########################\n", sub_array_size);
            //TODO mergeSort()
            //mergeSort(sub_array, 0, sub_array_size-1);

            // printf("Rank 0 has sub_array: ");
            // for (int i = 0; i < sub_array_size; i++) {
            //     printf("%d ", sub_array[i]);
            // }
            // printf("\n");
            fflush(stdout);

            //MPI_GATHER to receive the mergeserted sub-arrays
            MPI_Gather(sub_array, sub_array_size, MPI_INT, arr, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);
            printf("##############Gathered##################\n");

            fflush (stdout);
            }

            printf("dispacher finished \n");
            printf("arr: ");
            for (int i = 0; i < arr_size; i++) {
                printf("%d ", arr[i]);
            }
            printf("\n");
            fflush (stdout);
    }

    else {//workers use blocking sends and receives
        int sub_array_size;
        int arr_size;
        printf("worker %d initiated\n", rank);
        fflush(stdout);

        //! receive the size of the sub-array
        MPI_Bcast(&sub_array_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&arr_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
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
        while(sub_array_size < arr_size){//merge part
            printf("worker %d entered while sub_array_size: %d\n", rank, sub_array_size);
            fflush(stdout);
            sub_array_size = sub_array_size*2;
            //if rank is superior or equal to the number of sub-arrays, exit (32/32 = 1; the only worker would be the dispacher so the worker needes to exit)
            if (rank >= (arr_size/sub_array_size)){
                printf ("Rank %d is superior to the number of sub-arrays, exiting sub_size: %d arr_size: %d \n", rank, sub_array_size, arr_size);
                fflush(stdout);
                int *sub_array = (int *)malloc(sub_array_size * sizeof(int));
                //TODO SEND NOTHING
                MPI_Scatter(NULL, sub_array_size, MPI_INT, sub_array, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);
                MPI_Gather(sub_array, sub_array_size, MPI_INT, NULL, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);
            }
            else{
                int *sub_array = (int *)malloc(sub_array_size * sizeof(int));

                //receive the sub-array
                MPI_Scatter(NULL, sub_array_size, MPI_INT, sub_array, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);
                //TODO merge
                //mergeSort(sub_array, 0, sub_array_size-1);
                fflush(stdout);
                printf("Rank %d has sub_array: ", rank);
                for (int i = 0; i < sub_array_size; i++) {
                    printf("%d ", sub_array[i]);
                }
                printf("\n");
                fflush(stdout);
                //send the merged sub-array
                MPI_Gather(sub_array, sub_array_size, MPI_INT, NULL, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);
                free(sub_array);
            }

        }
    }

    MPI_Finalize ();
    return EXIT_SUCCESS;
}


int initializeDistributor(char **files, int numFiles) {



    return EXIT_SUCCESS;
}


