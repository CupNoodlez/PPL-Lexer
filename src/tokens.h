#ifndef TOKENS_H
#define TOKENS_H

#include <stdbool.h>   

// Token type constants
#define KEYWORD             "KEYWORD"
#define NOISE               "NOISE"
#define IDENTIFIER          "IDENTIFIER"
#define INTEGER_LITERAL     "INTEGER_LITERAL"
#define FLOAT_LITERAL       "FLOAT_LITERAL"
#define REL_OP              "REL_OP"
#define LOG_OP              "LOG_OP"
#define ARITH_OP            "ARITH_OP"
#define DELIMITER           "DELIMITER"
#define COMMENT             "COMMENT"
#define STRING_LITERAL      "STRING_LITERAL"
#define INVALID             "INVALID"

// Arithmetic operators
#define ARITHMETIC_PLUS      "ARITHMETIC_PLUS"
#define ARITHMETIC_MINUS     "ARITHMETIC_MINUS"
#define ARITHMETIC_MULTIPLY  "ARITHMETIC_MULTIPLY"
#define ARITHMETIC_DIVIDE    "ARITHMETIC_DIVIDE"
#define ARITHMETIC_MODULUS   "ARITHMETIC_MODULUS"
#define ARITHMETIC_EXPONENT  "ARITHMETIC_EXPONENT"

// Relational operators
#define RELATIONAL_EQUAL_EQUAL    "RELATIONAL_EQUAL_EQUAL"
#define RELATIONAL_NOT_EQUAL      "RELATIONAL_NOT_EQUAL"
#define RELATIONAL_GREATER        "RELATIONAL_GREATER"
#define RELATIONAL_GREATER_EQUAL  "RELATIONAL_GREATER_EQUAL"
#define RELATIONAL_LESS           "RELATIONAL_LESS"
#define RELATIONAL_LESS_EQUAL     "RELATIONAL_LESS_EQUAL"

// Assignment operators
#define ASSIGNMENT_ASSIGN        "ASSIGNMENT_ASSIGN"
#define ASSIGNMENT_PLUS_ASSIGN   "ASSIGNMENT_PLUS_ASSIGN"
#define ASSIGNMENT_MINUS_ASSIGN  "ASSIGNMENT_MINUS_ASSIGN"

// Delimiters
#define LPAREN      "LPAREN"
#define RPAREN      "RPAREN"
#define LBRACKET    "LBRACKET"
#define RBRACKET    "RBRACKET"
#define COLON       "COLON"
#define COMMA       "COMMA"
#define DOT         "DOT"

// Specific keyword tokens
#define KEYWORD_CHARACTER "KEYWORD_CHARACTER"
#define KEYWORD_SCENE     "KEYWORD_SCENE"
#define KEYWORD_DIALOGUE  "KEYWORD_DIALOGUE"
#define KEYWORD_CHOICE    "KEYWORD_CHOICE"
#define KEYWORD_OPTION    "KEYWORD_OPTION"
#define KEYWORD_IF        "KEYWORD_IF"
#define KEYWORD_THEN      "KEYWORD_THEN"
#define KEYWORD_ELSE      "KEYWORD_ELSE"
#define KEYWORD_ELIF      "KEYWORD_ELIF"
#define KEYWORD_REPEAT    "KEYWORD_REPEAT"
#define KEYWORD_FOR       "KEYWORD_FOR"
#define KEYWORD_DO        "KEYWORD_DO"
#define KEYWORD_WHILE     "KEYWORD_WHILE"
#define KEYWORD_DO_WHILE  "KEYWORD_DO-WHILE"
#define KEYWORD_JUMP      "KEYWORD_JUMP"
#define KEYWORD_STOP      "KEYWORD_STOP"
#define KEYWORD_CONTINUE  "KEYWORD_CONTINUE"
#define KEYWORD_IN        "KEYWORD_IN"
#define KEYWORD_SHOW      "KEYWORD_SHOW"
#define KEYWORD_END       "KEYWORD_END"
#define KEYWORD_TRUE      "KEYWORD_TRUE"
#define KEYWORD_FALSE     "KEYWORD_FALSE"
#define KEYWORD_BECOMES   "KEYWORD_BECOMES"
#define KEYWORD_EMOTION   "KEYWORD_EMOTION"
#define KEYWORD_ACTION    "KEYWORD_ACTION"
#define KEYWORD_SET       "KEYWORD_SET"
#define KEYWORD_AND       "KEYWORD_AND"
#define KEYWORD_OR        "KEYWORD_OR"
#define KEYWORD_NOT       "KEYWORD_NOT"
#define KEYWORD_IS        "KEYWORD_IS"
#define KEYWORD_WHEN      "KEYWORD_WHEN"

// Reserved keywords
#define RES_KEY_NULL      "RES_KEY_NULL"
#define RES_KEY_RETURN    "RES_KEY_RETURN"
#define RES_KEY_UNTIL     "RES_KEY_UNTIL"
#define RES_KEY_TIMES     "RES_KEY_TIMES"
#define RES_KEY_ERROR     "RES_KEY_ERROR"
#define RES_KEY_EXCEPT    "RES_KEY_EXCEPT"
#define RES_KEY_FIXED     "RES_KEY_FIXED"
#define RES_KEY_BREAK     "RES_KEY_BREAK"
#define RES_KEY_NONE      "RES_KEY_NONE"
#define RES_KEY_WITH      "RES_KEY_WITH"

// Noise word tokens
#define NOISE_THE         "NOISE_THE"
#define NOISE_A           "NOISE_A"
#define NOISE_AN          "NOISE_AN"
#define NOISE_AS          "NOISE_AS"
#define NOISE_THEN        "NOISE_THEN"
#define NOISE_OF          "NOISE_OF"
#define NOISE_TO          "NOISE_TO"

typedef struct {
    char type[24];        
    char lexeme[100];      
} Token;


// -----------------------------
// Token Identifiers
// -----------------------------
const char* getKeyword(const char* lexeme);
const char* getReservedKeyword(const char* lexeme);
const char* getArithmeticOp(const char* lexeme);
const char* getRelationalOp(const char* lexeme);
const char* getAssignmentOp(const char* lexeme);
const char* getDelimiter(const char* lexeme);
bool isNoiseWord(const char* lexeme);
bool isComment(const char* lexeme);
bool isStringLiteral(const char* lexeme);
bool isIdentifier(const char* lexeme);
bool isNumber(const char* lexeme);
bool isIntegerLiteral(const char* lexeme);
bool isFloatLiteral(const char* lexeme);
bool isWhitespace(char c);
bool isDelimiter(char c);
bool isOperator(char c);
bool isAlpha(char c);
bool isDigit(char c);

const char* getTokenType(const char* lexeme);

#endif // TOKENS_H