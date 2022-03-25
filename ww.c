#include <stdio.h>
#include <stdlib.h>
#include <string.h>

inline int word_length(const char *str){
    int getIndex = 0;
    while(str[getIndex]! = 0 && str[getIndex] ! = '' && str[getIndex]! = '\n'){
        getIndex++;
    }
    return(getIndex);
}

void word_wrap(char *s, const int newline){
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

int main(int argc, char** argv) {
    if (argc == 3){
        //argv[1] = columnNum and argv[2] = filename/directoryname
        
        //if argv[2] is directory
        if (argv[2][0] == "."){
            //dont wrap
        }
        if (strstr(argv[2], "wrap") != NULL){
            //dont wrap
        }
    }
    else if (argc == 2){
        //argv[1] = columnNum and do standard input way
    }

    //buffer array
    int columns = argv[1];
    char* buffer;
    buffer = (char*)malloc(columns*sizeof(char));


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
