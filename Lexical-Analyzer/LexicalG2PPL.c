#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

typedef struct {
    char type[26];        
    char lexeme[100];      
} Token;

typedef struct {
    int data[100];
    int topIndex;
} Stack;

void push(Stack *stack, int value);
int pop(Stack *stack);
int peek(Stack *stack);
bool isDelimiter(char c);
bool isOperator(char c);
bool isSeparator(char c);
char *read_file(const char *filename, unsigned int *out_size);
void emitToken(Token *tokens, int *tokenCount, const char *start, const char *end, const char *type); 
void outputTokens(Token* tokens);
void bufferChar(char *buffer, int *pos, char ch);

int main() {
    char filename[256];

    // user inputs filename
    printf("Enter filename: ");
    scanf("%255s", filename);  

    // check file extension
    int len = strlen(filename);
    if (len < 3 || strcmp(filename + len - 3, ".st") != 0) {
        printf("Please enter a valid .st file format.\n");
        return 1;
    }

    // read file content
    unsigned int size;
    char *inputBuffer = read_file(filename, &size);
    if (!inputBuffer) {
        printf("Error reading file.\n");
        return 1;
    }

    Token tokens[1000];
    int tokenCount = 0;

    Stack indentationStack;
    indentationStack.data[0] = 0;
    indentationStack.topIndex = 0;

    int beginningSpaces = 0;
    char* cursor = inputBuffer;
    char* lexemeIndex;

    while (*cursor) {
        /********************[START STATE]********************/
        lexemeIndex = cursor;
        
        switch (*cursor) {
            case '\n': goto NEWLINE;
            case '\r': case '\t': 
            case ' ': goto BLANK;
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
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': 
            case '9': goto INTEGER;
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
            case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
            case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
            case 'Y': case 'Z': case 'g': case 'h': case 'j': case 'k': 
            case 'l': case 'm': case 'q': case 'v': case 'x': case 'y':
            case 'z': goto IDENTIFIER;
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
            default: goto INVALID;
        }
        /********************[WHITESPACE]********************/
        BLANK: { 
            if (*++cursor == ' ' || *cursor == '\t' || *cursor == '\r') goto BLANK; 
            else continue; 
        }
        NEWLINE: {
            emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "NEWLINE");
            beginningSpaces = 0;
            
            if (*cursor == '\n') continue;
            if (*cursor == '\0') {
                while (peek(&indentationStack) > 0) {
                    pop(&indentationStack);
                    emitToken(tokens, &tokenCount, lexemeIndex, cursor, "DEDENT");
                }
                continue;
            }
            while (*cursor == ' ') {
                beginningSpaces++;
                cursor++;
            }
            if (beginningSpaces > peek(&indentationStack)) {
                push(&indentationStack, beginningSpaces);
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "INDENT");
                continue;
            } 
            else if (beginningSpaces < peek(&indentationStack)) {
                while (beginningSpaces != peek(&indentationStack)) {
                    int element = pop(&indentationStack);
                    if (element == -1) goto INVALID;
                    emitToken(tokens, &tokenCount, lexemeIndex, cursor, "DEDENT");
                }
                continue;
            }
            continue;
        }
        /********************[DELIMITERS]********************/
        LPAREN: { emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "LPAREN"); continue; }
        RPAREN: { emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "RPAREN"); continue; }
        LBRACKET: { emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "LBRACKET"); continue; }
        RBRACKET: { emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "RBRACKET"); continue; }
        COLON: { emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "COLON"); continue; }
        COMMA: { emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "COMMA"); continue; }
        DOT: { emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "DOT"); continue; }
        /********************[OPERATORS]********************/
        ARITHMETIC_PLUS: { 
            if (*++cursor == '=') goto ASSIGNMENT_PLUS_ASSIGN;
            if (isSeparator(*cursor)) {  
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "ARITHMETIC_PLUS");
                continue;
            }
            else goto INVALID;
        }
        ARITHMETIC_MINUS: { 
            if (*++cursor == '=') goto ASSIGNMENT_MINUS_ASSIGN;
            if (isSeparator(*cursor)) {  
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "ARITHMETIC_MINUS");
                continue;
            }
            else goto INVALID;
        }
        ARITHMETIC_MULTIPLY: { 
            if (*++cursor == '=') goto ASSIGNMENT_MULT_ASSIGN;
            if (isSeparator(*cursor)) {  
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "ARITHMETIC_MULTIPLY");
                continue;
            }
            else goto INVALID;
        }
        ARITHMETIC_DIVIDE: { 
            if (*++cursor == '/') goto ARITHMETIC_FLOOR_DIVIDE;
            if (*cursor == '=') goto ASSIGNMENT_DIV_ASSIGN;
            if (isSeparator(*cursor)) {  
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "ARITHMETIC_DIVIDE");
                continue;
            }
            else goto INVALID;
        }
        ARITHMETIC_MODULUS: { 
            if (*++cursor == '=') goto ASSIGNMENT_MOD_ASSIGN;
            if (isSeparator(*cursor)) {  
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "ARITHMETIC_MODULUS");
                continue;
            }
            else goto INVALID;
        }
        ASSIGNMENT_ASSIGN: {
            if (*++cursor == '=') goto RELATIONAL_EQUAL_EQUAL;
            if (isSeparator(*cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "ASSIGNMENT_ASSIGN");
                continue;
            }
            else goto INVALID;
        }
        ARITHMETIC_FLOOR_DIVIDE: { 
            emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "ARITHMETIC_FLOOR_DIVIDE");
            continue;
        }
        ARITHMETIC_EXPONENT: { 
            emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "ARITHMETIC_EXPONENT");
            continue;
        }

        ASSIGNMENT_PLUS_ASSIGN: {
            emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "ASSIGNMENT_PLUS_ASSIGN");
            continue;
        }
        ASSIGNMENT_MINUS_ASSIGN: {
            emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "ASSIGNMENT_MINUS_ASSIGN");
            continue;
        }
        ASSIGNMENT_MULT_ASSIGN: {
            emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "ASSIGNMENT_MULT_ASSIGN");
            continue;
        }
        ASSIGNMENT_DIV_ASSIGN: {
            emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "ASSIGNMENT_DIV_ASSIGN");
            continue;
        }
        ASSIGNMENT_MOD_ASSIGN: {
            emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "ASSIGNMENT_MOD_ASSIGN");
            continue;
        }
        RELATIONAL_LESS: {
            if (*++cursor == '=') goto RELATIONAL_LESS_EQUAL;
            if (isSeparator(*cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "RELATIONAL_LESS");
                continue;
            }
            else goto INVALID;
        }
        RELATIONAL_GREATER: {
            if (*++cursor == '=') goto RELATIONAL_GREATER_EQUAL;
            if (isSeparator(*cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "RELATIONAL_GREATER");
                continue;
            }
            else goto INVALID;
        }
        RELATIONAL_NOT: {
            if (*++cursor == '=') goto RELATIONAL_NOT_EQUAL;
            else goto INVALID;
        }
        RELATIONAL_NOT_EQUAL: {
            emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "RELATIONAL_NOT_EQUAL");
            continue;
        }
        RELATIONAL_EQUAL_EQUAL: {
            emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "RELATIONAL_EQUAL_EQUAL");
            continue;
        }
        RELATIONAL_GREATER_EQUAL: {
            emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "RELATIONAL_GREATER_EQUAL");
            continue;
        }
        RELATIONAL_LESS_EQUAL: {
            emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "RELATIONAL_LESS_EQUAL");
            continue;
        }
        /********************[LITERALS]********************/
        INTEGER: {
            if (isdigit(*++cursor)) goto INTEGER;
            if (*cursor == '.')  goto FLOAT; 
            if (isSeparator(*cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "INTEGER");
                continue;
            }
            else goto INVALID;
        }
        FLOAT: {
            if (isdigit(*++cursor)) goto FLOAT;
            if (isSeparator(*cursor) && *cursor != '.') {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "FLOAT");
                continue;
            }
            else goto INVALID;
        }
        STRING: {
            if (*++cursor == '\"') goto STRING_END;
            if (*cursor == '\0') goto INVALID;
            else goto STRING;
        }
        STRING_END: {
            emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "STRING");
            continue;
        }
        CHAR: {
            if (*++cursor == '\'') goto INVALID;
            else goto CHAR_CONTENT;
        }
        CHAR_CONTENT: {
            if (*++cursor == '\'') goto CHAR_END;
            else goto INVALID;
        }
        CHAR_END: {
            emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "CHAR");
            continue;
        }
        /********************[COMMENTS]********************/
        COMMENT: {
            if (*++cursor == '#' && cursor-1 == lexemeIndex) goto COMMENT_MULTI;
            if (*cursor == '\n' || *cursor == '\0') goto COMMENT_SINGLE_END;
            goto COMMENT;
        }
        COMMENT_SINGLE_END: {
            emitToken(tokens, &tokenCount, lexemeIndex, cursor, "COMMENT");
            continue;
        }
        COMMENT_MULTI: {
            if (*++cursor == '#') goto COMMENT_MULTI_POSSIBLE_END;
            if (*cursor == '\0') goto COMMENT_MULTI_END;
            goto COMMENT_MULTI;
        }
        COMMENT_MULTI_POSSIBLE_END: {
            if (*++cursor == '#') goto COMMENT_MULTI_END;  
            goto COMMENT_MULTI;
        }
        COMMENT_MULTI_END: {
            emitToken(tokens, &tokenCount, lexemeIndex, ++cursor, "COMMENT_MULTI");
            continue;
        }
        /********************[IDENTIFIERS & KEYWORDS]********************/
        IDENTIFIER: {
            if (isalnum(*++cursor) || *cursor == '_') goto IDENTIFIER;
            if (isSeparator(*cursor)) { 
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "IDENTIFIER");
                continue;
            }
            goto INVALID;
        }
        NOISE_A: {
            if (*++cursor == 's') goto NOISE_AS;
            if (*cursor == 'n') goto NOISE_AN;
            if (isSeparator(*cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "NOISE_A");
                continue;
            }
            goto IDENTIFIER;
        }
        NOISE_AS: {
            if (*++cursor == 'k') goto KEYWORD_ASK;
            if (isSeparator(*cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "NOISE_AS");
                continue;
            }
            goto IDENTIFIER;
        }
        KEYWORD_ASK: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_ASK");
                continue;
            }
            goto IDENTIFIER;
        }
        NOISE_AN: {
            if (*++cursor == 'd') goto LOGICAL_AND;
            if (isSeparator(*cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "NOISE_AN");
                continue;
            }
            goto IDENTIFIER;
        }
        LOGICAL_AND: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "LOGICAL_AND");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_B: {
            if (*++cursor == 'e') goto PREFIX_BE;
            if (*cursor == 'r') goto PREFIX_BR;
            goto IDENTIFIER;
        }
        PREFIX_BE: {
            if (*++cursor == 'c') goto PREFIX_BEC;
            goto IDENTIFIER;
        }
        PREFIX_BEC: {
            if (*++cursor == 'o') goto PREFIX_BECO;
            goto IDENTIFIER;
        }
        PREFIX_BECO: {
            if (*++cursor == 'm') goto PREFIX_BECOM;
            goto IDENTIFIER;
        }
        PREFIX_BECOM: {
            if (*++cursor == 'e') goto PREFIX_BECOME;
            goto IDENTIFIER;
        }
        PREFIX_BECOME: {
            if (*++cursor == 's') goto KEYWORD_BECOMES;
            goto IDENTIFIER;
        }
        KEYWORD_BECOMES: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_BECOMES");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_BR: {
            if (*++cursor == 'e') goto PREFIX_BRE;
            goto IDENTIFIER;
        }
        PREFIX_BRE: {
            if (*++cursor == 'a') goto PREFIX_BREA;
            goto IDENTIFIER;
        }
        PREFIX_BREA: {
            if (*++cursor == 'k') goto RES_KEY_BREAK;
            goto IDENTIFIER;
        }
        RES_KEY_BREAK: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "RES_KEY_BREAK");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_C: {
            if (*++cursor == 'h') goto PREFIX_CH;
            if (*cursor == 'o') goto PREFIX_CO;
            goto IDENTIFIER;
        }
        PREFIX_CO: {
            if (*++cursor == 'n') goto PREFIX_CON;
            goto IDENTIFIER;
        }
        PREFIX_CON: {
            if (*++cursor == 't') goto PREFIX_CONT;
            goto IDENTIFIER;
        }
        PREFIX_CONT: {
            if (*++cursor == 'i') goto PREFIX_CONTI;
            goto IDENTIFIER;
        }
        PREFIX_CONTI: {
            if (*++cursor == 'n') goto PREFIX_CONTIN;
            goto IDENTIFIER;
        }
        PREFIX_CONTIN: { 
            if (*++cursor == 'u') goto PREFIX_CONTINU;
            goto IDENTIFIER;
        }
        PREFIX_CONTINU: {
            if (*++cursor == 'e') goto RES_KEY_CONTINUE;
            goto IDENTIFIER;
        }
        RES_KEY_CONTINUE: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "RES_KEY_CONTINUE");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_CH: {
            if (*++cursor == 'a') goto PREFIX_CHA;
            if (*cursor == 'o') goto PREFIX_CHO;
            goto IDENTIFIER;
        }
        PREFIX_CHA: {
            if (*++cursor == 'r') goto PREFIX_CHAR;
            goto IDENTIFIER;
        }
        PREFIX_CHAR: {
            if (*++cursor == 'a') goto PREFIX_CHARA;
            goto IDENTIFIER;
        }
        PREFIX_CHARA: {
            if (*++cursor == 'c') goto PREFIX_CHARAC;
            goto IDENTIFIER;
        }
        PREFIX_CHARAC: {
            if (*++cursor == 't') goto PREFIX_CHARACT;
            goto IDENTIFIER;
        }
        PREFIX_CHARACT: {
            if (*++cursor == 'e') goto PREFIX_CHARACTE;
            goto IDENTIFIER;
        }
        PREFIX_CHARACTE: {
            if (*++cursor == 'r') goto KEYWORD_CHARACTER;
            goto IDENTIFIER;
        }
        KEYWORD_CHARACTER: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_CHARACTER");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_CHO: {
            if (*++cursor == 'i') goto PREFIX_CHOI;
            goto IDENTIFIER;
        }
        PREFIX_CHOI: {
            if (*++cursor == 'c') goto PREFIX_CHOIC;
            goto IDENTIFIER;
        }
        PREFIX_CHOIC: {
            if (*++cursor == 'e') goto KEYWORD_CHOICE;
            goto IDENTIFIER;
        }
        KEYWORD_CHOICE: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_CHOICE");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_D: {
            if (*++cursor == 'i') goto PREFIX_DI;
            goto IDENTIFIER;
        }
        PREFIX_DI: {
            if (*++cursor == 'a') goto PREFIX_DIA;
            goto IDENTIFIER;
        }
        PREFIX_DIA: {
            if (*++cursor == 'l') goto PREFIX_DIAL;
            goto IDENTIFIER;
        }
        PREFIX_DIAL: {
            if (*++cursor == 'o') goto PREFIX_DIALO;
            goto IDENTIFIER;
        }
        PREFIX_DIALO: {
            if (*++cursor == 'g') goto PREFIX_DIALOG;
            goto IDENTIFIER;
        }
        PREFIX_DIALOG: {
            if (*++cursor == 'u') goto PREFIX_DIALOGU;
            goto IDENTIFIER;
        }
        PREFIX_DIALOGU: {
            if (*++cursor == 'e') goto KEYWORD_DIALOGUE;
            goto IDENTIFIER;
        }
        KEYWORD_DIALOGUE: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_DIALOGUE");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_E: {
            if (*++cursor == 'l') goto PREFIX_EL;
            if (*cursor == 'n') goto PREFIX_EN;
            if (*cursor == 'r') goto PREFIX_ER;
            goto IDENTIFIER;
        }
        PREFIX_EL: {
            if (*++cursor == 'i') goto PREFIX_ELI;
            if (*cursor == 's') goto PREFIX_ELS;
            goto IDENTIFIER;
        }
        PREFIX_ELI: {
            if (*++cursor == 'f') goto KEYWORD_ELIF;
            goto IDENTIFIER;
        }
        KEYWORD_ELIF: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_ELIF");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_ELS: {
            if (*++cursor == 'e') goto KEYWORD_ELSE;
            goto IDENTIFIER;
        }
        KEYWORD_ELSE: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_ELSE");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_EN: {
            if (*++cursor == 'd') goto KEYWORD_END;
            goto IDENTIFIER;
        }
        KEYWORD_END: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_END");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_ER: {
            if (*++cursor == 'r') goto PREFIX_ERR;
            goto IDENTIFIER;
        }
        PREFIX_ERR: {
            if (*++cursor == 'o') goto PREFIX_ERRO;
            goto IDENTIFIER;
        }
        PREFIX_ERRO: {
            if (*++cursor == 'r') goto RES_KEY_ERROR;
            goto IDENTIFIER;
        }
        RES_KEY_ERROR: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "RES_KEY_ERROR");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_F: {
            if (*++cursor == 'o') goto PREFIX_FO;
            if (*cursor == 'a') goto PREFIX_FA;
            if (*cursor == 'i') goto PREFIX_FI;
            goto IDENTIFIER;
        }
        PREFIX_FA: {
            if (*++cursor == 'l') goto PREFIX_FAL;
            goto IDENTIFIER;
        }
        PREFIX_FAL: {
            if (*++cursor == 's') goto PREFIX_FALS;
            goto IDENTIFIER;
        }
        PREFIX_FALS: {
            if (*++cursor == 'e') goto RES_KEY_FALSE;
            goto IDENTIFIER;
        }
        RES_KEY_FALSE: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "RES_KEY_FALSE");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_FO: {
            if (*++cursor == 'r') goto KEYWORD_FOR;
            goto IDENTIFIER;
        }
        KEYWORD_FOR: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_FOR");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_FI: {
            if (*++cursor == 'x') goto PREFIX_FIX;
            goto IDENTIFIER;
        }
        PREFIX_FIX: {
            if (*++cursor == 'e') goto PREFIX_FIXE;
            goto IDENTIFIER;
        }
        PREFIX_FIXE: {
            if (*++cursor == 'd') goto RES_KEY_FIXED;
            goto IDENTIFIER;
        }
        RES_KEY_FIXED: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "RES_KEY_FIXED");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_I: {
            if (*++cursor == 'f') goto KEYWORD_IF;
            if (*cursor == 'n') goto RES_KEY_IN;
            if (*cursor == 's') goto KEYWORD_IS;
            goto IDENTIFIER;
        }
        KEYWORD_IF: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_IF");
                continue;
            }
            goto IDENTIFIER;
        }
        RES_KEY_IN: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "RES_KEY_IN");
                continue;
            }
            goto IDENTIFIER;
        }
        KEYWORD_IS: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_IS");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_N: {
            if (*++cursor == 'a') goto PREFIX_NA;
            if (*cursor == 'o') goto PREFIX_NO;
            goto IDENTIFIER;
        }
        PREFIX_NA: {
            if (*++cursor == 'r') goto PREFIX_NAR;
            goto IDENTIFIER;
        }
        PREFIX_NAR: {
            if (*++cursor == 'r') goto PREFIX_NARR;
            goto IDENTIFIER;
        }
        PREFIX_NARR: {
            if (*++cursor == 'a') goto PREFIX_NARRA;
            goto IDENTIFIER;
        }
        PREFIX_NARRA: {
            if (*++cursor == 't') goto PREFIX_NARRAT;
            goto IDENTIFIER;
        }
        PREFIX_NARRAT: {
            if (*++cursor == 'e') goto KEYWORD_NARRATE;
            goto IDENTIFIER;
        }
        KEYWORD_NARRATE: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_NARRATE");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_NO: {
            if (*++cursor == 't') goto LOGICAL_NOT;
            goto IDENTIFIER;
        }
        LOGICAL_NOT: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "LOGICAL_NOT");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_O: {
            if (*++cursor == 'p') goto PREFIX_OP;
            if (*cursor == 'r') goto LOGICAL_OR;
            if (*cursor == 'f') goto NOISE_OF;
            goto IDENTIFIER;
        }
        PREFIX_OP: {
            if (*++cursor == 't') goto PREFIX_OPT;
            goto IDENTIFIER;
        }
        PREFIX_OPT: {
            if (*++cursor == 'i') goto PREFIX_OPTI;
            goto IDENTIFIER;
        }
        PREFIX_OPTI: {
            if (*++cursor == 'o') goto PREFIX_OPTIO;
            goto IDENTIFIER;
        }
        PREFIX_OPTIO: {
            if (*++cursor == 'n') goto KEYWORD_OPTION;
            goto IDENTIFIER;
        }
        KEYWORD_OPTION: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_OPTION");
                continue;
            }
            goto IDENTIFIER;
        }
        LOGICAL_OR: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "LOGICAL_OR");
                continue;
            }
            goto IDENTIFIER;
        }
        NOISE_OF: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "NOISE_OF");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_P: {
            if (*++cursor == 'a') goto PREFIX_PA;
            goto IDENTIFIER;
        }
        PREFIX_PA: {
            if (*++cursor == 's') goto PREFIX_PAS;
            goto IDENTIFIER;
        }
        PREFIX_PAS: {
            if (*++cursor == 's') goto RES_KEY_PASS;
            goto IDENTIFIER;
        }
        RES_KEY_PASS: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "RES_KEY_PASS");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_R: {
            if (*++cursor == 'e') goto PREFIX_RE;
            goto IDENTIFIER;
        }
        PREFIX_RE: {
            if (*++cursor == 'p') goto PREFIX_REP;
            if (*cursor == 't') goto PREFIX_RET;
            goto IDENTIFIER;
        }
        PREFIX_REP: {
            if (*++cursor == 'e') goto PREFIX_REPE;
            goto IDENTIFIER;
        }
        PREFIX_REPE: {
            if (*++cursor == 'a') goto PREFIX_REPEA;
            goto IDENTIFIER;
        }
        PREFIX_REPEA: {
            if (*++cursor == 't') goto KEYWORD_REPEAT;
            goto IDENTIFIER;
        }
        KEYWORD_REPEAT: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_REPEAT");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_RET: {
            if (*++cursor == 'u') goto PREFIX_RETU;
            goto IDENTIFIER;
        }
        PREFIX_RETU: {
            if (*++cursor == 'r') goto PREFIX_RETUR;
            goto IDENTIFIER;
        }
        PREFIX_RETUR: {
            if (*++cursor == 'n') goto RES_KEY_RETURN;
            goto IDENTIFIER;
        }
        RES_KEY_RETURN: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "RES_KEY_RETURN");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_S: {
            if (*++cursor == 'c') goto PREFIX_SC;
            if (*cursor == 'h') goto PREFIX_SH;
            if (*cursor == 't') goto PREFIX_ST;
            goto IDENTIFIER;
        }
        PREFIX_SC: {
            if (*++cursor == 'e') goto PREFIX_SCE;
            goto IDENTIFIER;
        }
        PREFIX_SCE: {
            if (*++cursor == 'n') goto PREFIX_SCEN;
            goto IDENTIFIER;
        }
        PREFIX_SCEN: {
            if (*++cursor == 'e') goto KEYWORD_SCENE;
            goto IDENTIFIER;
        }
        KEYWORD_SCENE: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_SCENE");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_SH: {
            if (*++cursor == 'o') goto PREFIX_SHO;
            goto IDENTIFIER;
        }
        PREFIX_SHO: {
            if (*++cursor == 'w') goto KEYWORD_SHOW;
            goto IDENTIFIER;
        }
        KEYWORD_SHOW: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_SHOW");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_ST: {
            if (*++cursor == 'a') goto PREFIX_STA;
            goto IDENTIFIER;
        }
        PREFIX_STA: {
            if (*++cursor == 'r') goto PREFIX_STAR;
            goto IDENTIFIER;
        }
        PREFIX_STAR: {
            if (*++cursor == 't') goto KEYWORD_START;
            goto IDENTIFIER;
        }
        KEYWORD_START: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_START");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_T: {
            if (*++cursor == 'e') goto PREFIX_TE;
            if (*cursor == 'i') goto PREFIX_TI;
            if (*cursor == 'h') goto PREFIX_TH;
            if (*cursor == 'r') goto PREFIX_TR;
            if (*cursor == 'o') goto NOISE_TO;
            goto IDENTIFIER;
        }
        PREFIX_TE: {
            if (*++cursor == 'm') goto PREFIX_TEM;
            if (*cursor == 'n') goto NOISE_THEN;
            goto IDENTIFIER;
        }
        PREFIX_TEM: {
            if (*++cursor == 'p') goto PREFIX_TEMP;
            goto IDENTIFIER;
        }
        PREFIX_TEMP: {
            if (*++cursor == 'l') goto PREFIX_TEMPL;
            goto IDENTIFIER;
        }
        PREFIX_TEMPL: {
            if (*++cursor == 'a') goto PREFIX_TEMPLA;
            goto IDENTIFIER;
        }
        PREFIX_TEMPLA: {
            if (*++cursor == 't') goto PREFIX_TEMPLAT;
            goto IDENTIFIER;
        }
        PREFIX_TEMPLAT: {
            if (*++cursor == 'e') goto KEYWORD_TEMPLATE;
            goto IDENTIFIER;
        }
        KEYWORD_TEMPLATE: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_TEMPLATE");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_TI: {
            if (*++cursor == 'm') goto PREFIX_TIM;
            goto IDENTIFIER;
        }
        PREFIX_TIM: {
            if (*++cursor == 'e') goto PREFIX_TIME;
            goto IDENTIFIER;
        }
        PREFIX_TIME: {
            if (*++cursor == 's') goto KEYWORD_TIMES;
            goto IDENTIFIER;
        }
        KEYWORD_TIMES: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_TIMES");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_TH: {
            if (*++cursor == 'e') goto NOISE_THE;
            goto IDENTIFIER;
        }
        NOISE_THE: {
            if (*++cursor == 'n') goto NOISE_THEN;
            if (isSeparator(*cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "NOISE_THE");
                continue;
            }
            goto IDENTIFIER;
        }
        NOISE_THEN: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "NOISE_THEN");
                continue;
            }
            goto IDENTIFIER;
        }
        NOISE_TO: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "NOISE_TO");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_TR: {
            if (*++cursor == 'u') goto PREFIX_TRU;
            goto IDENTIFIER;
        }
        PREFIX_TRU: {
            if (*++cursor == 'e') goto RES_KEY_TRUE;
            goto IDENTIFIER;
        }
        RES_KEY_TRUE: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "RES_KEY_TRUE");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_U: {
            if (*++cursor == 'n') goto PREFIX_UN;
            goto IDENTIFIER;
        }
        PREFIX_UN: {
            if (*++cursor == 't') goto PREFIX_UNT;
            goto IDENTIFIER;
        }
        PREFIX_UNT: {
            if (*++cursor == 'i') goto PREFIX_UNTI;
            goto IDENTIFIER;
        }
        PREFIX_UNTI: {
            if (*++cursor == 'l') goto KEYWORD_UNTIL;
            goto IDENTIFIER;
        }
        KEYWORD_UNTIL: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "KEYWORD_UNTIL");
                continue;
            }
            goto IDENTIFIER;
        }
        PREFIX_W: {
            if (*++cursor == 'i') goto PREFIX_WI;
            goto IDENTIFIER;
        }
        PREFIX_WI: {
            if (*++cursor == 't') goto PREFIX_WIT;
            goto IDENTIFIER;
        }
        PREFIX_WIT: {
            if (*++cursor == 'h') goto RES_KEY_WITH;
            goto IDENTIFIER;
        }
        RES_KEY_WITH: {
            if (isSeparator(*++cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "RES_KEY_WITH");
                continue;
            }
            goto IDENTIFIER;
        }
        INVALID: { 
            ++cursor;
            if (*cursor == '\0' || isSeparator(*cursor)) {
                emitToken(tokens, &tokenCount, lexemeIndex, cursor, "INVALID");
                continue;
            }
            goto INVALID;
        }
    }
    outputTokens(tokens);
    free(inputBuffer);
    return 0;
}

bool isDelimiter(char c) {
    return c == ',' || c == ':' || c == '.' || c == '(' ||
           c == ')' || c == '[' || c == ']';
}

bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '=' || c == '!' || c == '<' || c == '>' || c == '^';
}

bool isSeparator(char c) {
    return isspace(c) || isDelimiter(c) || isOperator(c);
}

void push(Stack *stack, int value) {
    if (stack->topIndex < 99) {
        stack->data[++(stack->topIndex)] = value;
    }
}
int pop(Stack *stack) {
    if (stack->topIndex >= 0) {
        return stack->data[(stack->topIndex)--];
    }
    return -1;
}
int peek(Stack *stack) {
    if (stack->topIndex >= 0) {
        return stack->data[stack->topIndex];
    }
    return -1;
}

int read_file_ext(const char *filename){

    //finds the start of the file extension
    int dot_pos = -1;
    for (int i = 0; filename[i] != '\0'; ++i)
        if(filename[i] == '.')
            dot_pos = i;

    if (dot_pos < 0) return 0;  

    //checks if the file extension is in .st format
    const char *ext = filename + dot_pos + 1;
    if (ext[0] == 's' && ext[1] == 't' && ext[2] == '\0')
        return 1;
    return 0;
}

char *read_file(const char *filename, unsigned int *out_size) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char *buffer = malloc(size + 1);
    if (!buffer) return NULL;

    fread(buffer, 1, size, fp);
    buffer[size] = '\0';   // null-terminate for string logic
    fclose(fp);

    if (out_size) *out_size = size;
    return buffer;
}

void emitToken(Token *tokens, int *tokenCount,
                const char *start, const char *end,
                const char *type)
{
    int length = end - start;

    // copy lexeme substring
    memcpy(tokens[*tokenCount].lexeme, start, length);
    tokens[*tokenCount].lexeme[length] = '\0';

    // remove newlines for pretty print
    for (int i = 0; tokens[*tokenCount].lexeme[i] != '\0'; ++i)
        if (tokens[*tokenCount].lexeme[i] == '\n' || tokens[*tokenCount].lexeme[i] == '\r') 
            tokens[*tokenCount].lexeme[i] = ' ';

    // copy token type
    strcpy(tokens[*tokenCount].type, type);

    (*tokenCount)++;
}

void outputTokens(Token* tokens) {
    FILE *outputFile = fopen("SymbolTable.txt", "w");
    if (!outputFile) {
        printf("Error: Could not create output file.\n");
        return;
    }
    FILE *outputTokenFile = fopen("../Tokens.txt", "w");
    if (!outputTokenFile) {
        printf("Error: Could not create output token file.\n");
        return;
    }

    // File table header
    fprintf(outputFile, "TOKEN TABLE\n");
    fprintf(outputFile, "===========\n\n");
    fprintf(outputFile, "%-4s | %-26s | %-45s\n", "No.", "Type", "Lexeme");
    fprintf(outputFile, "-----+-------------------------+-----------------------------------------------\n");


    // Count tokens and print them
    int count = 0;
    while (tokens[count].type[0] != '\0') {
        count++;
        fprintf(outputFile, "%-4d | %-26s | %-45s\n", count, tokens[count-1].type, tokens[count-1].lexeme);
        fprintf(outputTokenFile, "%s\n", tokens[count-1].type);
    }

    printf("\nTotal tokens: %d\n", count);
    printf("Output saved to: Lexical-Analyzer/SymbolTable.txt\n\n");
    printf("Token types saved to: Tokens.txt\n");

    // Print to file
    fprintf(outputFile, "\nTotal tokens: %d\n", count);

    fclose(outputFile);
}