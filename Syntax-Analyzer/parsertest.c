#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// =======================================================
// === 1. TOKEN READER DECLARATIONS (Must match token_reader.c) ===
// =======================================================

// Token structure (Must match the definition in token_reader.c)
typedef struct {
    char type[26];
    char lexeme[100];
    int line;
} Token;

// Scanner state structure (Must match the definition in token_reader.c)
typedef struct {
    Token* tokens;
    int tokenCount;
    int currentIndex;
} Scanner;

// --- External Function Declarations (Defined in token_reader.c) ---
extern Scanner* scanner_init(const char* filename);
extern void scanner_cleanup(Scanner* s);
extern Token scanner_getCurrentToken(Scanner* s);
extern void scanner_advance(Scanner* s);
extern bool scanner_check(Scanner* s, const char* type);
extern bool scanner_match(Scanner* s, const char* type);
extern bool scanner_isAtEnd(Scanner* s);


// =======================================================
// === 2. PARSER CORE FUNCTIONS ==========================
// =======================================================

// Centralized error reporting function
void parseError(Scanner* s, const char* expected) {
    Token current = scanner_getCurrentToken(s);
    fprintf(stderr, "\n--- SYNTAX ERROR ---\n");
    fprintf(stderr, "Line %d: Expected '%s', but found token [%s] with lexeme '%s'.\n", 
            current.line, expected, current.type, current.lexeme);
    fprintf(stderr, "------------------\n");
    scanner_cleanup(s);
    exit(1); 
}

// Skips tokens that are not part of the structural grammar (comments, newlines, etc.)
void skip_noise_tokens(Scanner* s) {
    while (scanner_check(s, "COMMENT") || 
           scanner_check(s, "COMMENT_MULTI") || 
           scanner_check(s, "NEWLINE"))
    {
        scanner_advance(s);
    }
}

// Function to parse the grammar rule: IDENTIFIER ASSIGN_OP INTEGER
void parse_AssignmentStatement(Scanner* s) {
    printf("Parsing Assignment Statement...\n");
    
    // 1. Parse Left-Hand Side (LHS): <attribute_access> (Simplified to IDENTIFIER)
    if (!scanner_match(s, "IDENTIFIER")) {
        parseError(s, "IDENTIFIER (for variable name)");
    }
    printf(" -> Consumed IDENTIFIER.\n");

    // 2. Parse Assignment Operator: <assignment_operator>
    // We check for all possible simple assignment operators
    if (!(scanner_match(s, "ASSIGN_OP") ||
          scanner_match(s, "ASSIGN_PLUS_ASSIGN") ||
          scanner_match(s, "ASSIGN_MINUS_ASSIGN") ||
          scanner_match(s, "ASSIGN_MULT_ASSIGN") ||
          scanner_match(s, "ASSIGN_DIV_ASSIGN") ||
          scanner_match(s, "ASSIGN_MOD_ASSIGN"))) 
    {
        parseError(s, "an assignment operator (=, +=, etc.)");
    }
    printf(" -> Consumed Assignment Operator.\n");

    // 3. Parse Right-Hand Side (RHS): <expression> (Simplified to INTEGER)
    if (!scanner_match(s, "INTEGER")) {
        parseError(s, "INTEGER (for value)");
    }
    printf(" -> Consumed INTEGER.\n");
    
    printf("--- Assignment Statement Parsed OK ---\n");
}


// Function to parse the entire program (entry point)
void parse_Program(Scanner* s) {
    // CRITICAL: Skip noise before starting
    skip_noise_tokens(s);
    
    if (scanner_isAtEnd(s)) {
        printf("Program is empty or contains only noise.\n");
        return;
    }
    
    // We loop to consume all statements in the file
    while (scanner_check(s, "IDENTIFIER")) {
        parse_AssignmentStatement(s);
        skip_noise_tokens(s); // Skip noise between statements
    }
    
    // If we exit the loop and are not at EOF, it means an unhandled token was found.
    if (!scanner_isAtEnd(s)) {
         parseError(s, "an IDENTIFIER or EOF (unhandled statement type)");
    }
}


// =======================================================
// === 3. MAIN DRIVER ====================================
// =======================================================

int main() {
    printf("=== Parser Syntax Analysis Test ===\n");
    
    // 1. Initialize the Scanner (Token Reader)
    Scanner* s = scanner_init("../SymbolTable.txt"); 

    if (s == NULL) {
        return 1;
    }
    
    printf("Tokens loaded: %d. Starting parse.\n\n", s->tokenCount);

    // 2. Begin Parsing
    parse_Program(s); 
    
    // 3. Final Check: Ensure we hit EOF exactly
    if (!scanner_isAtEnd(s)) {
        // This is primarily for debugging, as parse_Program should catch this.
        parseError(s, "End-of-File (EOF)");
    } else {
        printf("\n✓ SUCCESS: All statements consumed and stream is at EOF.\n");
    }

    // 4. Cleanup
    scanner_cleanup(s);
    return 0;
}