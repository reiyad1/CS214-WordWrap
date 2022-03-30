#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include <limits.h>
#include <sys/stat.h>

#define size INT_MAX


void word_wrap(int filename, char *buffer, char *temp, int columns){
    int i;
    int counter = 0;
    int arrayIndex = -1;
    //int bytePosition;

    ssize_t checkEOF;

    while(1){
        counter = 0;
        arrayIndex = -1;
        //using columns+1 to see if we cut a word in half or not
        for (i = 0; i < (columns+1); i++){
            checkEOF = read(filename, &buffer[0], 1);
            temp[i] = buffer[0];    //so we dont rely on buffer length for wrapping
            buffer[0] = '\0';
            //printf("%c", temp[i]);
            arrayIndex++;
            
            //if you reach end of file
            if (checkEOF == 0){
                printf("%s", temp);
                //break;
                return;
            }
            
            //printf("%c", buffer[i]);
            //to ensure that we dont have consecutive spaces
            if (i > 0){
                //check for double space
                if (temp[i] == ' ' && temp[i-1] == ' '){
                    temp[i] = '\0';   //makes sure there is only one space
                    i--;
                    arrayIndex--;
                }
                if (temp[i] == '\n' && temp[i-1] == ' '){
                    //printf("CHECK");
                    if (i != columns+1){
                        checkEOF = read(filename, &buffer[0], 1); //MAKE SURE THIS ONLY HAPPENS IF IT HASNT GONE PAST 
                        temp[i+1] = buffer[0];
                        //printf("%c", temp[i]);
                        arrayIndex++;
                        //printf(" %c ", temp[i+1]);
                    }
                    break;
                }
                else if (temp[i] == '\n' && temp[i-1] != '\n'){
                    //printf("CHECK");
                    temp[i] = ' ';
                }
                
                
            }
        }
        //printf("%c", *temp); 


        if (temp[arrayIndex] == ' ' || temp[arrayIndex] == '\n'){
            for (i = 0; i < arrayIndex+1; i++){
                printf("%c", temp[i]);
            }
            //printf("%c", *buffer);
            printf("\n");
            counter = 0;
        }
        
        else if(temp[arrayIndex-1] == ' ' || temp[arrayIndex-1] == '\n'){
        //if(temp[arrayIndex-1] == ' ' || temp[arrayIndex-1] == '\n'){
            //printf("arrayindex %d", arrayIndex);
            for (i = 0; i < arrayIndex; i++){
                printf("%c", temp[i]);
            }
            //printf("%c", *buffer);
            printf("\n");
            counter = 1;
        }
        else{   //stops in the middle of a word
            //go backwards in buffer array until we reach a white space
            //then replace the characters after the white space at the beginning of the buffer array
            int index = arrayIndex;
            counter = 0;    //counts the characters after the whitespace we are printing till

            //gets index of what we are printing to in buffer array
            while (temp[index] != ' ' && index != 0){
                index--;
                counter++;
            }
            //printf("%d", index);

            //print characters
            //printf("%d\n", index);
            for (i = 0; i <= index; i++){
                printf("%c", temp[i]);
            }
            printf("\n");

        }
        //bytePosition = lseek(filename, -(counter), SEEK_CUR); 
        lseek(filename, -(counter), SEEK_CUR);

        //empty the buffer array
        for (i = 0; i < arrayIndex+1; i++){
            temp[i] = '\0';
        }
        //printf("new: %c!", temp[0]);

    }

}

void word_wrap_dir(int filename, char *buffer, char *temp, int columns){
    int i;
    int counter = 0;
    int arrayIndex = -1;
    //int bytePosition;

    ssize_t checkEOF;

    while(1){
        counter = 0;
        arrayIndex = -1;
        //using columns+1 to see if we cut a word in half or not
        for (i = 0; i < (columns+1); i++){
            checkEOF = read(filename, &buffer[0], 1);
            temp[i] = buffer[0];    //so we dont rely on buffer length for wrapping
            buffer[0] = '\0';
            //printf("%c", temp[i]);
            arrayIndex++;
            
            //if you reach end of file
            if (checkEOF == 0){
                printf("%s", temp); //WRITE
                //break;
                return;
            }
            
            //printf("%c", buffer[i]);
            //to ensure that we dont have consecutive spaces
            if (i > 0){
                //check for double space
                if (temp[i] == ' ' && temp[i-1] == ' '){
                    temp[i] = '\0';   //makes sure there is only one space
                    i--;
                    arrayIndex--;
                }
                if (temp[i] == '\n' && temp[i-1] == ' '){
                    //printf("CHECK");
                    if (i != columns+1){
                        checkEOF = read(filename, &buffer[0], 1); //MAKE SURE THIS ONLY HAPPENS IF IT HASNT GONE PAST 
                        temp[i+1] = buffer[0];
                        //printf("%c", temp[i]);
                        arrayIndex++;
                        //printf(" %c ", temp[i+1]);
                    }
                    break;
                }
                else if (temp[i] == '\n' && temp[i-1] != '\n'){
                    //printf("CHECK");
                    temp[i] = ' ';
                }
                
                
            }
        }
        //printf("%c", *temp); 


        if (temp[arrayIndex] == ' ' || temp[arrayIndex] == '\n'){
            for (i = 0; i < arrayIndex+1; i++){ //WRITE
                printf("%c", temp[i]);
            }
            //printf("%c", *buffer);
            printf("\n");
            counter = 0;
        }
        
        else if(temp[arrayIndex-1] == ' ' || temp[arrayIndex-1] == '\n'){
        //if(temp[arrayIndex-1] == ' ' || temp[arrayIndex-1] == '\n'){
            //printf("arrayindex %d", arrayIndex);
            for (i = 0; i < arrayIndex; i++){   //WRITE
                printf("%c", temp[i]);
            }
            //printf("%c", *buffer);
            printf("\n");
            counter = 1;
        }
        else{   //stops in the middle of a word
            //go backwards in buffer array until we reach a white space
            //then replace the characters after the white space at the beginning of the buffer array
            int index = arrayIndex;
            counter = 0;    //counts the characters after the whitespace we are printing till

            //gets index of what we are printing to in buffer array
            while (temp[index] != ' ' && index != 0){
                index--;
                counter++;
            }
            //printf("%d", index);

            //print characters
            //printf("%d\n", index);
            for (i = 0; i <= index; i++){   //WRITE
                printf("%c", temp[i]);
            }
            printf("\n");

        }
        //bytePosition = lseek(filename, -(counter), SEEK_CUR); 
        lseek(filename, -(counter), SEEK_CUR);

        //empty the buffer array
        for (i = 0; i < arrayIndex+1; i++){
            temp[i] = '\0';
        }
        //printf("new: %c!", temp[0]);

    }

}


int main(int argc, char** argv) {
    
    int columns = atoi(argv[1]);
    //buffer array
    char* buffer;
    //MAKE BUFFER LENGTH MACRO- FIX!
    buffer = (char*)malloc(size * sizeof(char));
    char *temp = (char*)malloc((columns+1) * sizeof(char));

    struct stat file_stat;
    if (argc == 3){
        stat(argv[2], &file_stat);
        //check if argv[2] is file or directory
        if (S_ISDIR(file_stat.st_mode) != 0){
            //it is directory
            //go into and wordwrap for each file
            DIR *directory = opendir(argv[2]);
            struct dirent *file;
            //char filename[255];
            while ((file = readdir(directory)) != NULL){
                //printf("INSIDE WHILE LOOP");
                //char filename[255];
                printf("%s\n", file->d_name);

                //GIVING INFINITE LOOP!!- FIX
                if (file->d_type == DT_REG && (strcmp(file->d_name, ".")!=0) && (strcmp(file->d_name, "..")!=0)){
                    printf("INSIDE FORLOOP");
                    strncpy(filename, file->d_name, 254);
                    filename[254-1] = '\0';
                    int fp = open(filename, O_RDONLY);
                    word_wrap(fp, buffer, temp, columns);
                    close(fp);
                    //return 0;
                }
                else if ((strcmp(file->d_name, ".")!=0) && (strcmp(file->d_name, "..")!=0))
                    continue;
                
            }
            closedir(directory);
            printf("done");
        }
        else if (S_ISREG(file_stat.st_mode) != 0){
            //word wrap with file- regular
            int fp = open(argv[2], O_RDONLY);
            word_wrap(fp, buffer, temp, columns);
            printf("\n");
            close(fp);
            
        }
    }
    free(buffer);
    free(temp);
    //else if (argc == 2){
        //stdin stdout wrap
    //}





    //printf("size %lu\n", sizeof(buffer));
    //buffer[0] = 't';
    //buffer[1] = 's';
    //printf("%c", buffer[1]);



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
*/
