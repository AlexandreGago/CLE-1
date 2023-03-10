#include <stdio.h>
#include <stdbool.h>
#include <string.h>

char processBuffer(unsigned char *buf, int size) {
    /*
    Convert a buffer of utf-8 bytes into an ascii character
    this will normalize vowels with accents and the letter c with cedilla
    as well as convert tabs, newlines and carriage returns to spaces and
    directed quotes to straight quotes
    */
    //translate buffer with UTF-8 bytes of a character into an ASCII character
    //printf("%x %x %x,",buf[0], buf[1], buf[2]);
    if (size == 1) {
        buf[0] = (buf[0] & 0xff);

        //transform apostrophe 0x27 to empty string
        if (buf[0] == 0x27) {
            return '!';
        }
        //transform spaces, tabs newlines and carriage returns to ascii space
        if (buf[0] == 0x20 || buf[0] == 0x09 || buf[0] == 0x0A || buf[0] == 0x0D) {
            return ' ';
        }
            //all punctuation -> - [ ] ( ) , . : ; ! ? are white spaces
        else if (buf[0] == 0x2D || buf[0] == 0x5B || buf[0] == 0x5D || buf[0] == 0x28 || buf[0] == 0x29 ||
                 buf[0] == 0x2C || buf[0] == 0x2E || buf[0] == 0x3A || buf[0] == 0x3B || buf[0] == 0x21 ||
                 buf[0] == 0x3F) {
            return ' ';
        } else {
            return buf[0];
        }
    } else if (size == 2) {

        buf[0] = (buf[0] & 0xff);
        buf[1] = (buf[1] & 0xff);

        //Transform every special type of 'A'and 'a'  to ascii 'a' (lower case) using the 2 byte utf-8 encoding
        if (buf[0] == 0xC3 &&
            (buf[1] == 0xA1 || buf[1] == 0xA0 || buf[1] == 0xA3 || buf[1] == 0xA2 || buf[1] == 0x81 || buf[1] == 0x80 || buf[1] == 0x82 ||
             buf[1] == 0x83)) {
            return 'a';
        }
            //Transform every special type of 'E'and 'e'  to ascii 'e' (lower case) using the 2 byte utf-8 encoding
        else if (buf[0] == 0xC3 &&
                 (buf[1] == 0xA9 || buf[1] == 0xA8 || buf[1] == 0xAA || buf[1] == 0x89 || buf[1] == 0x88 ||
                  buf[1] == 0x8A)) {
            return 'e';
        }
            //Transform every special type of 'I'and 'i'  to ascii 'i' (lower case) using the 2 byte utf-8 encoding
        else if (buf[0] == 0xC3 && (buf[1] == 0xAD || buf[1] == 0xAC || buf[1] == 0x8D || buf[1] == 0x8C)) {
            return 'i';
        }
            //Transform every special type of 'O'and 'o'  to ascii 'o' (lower case) using the 2 byte utf-8 encoding
        else if (buf[0] == 0xC3 &&
                 (buf[1] == 0xB3 || buf[1] == 0xB2 || buf[1] == 0xB4 || buf[1] == 0xB5 || buf[1] == 0x93 ||
                  buf[1] == 0x92 || buf[1] == 0x94 || buf[1] == 0x95)) {
            return 'o';
        }
            //Transform every special type of 'U'and 'u'  to ascii 'u' (lower case) using the 2 byte utf-8 encoding
        else if (buf[0] == 0xC3 && (buf[1] == 0xBA || buf[1] == 0xB9 || buf[1] == 0x9A || buf[1] == 0x99)) {
            return 'u';
        }
            //transform ç or Ç to ascii 'c'
        else if ((buf[0] == 0xC3 && buf[1] == 0xA7) || buf[1] == 0x87) {
            return 'c';
        } 
        //transform « and » to ascii " "
        else if (buf[0] == 0xC2 && (buf[1] == 0xAB || buf[1] == 0xBB)) {
            return ' ' ;
        }
        else {
            return '!';
        }
    } else if (size == 3) {
        buf[0] = (buf[0] & 0xff);
        buf[1] = (buf[1] & 0xff);
        buf[2] = (buf[2] & 0xff);

        // transform dash 0xe28093 and ellipsis 0xe280a6 to ascii ' '
        if (buf[0] == 0xe2 && buf[1] == 0x80 && (buf[2] == 0x93 || buf[2] == 0xa6 || buf[2] == 0x94)) {
            return ' ';
        }
            //transform single quotes 0xe28098 and 0xe28099 to empty string
        else if (buf[0] == 0xe2 && buf[1] == 0x80 && (buf[2] == 0x98 || buf[2] == 0x99)) {
            return '!';
        }
            //transform double quotes 0xe2809c and 0xe2809d to ascii "
        else if (buf[0] == 0xe2 && buf[1] == 0x80 && (buf[2] == 0x9c || buf[2] == 0x9d)) {
            return ' ';
        }

    }
    printf("failed to transform character");
    return '!';
}

//get next character from file, file is in utf-8
char getnextchar(FILE *fp) {
    //allocate 4 byte buffer
    unsigned char buf[4];

    //read 1 byte
    if(fread(buf, 1, 1, fp)){}

    //check EOF
    if (feof(fp)) {
        return 0;
    }

    //check if it is a 1 byte character
    if ((buf[0] & 0b10000000) == 0) {
        return processBuffer(buf, 1);
    }
        //check if it is a 2 byte character reading another byte into the buffer
    else if ((buf[0] & 0b11100000) == 0b11000000) {
        if(fread(&buf[1], 1, 1, fp)){}
        if ((buf[1] & 0b11000000) != 0b10000000) {

            printf("Invalid utf-8 character at");
            return 0;
        }
        return processBuffer(buf, 2);
    }
        //3 bytes
    else if ((buf[0] & 0b11110000) == 0b11100000) {
        if(fread(buf + 1, 1, 2, fp)){}
        if ((buf[1] & 0b11000000) != 0b10000000) {
            printf("Invalid utf-8 character");
            return 0;
        }
        if ((buf[2] & 0b11000000) != 0b10000000) {
            printf("Invalid utf-8 character");
            return 0;
        }
        return processBuffer(buf, 3);
    }

        //4 bytes are only symbols so they are not supported
        //all other cases are invalid
    else {
        printf("Invalid utf-8 character");
        return 0;
    }

}

int main(int argc, char *argv[]) {
    if (argc < 2) {//if there are no arguments
        printf("Provide some text files");
        return (1);
    } else {
        //variables,
        //i is used for loops
        //wordCount is used to count the number of words in a file
        //wordCharCount is used to count the number of words that contain the chosen character
        //chosenChar is the character that is being searched for
        int i;
        int wordCount = 0;
        int wordCharCount = 0;
        int wordVowelCount[5] = {0,0,0,0,0};
        bool foundFirstVowel[6] = {false,false,false,false,false,false};
        char chosenChar = '|';


        // goes through the arguments and finds the first character argument
        for (i = 1; i < argc; i++) {
            //check if this argument is a character
            if (strlen(argv[i]) == 1) {
                printf("Character %c )", argv[i][0]);
                chosenChar = argv[i][0];
                break;
            }
        }
        //process each file and count its words and words containing the chosen character
        printf("chosen character is: %c\n", chosenChar);
        for (i = 1; i < argc; i++) {
            wordCount = 0;
            wordCharCount = 0;

            FILE *fp = fopen(argv[i], "r");
            //check if file exists
            if (fp == NULL) {
                if (!(strlen(argv[i]) == 1))
                    printf("File %s does not exist", argv[i]);
            }

            //read file contents
            else {
                unsigned char c;
                //insideWord is used to check if we are in the same word or not
                bool insideWord = false;
                //loop that processes each character in a file until EOF is reached
                // here the total number of words in the file and the number of words that contain the chosen character are counted
                while ((c = getnextchar(fp)) != 0) {
                    printf("%c", c);    
                    if (c == '!') {
                        continue;
                    }
                    if (c != ' ' && !insideWord) {
                        wordCount++;
                        insideWord = true;
                        // check first letter of word
                        if((!foundFirstVowel[0]) && (c == 'a' || c == 'A')){
                            wordVowelCount[0]++;
                            foundFirstVowel[0] = true;
                        }
                        if(!foundFirstVowel[1] && (c == 'e' || c == 'E')){
                            wordVowelCount[1]++;
                            foundFirstVowel[1] = true;
                        }
                        if(!foundFirstVowel[2] && (c == 'i' || c == 'I')){
                            wordVowelCount[2]++;
                            foundFirstVowel[2] = true;
                        }
                        if(!foundFirstVowel[3] && (c == 'o' || c == 'O')){
                            wordVowelCount[3]++;
                            foundFirstVowel[3] = true;
                        }
                        if(!foundFirstVowel[4] && (c == 'u' || c == 'U')){
                            wordVowelCount[4]++;
                            foundFirstVowel[4] = true;
                        }
                        if (!foundFirstVowel[5] &&  c == chosenChar) {
                            wordCharCount++;
                            foundFirstVowel[5] = true;
                        }
                    } else if (c == ' ') {
                        insideWord = false;
                        foundFirstVowel[0] = false;
                        foundFirstVowel[1] = false;
                        foundFirstVowel[2] = false;
                        foundFirstVowel[3] = false;
                        foundFirstVowel[4] = false;
                        foundFirstVowel[5] = false;
                    }
                    else{
                        // check inside the word              
                        if((!foundFirstVowel[0]) && (c == 'a' || c == 'A')){
                            wordVowelCount[0]++;
                            foundFirstVowel[0] = true;
                        }
                        if(!foundFirstVowel[1] && (c == 'e' || c == 'E')){
                            wordVowelCount[1]++;
                            foundFirstVowel[1] = true;
                        }
                        if(!foundFirstVowel[2] && (c == 'i' || c == 'I')){
                            wordVowelCount[2]++;
                            foundFirstVowel[2] = true;
                        }
                        if(!foundFirstVowel[3] && (c == 'o' || c == 'O')){
                            wordVowelCount[3]++;
                            foundFirstVowel[3] = true;
                        }
                        if(!foundFirstVowel[4] && (c == 'u' || c == 'U')){
                            wordVowelCount[4]++;
                            foundFirstVowel[4] = true;
                        }
                        if (!foundFirstVowel[5] &&  c == chosenChar) {
                            wordCharCount++;
                            foundFirstVowel[5] = true;
                        }
                    }
                    
                }
                fclose(fp);
                //file: [file name]
                //    total words: [total words]
                //    words containing [chosen character]: [words containing chosen character]
                printf("\nfile: %s\n", argv[i]);
                printf("\ttotal words: %d\n", wordCount);
                if (chosenChar != '|'){
                    printf("\twords containing %c: %d\n", chosenChar, wordCharCount);
                }
                printf("\twords containing a: %d\n", wordVowelCount[0]);
                printf("\twords containing e: %d\n", wordVowelCount[1]);
                printf("\twords containing i: %d\n", wordVowelCount[2]);
                printf("\twords containing o: %d\n", wordVowelCount[3]);
                printf("\twords containing u: %d\n", wordVowelCount[4]);
            }
        }
    }
    return (0);
}
