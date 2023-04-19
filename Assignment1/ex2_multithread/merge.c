/**
 * @file merge.c
 * @brief Functions to perform iterative merge sort.
 * @author Bernardo Kaluza
 * @author Alexandre Gago
 */


#include "merge.h"
#include <time.h>


/**
 * @brief Returns the minimum of two integers.
 * 
 * @param x The first integer.
 * @param y The second integer.
 * @return The minimum of x and y.
 */
int min(int x, int y) { return (x<y)? x :y; }

void mergeItr(int arr[], int l, int m, int r);

/**
 * @brief Sorts an array using an iterative merge sort algorithm.
 * 
 * @param arr[] The array to be sorted.
 * @param n The size of the array.
 */
void mergeSortItr(int arr[], int n)
{
   int curr_size;  // For current size of subarrays to be merged
                   // curr_size varies from 1 to n/2
   int left_start; // For picking starting index of left subarray
                   // to be merged
  
   // Merge subarrays in bottom up manner.  First merge subarrays of
   // size 1 to create sorted subarrays of size 2, then merge subarrays
   // of size 2 to create sorted subarrays of size 4, and so on.
   for (curr_size=1; curr_size<=n-1; curr_size = 2*curr_size)
   {
       // Pick starting point of different subarrays of current size
       for (left_start=0; left_start<n-1; left_start += 2*curr_size)
       {
           // Find ending point of left subarray. mid+1 is starting 
           // point of right
           int mid = min(left_start + curr_size - 1, n-1);
  
           int right_end = min(left_start + 2*curr_size - 1, n-1);
  
           mergeItr(arr, left_start, mid, right_end);
       }
   }
}

/**
 * @brief Merges two sorted subarrays of a given array.
 * 
 * The subarrays are defined as arr[l..m] and arr[m+1..r].
 * 
 * @param arr[] The array containing the subarrays to be merged.
 * @param l The starting index of the first subarray.
 * @param m The ending index of the first subarray.
 * @param r The ending index of the second subarray.
 */
void mergeItr(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;
  
    /* create dynamic temp arrays */
    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

  
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];
  
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
  
    /* Copy the remaining elements of L[], if there are any */
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }
  
    /* Copy the remaining elements of R[], if there are any */
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
    free(L);
    free(R);
}