#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../Lexical-Analyzer/LexicalG2PPL.h"

Token* tokens;
int token_count;
int current_pos = 0;

bool isAtEnd();
Token* currentToken();
bool check(const char* token_name);
void advance();
bool match(const char* token_name);
void parseError(const char* expected);
void skip_noise_tokens();
void parse_AssignmentStatement();
void parse_Program();

int main() {
    char filetoken_name[256];
    printf("Enter the source filetoken_name to parse: ");
    scanf("%255s", filetoken_name);
    
    tokens = lex_all(filetoken_name, &token_count);
    if (tokens == NULL) {
        printf("Lexing failed.\n");
        return 1;
    }
    // test: print all tokens
    // for (int i = 0; i < token_count; i++) {
    //     printf("Token %d: token_name='%s', Lexeme='%s', Line=%d\n", 
    //         i + 1, tokens[i].token_name, tokens[i].lexeme, tokens[i].lineNumber);
    // }
    
    printf("=== Parser Syntax Analysis Test ===\n");
    printf("Tokens loaded: %d. Starting parse.\n\n", token_count);

    parse_Program(); 
    
    if (!isAtEnd()) {
        parseError("End-of-File (EOF)");
    } else {
        printf("\n✓ SUCCESS: All statements consumed and stream is at EOF.\n");
    }

    free(tokens);
    return 0;
}


bool isAtEnd() {
    return current_pos >= token_count;
}

Token* get_currentToken() {
    if (isAtEnd()) return NULL;
    return &tokens[current_pos];
}

bool check(const char* token_name) {  
    if (isAtEnd()) return false;
    return strcmp(tokens[current_pos].token_name, token_name) == 0;
}

void advance() {
    if (!isAtEnd()) current_pos++;
}

bool match(const char* token_name) {
    if (check(token_name)) {
        advance();
        return true;
    }
    return false;
}

void parseError(const char* expected) {
    if (isAtEnd()) {
        fprintf(stderr, "\n--- SYNTAX ERROR ---\n");
        fprintf(stderr, "Unexpected end of input. Expected '%s'.\n", expected);
    } else {
        Token* curr = get_currentToken();
        fprintf(stderr, "\n--- SYNTAX ERROR ---\n");
        fprintf(stderr, "Line %d: Expected '%s', but found token [%s] with lexeme '%s'.\n", 
                curr->lineNumber, expected, curr->token_name, curr->lexeme);
    }
    fprintf(stderr, "------------------\n");
    free(tokens);
    exit(1); 
}

void skip_noise_tokens() {
    while (check("COMMENT") || check("COMMENT_MULTI") || check("NEWLINE")) {
        advance();
    }
}

void parse_AssignmentStatement() {
    printf("Parsing Assignment Statement...\n");
    
    if (!match("IDENTIFIER")) {
        parseError("IDENTIFIER (for variable token_name)");
    }
    printf(" -> Consumed IDENTIFIER.\n");

    if (!(match("ASSIGN") || match("PLUS_ASSIGN") || match("MINUS_ASSIGN") ||
          match("MULT_ASSIGN") || match("DIV_ASSIGN") || match("MOD_ASSIGN"))) {
        parseError("an assignment operator (=, +=, etc.)");
    }
    printf(" -> Consumed Assignment Operator.\n");

    if (!match("INTEGER")) {
        parseError("INTEGER (for value)");
    }
    printf(" -> Consumed INTEGER.\n");
    
    printf("--- Assignment Statement Parsed OK ---\n");
}

void parse_Program() {
    skip_noise_tokens();
    
    while (check("IDENTIFIER")) {
        parse_AssignmentStatement();
        skip_noise_tokens();
    }
    
    if (!isAtEnd()) {
        parseError("an IDENTIFIER or EOF (unhandled statement token_name)");
    }
}

