#ifndef TOKENS_H
#define TOKENS_H

#include <stdbool.h>   

// Token type constants
#define KEYWORD     "KEYWORD"
#define NOISE       "NOISE"
#define IDENTIFIER  "IDENTIFIER"
#define NUMBER      "NUMBER"
#define REL_OP      "REL_OP"
#define BOOL_OP     "BOOL_OP"
#define DELIMITER   "DELIMITER"
#define COMMENT     "COMMENT"
#define UNKNOWN     "UNKNOWN"

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
extern const char* booleanOps[];
extern const char* delimiters[];
extern const int keywords_count;
extern const int noiseWords_count;
extern const int relationalOps_count;
extern const int booleanOps_count;
extern const int delimiters_count;


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
const char* getTokenType(const char* lexeme);

#endif // TOKENS_H