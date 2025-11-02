#include "tokens.h"
#include <stdlib.h>
#include <stdio.h>

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

const int kw_count = 4;
const int noise_count = 4;
const int relop_count = 6;
const int boolop_count = 3;
const int delim_count = 8;

bool strEqual(const char* a, const char* b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return false;
        i++;
    }
    return a[i] == b[i];
}

bool isKeyword(const char* lexeme) {
    for (int i = 0; keywords[i] != NULL; i++)
        if (strEqual(lexeme, keywords[i])) return true;
    return false;
}

bool isNoiseWord(const char* lexeme) {
    for (int i = 0; noiseWords[i] != NULL; i++)
        if (strEqual(lexeme, noiseWords[i])) return true;
    return false;
}

bool isRelationalOp(const char* lexeme) {
    for (int i = 0; relationalOps[i] != NULL; i++)
        if (strEqual(lexeme, relationalOps[i])) return true;
    return false;
}

bool isLogicalOp(const char* lexeme) {
    for (int i = 0; logicalOps[i] != NULL; i++)
        if (strEqual(lexeme, logicalOps[i])) return true;
    return false;
}

bool isDelimiter(char ch) {
    return (ch == ',' || ch == ';' || ch == '(' || ch == ')' ||
            ch == '{' || ch == '}' || ch == ':' || ch == '"');
}

bool isAlphabet(char ch) {
    return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'));
}

bool isDigit(char ch) {
    return (ch >= '0' && ch <= '9');
}

bool isIdentifier(const char* lexeme) {
    if (!(lexeme[0] == '_' || isAlphabet(lexeme[0])))
        return false;

    if (isKeyword(lexeme))
        return false;

    for (int i = 1; lexeme[i] != '\0'; i++)
        if (!(isAlphabet(lexeme[i]) || isDigit(lexeme[i]) || lexeme[i] == '_'))
            return false;

    return true;
}

bool isNumber(const char* lexeme) {
    int i = 0;
    if (lexeme[0] == '\0') return false;
    while (lexeme[i] != '\0') {
        if (!isDigit(lexeme[i])) return false;
        i++;
    }
    return true;
}

bool isWhiteSpace(char ch) {
    return (ch == ' ' || ch == '\n' || ch == '\t');
}

const char* getTokenType(const char* lexeme) {
    if (isKeyword(lexeme)) return KEYWORD;
    if (isNoiseWord(lexeme)) return NOISE;
    if (isIdentifier(lexeme)) return IDENTIFIER;
    if (isNumber(lexeme)) return NUMBER;
    if (isRelationalOp(lexeme)) return REL_OP;
    if (isLogicalOp(lexeme)) return LOG_OP;
    return UNKNOWN;
}
