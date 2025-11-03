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


void outputTokens(Token* tokens) {
    FILE *outputFile = fopen("tokens_output.txt", "w");
    if (!outputFile) {
        printf("Error: Could not create output file.\n");
        return;
    }
    // Console table header
    printf("\n╔═════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                     TOKEN TABLE                                 ║\n");
    printf("╠═════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ %-4s │ %-24s │ %-45s ║\n", "No.", "Type", "Lexeme");
    printf("╠═════════════════════════════════════════════════════════════════════════════════╣\n");

    // File table header
    fprintf(outputFile, "TOKEN TABLE\n");
    fprintf(outputFile, "===========\n\n");
    fprintf(outputFile, "%-4s | %-24s | %-45s\n", "No.", "Type", "Lexeme");
    fprintf(outputFile, "-----+-------------------------+-----------------------------------------------\n");


    // Count tokens and print them
    int count = 0;
    while (tokens[count].type[0] != '\0') {
        count++;
        
        // Print each data
        printf("║ %-4d │ %-24s │ %-45s ║\n", count, tokens[count-1].type, tokens[count-1].lexeme);
        fprintf(outputFile, "%-4d | %-24s | %-45s\n", count, tokens[count-1].type, tokens[count-1].lexeme);
    }

    printf("╚═════════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\nTotal tokens: %d\n", count);
    printf("Output saved to: tokens_output.txt\n\n");

    // Print to file
    fprintf(outputFile, "\nTotal tokens: %d\n", count);

    fclose(outputFile);
}

// Copy src to dest, including the null terminator
void str_copy(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}