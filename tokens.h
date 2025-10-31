#ifndef TOKENS_H
#define TOKENS_H

#include <stdbool.h>   

typedef enum {
    TOKEN_KEYWORD,
    TOKEN_NOISE,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_REL_OP,
    TOKEN_BOOL_OP,
    TOKEN_DELIMITER,
    TOKEN_COMMENT,
    TOKEN_UNKNOWN
} TokenType;


typedef struct {
    TokenType type;        
    char lexeme[100];      
    int line;              
} Token;


// -----------------------------
// Token Identifiers
// -----------------------------
bool isKeyword(const char* lexeme);
bool isNoiseWord(const char* lexeme);
bool isRelationalOp(const char* lexeme);
bool isBooleanOp(const char* lexeme);
bool isDelimiter(const char* lexeme);
bool isComment(const char* lexeme);
bool isIdentifier(const char* lexeme);
bool isNumber(const char* lexeme);
TokenType getTokenType(const char* lexeme);


// -----------------------------
// Token Definitions
// -----------------------------
static const char* keywords[] = {
    "test"
};

static const char* noiseWords[] = {
    "test"
};

static const char* relationalOps[] = {
    "test"
};

static const char* booleanOps[] = {
    "test"
};

static const char* delimiters[] = {
    "test"
};

#endif // TOKENS_H