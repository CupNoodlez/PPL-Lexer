#ifndef HELPERS_H
#define HELPERS_H

#include <stdbool.h> 
#include <string.h>  

typedef struct {
    char type[26];        
    char lexeme[100];      
} Token;

typedef struct {
    int data[100];
    int topIndex;
} Stack;

void push(Stack *stack, int value);
int pop(Stack *stack);
int peek(Stack *stack);
bool isDelimiter(char c);
bool isOperator(char c);
bool isSeparator(char c);
int read_file_ext(const char *filename);
char *read_file(const char *filename, unsigned int *out_size);
void make_token(Token *tokens, int *tokenCount, const char *start, const char *end, const char *type); 
void outputTokens(Token* tokens);
void bufferChar(char *buffer, int *pos, char ch);

#endif // HELPERS_H