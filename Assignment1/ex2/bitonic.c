#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#define clz(x) __builtin_clz(x)

void bitonic_sort(int arr[], int size) {
    // for (int i = 0; i < size; i++) {
    //     printf("%d ", arr[i]);
    // }
    int step = 2;
    int curr_step_2 = step/2;
    bool ascending = true;

    // getting log2 of a power of 2
    // A power of 2 has only one 1 in its binary representation
    // sizeof(int) * 8 is the number of bits of an int (32)
    // clz is the number of leading zeros of an int
    // 32 - clz(size) - 1 = log2(size)
    int total_steps = sizeof(size) * 8 - clz(size) - 1;
    // printf("total_steps: %d\n", total_steps);
    for (int i = 0; i < total_steps; i++) {// loop of the total steps of the algorithm (how many elements of the array we group)
        ascending = false;
        // printf("\nstep: %d\n", step);
        for (int j = 0; j < size; j=j+step) {// loop of each ascending and descending step
            ascending = !ascending;
            curr_step_2 = step/2;
            //!THREADS GO HERE
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


int main(int argc, char *argv[]) {
    FILE *f;
    int *arr;
    int n;
    //int i;
    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }
    f = fopen(argv[1], "rb");
    if (f == NULL) {
        printf("Error opening file %s   \n", argv[1]);
    }
    //read the number of elements
    if(fread(&n, sizeof(int), 1, f)){};
    //allocate memory for the array
    arr = (int *)malloc(n * sizeof(int));
    //read the array
    if(fread(arr, sizeof(int), n, f)){};
    //close the file
    fclose(f);
    //time bitonic_sort function
    clock_t start,end;
    double time;
    start = clock();
    bitonic_sort(arr, n);
    end = clock();
    time = (double)((end-start)/ CLOCKS_PER_SEC);
    printf("%f", time);
    // printarray
    // for (int i = 0; i < n; i++) {
    //     printf("%d ", arr[i]);
    // }

    // int arr_test[4] = {1, 2, 3, 4};
    // int arr_test[16] = {5, 8, 3, 4,1,6,7,2,9,10,16,12,13,14,15,11};

    // bitonic_sort(arr_test, sizeof(arr_test)/sizeof(arr_test[0]));
    //printarray
    // for (int i = 0; i < sizeof(arr_test)/sizeof(arr_test[0]); i++) {
    //     printf("%d ", arr_test[i]);
    // }
    // bitonic_sort(arr_test, 0, n-1);
    bool is_sorted = true;
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            is_sorted = false;
            printf("The array is incorrect at index %d and %d (%d,%d)", i, i + 1, arr[i], arr[i + 1]);
        }
    }
    if (is_sorted) {
        printf("\nThe array is sorted in crescent order \n");
    } else {
        printf("\nThe array is not sorted in crescent order \n");
    }
    return 1;
}