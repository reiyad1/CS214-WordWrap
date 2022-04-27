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
#include <pthread.h>


#define size INT_MAX
#define QUEUESIZE INT_MAX

//to use in worker function for threading
struct func_args{
    char *argumentTwo;
    char *basePath;
    char *buffer;
    char *temp;
    int columns;
    int output_type;
};


struct queue{
    //int data[QUEUESIZE];
    char data[QUEUESIZE];
    int start;
    int stop;
    int full;
    pthread_mutex_t lock;
    pthread_cond_t enqueue_ready; 
    pthread_cond_t dequeue_ready;
};

int queue_init(struct queue *q){
    q->start = 0;
    q->stop = 0;
    q->full = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->enqueue_ready, NULL);
    pthread_cond_init(&q->dequeue_ready, NULL);
    return 0;
}

int enqueue(char *n, struct queue *q){
    pthread_mutex_lock(&q->lock);
    while (q->full){
        pthread_cond_wait(&q->enqueue_ready, &q->lock); //waits for enqueue to be ready and unlocks?- suspends current thread, releases lock,
    }
    q->data[q->stop] = *n;
    q->stop++;
    if (q->stop == QUEUESIZE) q->stop = 0;
    if (q->start == q->stop) q->full = 1;
    pthread_cond_signal(&q->dequeue_ready);
    pthread_mutex_unlock(&q->lock);
    return 0;
}

int dequeue(char *n, struct queue *q){
    pthread_mutex_lock(&q->lock);
    while (!q->full && q->start == q->stop){
        pthread_cond_wait(&q->dequeue_ready, &q->lock);
    }
    *n = q->data[q->start];
    q->start++;
    if (q->start == QUEUESIZE) q->start = 0;
    q->full = 0;
    pthread_cond_signal(&q->enqueue_ready);
    pthread_mutex_unlock(&q->lock);
    return 0;
}

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

/*int wrapInDirectory(char *basePath, char *file, char *buffer, char *temp, int columns, int output_type){
    //char filename[sizeof(basePath) + sizeof(file) + 10];
    char *filename;
    filename = (char*)malloc(size * sizeof(char));

    strcpy(filename, basePath);
    strcat(filename, "/");
    strcat(filename, file);
    int fp = open(filename, O_RDONLY);
    if (fp == -1){
        perror(filename);
        free(buffer);
        free(temp);
        return EXIT_FAILURE;
    }
    //char outputFile[sizeof(basePath) + sizeof(file) + 10];
    char *outputFile;
    outputFile = (char*)malloc(size * sizeof(char));

    //if name of file contains "wrap", rewrite the file
    if (strstr(file, "wrap") != NULL){
        strcpy(outputFile, file);
        output_type = open(outputFile, O_RDWR, 0666);
    }
    //name of file does not contain "wrap", create and write to new file
    else{
        strcpy(outputFile, basePath);
        strcat(outputFile, "/wrap.");
        strcat(outputFile, file);
        output_type = open(outputFile, O_RDWR | O_CREAT, 0666);
    }

    word_wrap(fp, buffer, temp, columns, output_type);
    close(fp);
    close(output_type);
    free(filename);
    free(outputFile);
    return EXIT_SUCCESS;
}*/

//for threading directories
void *dir_worker(void *arg){
    //run wrapFilesRecursively using struct
    //dequeue the subdirectory once done with the thread- figure out how to dequeue once it is done
}

//for threading files
void *file_worker(void *arg){
    //run wordwrap function using struct
    //dequeue file path once done

    //extracts data to use in worker
    struct func_args *temp = arg;
    arg->argumentTwo = filename;
    arg->basePath = file->d_name;
    arg->buffer = buffer;
    arg->temp = temp;
    arg->columns = columns;
    arg->output_type = output_type;

    int fp = open(filename, O_RDONLY);
        if (fp == -1){
            perror(filename); 
            free(buffer);
            free(temp);
            return EXIT_FAILURE;
        }
                    
        char *outputFile;
        outputFile = (char*)malloc(size * sizeof(char));

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

        //call wordwrap
        word_wrap(fp, buffer, temp, columns, output_type);

        //DEQUEUE FROM QUEUE

        return NULL;


}
//for threading, the worker functions are the ones dequeueing- the reg main function or recursivewrap will enqueue when 
    //encountering a file/subdirectory


void wrapFilesRecursively(char *argumentTwo, char *basePath, char *buffer, char *temp, int columns, int output_type)
{
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    // Unable to open directory stream
    //if (!dir)
        //return; //give error statement here

    while ((dp = readdir(dir)) != NULL)
    {
        if (dp->d_type == DT_REG && (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)){  //if object being read is a reg file
            //call word wrap
            //convert dp to int and open using open()
            //word_wrap(dp, buffer, temp, columns, 1);
            //char *filename = dp->d_name;

            //PUT PATH IN FILE WORK QUEUE!!!

            //wrapInDirectory(basePath, filename, buffer, temp, columns, 1);

            char *filename;
            filename = (char*)malloc(size * sizeof(char));

            strcpy(filename, basePath);
            strcat(filename, "/");
            strcat(filename, dp->d_name);
            int fp = open(filename, O_RDONLY);
            if (fp == -1){
                perror(filename);
                free(buffer);
                free(temp);
                return; //RETURN WITH ERROR
            }

            char *outputFile;
            outputFile = (char*)malloc(size * sizeof(char));

            //if name of file contains "wrap", rewrite the file
            if (strstr(dp->d_name, "wrap") != NULL){
                strcpy(outputFile, dp->d_name);
                output_type = open(outputFile, O_RDWR, 0666);
            }
            //name of file does not contain "wrap", create and write to new file
            else{
                strcpy(outputFile, basePath);
                strcat(outputFile, "/wrap.");
                strcat(outputFile, dp->d_name);
                output_type = open(outputFile, O_RDWR | O_CREAT, 0666);
            }

            word_wrap(fp, buffer, temp, columns, output_type);
            close(fp);
            close(output_type);
            free(filename);
            free(outputFile);

        }
        else if (dp->d_type == DT_DIR && (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)){
            //change basePath name to new subdirectory
            //recursively call function again
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            printf("%s", path);

            //PUT PATH IN DIRECTORY WORK QUEUE
            
            wrapFilesRecursively(argumentTwo, path, buffer, temp, columns, output_type);
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
    return;

    //in the while loop, have if statements to see if its a file or another directory
        //if its a file, wrap and continue looking at the other stuff in the directory
        //if its a directory, call the function again recursively
}

/*int main(int argc, char **argv)
{
  int nthreads, ret_val, tid, chunk_size;
  void *status;
  FILE *fp;
  pthread_t *threadId;

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
    //thread stuff
    //pthread_t *threadId;
    
    if (argc < 2)
        return EXIT_FAILURE;
    
    int columns;
    char *argumentTwo;
    int is_recursive; //1 if yes, 0 if no
    char *M_temp;
    char *N_temp;
    int M;
    int N;

    //threading initialization stuff- file queue and directory queue
    struct queue *file_queue;
    struct queue *dir_queue;
    queue_init(file_queue);
    queue_init(dir_queue);



    
    char* buffer;   //buffer array
    buffer = (char*)malloc(size * sizeof(char));
    char *temp = (char*)malloc((size) * sizeof(char));
    
    int output_type;  //0 means write to stdout, 1 means write to file


    if (argc > 2){
        //printf("%d", argc);
        struct stat file_stat;
        if (argc == 3){
            argumentTwo = argv[2];
            columns = atoi(argv[1]);
        }
        else{
            argumentTwo = argv[3];
            columns = atoi(argv[2]);
            is_recursive = 1;
            M_temp = &argv[1][2];
            N_temp = &argv[1][4];
            M = atoi(M_temp);
            N = atoi(N_temp);

            //printf("IN\n");
            //printf("%s\n", argumentTwo);
        }

        //create N amount of threads for file wrapping
        pthread_t file_tid[N];
        int file_tid_count = 0;

        pthread_t dir_tid[M];
        int dir_tid_count = 0;
        
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
            //printf("is directory\n");
            output_type = 1;

            //go into and wordwrap for each file
            DIR *directory = opendir(argumentTwo);
            struct dirent *file;
            while ((file = readdir(directory)) != NULL){
                //printf("%s\n", file->d_name);

                if (file->d_type == DT_REG && (strcmp(file->d_name, ".")!=0) && (strcmp(file->d_name, "..")!=0)){
                    
                    //printf("IN 1\n");

                    //add to file queue and make a thread to wrap what has been enqueued
                    
                    //maybe dont call this since we are using threads
                    //wrapInDirectory(argumentTwo, file->d_name, buffer, temp, columns, output_type);

                    char *filename;
                    filename = (char*)malloc(size * sizeof(char));
                    strcpy(filename, argumentTwo);
                    strcat(filename, "/");
                    strcat(filename, file->d_name);
                    
                    struct func_args data_args;
                    data_args->argumentTwo = filename;
                    data_args->basePath = file->d_name;
                    data_args->buffer = buffer;
                    data_args->temp = temp;
                    data_args->columns = columns;
                    data_args->output_type = output_type;
                    //from here, the stuff should be in a worker function or some sort of open file function bc the thread has to open the file
                    /*int fp = open(filename, O_RDONLY);
                    if (fp == -1){
                        perror(filename); 
                        free(buffer);
                        free(temp);
                        return EXIT_FAILURE;
                    }
                    
                    char *outputFile;
                    outputFile = (char*)malloc(size * sizeof(char));

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
                    }*/
                    
                    //put file path in file queue
                    //make thread for the file
                    if (file_tid_count < N){
                        pthread_create(&tid[i], NULL, file_worker, &data_args);
                        file_tid_count++;
                    }

                    //maybe dont wrap here- call wrap in worker thread
                    //word_wrap(fp, buffer, temp, columns, output_type);
                    free(filename);
                    free(outputFile);
                    close(fp);
                    close(output_type);

                }
                
                    
                
                //else if its a subdirectory
                //figure out how to do it for diff thread numbers!!!
                else if(file->d_type == DT_DIR && is_recursive == 1){
                    
                    if ((strcmp(file->d_name, ".")!=0) && (strcmp(file->d_name, "..")!=0)){
                    //else if (file->d_type == DT_DIR && (strcmp(&argv[1][0], "-") != 0 && strcmp(&argv[1][1], "r") != 0)){
                        //call the function that does the recursive stuff
                        char filename[sizeof(argumentTwo) + sizeof(file->d_name) + 3];
                        strcpy(filename, argumentTwo);
                        strcat(filename, "/");
                        strcat(filename, file->d_name);
                        printf("%s\n", filename);
                        //char *basePath = file->d_name;  //ERROR- MUST BE TEST/SUB


                        if (dir_tid_count < M){
                            pthread_create(&tid[i], NULL, dir_worker, STUFF);
                            dir_tid_count++;

                        }
                        wrapFilesRecursively(argumentTwo, filename, buffer, temp, columns, output_type);
                    }

                }
                else if ((strcmp(file->d_name, ".")==0) && (strcmp(file->d_name, "..")==0))
                    continue;
                    
            }

            //thread stuff

            printf("done\n");
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
        columns = atoi(argv[2]);
        output_type = 0;
        word_wrap(0, buffer, temp, columns, output_type);

    }
    free(buffer);
    free(temp);
    return EXIT_SUCCESS;

}

