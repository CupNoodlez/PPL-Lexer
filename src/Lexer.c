#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"
#include "helpers.h"


int main() { 

    //user inputs filename
    char filename[256];  
    printf("Enter filename: ");
    scanf("%255s", filename);  

    //calls read_file_ext function and prints error if invalid extension
    int isSt = read_file_ext(filename);
    if (isSt == 0){
        printf("Please enter a valid .st file format.\n");
        return 1;
    }

    //opens file and prints error if no file is detected
    FILE *fp = fopen(filename, "r");
    int isValid = 1;
    if (!fp) {
        isValid = 0;
        printf("Error opening file.\n");
        return 1;
    }

    //checks for valid extension and if file is found
    if ((isValid && isSt) == 1) {
        printf("File opened successfully");
    }

    Token tokens[1000], buffer;

    char ch;
    while ((ch = fgetc(fp)) != EOF) {      
        // forward scan
        // if delimiter/whitespace (DW), scan backward until DW to form lexeme, then skip to the next 
        // store lexeme in buffer.lexeme

        // tokens[i].lexeme = buffer.lexeme;
        // tokens[i].type = getTokenType(buffer.lexeme);
    }


    // outputTokens(tokens);

    fclose(fp);
    return 0;
}