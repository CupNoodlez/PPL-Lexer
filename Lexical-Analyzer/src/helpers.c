#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "helpers.h"

bool isDelimiter(char c) {
    return c == ',' || c == ':' || c == '.' || c == '(' ||
           c == ')' || c == '[' || c == ']';
}

bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '=' || c == '!' || c == '<' || c == '>';
}

bool isSeparator(char c) {
    return isspace(c) || isDelimiter(c) || isOperator(c);
}

void push(Stack *stack, int value) {
    if (stack->topIndex < 99) {
        stack->data[++(stack->topIndex)] = value;
    }
}
int pop(Stack *stack) {
    if (stack->topIndex >= 0) {
        return stack->data[(stack->topIndex)--];
    }
    return -1;
}
int peek(Stack *stack) {
    if (stack->topIndex >= 0) {
        return stack->data[stack->topIndex];
    }
    return -1;
}

int read_file_ext(const char *filename){

    //finds the start of the file extension
    int dot_pos = -1;
    for (int i = 0; filename[i] != '\0'; ++i)
        if(filename[i] == '.')
            dot_pos = i;

    if (dot_pos < 0) return 0;  

    //checks if the file extension is in .st format
    const char *ext = filename + dot_pos + 1;
    if (ext[0] == 's' && ext[1] == 't' && ext[2] == '\0')
        return 1;
    return 0;
}

char *read_file(const char *filename, unsigned int *out_size) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char *buffer = malloc(size + 1);
    if (!buffer) return NULL;

    fread(buffer, 1, size, fp);
    buffer[size] = '\0';   // null-terminate for string logic
    fclose(fp);

    if (out_size) *out_size = size;
    return buffer;
}

void make_token(Token *tokens, int *tokenCount,
                const char *start, const char *end,
                const char *type)
{
    int length = end - start;

    // copy lexeme substring
    memcpy(tokens[*tokenCount].lexeme, start, length);
    tokens[*tokenCount].lexeme[length] = '\0';

    // remove newlines for pretty print
    for (int i = 0; tokens[*tokenCount].lexeme[i] != '\0'; ++i)
        if (tokens[*tokenCount].lexeme[i] == '\n') 
            tokens[*tokenCount].lexeme[i] = ' ';

    // copy token type
    strcpy(tokens[*tokenCount].type, type);

    (*tokenCount)++;
}


void outputTokens(Token* tokens) {
    FILE *outputFile = fopen("SymbolTable.txt", "w");
    if (!outputFile) {
        printf("Error: Could not create output file.\n");
        return;
    }
    FILE *outputTokenFile = fopen("../Tokens.txt", "w");
    if (!outputTokenFile) {
        printf("Error: Could not create output token file.\n");
        return;
    }

    // File table header
    fprintf(outputFile, "TOKEN TABLE\n");
    fprintf(outputFile, "===========\n\n");
    fprintf(outputFile, "%-4s | %-26s | %-45s\n", "No.", "Type", "Lexeme");
    fprintf(outputFile, "-----+-------------------------+-----------------------------------------------\n");


    // Count tokens and print them
    int count = 0;
    while (tokens[count].type[0] != '\0') {
        count++;
        fprintf(outputFile, "%-4d | %-26s | %-45s\n", count, tokens[count-1].type, tokens[count-1].lexeme);
        fprintf(outputTokenFile, "%s\n", tokens[count-1].type);
    }

    printf("\nTotal tokens: %d\n", count);
    printf("Output saved to: Lexical-Analyzer/SymbolTable.txt\n\n");
    printf("Token types saved to: Tokens.txt\n");

    // Print to file
    fprintf(outputFile, "\nTotal tokens: %d\n", count);

    fclose(outputFile);
}