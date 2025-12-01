#ifndef TOKEN_READER_H
#define TOKEN_READER_H

#include <stdbool.h>

// Token structure (Define once, use everywhere)
typedef struct {
    char type[26];
    char lexeme[100];
    int line;
} Token;

// Scanner state structure (Holds the token stream data)
typedef struct {
    Token* tokens;
    int tokenCount;
    int currentIndex;
} Scanner;


// FROM SCANNER(LEXER) TO PARSER(SYNTAXD)

// Initialization/Reading of tokens in the lexer
Scanner* scanner_init(const char* filename);

// Deallocation
void scanner_cleanup(Scanner* s);

// Read the current token (read lookahead)
Token scanner_getCurrentToken(Scanner* s);

// Consume (move the position)
void scanner_advance(Scanner* s);

// Test if the current token type matches the type
bool scanner_check(Scanner* s, const char* type);

// Test and consume (scanner_check then scanner_advance)
bool scanner_match(Scanner* s, const char* type);

// EOF
bool scanner_isAtEnd(Scanner* s);

#endif // TOKEN_READER_H