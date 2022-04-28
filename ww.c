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

/*****************************************************************************/
//to use in worker function for threading
struct cell{
    char *file_path;
    char *output_file_path;
};

struct fileQueue{
    //int data[QUEUESIZE];
    //char data[QUEUESIZE];
    struct cell data[QUEUESIZE];
    int start;
    int stop;
    int full;
    pthread_mutex_t lock;
    pthread_cond_t enqueue_ready; 
    pthread_cond_t dequeue_ready;
};

struct fileFunc_args{
    struct fileQueue *q;
    char *argumentTwo;
    char *basePath;
    char *buffer;
    char *temp;
    int columns;
    int output_type;
};

int fileQueue_init(struct fileQueue *q){
    q->start = 0;
    q->stop = 0;
    q->full = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->enqueue_ready, NULL);
    pthread_cond_init(&q->dequeue_ready, NULL);
    return 0;
}

int fileEnqueue(char *file_path, char *output_file_path, struct fileQueue *q){
    pthread_mutex_lock(&q->lock);
    while (q->full){
        pthread_cond_wait(&q->enqueue_ready, &q->lock); //waits for enqueue to be ready and unlocks?- suspends current thread, releases lock,
    }
    //q->data[q->stop] = *n;
    q->data[q->stop].file_path = file_path;
    q->data[q->stop].output_file_path = output_file_path;

    q->stop++;
    if (q->stop == QUEUESIZE) q->stop = 0;
    if (q->start == q->stop) q->full = 1;
    pthread_cond_signal(&q->dequeue_ready);
    pthread_mutex_unlock(&q->lock);
    return 0;
}

struct cell fileDequeue(struct fileQueue *q){
    pthread_mutex_lock(&q->lock);

    while (!q->full && q->start == q->stop){
        pthread_cond_wait(&q->dequeue_ready, &q->lock);
    }

    struct cell *temporary;
    temporary->file_path = q->data[q->start].file_path;
    temporary->output_file_path = q->data[q->start].output_file_path;

    q->start++;
    if (q->start == QUEUESIZE) q->start = 0;
    q->full = 0;
    pthread_cond_signal(&q->enqueue_ready);
    pthread_mutex_unlock(&q->lock);
    return *temporary;
}
/********************************************************************************/
struct dirQueue{
    char dirData[QUEUESIZE];
    int start;
    int stop;
    int full;
    pthread_mutex_t lock;
    pthread_cond_t enqueue_ready; 
    pthread_cond_t dequeue_ready;
};
struct dirFunc_args{
    struct dirQueue *dq;
    struct fileQueue *fq;
    char *basePath;
    char *buffer;
    char *temp;
    int columns;
};
int dirQueue_init(struct dirQueue *q){
    q->start = 0;
    q->stop = 0;
    q->full = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->enqueue_ready, NULL);
    pthread_cond_init(&q->dequeue_ready, NULL);
    return 0;
}
int dirEnqueue(char *dir_path, struct dirQueue *q){
    pthread_mutex_lock(&q->lock);
    while (q->full){
        pthread_cond_wait(&q->enqueue_ready, &q->lock); //waits for enqueue to be ready and unlocks?- suspends current thread, releases lock,
    }
    //q->data[q->stop] = *n;
    q->dirData[q->stop] = *dir_path;

    q->stop++;
    if (q->stop == QUEUESIZE) q->stop = 0;
    if (q->start == q->stop) q->full = 1;
    pthread_cond_signal(&q->dequeue_ready);
    pthread_mutex_unlock(&q->lock);
    return 0;
}

char *dirDequeue(struct dirQueue *q){
    pthread_mutex_lock(&q->lock);

    while (!q->full && q->start == q->stop){
        pthread_cond_wait(&q->dequeue_ready, &q->lock);
    }

    char *temp;
    *temp = q->dirData[q->start];
    q->start++;

    if (q->start == QUEUESIZE) q->start = 0;
    q->full = 0;
    pthread_cond_signal(&q->enqueue_ready);
    pthread_mutex_unlock(&q->lock);
    return temp;
}
/********************************************************************************/

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
void wrapFilesRecursively(char *basePath, char *buffer, char *temp, int columns, struct fileQueue *file_queue, struct dirQueue *dir_queue)
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

            char *outputFile;
            outputFile = (char*)malloc(size * sizeof(char));

            //if name of file contains "wrap", rewrite the file
            if (strstr(dp->d_name, "wrap") != NULL){
                strcpy(outputFile, dp->d_name);
                //output_type = open(outputFile, O_RDWR, 0666);
            }
            //name of file does not contain "wrap", create and write to new file
            else{
                strcpy(outputFile, basePath);
                strcat(outputFile, "/wrap.");
                strcat(outputFile, dp->d_name);
                //output_type = open(outputFile, O_RDWR | O_CREAT, 0666);
            }

            fileEnqueue(filename, outputFile, file_queue);

            //for this block of code, instead of opeing, we add the path to the file queue
            /*int fp = open(filename, O_RDONLY);
            if (fp == -1){
                perror(filename);
                free(buffer);
                free(temp);
                return; //RETURN WITH ERROR
            }*/

            //word_wrap(fp, buffer, temp, columns, output_type);
            /*close(fp);
            close(output_type);
            free(filename);
            free(outputFile);*/


        }
        else if (dp->d_type == DT_DIR && (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)){
            //change basePath name to new subdirectory
            //recursively call function again
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            //printf("%s", path);

            //put arguments into a struct
            //PUT PATH IN DIRECTORY WORK QUEUE
            //make a thread and call dir_worker
            dirEnqueue(path, dir_queue);
            wrapFilesRecursively(path, buffer, temp, columns, file_queue, dir_queue);
        }
        
    }

    closedir(dir);
    return;

    //in the while loop, have if statements to see if its a file or another directory
        //if its a file, wrap and continue looking at the other stuff in the directory
        //if its a directory, call the function again recursively
}


//for threading directories
void *dir_worker(void *arg){
    //run wrapFilesRecursively using struct
    //dequeue the subdirectory once done with the thread- figure out how to dequeue once it is done
    struct dirFunc_args *tempB = arg;
    while(1){
        char *sub_to_remove;
        sub_to_remove = dirDequeue(tempB->dq);
        wrapFilesRecursively(sub_to_remove, tempB->buffer, tempB->temp, tempB->columns, tempB->fq, tempB->dq);
    }
    return NULL;
}

//for threading files
void *file_worker(void *arg){
    //run wordwrap function using struct
    //dequeue file path once done

    //GRAB A FILE PATH FROM QUEUE, CHECK THAT IT OPENS, AND WORD WRAP IT
    //I DONT THINK IT HAS TO BE THE EXACT ONE THAT WE JUST READ, IT SHOULD JUST BE ANY

    //extracts data to use in worker
    struct fileFunc_args *tempA = arg;

    while (1){
        int output_type;
        //dequeue, then do this stuff
        struct cell file_to_remove = fileDequeue(tempA->q); //gives filepath and output file
        
        //char *outputFile;
        //outputFile = (char*)malloc(size * sizeof(char));
        //strcpy(outputFile, tempA->argumentTwo);
        //if name of file contains "wrap", rewrite the file
        if (strstr(file_to_remove.file_path, "wrap") != NULL){
            //strcpy(outputFile, file_to_remove);
            output_type = open(file_to_remove.output_file_path, O_RDWR, 0666);
        }
                    //name of file does not contain "wrap", create and write to new file
        else{
            //strcpy(outputFile, argumentTwo);
            //strcat(outputFile, "/wrap.");
            //strcat(outputFile, tempA->basePath);
            output_type = open(file_to_remove.output_file_path, O_RDWR | O_CREAT, 0666);
        }

        int fp = open(file_to_remove.file_path, O_RDONLY);
        if (fp == -1){
            perror(file_to_remove.file_path); 
            free(tempA->buffer);
            free(tempA->temp);
            return NULL;
        }

        //call wordwrap
        word_wrap(fp, tempA->buffer, tempA->temp, tempA->columns, output_type);

        free(file_to_remove.file_path);
        free(file_to_remove.output_file_path);
        close(fp);
        close(output_type);

        //HAVE SOME SORT OF CHECK TO SEE WHEN TO EXIT THE LOOP
    }

        //DEQUEUE FROM QUEUE

    return NULL;


}
//for threading, the worker functions are the ones dequeueing- the reg main function or recursivewrap will enqueue when 
    //encountering a file/subdirectory


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
    struct fileQueue *file_queue;
    struct dirQueue *dir_queue;
    fileQueue_init(file_queue);
    dirQueue_init(dir_queue);



    
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
                    
                    struct fileFunc_args *data_args;
                    data_args->q = file_queue;
                    data_args->argumentTwo = filename;
                    data_args->basePath = file->d_name;
                    data_args->buffer = buffer;
                    data_args->temp = temp;
                    data_args->columns = columns;
                    data_args->output_type = output_type;
                    
                    char *outputFile;
                    outputFile = (char*)malloc(size * sizeof(char));
                    strcpy(outputFile, argumentTwo);

                    //if name of file contains "wrap", rewrite the file
                    if (strstr(file->d_name, "wrap") != NULL){
                        strcpy(outputFile, file->d_name);
                        //output_type = open(outputFile, O_RDWR, 0666);
                    }
                    //name of file does not contain "wrap", create and write to new file
                    else{
                        //strcpy(outputFile, argumentTwo);
                        strcat(outputFile, "/wrap.");
                        strcat(outputFile, file->d_name);
                        //output_type = open(outputFile, O_RDWR | O_CREAT, 0666);
                    }
                    
                    fileEnqueue(filename, outputFile, file_queue);
                    //put file path in file queue
                    //make thread for the file
                    if (file_tid_count < N){
                        pthread_create(&file_tid[file_tid_count], NULL, file_worker, &data_args);
                        file_tid_count++;
                    }

                    //maybe dont wrap here- call wrap in worker thread
                    //word_wrap(fp, buffer, temp, columns, output_type);
                    /*free(filename);
                    free(outputFile);
                    close(fp);
                    close(output_type);*/

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
                        //printf("%s\n", filename);
                        //char *basePath = file->d_name;  //ERROR- MUST BE TEST/SUB

                        dirEnqueue(filename, dir_queue);

                        struct dirFunc_args *dirData_args;
                        dirData_args->dq = dir_queue;
                        dirData_args->fq = file_queue;
                        dirData_args->basePath = filename;
                        dirData_args->buffer = buffer;
                        dirData_args->temp = temp;
                        dirData_args->columns = columns;


                        if (dir_tid_count < M){
                            pthread_create(&dir_tid[dir_tid_count], NULL, dir_worker, &dirData_args);
                            dir_tid_count++;
                        }

                        //here is where we start threading for subdirectories
                        //wrapFilesRecursively(argumentTwo, filename, buffer, temp, columns, output_type);
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

