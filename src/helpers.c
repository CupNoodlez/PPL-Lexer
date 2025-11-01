#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"
#include "helpers.h"

int read_file_ext(const char *filename){

     //finds the start of the file extension
     int dot_pos = -1;
     for (int i = 0; filename[i] != '\0'; ++i){
          if(filename[i] == '.'){
               dot_pos = i;
          }
     }

     if (dot_pos < 0) {
          return 0;
     }

     //checks if the file extension is in .st format
     const char *ext = filename + dot_pos + 1;
     if (ext[0] == 's' && ext[1] == 't' && ext[2] == '\0'){
          return 1;
     } else return 0;
}



/*void outputTokens(Token* tokens) {

     print in table format
}
 ...existing code... */