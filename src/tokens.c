#include "tokens.h"

const char* keywords[] = {
    "character", "scene", "dialogue", "choice", "option", "if", 
    "then", "else", "repeat", "for", "in", "show", "end", "becomes", 
    "emotion", "action", "set"
};

const char* noiseWords[] = {
    "narrate", "action", "is"
};

const char* relationalOps[] = {
    "==", "!=", ">", ">=", "<", "<="
};

const char* logicalOps[] = {
    "and", "or", "not"
};

const char* arithmeticOps[] = {
    "+", "-", "*", "/", "%"
};

const char* delimiters[] = {
    ",", ":","\"", "\'", ".", "(", ")", "[", "]"
};

const char* separators[] = {
    ",", ":", ";", ".", "!", "?", "+", "-", "*", "/", "%", 
    "(", ")", "[", "]"
};

// Character sets for identifier and number validation
const char digits[] = "0123456789";
const char lowercase[] = "abcdefghijklmnopqrstuvwxyz";
const char uppercase[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

const int kw_count = sizeof(keywords) / sizeof(keywords[0]);
const int noise_count = sizeof(noiseWords) / sizeof(noiseWords[0]);
const int relop_count = sizeof(relationalOps) / sizeof(relationalOps[0]);
const int boolop_count = sizeof(logicalOps) / sizeof(logicalOps[0]);
const int arithop_count = sizeof(arithmeticOps) / sizeof(arithmeticOps[0]);
const int delim_count = sizeof(delimiters) / sizeof(delimiters[0]);
const int sep_count = sizeof(separators) / sizeof(separators[0]);

bool strEqual(const char* a, const char* b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return false;
        i++;
    }
    return a[i] == b[i];
}

bool isInSet(char c, const char* set) {
    for (int i = 0; set[i] != '\0'; i++) {
        if (c == set[i]) return true;
    }
    return false;
}

bool isKeyword(const char* lexeme) {
    
    for(int i = 0; i < kw_count; i++){
        if(strEqual(lexeme, keywords[i])){
            return true;
        }
    }

    return false;
}

bool isNoiseWord(const char* lexeme) {
    for(int i = 0; i < noise_count; i++){
        if(strEqual(lexeme, noiseWords[i])){
            return true;
        }
    }

    return false;
}

bool isRelationalOp(const char* lexeme) {
    for(int i = 0; i < relop_count; i++){
        if(strEqual(lexeme, relationalOps[i])){
            return true;
        }
    }

    return false;
}

bool isLogicalOp(const char* lexeme) {
    for(int i = 0; i < boolop_count; i++){
        if(strEqual(lexeme, logicalOps[i])){
            return true;
        }
    }

    return false;
}

bool isArithmeticOp(const char* lexeme) {
    for(int i = 0; i < arithop_count; i++){
        if(strEqual(lexeme, arithmeticOps[i])){
            return true;
        }
    }

    return false;
}

bool isDelimiter(const char* lexeme) {
    for(int i = 0; i < delim_count; i++){
        if(strEqual(lexeme, delimiters[i])){
            return true;
        }
    }

    return false;
}

bool isComment(const char* lexeme) {
    if (lexeme[0] == '\0') 
        return false;
    return lexeme[0] == '#';
}

bool isStringLiteral(const char* lexeme) {
    int len = 0;
    while (lexeme[len] != '\0') len++;
    return (len >= 2 &&
            ((lexeme[0] == '\"' && lexeme[len-1] == '\"') ||
             (lexeme[0] == '\'' && lexeme[len-1] == '\'')));
}

bool isIdentifier(const char* lexeme) {
    if (lexeme[0] == '\0') return false;
    
    // First character must be a letter
    if (!isInSet(lexeme[0], lowercase) && !isInSet(lexeme[0], uppercase)) {
        return false;
    }
    
    // Remaining characters must be letters, digits, or underscores
    for (int i = 1; lexeme[i] != '\0'; i++) {
        if (!isInSet(lexeme[i], lowercase) && 
            !isInSet(lexeme[i], uppercase) && 
            !isInSet(lexeme[i], digits) && 
            lexeme[i] != '_') {
            return false;
        }
    }
    
    return true;
}

bool isNumber(const char* lexeme) {
    if (lexeme[0] == '\0') return false;
    
    // All characters must be digits
    for (int i = 0; lexeme[i] != '\0'; i++) {
        if (!isInSet(lexeme[i], digits)) {
            return false;
        }
    }
    
    return true;
}

bool isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool isSeparator(char c) {
    char str[2] = {c, '\0'};
    
    for (int i = 0; i < sep_count; i++) {
        if (strEqual(str, separators[i])) {
            return true;
        }
    }
    
    return false;
}

const char* getTokenType(const char* lexeme) {
    if (isKeyword(lexeme)) return KEYWORD;
    if (isNoiseWord(lexeme)) return NOISE;
    if (isComment(lexeme)) return COMMENT;
    if (isStringLiteral(lexeme)) return STRING_LITERAL;
    if (isIdentifier(lexeme)) return IDENTIFIER;
    if (isNumber(lexeme)) return NUMBER;
    if (isRelationalOp(lexeme)) return REL_OP;
    if (isLogicalOp(lexeme)) return LOG_OP;
    if (isArithmeticOp(lexeme)) return ARITH_OP;
    if (isDelimiter(lexeme)) return DELIMITER;
    return UNKNOWN;
}