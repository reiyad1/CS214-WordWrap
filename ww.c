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

//int fileOpen(char* readFile, char* word_wrap);
void word_wrap(int filename, char *buffer, char *temp, int columns, int output_type){
    int i;
    int counter = 0;
    int arrayIndex = -1;
    //int bytePosition;
    int write_to;
    if (filename == 0 || output_type == 0){
        write_to = 1;
    }
    //else{   //write to file
        //write_to = filename (in int form)
    //}

    //read() and write() 0 for read from stdin, 1 for write to stdout, filename for file

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
                //printf("%c", temp[i]);
                write(write_to, &temp[i], 1);
            }
            //printf("%c", *buffer);
            //printf("\n");
            write(1, "\n", 1);
            counter = 0;
        }
        
        else if(temp[arrayIndex-1] == ' ' || temp[arrayIndex-1] == '\n'){
        //if(temp[arrayIndex-1] == ' ' || temp[arrayIndex-1] == '\n'){
            //printf("arrayindex %d", arrayIndex);
            for (i = 0; i < arrayIndex; i++){
                //printf("%c", temp[i]);
                write(write_to, &temp[i], 1);
            }
            //printf("%c", *buffer);
            //printf("\n");
            write(write_to, "\n", 1);
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
                //printf("%c", temp[i]);
                write(write_to, &temp[i], 1);
            }
            //printf("\n");
            write(write_to, "\n", 1);

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

/*int fileOpen (char* readFile, char* word_wrap){
    int bytes;
    char buffer[BUFFERSIZE];
    
    if(readFile != NULL){
        int fd = open(readFile, O_RDWR);
        
        if(fd == -1){
            perror(readFile);
            return EXIT_FAILURE;
        }
        
        bytes = read(fd, buffer, BUFFERSIZE);
        close(fd);
    }
    else
    {
        bytes = read(0, buffer, BUFFERSIZE);
    }
    
    if(bytes == 0){
        printf("ERROR: NOTHING IN FILE.");
        return EXIT_FAILURE;
    }
    
    else if(bbytes < 0)
    {
        perror("Error");
        return EXIT_FAILURE;
    }
    
    word_wrap(filename, colums, bytes, temp, buffer);
    return EXIT_SUCCESS;
}*/


int main(int argc, char** argv) {
    
    int columns = atoi(argv[1]);
    //buffer array
    char* buffer;
    //MAKE BUFFER LENGTH MACRO- FIX!
    buffer = (char*)malloc(size * sizeof(char));
    char *temp = (char*)malloc((columns+1) * sizeof(char));
    char filename[255];
    int output_type;     //0 means write to stdout, 1 means write to file

    struct stat file_stat;
    if (argc == 3){
        stat(argv[2], &file_stat);
        //check if argv[2] is file or directory
        if (S_ISDIR(file_stat.st_mode) != 0){
            //it is directory
            output_type = 1;
            //go into and wordwrap for each file
            DIR *directory = opendir(argv[2]);
            struct dirent *file;
            //char filename[255];
            while ((file = readdir(directory)) != NULL){
                //printf("INSIDE WHILE LOOP");
                printf("%s\n", file->d_name);

                //GIVING INFINITE LOOP!!- FIX
                if (file->d_type == DT_REG && (strcmp(file->d_name, ".")!=0) && (strcmp(file->d_name, "..")!=0)){
                    printf("INSIDE FORLOOP");
                    strncpy(filename, file->d_name, 254);
                    filename[254-1] = '\0';
                    int fp = open(filename, O_RDONLY);
                    word_wrap(fp, buffer, temp, columns, output_type);
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
            output_type = 0;
            int fp = open(argv[2], O_RDONLY);
            word_wrap(fp, buffer, temp, columns, output_type);
            printf("\n");
            close(fp);
            
        }
    }
    else if (argc == 2){    //no filename given, read from stdin
        output_type = 0;
        word_wrap(0, buffer, temp, columns, output_type);

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
    
    /*if(argc == 2)
    {
       fileOpen(NULL, NULL);
       return EXIT_SUCCESS;
    }
    
    if(argc < 2) return EXIT_FAILURE;
    columns = atoi(argv[1]);
    
    
    fileOpen(argv[2], NULL);
    return EXIT_SUCCESS:
    
    int fp = open(argv[2], O_RDONLY);
    word_wrap(fp, buffer, temp, columns);
    free(buffer);
    free(temp);
    printf("\n");*/



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
