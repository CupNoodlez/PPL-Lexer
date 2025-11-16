#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "helpers.h"

int main() {
    char filename[256];

    // user inputs filename
    printf("Enter filename: ");
    scanf("%255s", filename);  
    if (!read_file_ext(filename)) {
        printf("Please enter a valid .st file format.\n");
        return 1;
    }
    unsigned int size;
    char *inputBuffer = read_file(filename, &size);
    if (!inputBuffer) {
        printf("Error reading file.\n");
        return 1;
    }
    printf("File opened successfully!\n");

    Token tokens[1000];
    int tokenCount = 0;
    char* cursor = inputBuffer;
    char* tokenIndex;

    while (*cursor) {
        /********************[START STATE]********************/
        tokenIndex = cursor;
        char ch = *cursor;
        if (isspace(ch)) goto BLANK;
        if (isdigit(ch)) goto INTEGER;
        if (isalpha(ch)) goto IDENTIFIER;

        switch (ch) {
            case '.': goto DOT;
            case ',': goto COMMA;
            case ':': goto COLON;
            case '(': goto LPAREN;
            case ')': goto RPAREN;
            case '[': goto LBRACKET;
            case ']': goto RBRACKET;
            case '=': goto ASSIGNMENT_ASSIGN;
            case '+': goto ARITHMETIC_PLUS;
            case '-': goto ARITHMETIC_MINUS;
            case '*': goto ARITHMETIC_MULTIPLY;
            case '/': goto ARITHMETIC_DIVIDE;
            case '%': goto ARITHMETIC_MODULUS;
            case '^': goto ARITHMETIC_EXPONENT;
            case '>': goto RELATIONAL_GREATER;
            case '<': goto RELATIONAL_LESS;
            case '!': goto RELATIONAL_NOT;
            case '\'': goto CHAR;
            case '\"': goto STRING;
            case '#': goto COMMENT;
            default: goto INVALID;
        }
        /********************[WHITESPACE]********************/
        BLANK: { 
            ch = *++cursor;
            if (isspace(ch)) goto BLANK; 
            else continue; 
        }
        /********************[DELIMITERS]********************/
        LPAREN: { make_token(tokens, &tokenCount, tokenIndex, ++cursor, "LPAREN"); continue; }
        RPAREN: { make_token(tokens, &tokenCount, tokenIndex, ++cursor, "RPAREN"); continue; }
        LBRACKET: { make_token(tokens, &tokenCount, tokenIndex, ++cursor, "LBRACKET"); continue; }
        RBRACKET: { make_token(tokens, &tokenCount, tokenIndex, ++cursor, "RBRACKET"); continue; }
        COLON: { make_token(tokens, &tokenCount, tokenIndex, ++cursor, "COLON"); continue; }
        COMMA: { make_token(tokens, &tokenCount, tokenIndex, ++cursor, "COMMA"); continue; }
        DOT: { make_token(tokens, &tokenCount, tokenIndex, ++cursor, "DOT"); continue; }
        /********************[OPERATORS]********************/
        ARITHMETIC_PLUS: { 
            ch = *++cursor;
            if (ch == '=') goto ASSIGNMENT_PLUS_ASSIGN;
            if (isSeparator(ch)) {  
                make_token(tokens, &tokenCount, tokenIndex, cursor, "ARITHMETIC_PLUS");
                continue;
            }
            else goto INVALID;
        }
        ARITHMETIC_MINUS: { 
            ch = *++cursor;
            if (ch == '=') goto ASSIGNMENT_MINUS_ASSIGN;
            if (isSeparator(ch)) {  
                make_token(tokens, &tokenCount, tokenIndex, cursor, "ARITHMETIC_MINUS");
                continue;
            }
            else goto INVALID;
        }
        ARITHMETIC_MULTIPLY: { 
            ch = *++cursor;
            if (ch == '=') goto ASSIGNMENT_MULT_ASSIGN;
            if (isSeparator(ch)) {  
                make_token(tokens, &tokenCount, tokenIndex, cursor, "ARITHMETIC_MULTIPLY");
                continue;
            }
            else goto INVALID;
        }
        ARITHMETIC_DIVIDE: { 
            ch = *++cursor;
            if (ch == '/') goto ARITHMETIC_FLOOR_DIVIDE;
            if (ch == '=') goto ASSIGNMENT_DIV_ASSIGN;
            if (isSeparator(ch)) {  
                make_token(tokens, &tokenCount, tokenIndex, cursor, "ARITHMETIC_DIVIDE");
                continue;
            }
            else goto INVALID;
        }
        ARITHMETIC_MODULUS: { 
            ch = *++cursor;
            if (ch == '=') goto ASSIGNMENT_MOD_ASSIGN;
            if (isSeparator(ch)) {  
                make_token(tokens, &tokenCount, tokenIndex, cursor, "ARITHMETIC_MODULUS");
                continue;
            }
            else goto INVALID;
        }
        ASSIGNMENT_ASSIGN: {
            ch = *++cursor;
            if (ch == '=') goto RELATIONAL_EQUAL_EQUAL;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "ASSIGNMENT_ASSIGN");
                continue;
            }
            else goto INVALID;
        }
        ARITHMETIC_FLOOR_DIVIDE: { 
            make_token(tokens, &tokenCount, tokenIndex, ++cursor, "ARITHMETIC_FLOOR_DIVIDE");
            continue;
        }
        ARITHMETIC_EXPONENT: { 
            make_token(tokens, &tokenCount, tokenIndex, ++cursor, "ARITHMETIC_EXPONENT");
            continue;
        }

        ASSIGNMENT_PLUS_ASSIGN: {
            make_token(tokens, &tokenCount, tokenIndex, ++cursor, "ASSIGNMENT_PLUS_ASSIGN");
            continue;
        }
        ASSIGNMENT_MINUS_ASSIGN: {
            make_token(tokens, &tokenCount, tokenIndex, ++cursor, "ASSIGNMENT_MINUS_ASSIGN");
            continue;
        }
        ASSIGNMENT_MULT_ASSIGN: {
            make_token(tokens, &tokenCount, tokenIndex, ++cursor, "ASSIGNMENT_MULT_ASSIGN");
            continue;
        }
        ASSIGNMENT_DIV_ASSIGN: {
            make_token(tokens, &tokenCount, tokenIndex, ++cursor, "ASSIGNMENT_DIV_ASSIGN");
            continue;
        }
        ASSIGNMENT_MOD_ASSIGN: {
            make_token(tokens, &tokenCount, tokenIndex, ++cursor, "ASSIGNMENT_MOD_ASSIGN");
            continue;
        }
        RELATIONAL_LESS: {
            ch = *++cursor;
            if (ch == '=') goto RELATIONAL_LESS_EQUAL;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "RELATIONAL_LESS");
                continue;
            }
            else goto INVALID;
        }
        RELATIONAL_GREATER: {
            ch = *++cursor;
            if (ch == '=') goto RELATIONAL_GREATER_EQUAL;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "RELATIONAL_GREATER");
                continue;
            }
            else goto INVALID;
        }
        RELATIONAL_NOT: {
            ch = *++cursor;
            if (ch == '=') goto RELATIONAL_NOT_EQUAL;
            else goto INVALID;
        }
        RELATIONAL_NOT_EQUAL: {
            make_token(tokens, &tokenCount, tokenIndex, ++cursor, "RELATIONAL_NOT_EQUAL");
            continue;
        }
        RELATIONAL_EQUAL_EQUAL: {
            make_token(tokens, &tokenCount, tokenIndex, ++cursor, "RELATIONAL_EQUAL_EQUAL");
            continue;
        }
        RELATIONAL_GREATER_EQUAL: {
            make_token(tokens, &tokenCount, tokenIndex, ++cursor, "RELATIONAL_GREATER_EQUAL");
            continue;
        }
        RELATIONAL_LESS_EQUAL: {
            make_token(tokens, &tokenCount, tokenIndex, ++cursor, "RELATIONAL_LESS_EQUAL");
            continue;
        }
        /********************[LITERALS]********************/
        INTEGER: {
            ch = *++cursor;
            if (isdigit(ch)) goto INTEGER;
            if (ch == '.')  goto FLOAT; 
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "INTEGER");
                continue;
            }
            else goto INVALID;
        }
        FLOAT: {
            ch = *++cursor;
            if (isdigit(ch)) goto FLOAT;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "FLOAT");
                continue;
            }
            else goto INVALID;
        }
        STRING: {
            ch = *++cursor;
            if (ch == '\"') goto STRING_END;
            if (ch == '\0') goto INVALID;
            else goto STRING;
        }
        STRING_END: {
            make_token(tokens, &tokenCount, tokenIndex, ++cursor, "STRING");
            continue;
        }
        CHAR: {
            ch = *++cursor;
            if (ch == '\'') goto INVALID;
            else goto CHAR_CONTENT;
        }
        CHAR_CONTENT: {
            ch = *++cursor;
            if (ch == '\'') goto CHAR_END;
            else goto INVALID;
        }
        CHAR_END: {
            make_token(tokens, &tokenCount, tokenIndex, ++cursor, "CHAR");
            continue;
        }
        /********************[COMMENTS]********************/
        COMMENT: {
            ch = *++cursor;
            if (ch == '#' && cursor-1 == tokenIndex) goto COMMENT_MULTI;
            if (ch == '\n' || ch == '\0') goto COMMENT_SINGLE_END;
            goto COMMENT;
        }
        COMMENT_SINGLE_END: {
            make_token(tokens, &tokenCount, tokenIndex, ++cursor, "COMMENT");
            continue;
        }
        COMMENT_MULTI: {
            ch = *++cursor;
            if (ch == '#') goto COMMENT_MULTI_POSSIBLE_END;
            if (ch == '\0') goto COMMENT_MULTI_END;
            goto COMMENT_MULTI;
        }
        COMMENT_MULTI_POSSIBLE_END: {
            ch = *++cursor;
            if (ch == '#') goto COMMENT_MULTI_END;  
            goto COMMENT_MULTI;
        }
        COMMENT_MULTI_END: {
            make_token(tokens, &tokenCount, tokenIndex, ++cursor, "COMMENT_MULTI");
            continue;
        }
        /********************[IDENTIFIERS & KEYWORDS]********************/
        IDENTIFIER: {
            if (cursor - tokenIndex == 0) {
                switch (ch) {
                    case 'a': goto NOISE_A;
                    case 'b': goto PREFIX_B;
                    case 'c': goto PREFIX_C;
                    case 'd': goto PREFIX_D;
                    case 'e': goto PREFIX_E;
                    case 'f': goto PREFIX_F;
                    case 'i': goto PREFIX_I;
                    case 'n': goto PREFIX_N;
                    case 'o': goto PREFIX_O;
                    case 'p': goto PREFIX_P;
                    case 'r': goto PREFIX_R;
                    case 's': goto PREFIX_S;
                    case 't': goto PREFIX_T;
                    case 'u': goto PREFIX_U;
                    case 'w': goto PREFIX_W;
                    default: break;
                }
            }
            ch = *++cursor;
            if (isalnum(ch) || ch == '_') goto IDENTIFIER;
            if (isSeparator(ch)) { 
                make_token(tokens, &tokenCount, tokenIndex, cursor, "IDENTIFIER");
                continue;
            }
            else goto INVALID;
        }
        NOISE_A: {
            ch = *++cursor;
            if (ch == 's') goto NOISE_AS;
            if (ch == 'n') goto NOISE_AN;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "NOISE_A");
                continue;
            }
            goto IDENTIFIER;
        }
        NOISE_AS: {
            ch = *++cursor;
            if (ch == 'k') goto KEYWORD_ASK;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "NOISE_AS");
                continue;
            }
            goto IDENTIFIER;
        }
        KEYWORD_ASK: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_ASK");
                continue;
            }
            goto IDENTIFIER;
        }
        NOISE_AN: {
            ch = *++cursor;
            if (ch == 'd') goto KEYWORD_AND;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "NOISE_AN");
                continue;
            }
            goto IDENTIFIER;
        }
        KEYWORD_AND: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_AND");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_B: {
            ch = *++cursor;
            if (ch == 'e') goto PREFIX_BE;
            if (ch == 'r') goto PREFIX_BR;
            goto IDENTIFIER;
        }
        PREFIX_BE: {
            ch = *++cursor;
            if (ch == 'c') goto PREFIX_BEC;
            goto IDENTIFIER;
        }
        PREFIX_BEC: {
            ch = *++cursor;
            if (ch == 'o') goto PREFIX_BECO;
            goto IDENTIFIER;
        }
        PREFIX_BECO: {
            ch = *++cursor;
            if (ch == 'm') goto PREFIX_BECOM;
            goto IDENTIFIER;
        }
        PREFIX_BECOM: {
            ch = *++cursor;
            if (ch == 'e') goto PREFIX_BECOME;
            goto IDENTIFIER;
        }
        PREFIX_BECOME: {
            ch = *++cursor;
            if (ch == 's') goto KEYWORD_BECOMES;
            goto IDENTIFIER;
        }
        KEYWORD_BECOMES: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_BECOMES");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_BR: {
            ch = *++cursor;
            if (ch == 'e') goto PREFIX_BRE;
            goto IDENTIFIER;
        }
        PREFIX_BRE: {
            ch = *++cursor;
            if (ch == 'a') goto PREFIX_BREA;
            goto IDENTIFIER;
        }
        PREFIX_BREA: {
            ch = *++cursor;
            if (ch == 'k') goto RES_KEY_BREAK;
            goto IDENTIFIER;
        }
        RES_KEY_BREAK: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "RES_KEY_BREAK");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_C: {
            ch = *++cursor;
            if (ch == 'h') goto PREFIX_CH;
            if (ch == 'o') goto PREFIX_CO;
            goto IDENTIFIER;
        }
        PREFIX_CO: {
            ch = *++cursor;
            if (ch == 'n') goto PREFIX_CON;
            goto IDENTIFIER;
        }
        PREFIX_CON: {
            ch = *++cursor;
            if (ch == 't') goto PREFIX_CONT;
            goto IDENTIFIER;
        }
        PREFIX_CONT: {
            ch = *++cursor;
            if (ch == 'i') goto PREFIX_CONTI;
            goto IDENTIFIER;
        }
        PREFIX_CONTI: {
            ch = *++cursor;
            if (ch == 'n') goto PREFIX_CONTIN;
            goto IDENTIFIER;
        }
        PREFIX_CONTIN: {
            ch = *++cursor;
            if (ch == 'u') goto PREFIX_CONTINU;
            goto IDENTIFIER;
        }
        PREFIX_CONTINU: {
            ch = *++cursor;
            if (ch == 'e') goto RES_KEY_CONTINUE;
            goto IDENTIFIER;
        }
        RES_KEY_CONTINUE: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "RES_KEY_CONTINUE");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_CH: {
            ch = *++cursor;
            if (ch == 'a') goto PREFIX_CHA;
            if (ch == 'o') goto PREFIX_CHO;
            goto IDENTIFIER;
        }
        PREFIX_CHA: {
            ch = *++cursor;
            if (ch == 'r') goto PREFIX_CHAR;
            goto IDENTIFIER;
        }
        PREFIX_CHAR: {
            ch = *++cursor;
            if (ch == 'a') goto PREFIX_CHARA;
            goto IDENTIFIER;
        }
        PREFIX_CHARA: {
            ch = *++cursor;
            if (ch == 'c') goto PREFIX_CHARAC;
            goto IDENTIFIER;
        }
        PREFIX_CHARAC: {
            ch = *++cursor;
            if (ch == 't') goto PREFIX_CHARACT;
            goto IDENTIFIER;
        }
        PREFIX_CHARACT: {
            ch = *++cursor;
            if (ch == 'e') goto PREFIX_CHARACTE;
            goto IDENTIFIER;
        }
        PREFIX_CHARACTE: {
            ch = *++cursor;
            if (ch == 'r') goto KEYWORD_CHARACTER;
            goto IDENTIFIER;
        }
        KEYWORD_CHARACTER: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_CHARACTER");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_CHO: {
            ch = *++cursor;
            if (ch == 'i') goto PREFIX_CHOI;
            goto IDENTIFIER;
        }
        PREFIX_CHOI: {
            ch = *++cursor;
            if (ch == 'c') goto PREFIX_CHOIC;
            goto IDENTIFIER;
        }
        PREFIX_CHOIC: {
            ch = *++cursor;
            if (ch == 'e') goto KEYWORD_CHOICE;
            goto IDENTIFIER;
        }
        KEYWORD_CHOICE: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_CHOICE");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_D: {
            ch = *++cursor;
            if (ch == 'i') goto PREFIX_DI;
            goto IDENTIFIER;
        }
        PREFIX_DI: {
            ch = *++cursor;
            if (ch == 'a') goto PREFIX_DIA;
            goto IDENTIFIER;
        }
        PREFIX_DIA: {
            ch = *++cursor;
            if (ch == 'l') goto PREFIX_DIAL;
            goto IDENTIFIER;
        }
        PREFIX_DIAL: {
            ch = *++cursor;
            if (ch == 'o') goto PREFIX_DIALO;
            goto IDENTIFIER;
        }
        PREFIX_DIALO: {
            ch = *++cursor;
            if (ch == 'g') goto PREFIX_DIALOG;
            goto IDENTIFIER;
        }
        PREFIX_DIALOG: {
            ch = *++cursor;
            if (ch == 'u') goto PREFIX_DIALOGU;
            goto IDENTIFIER;
        }
        PREFIX_DIALOGU: {
            ch = *++cursor;
            if (ch == 'e') goto KEYWORD_DIALOGUE;
            goto IDENTIFIER;
        }
        KEYWORD_DIALOGUE: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_DIALOGUE");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_E: {
            ch = *++cursor;
            if (ch == 'l') goto PREFIX_EL;
            if (ch == 'n') goto PREFIX_EN;
            if (ch == 'r') goto PREFIX_ER;
            goto IDENTIFIER;
        }
        PREFIX_EL: {
            ch = *++cursor;
            if (ch == 'i') goto PREFIX_ELI;
            if (ch == 's') goto PREFIX_ELS;
            goto IDENTIFIER;
        }
        PREFIX_ELI: {
            ch = *++cursor;
            if (ch == 'f') goto KEYWORD_ELIF;
            goto IDENTIFIER;
        }
        KEYWORD_ELIF: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_ELIF");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_ELS: {
            ch = *++cursor;
            if (ch == 'e') goto KEYWORD_ELSE;
            goto IDENTIFIER;
        }
        KEYWORD_ELSE: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_ELSE");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_EN: {
            ch = *++cursor;
            if (ch == 'd') goto KEYWORD_END;
            goto IDENTIFIER;
        }
        KEYWORD_END: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_END");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_ER: {
            ch = *++cursor;
            if (ch == 'r') goto PREFIX_ERR;
            goto IDENTIFIER;
        }
        PREFIX_ERR: {
            ch = *++cursor;
            if (ch == 'o') goto PREFIX_ERRO;
            goto IDENTIFIER;
        }
        PREFIX_ERRO: {
            ch = *++cursor;
            if (ch == 'r') goto RES_KEY_ERROR;
            goto IDENTIFIER;
        }
        RES_KEY_ERROR: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "RES_KEY_ERROR");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_F: {
            ch = *++cursor;
            if (ch == 'o') goto PREFIX_FO;
            if (ch == 'a') goto PREFIX_FA;
            if (ch == 'i') goto PREFIX_FI;
            goto IDENTIFIER;
        }
        PREFIX_FA: {
            ch = *++cursor;
            if (ch == 'l') goto PREFIX_FAL;
            goto IDENTIFIER;
        }
        PREFIX_FAL: {
            ch = *++cursor;
            if (ch == 's') goto PREFIX_FALS;
            goto IDENTIFIER;
        }
        PREFIX_FALS: {
            ch = *++cursor;
            if (ch == 'e') goto RES_KEY_FALSE;
            goto IDENTIFIER;
        }
        RES_KEY_FALSE: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "RES_KEY_FALSE");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_FO: {
            ch = *++cursor;
            if (ch == 'r') goto KEYWORD_FOR;
            goto IDENTIFIER;
        }
        KEYWORD_FOR: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_FOR");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_FI: {
            ch = *++cursor;
            if (ch == 'x') goto PREFIX_FIX;
            goto IDENTIFIER;
        }
        PREFIX_FIX: {
            ch = *++cursor;
            if (ch == 'e') goto PREFIX_FIXE;
            goto IDENTIFIER;
        }
        PREFIX_FIXE: {
            ch = *++cursor;
            if (ch == 'd') goto RES_KEY_FIXED;
            goto IDENTIFIER;
        }
        RES_KEY_FIXED: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "RES_KEY_FIXED");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_I: {
            ch = *++cursor;
            if (ch == 'f') goto KEYWORD_IF;
            if (ch == 'n') goto RES_KEY_IN;
            if (ch == 's') goto KEYWORD_IS;
            goto IDENTIFIER;
        }
        KEYWORD_IF: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_IF");
                continue;
            }
            goto IDENTIFIER;
        }
        RES_KEY_IN: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "RES_KEY_IN");
                continue;
            }
            goto IDENTIFIER;
        }
        KEYWORD_IS: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_IS");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_N: {
            ch = *++cursor;
            if (ch == 'a') goto PREFIX_NA;
            if (ch == 'o') goto PREFIX_NO;
            goto IDENTIFIER;
        }
        PREFIX_NA: {
            ch = *++cursor;
            if (ch == 'r') goto PREFIX_NAR;
            goto IDENTIFIER;
        }
        PREFIX_NAR: {
            ch = *++cursor;
            if (ch == 'r') goto PREFIX_NARR;
            goto IDENTIFIER;
        }
        PREFIX_NARR: {
            ch = *++cursor;
            if (ch == 'a') goto PREFIX_NARRA;
            goto IDENTIFIER;
        }
        PREFIX_NARRA: {
            ch = *++cursor;
            if (ch == 't') goto PREFIX_NARRAT;
            goto IDENTIFIER;
        }
        PREFIX_NARRAT: {
            ch = *++cursor;
            if (ch == 'e') goto KEYWORD_NARRATE;
            goto IDENTIFIER;
        }
        KEYWORD_NARRATE: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_NARRATE");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_NO: {
            ch = *++cursor;
            if (ch == 't') goto KEYWORD_NOT;
            goto IDENTIFIER;
        }
        KEYWORD_NOT: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_NOT");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_O: {
            ch = *++cursor;
            if (ch == 'p') goto PREFIX_OP;
            if (ch == 'r') goto KEYWORD_OR;
            if (ch == 'f') goto NOISE_OF;
            goto IDENTIFIER;
        }
        PREFIX_OP: {
            ch = *++cursor;
            if (ch == 't') goto PREFIX_OPT;
            goto IDENTIFIER;
        }
        PREFIX_OPT: {
            ch = *++cursor;
            if (ch == 'i') goto PREFIX_OPTI;
            goto IDENTIFIER;
        }
        PREFIX_OPTI: {
            ch = *++cursor;
            if (ch == 'o') goto PREFIX_OPTIO;
            goto IDENTIFIER;
        }
        PREFIX_OPTIO: {
            ch = *++cursor;
            if (ch == 'n') goto KEYWORD_OPTION;
            goto IDENTIFIER;
        }
        KEYWORD_OPTION: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_OPTION");
                continue;
            }
            goto IDENTIFIER;
        }
        KEYWORD_OR: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_OR");
                continue;
            }
            goto IDENTIFIER;
        }
        NOISE_OF: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "NOISE_OF");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_P: {
            ch = *++cursor;
            if (ch == 'a') goto PREFIX_PA;
            goto IDENTIFIER;
        }
        PREFIX_PA: {
            ch = *++cursor;
            if (ch == 's') goto PREFIX_PAS;
            goto IDENTIFIER;
        }
        PREFIX_PAS: {
            ch = *++cursor;
            if (ch == 's') goto RES_KEY_PASS;
            goto IDENTIFIER;
        }
        RES_KEY_PASS: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "RES_KEY_PASS");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_R: {
            ch = *++cursor;
            if (ch == 'e') goto PREFIX_RE;
            goto IDENTIFIER;
        }
        PREFIX_RE: {
            ch = *++cursor;
            if (ch == 'p') goto PREFIX_REP;
            if (ch == 't') goto PREFIX_RET;
            goto IDENTIFIER;
        }
        PREFIX_REP: {
            ch = *++cursor;
            if (ch == 'e') goto PREFIX_REPE;
            goto IDENTIFIER;
        }
        PREFIX_REPE: {
            ch = *++cursor;
            if (ch == 'a') goto PREFIX_REPEA;
            goto IDENTIFIER;
        }
        PREFIX_REPEA: {
            ch = *++cursor;
            if (ch == 't') goto KEYWORD_REPEAT;
            goto IDENTIFIER;
        }
        KEYWORD_REPEAT: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_REPEAT");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_RET: {
            ch = *++cursor;
            if (ch == 'u') goto PREFIX_RETU;
            goto IDENTIFIER;
        }
        PREFIX_RETU: {
            ch = *++cursor;
            if (ch == 'r') goto PREFIX_RETUR;
            goto IDENTIFIER;
        }
        PREFIX_RETUR: {
            ch = *++cursor;
            if (ch == 'n') goto RES_KEY_RETURN;
            goto IDENTIFIER;
        }
        RES_KEY_RETURN: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "RES_KEY_RETURN");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_S: {
            ch = *++cursor;
            if (ch == 'c') goto PREFIX_SC;
            if (ch == 'h') goto PREFIX_SH;
            if (ch == 't') goto PREFIX_ST;
            goto IDENTIFIER;
        }
        PREFIX_SC: {
            ch = *++cursor;
            if (ch == 'e') goto PREFIX_SCE;
            goto IDENTIFIER;
        }
        PREFIX_SCE: {
            ch = *++cursor;
            if (ch == 'n') goto PREFIX_SCEN;
            goto IDENTIFIER;
        }
        PREFIX_SCEN: {
            ch = *++cursor;
            if (ch == 'e') goto KEYWORD_SCENE;
            goto IDENTIFIER;
        }
        KEYWORD_SCENE: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_SCENE");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_SH: {
            ch = *++cursor;
            if (ch == 'o') goto PREFIX_SHO;
            goto IDENTIFIER;
        }
        PREFIX_SHO: {
            ch = *++cursor;
            if (ch == 'w') goto KEYWORD_SHOW;
            goto IDENTIFIER;
        }
        KEYWORD_SHOW: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_SHOW");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_ST: {
            ch = *++cursor;
            if (ch == 'a') goto PREFIX_STA;
            goto IDENTIFIER;
        }
        PREFIX_STA: {
            ch = *++cursor;
            if (ch == 'r') goto PREFIX_STAR;
            goto IDENTIFIER;
        }
        PREFIX_STAR: {
            ch = *++cursor;
            if (ch == 't') goto KEYWORD_START;
            goto IDENTIFIER;
        }
        KEYWORD_START: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_START");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_T: {
            ch = *++cursor;
            if (ch == 'e') goto PREFIX_TE;
            if (ch == 'i') goto PREFIX_TI;
            if (ch == 'h') goto PREFIX_TH;
            if (ch == 'r') goto PREFIX_TR;
            if (ch == 'o') goto NOISE_TO;
            goto IDENTIFIER;
        }
        PREFIX_TE: {
            ch = *++cursor;
            if (ch == 'm') goto PREFIX_TEM;
            if (ch == 'n') goto NOISE_THEN;
            goto IDENTIFIER;
        }
        PREFIX_TEM: {
            ch = *++cursor;
            if (ch == 'p') goto PREFIX_TEMP;
            goto IDENTIFIER;
        }
        PREFIX_TEMP: {
            ch = *++cursor;
            if (ch == 'l') goto PREFIX_TEMPL;
            goto IDENTIFIER;
        }
        PREFIX_TEMPL: {
            ch = *++cursor;
            if (ch == 'a') goto PREFIX_TEMPLA;
            goto IDENTIFIER;
        }
        PREFIX_TEMPLA: {
            ch = *++cursor;
            if (ch == 't') goto PREFIX_TEMPLAT;
            goto IDENTIFIER;
        }
        PREFIX_TEMPLAT: {
            ch = *++cursor;
            if (ch == 'e') goto KEYWORD_TEMPLATE;
            goto IDENTIFIER;
        }
        KEYWORD_TEMPLATE: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_TEMPLATE");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_TI: {
            ch = *++cursor;
            if (ch == 'm') goto PREFIX_TIM;
            goto IDENTIFIER;
        }
        PREFIX_TIM: {
            ch = *++cursor;
            if (ch == 'e') goto PREFIX_TIME;
            goto IDENTIFIER;
        }
        PREFIX_TIME: {
            ch = *++cursor;
            if (ch == 's') goto KEYWORD_TIMES;
            goto IDENTIFIER;
        }
        KEYWORD_TIMES: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_TIMES");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_TH: {
            ch = *++cursor;
            if (ch == 'e') goto NOISE_THE;
            goto IDENTIFIER;
        }
        NOISE_THE: {
            ch = *++cursor;
            if (ch == 'n') goto NOISE_THEN;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "NOISE_THE");
                continue;
            }
            goto IDENTIFIER;
        }
        NOISE_THEN: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "NOISE_THEN");
                continue;
            }
            goto IDENTIFIER;
        }
        NOISE_TO: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "NOISE_TO");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_TR: {
            ch = *++cursor;
            if (ch == 'u') goto PREFIX_TRU;
            goto IDENTIFIER;
        }
        PREFIX_TRU: {
            ch = *++cursor;
            if (ch == 'e') goto RES_KEY_TRUE;
            goto IDENTIFIER;
        }
        RES_KEY_TRUE: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "RES_KEY_TRUE");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_U: {
            ch = *++cursor;
            if (ch == 'n') goto PREFIX_UN;
            goto IDENTIFIER;
        }
        PREFIX_UN: {
            ch = *++cursor;
            if (ch == 't') goto PREFIX_UNT;
            goto IDENTIFIER;
        }
        PREFIX_UNT: {
            ch = *++cursor;
            if (ch == 'i') goto PREFIX_UNTI;
            goto IDENTIFIER;
        }
        PREFIX_UNTI: {
            ch = *++cursor;
            if (ch == 'l') goto KEYWORD_UNTIL;
            goto IDENTIFIER;
        }
        KEYWORD_UNTIL: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "KEYWORD_UNTIL");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_W: {
            ch = *++cursor;
            if (ch == 'i') goto PREFIX_WI;
            goto IDENTIFIER;
        }
        PREFIX_WI: {
            ch = *++cursor;
            if (ch == 't') goto PREFIX_WIT;
            goto IDENTIFIER;
        }
        PREFIX_WIT: {
            ch = *++cursor;
            if (ch == 'h') goto RES_KEY_WITH;
            goto IDENTIFIER;
        }
        RES_KEY_WITH: {
            ch = *++cursor;
            if (isSeparator(ch)) {
                make_token(tokens, &tokenCount, tokenIndex, cursor, "RES_KEY_WITH");
                continue;
            }
            goto IDENTIFIER;
        }
        INVALID: { break; }
    }
    outputTokens(tokens);
    free(inputBuffer);
    return 0;
}