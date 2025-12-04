#include <stdbool.h>


typedef struct {
    int data[100];
    int topIndex;
} Stack;

void push(Stack *stack, int value);
int pop(Stack *stack);
int peek(Stack *stack);

typedef struct {
    char  token_name[26];        
    char lexeme[100];      
    int lineNumber;
} Token;

bool isDelimiter(char c);
bool isOperator(char c);
bool isSeparator(char c);


char *read_file(const char *filename, unsigned int *out_size);
void emitToken(Token *tokens, int *token_num, const char *start, const char *end, int lineNumber, const char *type); 
Token* lex_all(char *filename, int* token_num);