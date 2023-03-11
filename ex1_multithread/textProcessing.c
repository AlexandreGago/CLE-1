/**
 * @file TextProcessing.c
 * @author Bernardo Kaluza 97521
 * @author Alexandre Gago 98123
 * @brief Problem name: Text Processing in Portuguese
 * @date 2023-03-10
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//! TEMPORARY CHUNK STRUCTURE
struct Chunk
{
    int FileId;
    int finished;
    int size;
    unsigned char *data;
    
    int nWords;
    int nVowels[6];
};
/**
 * @brief Get the decimal codepoint of the utf 8 character
 * 
 * @param buffer the buffer of bytes to read from
 * @param index the index of the buffer to start reading on
 * @return int the decimal codepoint of the utf 8 character
 */
int getChar(unsigned char *buffer, int *index){
    int c;
    //check if first bit is 0
    if ((buffer[*index] & 0b10000000) == 0) {
        c= buffer[*index];
        (*index)++;
        return c;
    }
    //check if first 3 bits are 110
    if ((buffer[*index] & 0b11100000) == 0b11000000) {
        //the decimal codepoint is the 5 bits of the first byte and the 6 bits of the second byte
        c = (buffer[*index] & 0b00011111) << 6;
        (*index)++; 
        //check if the second byte is a valid utf 8 byte
        if ((buffer[*index] & 0b11000000) == 0b10000000) {
            //add the 6 bits of the second byte to the decimal codepoint
            c += (buffer[*index] & 0b00111111);
            (*index)++;
            return c;
        }
        else{
            //character is invalid/corrupted
            return -1;
        }
    }
    //check if first 4 bits are 1110
    if ((buffer[*index] & 0b11110000) == 0b11100000) {
        //the decimal codepoint is the 4 bits of the first byte and the 6 bits of the second and third bytes
        c = (buffer[*index] & 0b00001111) << 12;
        (*index)++; 
        //check if the second byte is a valid utf 8 byte
        if ((buffer[*index] & 0b11000000) == 0b10000000) {
            //add the 6 bits of the second byte to the decimal codepoint
            c += (buffer[*index] & 0b00111111) << 6;
            (*index)++;
            //check if the third byte is a valid utf 8 byte
            if ((buffer[*index] & 0b11000000) == 0b10000000) {
                //add the 6 bits of the third byte to the decimal codepoint
                c += (buffer[*index] & 0b00111111);
                (*index)++;
                return c;
            }
            else{
                //character is invalid/corrupted
                return -1;
            }
        }
        else{
            //character is invalid/corrupted
            return -1;
        }
    }
    return -1;
}
/**
 * @brief Check if a character is a separation or punctuation character
 * 
 * @param c the decimal codepoint of the character
 * @return true  if the character is a separation or punctuation character
 * @return false if the character is not a separation or punctuation character
 */
bool isSepPunc(int c){
    //a character is separation or punctuation if it is:
    //space, tab, newline, carriage return
    /* - [ ] ( ) , . : ; ! ? « » " “ ” – …  — */
    if (c == 32 || c == 9 || c == 10 || c == 13 || c == 45 || c == 91 || c == 93 || c == 40 || c == 41 
        || c == 44 || c == 46 || c == 58 || c == 59 || c == 33 || c == 63 || c == 171 || c == 187 || c == 34 
        || c == 147 || c == 148 || c == 8211 || c == 8230 || c == 8220 || c == 8221 || c == 8212) {

        return true;
    }
    else{
        return false;
    }
}
/**
 * @brief Check if a character is a merge character
 * 
 * @param c the decimal codepoint of the character
 * @return true if the character is a merge character
 * @return false if the character is not a merge character
 */
bool isMerge(int c) {
    //a character is a merge character if it is:
    /* ‘ ’ ' */
    return (c == 39 || c == 8216 || c == 8217);
}

/**
 * @brief Check if a character is a vowel and update the vowelInWord and nVowels arrays accordingly
 * @param c  the decimal codepoint of the character
 * @param vowelInWord an array of booleans that indicates if a vowel has already been found in the word
 * @param nVowels an array of integers that indicates the number of vowels found in the chunk
 */
void isVowel(int c, bool *vowelInWord, int *nVowels){
    //c is a vowel if it is:
    //a,e,i,o,u,y A,E,I,O,U,Y   á,é,í,ó,ú  Á,É,Í,Ó,Ú    à,è,ì,ò,ù À,È,Ì,Ò,Ù     â,ê,ô Â,Ê,Ô     ã,õ Ã,Õ
    
    //check all "a" and "A" variants
    if (c == 97 || c == 65 || c == 225 || c == 193 || c == 224 || c == 192 || c == 226 || c == 194 || c == 227 || c == 195) {
        //check if any "a" has been found in the word
        if (!vowelInWord[0]) {
            vowelInWord[0] = true;
            nVowels[0]++;
        }
    }
    //check all "e" and "E" variants
    if (c == 101 || c == 69 || c == 233 || c == 201 || c == 232 || c == 200 || c == 234 || c == 202) {
        if (!vowelInWord[1]) {
            vowelInWord[1] = true;
            nVowels[1]++;
        }
    }
    //check all "i" and "I" variants
    if (c == 105 || c == 73 || c == 237 || c == 205 || c == 236 || c == 204 ) {
        if (!vowelInWord[2]) {
            vowelInWord[2] = true;
            nVowels[2]++;
        }
    }
    //check all "o" and "O" variants
    if (c == 111 || c == 79 || c == 243 || c == 211 || c == 242 || c == 210 || c == 244 || c == 212 || c == 245 || c == 213) {
        if (!vowelInWord[3]) {
            vowelInWord[3] = true;
            nVowels[3]++;
        }
    }
    //check all "u" and "U" variants
    if (c == 117 || c == 85 || c == 250 || c == 218 || c == 249 || c == 217 || c == 251 || c == 219) {
        if (!vowelInWord[4]) {
            vowelInWord[4] = true;
            nVowels[4]++;
        }
    }
    //check all "y" and "Y" variants
    if (c == 121 || c == 89) {
        printf(" %d", vowelInWord[5]);
        if (!vowelInWord[5]) {
            vowelInWord[5] = true;
            nVowels[5]++;
        }
    }
}
/**
 * @brief Performs the processing of a chunk of text
 * It is called by the threads and receives a pointer to a Chunk struct
 * The function will process the chunk and count the number of words, and how many words have each vowel and update the chunk struct accordingly
 * @param Chunk Struct that contains the data needed to process the chunk
 * It will also be used to store the results of the processing.
 */
void processChunk(struct Chunk *fileChunk){
    //index of the buffer
    int buffIndex = 0;
    //variables to store the results of the processing
    bool inWord = false;
    int nWords = 0;
    int nVowels[] = {0,0,0,0,0,0};
    bool vowelInWord[] = {false, false, false, false, false, false};

    //loop that will process the chunk
    while (buffIndex < fileChunk->size) {

        //get the integer decimal representation of the char
        int c = getChar(fileChunk->data, &buffIndex);
        if (c == -1) {
            //invalid character, skip it
            //!TODO: log error
            printf("Invalid character found in file %d at position %d", fileChunk->FileId, buffIndex);
            continue;
        }
        //if inside word
        if(inWord) {
            //check if its a separator or punctuation to end the word
            if(isSepPunc(c)){
                inWord = false;
                //reset the vowelInWord array
                for(int i = 0; i < 6; i++){
                    vowelInWord[i] = false;
                } 
            }
            else {
                //void function that checks if its a vowel and updates accordingly
                isVowel(c, vowelInWord, nVowels);
            }
        }
        //if not inside word
        else{
            //check if its not a separator oor punctuation to start a new word
            if(!isSepPunc(c)){
                //check if its not a merge character
                if (!isMerge(c)) {
                    //start a new word
                    inWord = true;
                    //update the number of words
                    nWords++;
                    //void function that checks if its a vowel and updates accordingly
                    isVowel(c, vowelInWord, nVowels);
                }

            }
        }

    }
    //store the results in the struct
    fileChunk->nWords = nWords;
    for(int i = 0; i < 6; i++){
        fileChunk->nVowels[i] = nVowels[i];
    }
    fileChunk->finished = 1;
    
}
/*//! test function
change file name to test all rest is automatic
Might cause leaks
*/
// int main(int argc, char *argv[]) {
    
//     FILE *fp = fopen("../ex1/dataSet1/text4.txt", "r");
//     if (fp == NULL) {
//         printf("Error: Unable to open file!\n");
//         exit(1);
//     }
//     fseek(fp, 0L, SEEK_END);
//     long filesize = ftell(fp);
//     rewind(fp);
//     printf("filesize: %ld \n", filesize);
    

//     struct Chunk fileChunk;
//     fileChunk.FileId = 1;
//     fileChunk.finished = 0;
//     fileChunk.size = filesize;
//     fileChunk.data = malloc(filesize);
//     fread(fileChunk.data, filesize, 1, fp);

//     fclose(fp);
//     processChunk(&fileChunk);
    
//     //print all chunk
//     printf("Chunkk:\n");
//     printf("nWords: %d\n", fileChunk.nWords);
//     for(int i = 0; i < 6; i++){
//         printf("nVowels[%d]: %d\n", i, fileChunk.nVowels[i]);
//     }
//     printf("finished: %d\n", fileChunk.finished);
//     printf("FileId: %d\n", fileChunk.FileId);
//     free(fileChunk.data);

    
//     return 0;
// }

