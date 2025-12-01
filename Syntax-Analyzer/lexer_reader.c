#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

// Token structure 
typedef struct {
    char type[26];
    char lexeme[100];
    int line;
} Token;

// Scanner state structure 
typedef struct {
    Token* tokens;
    int tokenCount;
    int currentIndex;
} Scanner;

// --- Forward Declarations (Public Interface) ---

Scanner* scanner_init(const char* filename);
void scanner_cleanup(Scanner* s);
Token scanner_getCurrentToken(Scanner* s);
void scanner_advance(Scanner* s);
bool scanner_check(Scanner* s, const char* type);
bool scanner_match(Scanner* s, const char* type);
bool scanner_isAtEnd(Scanner* s);


#define INITIAL_CAPACITY 1024

/*
  Read file (SymbolTable.txt) from lexer
  Expected file format: TYPE|LINE|LEXEME
*/
Scanner* scanner_init(const char* filename) {
    FILE* file = fopen(filename, "r");

    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return NULL;
    }

    Scanner* s = (Scanner*)malloc(sizeof(Scanner));
    if (!s) {
        fprintf(stderr, "Error: Memory allocation for Scanner failed.\n");
        fclose(file);
        return NULL;
    }
    
    // Initialize Scanner State
    s->tokenCount = 0;
    s->currentIndex = 0;
    int capacity = INITIAL_CAPACITY;
    s->tokens = (Token*)malloc(capacity * sizeof(Token));

    if (!s->tokens) {
        fprintf(stderr, "Error: Initial memory allocation for tokens failed.\n");
        free(s);
        fclose(file);
        return NULL;
    }

    char line[256];
    int result;

    while (fgets(line, sizeof(line), file)) {
        
        // Remove trailing newline character from fgets to ensure clean parsing
        line[strcspn(line, "\n")] = 0;

        // Skip empty lines
        if (strlen(line) == 0) continue;

        //format: TYPE|LINE|LEXEME
        result = sscanf(line, "%25[^|]|%d|%99[^\n]", 
                        s->tokens[s->tokenCount].type, 
                        &s->tokens[s->tokenCount].line,  
                        s->tokens[s->tokenCount].lexeme);
        
        // We expect exactly 3 items to be matched
        if (result == 3) {
            s->tokenCount++;
    
            if (s->tokenCount >= capacity) {
                break; 
            }
        } 
    }

    fclose(file);
    return s;
}

// Free allocated memory for the Scanner
void scanner_cleanup(Scanner* s) {
    if (s) {
        if (s->tokens) {
            free(s->tokens);
        }
        free(s);
    }
}

// Read the current token (read lookahead)
Token scanner_getCurrentToken(Scanner* s) {
    // For EOF
    if (s->currentIndex >= s->tokenCount) {
        // Return a static EOF token
        static Token eof = {"EOF", "", -1};
        return eof;
    }
    // lookahead 
    return s->tokens[s->currentIndex];
}

// Consume (move the position)
void scanner_advance(Scanner* s) {
    if (s->currentIndex < s->tokenCount) {
        // increment if not yet EOF
        s->currentIndex++;
    }
}

// Test if the current token type matches the type (without consuming) 
bool scanner_check(Scanner* s, const char* type) {
    return strcmp(scanner_getCurrentToken(s).type, type) == 0;
}

// Match token type and advance(consume) if matches
bool scanner_match(Scanner* s, const char* type) {
    if (scanner_check(s, type)) {
        scanner_advance(s);
        return true;
    }
    return false;
}

// Check if at the end of the tokens
bool scanner_isAtEnd(Scanner* s) {
    return s->currentIndex >= s->tokenCount;
}