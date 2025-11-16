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
        tokenIndex = cursor;
        char ch = *cursor;
        START: {
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
        }
        BLANK: { 
            ch = *++cursor;
            if (isspace(ch)) goto BLANK; 
            else continue; 
        }

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
        ARITHMETIC_FLOOR_DIVIDE: { 
            ch = *++cursor;
            make_token(tokens, &tokenCount, tokenIndex, cursor, "ARITHMETIC_FLOOR_DIVIDE");
            continue;
        }
        ARITHMETIC_MODULUS: { 
            ch = *++cursor;
            make_token(tokens, &tokenCount, tokenIndex, cursor, "ARITHMETIC_MODULUS");
            continue;
        }
        ARITHMETIC_EXPONENT: { 
            ch = *++cursor;
            make_token(tokens, &tokenCount, tokenIndex, cursor, "ARITHMETIC_EXPONENT");
            continue;
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
        ASSIGNMENT_PLUS_ASSIGN: {
            ch = *++cursor;
            make_token(tokens, &tokenCount, tokenIndex, cursor, "ASSIGNMENT_PLUS_ASSIGN");
            continue;
        }
        ASSIGNMENT_MINUS_ASSIGN: {
            ch = *++cursor;
            make_token(tokens, &tokenCount, tokenIndex, cursor, "ASSIGNMENT_MINUS_ASSIGN");
            continue;
        }
        ASSIGNMENT_MULT_ASSIGN: {
            ch = *++cursor;
            make_token(tokens, &tokenCount, tokenIndex, cursor, "ASSIGNMENT_MULT_ASSIGN");
            continue;
        }
        ASSIGNMENT_DIV_ASSIGN: {
            ch = *++cursor;
            make_token(tokens, &tokenCount, tokenIndex, cursor, "ASSIGNMENT_DIV_ASSIGN");
            continue;
        }
        ASSIGNMENT_MOD_ASSIGN: {
            ch = *++cursor;
            make_token(tokens, &tokenCount, tokenIndex, cursor, "ASSIGNMENT_MOD_ASSIGN");
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
            ch = *++cursor;
            make_token(tokens, &tokenCount, tokenIndex, cursor, "RELATIONAL_NOT_EQUAL");
            continue;
        }
        RELATIONAL_EQUAL_EQUAL: {
            ch = *++cursor;
            make_token(tokens, &tokenCount, tokenIndex, cursor, "RELATIONAL_EQUAL_EQUAL");
            continue;
        }
        RELATIONAL_GREATER_EQUAL: {
            ch = *++cursor;
            make_token(tokens, &tokenCount, tokenIndex, cursor, "RELATIONAL_GREATER_EQUAL");
            continue;
        }
        RELATIONAL_LESS_EQUAL: {
            make_token(tokens, &tokenCount, tokenIndex, cursor, "RELATIONAL_LESS_EQUAL");
            ch = *++cursor;
            continue;
        }
        
        // Delimiters
        LPAREN: {
            ch = *++cursor;
            make_token(tokens, &tokenCount, tokenIndex, cursor, "LPAREN");
            continue;
        }
        RPAREN: {
            ch = *++cursor;
            make_token(tokens, &tokenCount, tokenIndex, cursor, "RPAREN");
            continue;
        }
        LBRACKET: {
            ch = *++cursor;
            make_token(tokens, &tokenCount, tokenIndex, cursor, "LBRACKET");
            continue;
        }
        RBRACKET: {
            ch = *++cursor;
            make_token(tokens, &tokenCount, tokenIndex, cursor, "RBRACKET");
            continue;
        }
        COLON: {
            ch = *++cursor;
            make_token(tokens, &tokenCount, tokenIndex, cursor, "COLON");
            continue;
        }
        COMMA: {
            ch = *++cursor;
            make_token(tokens, &tokenCount, tokenIndex, cursor, "COMMA");
            continue;
        }
        DOT: {
            ch = *++cursor;
            make_token(tokens, &tokenCount, tokenIndex, cursor, "DOT");
            continue;
        }

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
        IDENTIFIER: {
            if (cursor - tokenIndex == 0) {
                switch (ch) {
                    case 'a': goto PREFIX_A;
                    case 'b': goto PREFIX_B;
                    case 'c': goto PREFIX_C;
                    case 'd': goto PREFIX_D;
                    case 'e': goto PREFIX_E;
                    case 'f': goto PREFIX_F;
                    case 'i': goto PREFIX_I;
                    case 'n': goto PREFIX_N;
                    case 'o': goto PREFIX_O;
                    case 'r': goto PREFIX_R;
                    case 's': goto PREFIX_S;
                    case 't': goto PREFIX_T;
                    case 'u': goto PREFIX_U;
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

        COMMENT:
        COMMENT_MULTI:
        STRING:
        CHAR:
        
        // Keywords
        PREFIX_C:
        PREFIX_CH:
        PREFIX_CHA:
        PREFIX_CHAR:
        PREFIX_CHARA:
        PREFIX_CHARAC:
        PREFIX_CHARACT:
        PREFIX_CHARACTE:
        KEYWORD_CHARACTER:
        
        PREFIX_S:
        PREFIX_SC:
        PREFIX_SCE:
        PREFIX_SCEN:
        KEYWORD_SCENE:
        
        PREFIX_T:
        PREFIX_TE:
        PREFIX_TEM:
        PREFIX_TEMP:
        PREFIX_TEMPL:
        PREFIX_TEMPLA:
        PREFIX_TEMPLAT:
        KEYWORD_TEMPLATE:
        
        PREFIX_D:
        PREFIX_DI:
        PREFIX_DIA:
        PREFIX_DIAL:
        PREFIX_DIALO:
        PREFIX_DIALOG:
        PREFIX_DIALOGU:
        KEYWORD_DIALOGUE:
        
        PREFIX_N:
        PREFIX_NA:
        PREFIX_NAR:
        PREFIX_NARR:
        PREFIX_NARRA:
        PREFIX_NARRAT:
        KEYWORD_NARRATE:
        
        PREFIX_CHO:
        PREFIX_CHOI:
        PREFIX_CHOIC:
        KEYWORD_CHOICE:
        
        PREFIX_O:
        PREFIX_OP:
        PREFIX_OPT:
        PREFIX_OPTI:
        PREFIX_OPTIO:
        KEYWORD_OPTION:
        
        PREFIX_A:
        PREFIX_AS:
        KEYWORD_ASK:
        
        PREFIX_I:
        KEYWORD_IF:
        
        PREFIX_E:
        PREFIX_EL:
        PREFIX_ELI:
        KEYWORD_ELIF:
        PREFIX_ELS:
        KEYWORD_ELSE:
        
        PREFIX_R:
        PREFIX_RE:
        PREFIX_REP:
        PREFIX_REPE:
        PREFIX_REPEA:
        KEYWORD_REPEAT:
        
        PREFIX_F:
        PREFIX_FO:
        KEYWORD_FOR:
        
        PREFIX_SH:
        PREFIX_SHO:
        KEYWORD_SHOW:
        
        PREFIX_ST:
        PREFIX_STA:
        PREFIX_STAR:
        KEYWORD_START:
        
        PREFIX_EN:
        KEYWORD_END:
        
        PREFIX_B:
        PREFIX_BE:
        PREFIX_BEC:
        PREFIX_BECO:
        PREFIX_BECOM:
        PREFIX_BECOME:
        KEYWORD_BECOMES:
        
        PREFIX_AN:
        KEYWORD_AND:
        
        PREFIX_NO:
        KEYWORD_NOT:
        
        PREFIX_OR:
        KEYWORD_OR:
        
        PREFIX_IS:
        KEYWORD_IS:
        
        PREFIX_U:
        PREFIX_UN:
        PREFIX_UNT:
        PREFIX_UNTI:
        KEYWORD_UNTIL:
        
        PREFIX_TI:
        PREFIX_TIM:
        PREFIX_TIME:
        KEYWORD_TIMES:
        
        PREFIX_TR:
        PREFIX_TRU:
        RES_KEY_TRUE:
        
        PREFIX_FA:
        PREFIX_FAL:
        PREFIX_FALS:
        RES_KEY_FALSE:
        
        PREFIX_RET:
        PREFIX_RETU:
        PREFIX_RETUR:
        RES_KEY_RETURN:
        
        RES_KEY_IN:
        
        PREFIX_ER:
        PREFIX_ERR:
        PREFIX_ERRO:
        RES_KEY_ERROR:
        
        PREFIX_FIX:
        PREFIX_FIXE:
        RES_KEY_FIXED:
        
        PREFIX_BR:

        
        INVALID: { break; }

    }

    outputTokens(tokens);
    free(inputBuffer);
    return 0;
}