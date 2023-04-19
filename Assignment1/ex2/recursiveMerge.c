#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

//bitonic sort
void print_array(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
}
void merge(int arr[], int l, int m, int r) {
    //l is first index of left sub array
    //m is index of middle element
    //r is last index of right sub array

    //left subarray is arr[l..m]
    //right subarray is arr[m+1..r]

    // i is index for left subarray
    // j is index for right subarray
    // k is index for merged subarray

    //n1 is size of left subarray
    //n2 is size of right subarray
    
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    //create temp arrays
    int L[n1], R[n2];

    //fill temp arrays
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    // printf("\n");
    // print_array(L, n1);
    // printf("\n");
    // print_array(R, n2);
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
       are any */
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
       are any */
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void merge_sort(int arr[],int l,int r) {
    if (l < r) {
        //same as (l+r)/2 but avoids overflow for large l and h
        int m = l + (r - l) / 2;
        // printf("\n m=%d r=%d l=%d",m,r,l);
        //sort first and second halves

        merge_sort(arr, l, m);
        merge_sort(arr, m + 1, r);

        // printf("\n");
        merge(arr, l, m, r);
    }
}


int main(int argc, char *argv[]) {

    FILE *f;
    int *arr;
    int arr_size;
    // int i;
    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }
    f = fopen(argv[1], "rb");
    if (f == NULL) {
        printf("Error opening file %s   \n", argv[1]);
    }
    //read the number of elements
    if(fread(&arr_size, sizeof(int), 1, f)<=0){
        printf("Error reading file %s   \n", argv[1]);
        exit(0);
    };
    //allocate memory for the array
    arr = (int *)malloc(arr_size * sizeof(int));
    //read to the array with malloc
    if(fread(arr, sizeof(int), arr_size, f)<=0){
        printf("Error reading file %s   \n", argv[1]);
        exit(0);
    };
    //close the file
    fclose(f);
    //print the array
    // for (i = 0; i < arr_size; i++) {
    //     printf("%d ", arr[i]);
    // }

    // printf("\n");

    clock_t start,end;
    double time;
    start = clock();
    merge_sort(arr,0,arr_size-1);
    end = clock();
    time = (double)((end-start)/ CLOCKS_PER_SEC);
    printf("%f", time);
    // printf("\nSorted array is: ");
    // print_array(arr, arr_size);

    //check if the array is in crescent order
    bool is_sorted = true;
    for (int i = 0; i < arr_size - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            is_sorted = false;
            printf("The array is incorrect at index %d and %d (%d,%d)", i, i + 1, arr[i], arr[i + 1]);
        }
    }
    if (is_sorted) {
        printf("The array is sorted in crescent order \n");
    } else {
        printf("The array is not sorted in crescent order \n");
    }

    return 1;
}
