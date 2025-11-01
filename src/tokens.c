#include "tokens.h"
#include <stdlib.h>

const char* keywords[] = {
    "scene", "dialogue", "choice", "character", NULL
};

const char* noiseWords[] = {
    ":", "narrate", "action", "is", NULL
};

const char* relationalOps[] = {
    "==", "!=", ">", ">=", "<", "<=", NULL
};

const char* logicalOps[] = {
    "and", "or", "not", NULL
};

const char* delimiters[] = {
    ",", ":", "“","”", "‘", "’", "{", "}", NULL
};

const int kw_count = sizeof(keywords) / sizeof(keywords[0]);
const int noise_count = sizeof(noiseWords) / sizeof(noiseWords[0]);
const int relop_count = sizeof(relationalOps) / sizeof(relationalOps[0]);
const int boolop_count = sizeof(logicalOps) / sizeof(logicalOps[0]);
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
    
    for(int i = 0; keywords[i] != NULL; i++){
        if(strEqual(lexeme, keywords[i])){
            return true;
        }
    }

    return false;
}

bool isNoiseWord(const char* lexeme) {
    for(int i = 0; noiseWords[i] != NULL; i++){
        if(strEqual(lexeme, noiseWords[i])){
            return true;
        }
    }

    return false;
}

bool isRelationalOp(const char* lexeme) {
    for(int i = 0; relationalOps[i] != NULL; i++){
        if(strEqual(lexeme, relationalOps[i])){
            return true;
        }
    }

    return false;
}

bool isLogicalOp(const char* lexeme) {
    for(int i = 0; logicalOps[i] != NULL; i++){
        if(strEqual(lexeme, logicalOps[i])){
            return true;
        }
    }

    return false;
}

bool isDelimiter(const char* lexeme) {
    for(int i = 0; delimiters[i] != NULL; i++){
        if(strEqual(lexeme, delimiters[i])){
            return true;
        }
    }

    return false;
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
    if (isLogicalOp(lexeme)) return LOG_OP;
    if (isDelimiter(lexeme)) return DELIMITER;
    if (isComment(lexeme)) return COMMENT;
    return UNKNOWN;
}