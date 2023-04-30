#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// #include "structs.h"
#include <time.h>
#include "merge.h"


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
    // MPI_Status status;

    // int sub_arr_size;
    // int *sub_arr;

    //if size isn't 1,2,4 or 8 abort
    if (size != 2 && size != 4 && size != 8 && size != 1){
        printf("This exercice requires exactly 1,2,4 or 8 processes.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0){

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

        // mergeSort
        mergeSortItr(sub_array, sub_array_size);


        MPI_Gather(sub_array, sub_array_size, MPI_INT, arr, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);
        


        free(sub_array);


        //merge part
        while (sub_array_size < arr_size){
            sub_array_size = sub_array_size*2;
            printf("#####Scatter#####\n");

            //send the sub-arrays to the workers
            printf("Malloc sub_array\n");
            fflush  (stdout);
            int *sub_array = (int *)malloc(sub_array_size * sizeof(int));
            printf("Malloc sub_array after\n");
            fflush  (stdout);
            
            MPI_Scatter(arr, sub_array_size, MPI_INT, sub_array, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);
            printf("Scatter after");
            fflush  (stdout);
            mergeItr(sub_array,0,sub_array_size/2-1,sub_array_size-1);

            MPI_Gather(sub_array, sub_array_size, MPI_INT, arr, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);
            printf("#####Gather 2nd time#####");
            fflush  (stdout);

        }
            
        //check if the array is sorted
        bool sorted = true;
        for (int i = 0; i < arr_size-1; i++) {
            if (arr[i] > arr[i+1]) {
                sorted = false;
                break;
            }
        }
        if (sorted) {
            printf("Array is sorted\n");
        }
        else {
            printf("Array is not sorted\n");
        }
    }

    else {
        int sub_array_size;
        int arr_size;
        printf("worker %d initiated\n", rank);
        fflush(stdout);

        //! receive the size of the sub-array
        MPI_Bcast(&sub_array_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&arr_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        int *sub_array = (int *)malloc(sub_array_size * sizeof(int));

        //receive the sub-array
        MPI_Scatter(NULL, sub_array_size, MPI_INT, sub_array, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);
        mergeSortItr(sub_array, sub_array_size);
        printf("Rank %d sorted sub-array\n", rank);
        //send the sub-array back
        MPI_Gather(sub_array, sub_array_size, MPI_INT, NULL, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);
        free(sub_array);
        while(sub_array_size < arr_size){//merge part

            sub_array_size = sub_array_size*2;
            
            //if rank is superior or equal to the number of sub-arrays, exit (32/32 = 1; the only worker would be the dispacher so the worker needes to exit)
            if (rank >= (arr_size/sub_array_size)){
                int *sub_array = (int *)malloc(sub_array_size * sizeof(int));
                //TODO SEND NOTHING

                MPI_Scatter(NULL, 0, MPI_INT, sub_array, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);
                printf("Rank %d received sub-array size: %d\n", rank, sub_array_size);
                MPI_Gather(sub_array, sub_array_size, MPI_INT, NULL, 0, MPI_INT, 0, MPI_COMM_WORLD);

                free(sub_array);
            }
            else{
                int *sub_array = (int *)malloc(sub_array_size * sizeof(int));

                //receive the sub-array
                MPI_Scatter(NULL, sub_array_size, MPI_INT, sub_array, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);
                printf("Rank %d received sub-array size: %d\n", rank, sub_array_size);
               
                //merge
                mergeItr(sub_array,0,sub_array_size/2-1,sub_array_size-1);

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


