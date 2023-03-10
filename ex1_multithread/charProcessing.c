/**
 * @file charProcessing.c
 * @author Bernardo Kaluza 97521
 * @author Alexandre Gago 98123
 * @brief Problem name: Text Processing in Portuguese
 * @date 2023-03-10
 * @copyright Copyright (c) 2023
 * 
 */


#include <stdio.h>
#include <stdlib.h>



//function called getChar that receives an array of unsigned chars and reads the first byte
//after reading it updates the array to the next byte
unsigned char getChar(unsigned char *array){
    unsigned char c = *array;
    array++;
    return c;
}

int main(){
    unsigned char *array = (unsigned char *) malloc(1000);
    printf("Hello World");
    //fill array with random values
    for(int i = 0; i < 1000; i++){
        array[i] = rand() % 256;
    }
    unsigned char c = getChar(array);
    printf("%c", c);
    free(array);
    return 0;
}
