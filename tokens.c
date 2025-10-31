#include "tokens.h"

bool strEqual(const char* a, const char* b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return false;
        i++;
    }
    return a[i] == b[i];
}

bool isNoiseWord(const char* lexeme) {
    // implementation goes here
}

bool isRelationalOp(const char* lexeme) {
    // implementation goes here
}

bool isBooleanOp(const char* lexeme) {
    // implementation goes here
}

bool isDelimiter(const char* lexeme) {
    // implementation goes here
}

bool isComment(const char* lexeme) {
    // implementation goes here
}

bool isIdentifier(const char* lexeme) {
    // implementation goes here
}

bool isNumber(const char* lexeme) {
    // implementation goes here
}

TokenType getTokenType(const char* lexeme) {
    if (isKeyword(lexeme)) return TOKEN_KEYWORD;
    if (isNoiseWord(lexeme)) return TOKEN_NOISE;
    if (isIdentifier(lexeme)) return TOKEN_IDENTIFIER;
    if (isNumber(lexeme)) return TOKEN_NUMBER;
    if (isRelationalOp(lexeme)) return TOKEN_REL_OP;
    if (isBooleanOp(lexeme)) return TOKEN_BOOL_OP;
    if (isDelimiter(lexeme)) return TOKEN_DELIMITER;
    if (isComment(lexeme)) return TOKEN_COMMENT;
    return TOKEN_UNKNOWN;
}