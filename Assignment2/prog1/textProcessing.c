/**
 * @file TextProcessing.c
 * @author Bernardo Kaluza 97521
 * @author Alexandre Gago 98123
 * @brief Problem name: Text Processing in Portuguese
 * Text Processing functions, used to count the number of words and vowels in a given chunk of text.
 * Also used to read a chunk from a file.
 * 
 */

#include <stdio.h>
#include <stdbool.h>
#include "structs.h"

//use chunkSize provided by main
//!EXTERN IS NOT WORKING
extern int chunkSize;

/**
 * @brief Get the decimal codepoint of the utf 8 character
 * 
 * @param buffer the buffer of bytes to read from
 * @param index the index of the buffer to start reading on
 * @return int the decimal codepoint of the utf 8 character
 */
int getChar(unsigned char *buffer, int *index) {
    int c;
    //check if first bit is 0
    if ((buffer[*index] & 0b10000000) == 0) {
        c = buffer[*index];
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
        } else {
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
            } else {
                //character is invalid/corrupted
                return -1;
            }
        } else {
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
bool isSepPunc(int c) {
    //a character is separation or punctuation if it is:
    //space, tab, newline, carriage return
    /* - [ ] ( ) , . : ; ! ? « » " “ ” – …  — */
    if (c == 32 || c == 9 || c == 10 || c == 13 || c == 45 || c == 91 || c == 93 || c == 40 || c == 41
        || c == 44 || c == 46 || c == 58 || c == 59 || c == 33 || c == 63 || c == 171 || c == 187 || c == 34
        || c == 147 || c == 148 || c == 8211 || c == 8230 || c == 8220 || c == 8221 || c == 8212) {

        return true;
    } else {
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
void isVowel(int c, bool *vowelInWord, int *nVowels) {
    //c is a vowel if it is:
    //a,e,i,o,u,y A,E,I,O,U,Y   á,é,í,ó,ú  Á,É,Í,Ó,Ú    à,è,ì,ò,ù À,È,Ì,Ò,Ù     â,ê,ô Â,Ê,Ô     ã,õ Ã,Õ

    //check all "a" and "A" variants
    if (c == 97 || c == 65 || c == 225 || c == 193 || c == 224 || c == 192 || c == 226 || c == 194 || c == 227 ||
        c == 195) {
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
    if (c == 105 || c == 73 || c == 237 || c == 205 || c == 236 || c == 204) {
        if (!vowelInWord[2]) {
            vowelInWord[2] = true;
            nVowels[2]++;
        }
    }
    //check all "o" and "O" variants
    if (c == 111 || c == 79 || c == 243 || c == 211 || c == 242 || c == 210 || c == 244 || c == 212 || c == 245 ||
        c == 213) {
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
int processChunk( Chunk *fileChunk) {
    //index of the buffer
    int buffIndex = 0;
    //variables to store the results of the processing
    bool inWord = false;
    int nWords = 0;
    int nVowels[] = {0, 0, 0, 0, 0, 0};
    bool vowelInWord[] = {false, false, false, false, false, false};
    //loop that will process the chunk
    while (buffIndex < fileChunk->size) {

        //get the integer decimal representation of the char
        int c = getChar(fileChunk->data, &buffIndex);
        if (c == -1) {
            //invalid character, abort file
            return -1;
        }
        //if inside word
        if (inWord) {
            //check if its a separator or punctuation to end the word
            if (isSepPunc(c)) {
                inWord = false;
                //reset the vowelInWord array
                for (int i = 0; i < 6; i++) {
                    vowelInWord[i] = false;
                }
            } else {
                //void function that checks if its a vowel and updates accordingly
                isVowel(c, vowelInWord, nVowels);
            }
        }
            //if not inside word
        else {
            //check if its not a separator oor punctuation to start a new word
            if (!isSepPunc(c)) {
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
    for (int i = 0; i < 6; i++) {
        fileChunk->nVowels[i] = nVowels[i];
    }
    return 0;
}

int readToChunk( FileData *fileData,  Chunk *fileChunk) {
    int n = fread(fileChunk->data, 1, chunkSize, fileData->file);
    //if error reading file
    if (n < 0) {
        printf("Error reading file %s, ignoring it \n", fileData->name);
        fileData->finished = 1;
    }
    //if file is at the end we didn't cut words
    if (feof(fileData->file) || n == 0) {
        fileData->finished = 1;
        return n;
    }
    //if after reading we are at the end of file
    if (n == chunkSize) {
        //see if next byte is EOF
        int c = fgetc(fileData->file);
        if (c == EOF) {
            fileData->finished = 1;
            return n;
        } else {
            //go back one byte
            fseek(fileData->file, -1, SEEK_CUR);
            //reset EOF flag
            clearerr(fileData->file);
        }
    }
    //check if last read byte is a 1 byte UTF-8 character and see if it is a separation symbol
    //if it is not we must go backwards until we find the start of an UTF-8 character
    //if that UTF-8 Character is not a separation symbol repeat the process
    int copy = n - 1;

    //check if last read is 1 byte separator
    if ((fileChunk->data[n - 1] & 0b10000000) == 0) {
        int c = getChar(fileChunk->data, &copy);

        if (c != -1) {
            if (isSepPunc(c)) {
                return n;
            }
        }
    }
    //go back until we find a separator
    while (1 && (n > 0)) {
        //match beginning of UTF-8 character
        if (((fileChunk->data[n - 1] & 0b10000000) == 0) || ((fileChunk->data[n - 1] & 0b11100000) == 0b11000000) ||
            ((fileChunk->data[n - 1] & 0b11110000) == 0b11100000)) {
            copy = n - 1;
            int c = getChar(fileChunk->data, &copy);

            if (c != -1) {
                if (isSepPunc(c)) {

                    fseek(fileData->file, -1 * (chunkSize - copy), SEEK_CUR);
                    //clear fileChunkdata past the separator
                    for (int i = copy; i < chunkSize; i++) {
                        fileChunk->data[i] = 0;
                    }
                    return n;
                } else {
                    n--;
                }
            } else {
                n--;
            }

        } else {
            n--;
        }

    }
    printf("Failed to find a separator, ignoring file %s \n", fileData->name);
    fileData->finished = 1;
    return 0;
}

