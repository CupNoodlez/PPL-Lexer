#ifndef TOKENS_H
#define TOKENS_H

#include <stdbool.h>   

// Token type constants
#define KEYWORD     "KEYWORD"
#define NOISE       "NOISE"
#define IDENTIFIER  "IDENTIFIER"
#define NUMBER      "NUMBER"
#define REL_OP      "REL_OP"
#define LOG_OP     "LOG_OP"
#define DELIMITER   "DELIMITER"
#define COMMENT     "COMMENT"
#define UNKNOWN     "UNKNOWN"
#define ADD_OP      "ADD_OP"
#define SUB_OP      "SUB_OP"
#define MUL_OP      "MUL_OP"
#define DIV_OP      "DIV_OP"
#define EXP_OP      "EXP_OP"
#define MOD_OP      "MOD_OP"
#define EQUAL       "EQUAL_SIGN"

typedef struct {
    char type[20];        
    char lexeme[100];      
} Token;

// -----------------------------
// Token Definitions
// -----------------------------
extern const char* keywords[];
extern const char* noiseWords[];
extern const char* relationalOps[];
extern const char* logicalOps[];   
extern const char* delimiters[];
extern const int kw_count;
extern const int noise_count;
extern const int relop_count;
extern const int boolop_count;
extern const int delim_count;

// -----------------------------
// Token Identifiers (signatures)
// -----------------------------
bool isNumber(const char* lexeme);
bool isAlphabet(char lexeme);
bool isWhiteSpace(char lexeme);
bool isDelimiter(char lexeme);

bool isKeyword(const char* lexeme);
bool isNoiseWord(const char* lexeme);
bool isRelationalOp(const char* lexeme);
bool isLogicalOp(const char* lexeme);   
bool isIdentifier(const char* lexeme);

const char* getTokenType(const char* lexeme);

#endif // TOKENS_H
