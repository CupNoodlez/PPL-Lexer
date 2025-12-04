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
Token* nextToken();
bool check(const char* token_name);
void advance();
bool match(const char* token_name);
void parseError(const char* expected);
void skip_noise_tokens();
void parse_AssignmentStatement();
void parse_Program();
void parse_IDList();

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

    printf("Next token is: %s  Next lexeme is: %s\n", tokens[current_pos].token_name, tokens[current_pos].lexeme);
    parse_IDList(); 
    
    if (!isAtEnd()) {
        parseError("End-of-File (EOF)");
    } else {
        printf("\nSUCCESS: All statements consumed and stream is at EOF.\n");
    }

    free(tokens);
    return 0;
}


bool isAtEnd() {
    return current_pos >= token_count;
}

Token* currentToken() {
    if (isAtEnd()) return NULL;
    return &tokens[current_pos];
}

Token* nextToken() {
    if (current_pos + 1 >= token_count) return NULL;
    return &tokens[current_pos + 1];
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
        if(!isAtEnd()){
            printf("Next token is: %s  Next lexeme is: %s\n", tokens[current_pos].token_name, tokens[current_pos].lexeme);
        }
        
        return true;
    }
    return false;
}

void parseError(const char* expected) {
    if (isAtEnd()) {
        fprintf(stderr, "\n--- SYNTAX ERROR ---\n");
        fprintf(stderr, "Unexpected end of input. Expected '%s'.\n", expected);
    } else {
        Token* curr = currentToken();
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

void parse_Program() {
    skip_noise_tokens();
    
    while (check("IDENTIFIER")) {
        // parse_AssignmentStatement();
        skip_noise_tokens();
    }
    
    if (!isAtEnd()) {
        parseError("an IDENTIFIER or EOF (unhandled statement token_name)");
    }
}

void parse_IDList(){
    printf("Enter <id_list>\n");

    if(!match("IDENTIFIER")){
        parseError("an IDENTIFIER");
    }

    while (check("COMMA"))
    {
        match("COMMA");
        if(!match("IDENTIFIER")){
            parseError("an IDENTIFIER");
        }
    }

    skip_noise_tokens();
    printf("<id_list> (done)\n");
}
