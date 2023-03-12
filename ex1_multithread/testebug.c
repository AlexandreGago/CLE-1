#include <stdio.h>
#include <stdlib.h>
struct Chunk
{
    int FileId;
    int finished;
    int size;
    unsigned char *data;
    
    int nWords;
    int nVowels[6];
};

    
int main(int argc, char *argv[]) {
    
   FILE *fp = fopen("./dataSet1/asd.txt", "rb");
   long curpos = ftell(fp);
   printf("curpos: %ld \n", curpos);

}