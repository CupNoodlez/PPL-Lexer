#include "tokens.h"


const char* keywords[] = {
    "test"
};

const char* noiseWords[] = {
    "test"
};

const char* relationalOps[] = {
    "test"
};

const char* booleanOps[] = {
    "test"
};

const char* delimiters[] = {
    "test"
};

const int kw_count = sizeof(keywords) / sizeof(keywords[0]);
const int noise_count = sizeof(noiseWords) / sizeof(noiseWords[0]);
const int relop_count = sizeof(relationalOps) / sizeof(relationalOps[0]);
const int boolop_count = sizeof(booleanOps) / sizeof(booleanOps[0]);
const int delim_count = sizeof(delimiters) / sizeof(delimiters[0]);

bool strEqual(const char* a, const char* b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return false;
        i++;
    }
    return a[i] == b[i];
}

bool isKeyword(const char* lexeme) {
    // implementation goes here
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

const char* getTokenType(const char* lexeme) {
    if (isKeyword(lexeme)) return KEYWORD;
    if (isNoiseWord(lexeme)) return NOISE;
    if (isIdentifier(lexeme)) return IDENTIFIER;
    if (isNumber(lexeme)) return NUMBER;
    if (isRelationalOp(lexeme)) return REL_OP;
    if (isBooleanOp(lexeme)) return BOOL_OP;
    if (isDelimiter(lexeme)) return DELIMITER;
    if (isComment(lexeme)) return COMMENT;
    return UNKNOWN;
}