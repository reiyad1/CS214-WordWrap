#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>

inline int word_length(const char *str){
    int getIndex = 0;
    while(str[getIndex]! = 0 && str[getIndex] ! = '' && str[getIndex]! = '\n'){
        getIndex++;
    }
    return(getIndex);
}

void word_wrap2(char *s, const int newline){
    int length;
    int w;
    int word_length = 0;
    int ouput_length = 0;
    int line_position = 0;

    for(length = 0; str[length]; length++)

    char word[length];

    for(w = 0; w<=length; n++){
        if(str[w] == '\n' || str[w] == '' || w = length ){
         
            }
        }
    }
    //read(filename, buffer, number of char bytes)
    

}


void word_wrap(int filename, char *buffer, int columns){
    int i;
    int counter = 0;
    int arrayIndex = -1;

    ssize_t checkEOF;

    while(1){
        counter = 0;
        arrayIndex = -1;
        //using columns+1 to see if we cut a word in half or not
        for (i = 0; i < (columns+1); i++){
            checkEOF = read(filename, &buffer[i], 1);
            arrayIndex++;
            
            //if you reach end of file
            if (checkEOF == 0){
                printf("%lu", checkEOF);
                printf("%c", buffer[i]);
                return;
            }
            
            //if (i == 0){
                //if (buffer[i] == '\n'){
                    //printf("\n");
                //}
            //}
            //printf("%c", buffer[i]);
            //to ensure that we dont have consecutive spaces
            if (i > 0){
                //check for double space
                if (buffer[i] == ' ' && buffer[i-1] == ' '){
                    buffer[i] = '\0';   //makes sure there is only one space
                    i--;
                    arrayIndex--;
                }
                if (buffer[i] == '\n' && buffer[i-1] == ' '){
                    //printf("CHECK");
                    if (i != columns+1){
                        checkEOF = read(filename, &buffer[i+1], 1); //MAKE SURE THIS ONLY HAPPENS IF IT HASNT GONE PAST 
                        printf(" %c ", buffer[i+1]);
                    }
                    break;
                }
                else if (buffer[i] == '\n' && buffer[i-1] != '\n'){
                    //printf("CHECK");
                    buffer[i] = ' ';
                }
                
            }
        }
        //printf(" last char: %c ", buffer[columns]); 

        /*for (i = 0; i < arrayIndex+1; i++){
            if (buffer[i] == '\n'){
                if (i > 0 && isalnum(buffer[i-1]) == 1 && isalnum(buffer[i+1]) == 1){
                    //printf("inside");
                    buffer[i] = ' ';
                }
            }
        }*/


        if (buffer[arrayIndex] == ' ' || buffer[arrayIndex] == '\n'){
            for (i = 0; i < arrayIndex+1; i++){
                printf("%c", buffer[i]);
            }
            //printf("%c", *buffer);
            printf("\n");
            counter = 0;
        }
        //else if (strcmp(&buffer[columns-1], " ") == 0 || buffer[columns-1] == '\n'){
        else if(buffer[arrayIndex-1] == ' ' || buffer[arrayIndex-1] == '\n'){
            //printf("arrayindex %d", arrayIndex);
            for (i = 0; i < arrayIndex; i++){
                printf("%c", buffer[i]);
            }
            //printf("%c", *buffer);
            printf("\n");
            counter = 1;
        }
        else{   //stops in the middle of a word
            //go backwards in buffer array until we reach a white space
            //then replace the characters after the white space at the beginning of the buffer array
            int index = arrayIndex;
            //printf("%d\n", index);
            //printf("%d", index);
            counter = 0;    //counts the characters after the whitespace we are printing till

            //gets index of what we are printing to in buffer array
            while (buffer[index] != ' '){
                index--;
                counter++;
            }
            //printf("%d", index);

            //print characters
            //printf("%d\n", index);
            for (i = 0; i <= index; i++){
                printf("%c", buffer[i]);
            }
            printf("\n");

            //move pointer in file to where we last printed
            //printf("%d", counter);
            //lseek(filename, -(counter+1), SEEK_CUR); //2nd argument should be -counter

            //empty the buffer array
            //for (i = 0; i < columns+1; i++){
                //buffer[i] = '\0';
            //}
            //read(filename, &buffer[0], 1);
            //printf("%c\n", buffer[0]);
        }
        lseek(filename, -(counter), SEEK_CUR); //2nd argument should be -counter
        //printf(" char %c", buffer[arrayIndex]);

            //empty the buffer array
        for (i = 0; i < arrayIndex+1; i++){
            buffer[i] = '\0';
        }
        printf("new: %c!", buffer[0]);
    }

}

int main(int argc, char** argv) {


    //buffer array
    int columns = atoi(argv[1]); 
    //printf("%d\n", columns);
    char* buffer;
    //MAKE BUFFER LENGTH MACRO- FIX!
    buffer = (char*)malloc((columns+1) * sizeof(char));
    //printf("size %lu\n", sizeof(buffer));
    //buffer[0] = 't';
    //buffer[1] = 's';
    //printf("%c", buffer[1]);
    
    int fp = open(argv[2], O_RDONLY);
    word_wrap(fp, buffer, columns);
    free(buffer);


    // read one character at a time
/* while((ch = getc(fp)) != EOF)
    {
      //...
    } 
...
int ReadFile(FILE* fp)
{
   //Do what you wish... :)
  return getc(fp); 
}  
*/

}

/* PSEUDOCODE
use char buffer array (put it as second argument in read())
- read(filename, bufferarray, NULL/sizeofread)
- bufferarray length will be length of desired columns
- once bufferarray is full AND/OR word cannot fit in bufferarray, start a new line 
- use fseek(filename, charposition, SEEK_SET)
- PRINT TO STANDARD OUTPUT!!!
