#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
//read an array from a binary file with integers
    FILE *f;
    int *arr;
    int n;
    int i;
    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }
    f = fopen(argv[1], "rb");
    if (f == NULL) {
        printf("Error opening file %s   \n", argv[1]);
    }
    //read the number of elements
    fread(&n, sizeof(int), 1, f);
    //allocate memory for the array
    arr = (int *)malloc(n * sizeof(int));
    //read the array
    fread(arr, sizeof(int), n, f);
    //close the file
    fclose(f);
    //print the array
    for (i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }

    printf("\n");
    free(arr);

return 1;
}