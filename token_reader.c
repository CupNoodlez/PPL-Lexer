#include "token_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a maximum token count for initial array size
#define INITIAL_CAPACITY 1024

/*
  Read file (SymbolTable.txt) from lexer
  Expected file format: TYPE|LEXEME|LINE\n
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

    // Use a simple, robust loop to read and parse the tokens
    while (fgets(line, sizeof(line), file)) {
        
        // Use sscanf for reliable parsing of a delimited format: TYPE|LEXEME|LINE
        // Note: The %...[^|] part reads non-'|' characters up to the specified width.
        result = sscanf(line, "%25[^|]|%99[^|]|%d", 
                        s->tokens[s->tokenCount].type, 
                        s->tokens[s->tokenCount].lexeme, 
                        &s->tokens[s->tokenCount].line);
        
        // The result should be TYPE|LEXEME|LINE
        if (result == 3) {
            s->tokenCount++;
        } 
        // We skip lines that don't match the expected format (e.g., headers, footers)
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

// Get current token (lookahead)
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

// Move to next token (consume)
void scanner_advance(Scanner* s) {
    if (s->currentIndex < s->tokenCount) {
        // increment if not yet EOF
        s->currentIndex++;
    }
}

// Check if current token matches type (without consuming) 
// current token type is equal to the type that was passed/expected
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
    //The EOF token check in getCurrentToken handles the actual end condition
    return s->currentIndex >= s->tokenCount;
}