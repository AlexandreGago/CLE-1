/**
 * @file merge.h (header file for merge.c)
 * @brief Functions to perform iterative merge sort.
 * @author Bernardo Kaluza
 * @author Alexandre Gago
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef MERGE_H
#define MERGE_H


/**
 * @brief Sorts an array using an iterative merge sort algorithm.
 * 
 * @param arr[] The array to be sorted.
 * @param n The size of the array.
 */
extern void mergeSortItr(int arr[], int n);


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
void mergeItr(int arr[], int l, int m, int r);

#endif /* MERGE_H */