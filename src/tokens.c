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

const char* isComment(const char* lexeme) {
    
    if (lexeme[0] == '/' && lexeme[1] == '/'){
        //next text seuence are identifier until space
    }
}

bool isAlphabet(const char ch){
    return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z' ));
}

bool isDigit(char ch){
  return (ch == '0' || ch == '1' || ch == '2' || ch == '3'||
          ch == '4' || ch == '5' || ch == '6' || ch == '7' ||
          ch == '8' || ch == '9');
}

bool isIdentifier(const char* lexeme) {
    
    if (!(lexeme[0] == '_' || isAlphabet(lexeme[0]))){
        return false;
    } 

    if(isKeyword(lexeme)){
        return false;
    }

    //check if consecutive characters are only either digit or alphabet
    int i = 1;
    while(lexeme[i] != '\0' && lexeme[i] != ' '){
        char ch = lexeme[i];
        if(!(isAlphabet(ch)) && !(isDigit(ch)) && ch != '_'){
            return false;
        }
        i++;
    }

    return true;
}


bool isNumber(const char* lexeme) {
    // implementation goes here
}

bool isWhiteSpace(const char* lexeme){
    return (lexeme == ' ' || lexeme == "\n" || lexeme == "\t");
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

// int main() {
//     char lexeme[50];

//     printf("Enter a word: ");
//     scanf("%s", lexeme);

//     if (isIdentifier(lexeme))
//         printf("Valid identifier!\n");
//     else
//         printf("Invalid identifier!\n");

//     return 0;
// }