#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "fifo.h"

int main (int argc, char *argv[]){
    putVal(0,'a');
    putVal(1,'b');
    putVal(2,'c');
    putVal(3,'d');

    
    printf("%s",chunk);
    for (int i = 0; i < 8000; i++){
        printf("%c",chunk[i]);
    }
}