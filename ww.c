#include <stdio.h>
#include <stdlib.h>

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

    

}

/* PSEUDOCODE
use char buffer array (put it as second argument in read())
- read(filename, bufferarray, NULL/sizeofread)
- bufferarray length will be length of desired columns
- once bufferarray is full AND/OR word cannot fit in bufferarray, start a new line
