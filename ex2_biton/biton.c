#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#define clz(x) __builtin_clz(x)

void bitonic_sort(int arr[], int size, int step_arg,bool ascending_arg) {
    // for (int i = 0; i < size; i++) {
    //     printf("%d ", arr[i]);
    // }
    int step = step_arg;
    int curr_step_2 = step/2;
    bool ascending;

    // printf("total_steps: %d\n", total_steps);
    while (step<=size) {// loop of the total steps of the algorithm (how many elements of the array we group)
        ascending = ascending_arg;
        // printf("\nstep: %d\n", step);
        for (int j = 0; j < size; j=j+step) {// loop of each ascending and descending step
            ascending = !ascending;
            curr_step_2 = step/2;
            if (ascending) {//unite the ascending and descending arrays in an ascending one
                // printf("ascending\n");
                int index=0,distance = curr_step_2;
                for (int l = 0;l< (sizeof(step) * 8 - clz(step) - 1);l++){//next step
                // printf("\nl=%d \n\n", l);
                    for(int k = 0; k < curr_step_2; k++) {//each two element comparison
                        // printf("%d\n",((index+1) % separation) == 0 );
                        if (((index) % distance) == 0 && index != 0){
                            // printf("SEPARATE \n");
                            index += distance;
                        }
                        // printf("k=%d\n ", k);
                        // printf("index=%d \n", index);
                        // printf("distance=%d \n", distance);
                        // printf("separation=%d \n", separation);
                        // printf("%d ", arr[index+j]);
                        // printf("%d \n", arr[index+distance+j]);
                        if(arr[index+j] > arr[index+distance+j]){
                            int temp = arr[index+j];
                            arr[index+j] = arr[index+distance+j];
                            arr[index+distance+j] = temp;
                        }
                        index++;
                    }
                index=0;
                distance = distance/2;
                }

            } 
            else {//unite the ascending and descending arrays in a descending one
                // printf("descending \n");
                int index=0,distance = curr_step_2;
                for (int l = 0;l< (sizeof(step) * 8 - clz(step) - 1);l++){//next step
                    // printf("\nl=%d \n\n", l);

                    for(int k = 0; k < curr_step_2; k++) {
                        // printf("%d\n",((index+1) % separation) == 0 );
                        if (((index) % distance) == 0 && index != 0){
                            // printf("SEPARATE \n");
                            index += distance;
                        }
                        // printf("k=%d\n ", k);
                        // printf("index=%d \n", index);
                        // printf("distance=%d \n", distance);
                        // printf("separation=%d \n", separation);
                        // printf("%d ", arr[index+j]);
                        // printf("%d \n", arr[index+distance+j]);
                        if (arr[index+j] < arr[index+distance+j]) {
                            int temp = arr[index+j];
                            arr[index+j] = arr[index+distance+j];
                            arr[index+distance+j] = temp;
                        }
                        index++;
                    }
                    index=0;
                    distance = distance/2;
                }

            }

        }
        step *= 2;
    }   
}

void checkSort(int n, int arr[]){
    bool creacent = true;
    bool decrescent = true;
    for (int i = 0; i < n-1; i++) {
        if (arr[i] > arr[i+1]) {
            creacent = false;
            break;
        }
        if (arr[i] < arr[i+1]) {
            decrescent = false;
            break;
        }
    }
    if (creacent) {
        printf("arr1 is sorted in ascending order\n");
    }
    else if (decrescent) {
        printf("arr1 is sorted in descending order\n");
    }
    else {
        printf("arr1 is not sorted\n");
    }
}
// int main(int argc, char *argv[]) {
//     FILE *f;
//     int *arr;
//     int n;
//     //int i;
//     if (argc != 2) {
//         printf("Usage: %s <file>\n", argv[0]);
//         return 1;
//     }
//     f = fopen(argv[1], "rb");
//     if (f == NULL) {
//         printf("Error opening file %s   \n", argv[1]);
//     }
//     //read the number of elements
//     if(fread(&n, sizeof(int), 1, f)){};
//     //allocate memory for the array
//     arr = (int *)malloc(n * sizeof(int));
//     //read the array
//     if(fread(arr, sizeof(int), n, f)){};
//     //close the file
//     fclose(f);
//     //time bitonic_sort function

//     clock_t start,end;
//     double time;
//     start = clock();

//     //divide the array in 4 part and sort them in parallel
//     int* arr1 = (int *)malloc(n/4 * sizeof(int));
//     int* arr2 = (int *)malloc(n/4 * sizeof(int));
//     int* arr3 = (int *)malloc(n/4 * sizeof(int));
//     int* arr4 = (int *)malloc(n/4 * sizeof(int));

//     int size = n/4;

//     for (int i = 0; i < n/4; i++) {
//         arr1[i] = arr[i];
//         arr2[i] = arr[i+n/4];
//         arr3[i] = arr[i+2*n/4];
//         arr4[i] = arr[i+3*n/4];
//     }

//     //create threads for bitonic_sort
//     pthread_t thread1, thread2, thread3, thread4;

//     bitonic_sort(arr1, n/4,2,false);
//     bitonic_sort(arr2, n/4,2,true);
//     bitonic_sort(arr3, n/4,2,false);
//     bitonic_sort(arr4, n/4,2,true);

    
//     checkSort(n/4,arr1);
//     checkSort(n/4,arr2);
//     checkSort(n/4,arr3);
//     checkSort(n/4,arr4);
    
//     //unite the arr1, arr2,arr3,arr4 into arr5 one by one
//     int* arr5 = (int *)malloc(n * sizeof(int));
//     int index = 0;
//     for (int i = 0; i < n/4; i++) {
//         arr5[index] = arr1[i];
//         index ++;
//     }
//     for (int i = 0; i < n/4; i++) {
//         arr5[index] = arr2[i];
//         index ++;
//     }
//     for (int i = 0; i < n/4; i++) {
//         arr5[index] = arr3[i];
//         index ++;
//     }
//     for (int i = 0; i < n/4; i++) {
//         arr5[index] = arr4[i];
//         index ++;
//     }
//     //print the array
//     for (int i = 0; i < n; i++) {
//         printf("%d ", arr5[i]);
//     }
//     printf("\n");
//     fflush(stdout);
//     //do the last bitonic sort
//     bitonic_sort(arr5, n,8,false);
//     //print the array
//     for (int i = 0; i < n; i++) {
//         printf("%d ", arr5[i]);
//     }
//     checkSort(n,arr5);
    

    
//     end = clock();
//     time = (double)((end-start)/ CLOCKS_PER_SEC);
//     printf("%f", time);


//     return 1;
// }