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

// Token type specific for keywords
#define KEYWORD_CHARACTER   "KEYWORD_CHARACTER"
#define KEYWORD_SCENE       "KEYWORD_SCENE"
#define KEYWORD_DIALOGUE    "KEYWORD_DIALOGUE"
#define KEYWORD_CHOICE      "KEYWORD_CHOICE"
#define KEYWORD_OPTION      "KEYWORD_OPTION"
#define KEYWORD_IF          "KEYWORD_IF"
#define KEYWORD_THEN        "KEYWORD_THEN"
#define KEYWORD_ELSE        "KEYWORD_ELSE"
#define KEYWORD_REPEAT      "KEYWORD_REPEAT"
#define KEYWORD_FOR         "KEYWORD_FOR"
#define KEYWORD_IN          "KEYWORD_IN"
#define KEYWORD_SHOW        "KEYWORD_SHOW"
#define KEYWORD_END         "KEYWORD_END"
#define KEYWORD_TRUE        "KEYWORD_TRUE"
#define KEYWORD_FALSE       "KEYWORD_FALSE"
#define KEYWORD_BECOMES     "KEYWORD_BECOMES"
#define KEYWORD_EMOTION     "KEYWORD_EMOTION"
#define KEYWORD_ACTION      "KEYWORD_ACTION"
#define KEYWORD_SET         "KEYWORD_SET"
#define KEYWORD_AND         "KEYWORD_AND"
#define KEYWORD_OR          "KEYWORD_OR"
#define KEYWORD_NOT         "KEYWORD_NOT"
#define KEYWORD_IS          "KEYWORD_IS"
#define KEYWORD_WHEN        "KEYWORD_WHEN"

// Resource key token types
#define RES_KEY_SCENE           "RES_KEY_SCENE"
#define RES_KEY_DIALOGUE        "RES_KEY_DIALOGUE"
#define RES_KEY_CHOICE          "RES_KEY_CHOICE"
#define RES_KEY_CHARACTER       "RES_KEY_CHARACTER"

// Arithmetic operators
#define ARITHMETIC_PLUS         "ARITHMETIC_PLUS"
#define ARITHMETIC_MINUS        "ARITHMETIC_MINUS"
#define ARITHMETIC_MULTIPLY     "ARITHMETIC_MULTIPLY"
#define ARITHMETIC_DIVIDE       "ARITHMETIC_DIVIDE"
#define ARITHMETIC_MODULUS      "ARITHMETIC_MODULUS"
#define ARITHMETIC_EXPONENT     "ARITHMETIC_EXPONENT"

// Relational operators
#define RELATIONAL_EQUAL_EQUAL  "RELATIONAL_EQUAL_EQUAL"
#define RELATIONAL_NOT_EQUAL    "RELATIONAL_NOT_EQUAL"
#define RELATIONAL_GREATER      "RELATIONAL_GREATER"
#define RELATIONAL_GREATER_EQUAL "RELATIONAL_GREATER_EQUAL"
#define RELATIONAL_LESS         "RELATIONAL_LESS"
#define RELATIONAL_LESS_EQUAL   "RELATIONAL_LESS_EQUAL"

// Assignment operators
#define ASSIGNMENT_ASSIGN       "ASSIGNMENT_ASSIGN"
#define ASSIGNMENT_PLUS_ASSIGN  "ASSIGNMENT_PLUS_ASSIGN"
#define ASSIGNMENT_MINUS_ASSIGN "ASSIGNMENT_MINUS_ASSIGN"

// Logical operators
#define LOGICAL_BANG            "LOGICAL_BANG"
#define LOGICAL_AND             "LOGICAL_AND"
#define LOGICAL_OR              "LOGICAL_OR"

// Delimiters / punctuation
#define LPAREN                  "LPAREN"
#define RPAREN                  "RPAREN"
#define LBRACKET                "LBRACKET"
#define RBRACKET                "RBRACKET"
#define COLON                   "COLON"
#define COMMA                   "COMMA"
#define DOT                     "DOT"

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
extern const int kw_count;
extern const int noise_count;
extern const int relop_count;
extern const int boolop_count;
extern const int delim_count;


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