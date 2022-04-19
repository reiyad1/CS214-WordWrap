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

int word_wrap(int filename, char *buffer, char *temp, int columns, int output_type){
    int i;
    int counter = 0;
    int arrayIndex = -1;
    int i_val = 0;
    int length = 0;
    int check_line_line = 0; //0 means no double new line, 1 means double new line
    int ret_statement;
    //int bytePosition;
    int write_to;
    if (filename == 0 || output_type == 0){
        write_to = 1;
    }
    else{   //write to file
        write_to = output_type; //filename (in int form)
    }

    //read() and write() 0 for read from stdin, 1 for write to stdout, filename for file

    ssize_t checkEOF;

    while(1){
        arrayIndex = counter-1;
        counter = 0;
        check_line_line = 0;
        //using columns+1 to see if we cut a word in half or not
        for (i = i_val; i < (columns+1); i++){
            checkEOF = read(filename, &buffer[0], 1);
            temp[i] = buffer[0];    //so we dont rely on buffer length for wrapping
            buffer[0] = '\0';
            arrayIndex++;
            
            //if you reach end of file
            if (checkEOF == 0){
                for (i = 0; i < arrayIndex+1; i++)
                    write(write_to, &temp[i], 1);
                ret_statement = 0;
                return ret_statement;
            }
            
            //to ensure that we dont have consecutive spaces
            if (i > 0){

                if (temp[i] == '\n' && temp[i-1] == '\n'){
                    arrayIndex--;
                    check_line_line = 1;
                    break;
                }
                if ((temp[i] == ' ' && temp[i-1] == ' ') || (temp[i] == ' ' && temp[i-1] == '\n')){
                    temp[i] = '\0';   //makes sure there is only one space
                    i--;
                    arrayIndex--;
                }
                
                
            }
            int j;
            for (j = 1; j < arrayIndex-1; j++){
                if (temp[j] == '\n' && temp[j-1] != '\n' && temp[j+1] != '\n'){
                    temp[j] = ' ';
                }
            }
        }

        //check if full word is too big for column amount, return exit_failure if so
        int too_big = 0;    //0 means there is no spaces, 1 means there is a space
        for (i = 0; i < arrayIndex+1; i++){
            if (temp[i] == ' ' || temp[i] == '\n'){
                too_big = 1;
                break;
            }
        }

        if (too_big == 0){
            arrayIndex++;
            while (1){
                read(filename, &buffer[0], 1);
                temp[arrayIndex] = buffer[0];
                if (temp[arrayIndex] == ' ' || (temp[arrayIndex] == '\n'))
                    break;
                arrayIndex++;
            }
            for (i = 0; i < arrayIndex; i++){
                write(write_to, &temp[i], 1);
            }
            for (i = 0; i < arrayIndex; i++){
                temp[i] = '\0';
            }
            ret_statement = 1;
        }

        //when word fits perfectly
        if (temp[arrayIndex] == ' '){
            for (i = 0; i < arrayIndex; i++){
                write(write_to, &temp[i], 1);
            }
            write(write_to, "\n", 1);
            counter = 0;
        }
        else if (temp[arrayIndex] == '\n' && check_line_line == 0){
            temp[arrayIndex] = ' ';
            for (i = 0; i < arrayIndex; i++){
                write(write_to, &temp[i], 1);
            }
            write(write_to, "\n", 1);
            counter = 0;

        }
        
        else if((arrayIndex > 0) && ((temp[arrayIndex-1] == ' ') || (temp[arrayIndex-1] == '\n'))){
            for (i = 0; i < arrayIndex; i++){
                //printf("%c", temp[i]);
                write(write_to, &temp[i], 1);
            }
            write(write_to, "\n", 1);
            counter = 1;
        }
        else{   //stops in the middle of a word
            //go backwards in buffer array until we reach a white space
            //then replace the characters after the white space at the beginning of the buffer array
            int index = arrayIndex;
            counter = 0;    //counts the characters after the whitespace we are printing till

            //gets index of what we are printing to in buffer array
            while (temp[index] != ' ' && index != 0 && temp[index] != '\n'){
                index--;
                counter++;
            }

            for (i = 0; i < index; i++){
                write(write_to, &temp[i], 1);
            }
            if (temp[index] == '\n'){
                write(write_to, &temp[i+1], 1);
            }
            write(write_to, "\n", 1);

        }

            length = columns+1 - counter;
            for (i = 0; i < counter; i++){
                temp[i] = temp[length + i];
            }

            //empty the buffer array
            for (i = counter; i < arrayIndex+1; i++){
                temp[i] = '\0';
            }

            if (temp[0] == ' '){
                for (i = 1; i < arrayIndex+1; i++){
                    temp[i] = temp[i-1];
                }
            }

            i_val = counter;


    }
    write(write_to, "\n", 1);
    if (ret_statement == 1)
        return 1;
    else
        return 0;

}

wrapInDirectory(char *argumentTwo, struct dirent *file, char *buffer, char *temp, int columns, int output_type){
    char filename[sizeof(argumentTwo) + sizeof(file->d_name) + 3];
    strcpy(filename, argumentTwo);
    strcat(filename, "/");
    strcat(filename, file->d_name);
    int fp = open(filename, O_RDONLY);
    if (fp == -1){
        perror(filename); 
        free(buffer);
        free(temp);
        return EXIT_FAILURE;
    }
    char outputFile[sizeof(argumentTwo) + sizeof(file->d_name) + 6];

    //if name of file contains "wrap", rewrite the file
    if (strstr(file->d_name, "wrap") != NULL){
        strcpy(outputFile, file->d_name);
        output_type = open(outputFile, O_RDWR, 0666);
    }
    //name of file does not contain "wrap", create and write to new file
    else{
        strcpy(outputFile, argumentTwo);
        strcat(outputFile, "/wrap.");
        strcat(outputFile, file->d_name);
        output_type = open(outputFile, O_RDWR | O_CREAT, 0666);
    }

    word_wrap(fp, buffer, temp, columns, output_type);
    close(fp);
    close(output_type);
}

void wrapFilesRecursively(char *argumentTwo, char *basePath, char *buffer, char *temp, int columns)
{
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    // Unable to open directory stream
    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (dp->d_type == DT_REG && (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)){  //if object being read is a reg file
            //call word wrap
            //convert dp to int and open using open()
            //word_wrap(dp, buffer, temp, columns, 1);
            wrapInDirectory(argumentTwo, dp, buffer, temp, columns, 1);
        }
        else if (dp->d_type == DT_DIR){
            //change basePath name to new subdirectory
            //recursively call function again
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            wrapFilesRecursively(argumentTwo, path, buffer, temp, columns);
        }
        /*if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            printf("%s\n", dp->d_name); //we would wrap here instead of print

            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            listFilesRecursively(path);
        }*/
    }

    closedir(dir);

    //in the while loop, have if statements to see if its a file or another directory
        //if its a file, wrap and continue looking at the other stuff in the directory
        //if its a directory, call the function again recursively
}

/*int main(int argc, char **argv)
{
  int nthreads, ret_val, tid, chunk_size;
  void *status;
  FILE *fp;
  pthread_t *threads;

  struct to_read data;

  fp = fopen("file.txt", "r");
  
  if (fp==NULL)
    {
      perror("Could not open file. Exiting");
      exit(1);
    }

  printf("Enter the number of threads: ");
  scanf("%d",&nthreads);
  threads = malloc(nthreads*sizeof(pthread_t));

  
  chunk_size = /nthreads;

  for(tid = 0; tid < nthreads-1; tid++)
    {
      data.fp=fp;
      data.offset = chunk_size;
      data.start = tid*chunk_size+1;
      ret_val = pthread_create(&threads[tid], NULL, &word_freq, &data);
      
      if(ret_val!= 0) {
        printf ("Create pthread error!\n");
        exit (1);
      }
      pthread_join(threads[tid],&status);
    }

  //last thread process the chunk_size bytes + any remaining over
  data.fp=fp;
  data.offset = chunk_size + (13219 % nthreads);
  data.start = (nthreads-1)*chunk_size+1;
  ret_val = pthread_create(&threads[nthreads-1], NULL, &word_freq, &data);
      
  if(ret_val!= 0) {
    printf ("Create pthread error!\n");
    exit (1);
  }


  fclose(fp);
  free(threads);

  pthread_exit(NULL);
  return 0;*/

int main(int argc, char** argv) {
    
    if (argc < 2)
        return EXIT_FAILURE;
    int columns = atoi(argv[1]);
    char* buffer;   //buffer array
    buffer = (char*)malloc(size * sizeof(char));
    char *temp = (char*)malloc((size) * sizeof(char));
    
    int output_type;  //0 means write to stdout, 1 means write to file


    if (argc == 3){
        struct stat file_stat;
        char *argumentTwo = argv[2];
        stat(argumentTwo, &file_stat);
        if(stat(argumentTwo, &file_stat) == -1){
            perror(argumentTwo);
            free(buffer);
            free(temp);
            return EXIT_FAILURE;
        }


        //check if argv[2] is file or directory
        if (S_ISDIR(file_stat.st_mode) != 0){
            //it is directory
            output_type = 1;

            //go into and wordwrap for each file
            DIR *directory = opendir(argumentTwo);
            struct dirent *file;
            while ((file = readdir(directory)) != NULL){
                if (file->d_type == DT_REG && (strcmp(file->d_name, ".")!=0) && (strcmp(file->d_name, "..")!=0)){
                    
                    char filename[sizeof(argumentTwo) + sizeof(file->d_name) + 3];
                    strcpy(filename, argumentTwo);
                    strcat(filename, "/");
                    strcat(filename, file->d_name);
                    int fp = open(filename, O_RDONLY);
                    if (fp == -1){
                        perror(filename); 
                        free(buffer);
                        free(temp);
                        return EXIT_FAILURE;
                    }
                    char outputFile[sizeof(argumentTwo) + sizeof(file->d_name) + 6];

                    //if name of file contains "wrap", rewrite the file
                    if (strstr(file->d_name, "wrap") != NULL){
                        strcpy(outputFile, file->d_name);
                        output_type = open(outputFile, O_RDWR, 0666);
                    }
                    //name of file does not contain "wrap", create and write to new file
                    else{
                        strcpy(outputFile, argumentTwo);
                        strcat(outputFile, "/wrap.");
                        strcat(outputFile, file->d_name);
                        output_type = open(outputFile, O_RDWR | O_CREAT, 0666);
                    }

                    word_wrap(fp, buffer, temp, columns, output_type);
                    close(fp);
                    close(output_type);
                }
                //ignores the "." and ".." files
                else if ((strcmp(file->d_name, ".")==0) && (strcmp(file->d_name, "..")==0))
                    continue;
                
                //else if its a subdirectory
                else if(file->d_type == DT_DIR){
                    //call the function that does the recursive stuff
                    char *basePath = file;
                    wrapFilesRecursively(argumentTwo, basePath, buffer, temp, columns);

                }
                    
            }
            closedir(directory);
        }
        //if argv[2] is a regular file and NOT a directory
        else if (S_ISREG(file_stat.st_mode) != 0){
            output_type = 0;
            int fp = open(argumentTwo, O_RDONLY);
            if (fp == -1)
            {
                perror(argumentTwo); 
                free(buffer);
                free(temp);
                return EXIT_FAILURE;
            } 
            word_wrap(fp, buffer, temp, columns, output_type);
            close(fp);
            
        }
    }
    //if no filename given, read from stdin
    else if (argc == 2){
        output_type = 0;
        word_wrap(0, buffer, temp, columns, output_type);

    }
    free(buffer);
    free(temp);
    return EXIT_SUCCESS;

}

