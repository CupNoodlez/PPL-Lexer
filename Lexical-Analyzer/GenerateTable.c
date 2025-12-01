#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "LexicalG2PPL.h"

void outputTokens(Token* tokens);

int main() {
    char filename[256];

    // user inputs filename
    printf("Enter filename: ");
    scanf("%255s", filename);  

    // check file extension
    int len = strlen(filename);
    if (len < 3 || strcmp(filename + len - 3, ".st") != 0) {
        printf("Please enter a valid .st file format.\n");
        return 1;
    }
    
    // get tokens
    Token* tokens;
    int token_count;
    if (!(tokens = lex_all(filename, &token_count)))
        return 1; 

    outputTokens(tokens);
    free(tokens);
    return 0;
}
void outputTokens(Token* tokens) {
    FILE *outputFile = fopen("SymbolTable.txt", "w");
    if (!outputFile) {
        printf("Error: Could not create output file.\n");
        return;
    }

    int count = 0;
    while (tokens[count].type[0] != '\0') {
        count++;
        fprintf(outputFile, "%s|%d|%s\n", tokens[count-1].type, tokens[count-1].lineNumber, tokens[count-1].lexeme);
    }

    printf("\nTotal tokens: %d\n", count);
    printf("Output saved to: SymbolTable.txt\n\n");

    // Print to file
    fprintf(outputFile, "\nTotal tokens: %d\n", count);
    fclose(outputFile);
}