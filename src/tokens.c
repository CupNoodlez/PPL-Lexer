#include "tokens.h"
#include <stdlib.h>

// Delimiters
const char* delimiters[] = {
    ",", ":", ".", "(", ")", "[", "]"
};

// Operators
const char* operators[] = {
    "+", "-", "*", "/", "%", "=", "!", "<", ">"
};

// Character sets for identifier and number validation
const char digits[] = "0123456789";
const char lowercase[] = "abcdefghijklmnopqrstuvwxyz";
const char uppercase[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

const int delim_count = sizeof(delimiters) / sizeof(delimiters[0]);
const int op_count = sizeof(operators) / sizeof(operators[0]);

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

bool isAlpha(char c) {
    return isInSet(c, lowercase) || isInSet(c, uppercase);
}

bool isDigit(char c) {
    return isInSet(c, digits);
}

const char* getKeyword(const char* lexeme) {
    // Check regular keywords
    if (strEqual(lexeme, "character")) return KEYWORD_CHARACTER;
    if (strEqual(lexeme, "scene")) return KEYWORD_SCENE;
    if (strEqual(lexeme, "template")) return KEYWORD_TEMPLATE;
    if (strEqual(lexeme, "dialogue")) return KEYWORD_DIALOGUE;
    if (strEqual(lexeme, "narrate")) return KEYWORD_NARRATE;
    if (strEqual(lexeme, "choice")) return KEYWORD_CHOICE;
    if (strEqual(lexeme, "option")) return KEYWORD_OPTION;
    if (strEqual(lexeme, "ask")) return KEYWORD_ASK;
    if (strEqual(lexeme, "if")) return KEYWORD_IF;
    if (strEqual(lexeme, "else")) return KEYWORD_ELSE;
    if (strEqual(lexeme, "elif")) return KEYWORD_ELIF;
    if (strEqual(lexeme, "repeat")) return KEYWORD_REPEAT;
    if (strEqual(lexeme, "for")) return KEYWORD_FOR;
    if (strEqual(lexeme, "show")) return KEYWORD_SHOW;
    if (strEqual(lexeme, "start")) return KEYWORD_START;
    if (strEqual(lexeme, "end")) return KEYWORD_END;
    if (strEqual(lexeme, "becomes")) return KEYWORD_BECOMES;
    if (strEqual(lexeme, "and")) return KEYWORD_AND;
    if (strEqual(lexeme, "not")) return KEYWORD_NOT;
    if (strEqual(lexeme, "or")) return KEYWORD_OR;
    if (strEqual(lexeme, "is")) return KEYWORD_IS;
    if (strEqual(lexeme, "until")) return KEYWORD_UNTIL;
    if (strEqual(lexeme, "times")) return KEYWORD_TIMES;

    
    return NULL;  // Not a keyword
}

const char* getReservedKeyword(const char* lexeme) {

    if (strEqual(lexeme, "true")) return RES_KEY_TRUE;
    if (strEqual(lexeme, "false")) return RES_KEY_FALSE;
    if (strEqual(lexeme, "return")) return RES_KEY_RETURN;
    if (strEqual(lexeme, "in")) return RES_KEY_IN;
    if (strEqual(lexeme, "error")) return RES_KEY_ERROR;
    if (strEqual(lexeme, "fixed")) return RES_KEY_FIXED;
    if (strEqual(lexeme, "break")) return RES_KEY_BREAK;
    if (strEqual(lexeme, "continue")) return RES_KEY_CONTINUE;
    if (strEqual(lexeme, "pass")) return RES_KEY_PASS;
    if (strEqual(lexeme, "with")) return RES_KEY_WITH;
    
    return NULL;  // Not a reserved keyword
}

const char* getNoiseWord(const char* lexeme) {
    if (strEqual(lexeme, "the")) return NOISE_THE;
    if (strEqual(lexeme, "a")) return NOISE_A;
    if (strEqual(lexeme, "an")) return NOISE_AN;
    if (strEqual(lexeme, "as")) return NOISE_AS;
    if (strEqual(lexeme, "then")) return NOISE_THEN;
    if (strEqual(lexeme, "of")) return NOISE_OF;
    if (strEqual(lexeme, "to")) return NOISE_TO;
    return NULL;
}

const char* getRelationalOp(const char* lexeme) {
    if (strEqual(lexeme, "==")) return RELATIONAL_EQUAL_EQUAL;
    if (strEqual(lexeme, "!=")) return RELATIONAL_NOT_EQUAL;
    if (strEqual(lexeme, ">")) return RELATIONAL_GREATER;
    if (strEqual(lexeme, ">=")) return RELATIONAL_GREATER_EQUAL;
    if (strEqual(lexeme, "<")) return RELATIONAL_LESS;
    if (strEqual(lexeme, "<=")) return RELATIONAL_LESS_EQUAL;
    return NULL;
}

const char* getArithmeticOp(const char* lexeme) {
    if (strEqual(lexeme, "+")) return ARITHMETIC_PLUS;
    if (strEqual(lexeme, "-")) return ARITHMETIC_MINUS;
    if (strEqual(lexeme, "*")) return ARITHMETIC_MULTIPLY;
    if (strEqual(lexeme, "//")) return ARITHMETIC_FLOOR_DIVIDE;
    if (strEqual(lexeme, "/")) return ARITHMETIC_DIVIDE;
    if (strEqual(lexeme, "%")) return ARITHMETIC_MODULUS;
    if (strEqual(lexeme, "^")) return ARITHMETIC_EXPONENT;
    return NULL;
}

const char* getAssignmentOp(const char* lexeme) {
    if (strEqual(lexeme, "=")) return ASSIGNMENT_ASSIGN;
    if (strEqual(lexeme, "+=")) return ASSIGNMENT_PLUS_ASSIGN;
    if (strEqual(lexeme, "-=")) return ASSIGNMENT_MINUS_ASSIGN;
    if (strEqual(lexeme, "*=")) return ASSIGNMENT_MULT_ASSIGN;
    if (strEqual(lexeme, "/=")) return ASSIGNMENT_DIV_ASSIGN;
    if (strEqual(lexeme, "%=")) return ASSIGNMENT_MOD_ASSIGN;
    return NULL;
}

const char* getDelimiter(const char* lexeme) {
    if (strEqual(lexeme, "(")) return LPAREN;
    if (strEqual(lexeme, ")")) return RPAREN;
    if (strEqual(lexeme, "[")) return LBRACKET;
    if (strEqual(lexeme, "]")) return RBRACKET;
    if (strEqual(lexeme, ":")) return COLON;
    if (strEqual(lexeme, ",")) return COMMA;
    if (strEqual(lexeme, ".")) return DOT;
    return NULL;
}

bool isMultilineComment(const char* lexeme){
    if (lexeme[0] == '\0')
        return false;
    if (lexeme[0] == '#' && lexeme[1] == '#'){
        return true;
    }else{
        return false;
    }
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

bool isIntegerLiteral(const char* lexeme) {
    if (lexeme[0] == '\0') return false;
    
    // All characters must be digits
    for (int i = 0; lexeme[i] != '\0'; i++) {
        if (!isInSet(lexeme[i], digits)) {
            return false;
        }
    }
    
    return true;
}

bool isFloatLiteral(const char* lexeme) {
    if (lexeme[0] == '\0') return false;
    
    bool hasDot = false;
    
    for (int i = 0; lexeme[i] != '\0'; i++) {
        if (lexeme[i] == '.') {
            if (hasDot) return false;  // More than one dot
            hasDot = true;
        } else if (!isInSet(lexeme[i], digits)) {
            return false;  // Non-digit, non-dot character
        }
    }
    
    // Must have at least one dot to be a float
    return hasDot;
}

bool isNumber(const char* lexeme) {
    return isIntegerLiteral(lexeme) || isFloatLiteral(lexeme);
}

bool isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool isDelimiter(char c) {
    char str[2] = {c, '\0'};
    
    for (int i = 0; i < delim_count; i++) {
        if (strEqual(str, delimiters[i])) {
            return true;
        }
    }
    
    return false;
}

bool isOperator(char c) {
    char str[2] = {c, '\0'};
    
    for (int i = 0; i < op_count; i++) {
        if (strEqual(str, operators[i])) {
            return true;
        }
    }
    
    return false;
}

const char* getTokenType(const char* lexeme) {
    const char* kw = getKeyword(lexeme);
    if (kw) return kw;
    
    const char* res = getReservedKeyword(lexeme);
    if (res) return res;  // Return the specific reserved keyword token type
    
    const char* arith = getArithmeticOp(lexeme);
    if (arith) return arith;  // Return specific arithmetic operator
    
    const char* rel = getRelationalOp(lexeme);
    if (rel) return rel;  // Return specific relational operator
    
    const char* assign = getAssignmentOp(lexeme);
    if (assign) return assign;  // Return specific assignment operator
    
    const char* delim = getDelimiter(lexeme);
    if (delim) return delim;  // Return specific delimiter
    
    const char* noise = getNoiseWord(lexeme);
    if (noise) return noise;  // Return specific noise word token
    
    if (isMultilineComment(lexeme)) return MULTILINE_COMMENT;
    if (isComment(lexeme)) return COMMENT;
    if (isStringLiteral(lexeme)) return STRING_LITERAL;
    if (isIdentifier(lexeme)) return IDENTIFIER;
    if (isFloatLiteral(lexeme)) return FLOAT_LITERAL;
    if (isIntegerLiteral(lexeme)) return INTEGER_LITERAL;
    return INVALID;
}