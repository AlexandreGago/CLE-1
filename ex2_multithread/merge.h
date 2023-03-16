/**
 * @file merge.h (header file)
 * @brief Text processing functions
 * 
 * @author Bernardo Kaluza 97521
 * @author Alexandre Gago 98123
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef MERGE_H
#define MERGE_H


/**
 * @brief 
 * 
 * @param arr 
 * @param n 
 */
extern void mergeSortItr(int arr[], int n);

void mergeItr(int arr[], int l, int m, int r);

#endif /* MERGE_H */