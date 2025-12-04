
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

void parse_InputStatement();
void parse_PromptContent();
void parse_TargetID();
void parse_AttributeAccess();
void parse_ChoiceBlock();
void parse_ChoiceList();
void parse_AttributeAccess();
void parse_Attribute();
void parse_Scenario();


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
    
    while (!isAtEnd()) {
        
        if (check("IDENTIFIER")) {
            parse_AssignmentStatement();
        }
        else if (check("ASK") || check("CHOICE")) {
            parse_InputStatement();
        }
        else if (check("NEWLINE") || check("COMMENT") || check("COMMENT_MULTI")) {
            skip_noise_tokens();
            continue;
        }
        else {
            parseError("IDENTIFIER, ASK, CHOICE, or EOF");
        }

        skip_noise_tokens();
    }
}


void parse_InputStatement(){

    skip_noise_tokens();
    printf("Parsing Input Statement...\n");


    if(match("ASK")){
        printf(" -> Consumed ASK keyword.\n");

        if(check("IDENTIFIER")){
            parse_TargetID();
        }

        parse_PromptContent();

        if(!match("AS")){
            parseError("///");  //expect an "as" keyword
        }
        printf(" -> Consumed AS keyword.\n");

        if(!match("IDENTIFIER")){
            parseError("///"); //expect an "identifier" 
        }
        printf(" -> Consumed IDENTIFIER.\n");

    } else if (match("CHOICE")){
        printf(" -> Consumed CHOICE keyword.\n");

        if(!match("AS")){
            parseError("///"); //expect an "as" keyword
        }

        if(!match("IDENTIFIER")){
            parseError("///"); //expect an "identifier" 
        }

         if(!match("COLON")){
            parseError("///"); //expect a "colon" 
        }

        parse_ChoiceBlock();

    } else {
        parseError("///"); //expects an "AS" keyword
    } 
}

void parse_TargetID(){
    parse_AttributeAccess();
    printf(" -> Consumed Target_ID.\n");
}

void parse_PromptContent(){
    if(!match("STRING")){
        parseError("///");
    }
    printf(" -> Consumed STRING.\n");
}

void parse_AttributeAccess() {

    // id is ALWAYS required
    if (!match("IDENTIFIER")) {
        parseError("IDENTIFIER");
    }
    printf(" -> Consumed IDENTIFIER.\n");

    // Optional: .attribute or .scenario
    if (match("DOT")) {
        printf(" -> Consumed DOT.\n");
        if (!match("IDENTIFIER")) {
            parseError("ATTRIBUTE or SCENARIO IDENTIFIER");
        }
        printf(" -> Consumed ATTRIBUTE/SCENARIO IDENTIFIER.\n");
    }
}

void parse_Attribute(){
    if(!match("IDENTIFIER")){
        parseError("///"); //expects an "Identifier"
    } 
    printf(" -> Consumed ATTRIBUTE.\n");
}

void parse_Scenario(){
    if(!match("IDENTIFIER")){
        parseError("///"); //expects an "Identifier"
    } 
    printf(" -> Consumed SCENARIO.\n");
}

void parse_ChoiceBlock(){
    if(!match("INDENT")){
        parseError("///"); //expects an "INDENT"
    }
    printf(" -> Consumed INDENT.\n");

    parse_ChoiceList();

    if(!match("DEDENT")){
        parseError("///"); //expects an "DEDENT"
    }
    printf(" -> Consumed DEDENT.\n");
}

//<choice_list> ::=  { “[“ <string_literal> ":" <literal> “]” “,” "NEWLINE"}
void parse_ChoiceList() {
    printf("Parsing Choice List...\n");

    // Repeated zero or more times
    while (!isAtEnd() && check("LBRACKET")) { 
        advance(); 
        printf(" -> Consumed '['\n");

        if (!match("STRING")) {
            parseError("string literal for choice text");
        }
        printf(" -> Consumed choice text (STRING)\n");


        if (!match("COLON")) { 
            parseError("':' after choice text");
        }
        printf(" -> Consumed ':'\n");

        // <literal> (assume INTEGER or STRING for now)
        if (!(match("INTEGER") || match("STRING"))) {
            parseError("///"); //expects a literal values for choice
        }
        printf(" -> Consumed choice value (literal)\n");

       
        if (!match("RBRACKET")) { 
            parseError("']' to close choice");
        }
        printf(" -> Consumed ']'\n");

   
        if (!match("COMMA")) {
            parseError("',' after choice");
        }
        printf(" -> Consumed ','\n");

  
        if (!match("NEWLINE")) {
            parseError("NEWLINE after choice");
        }
        printf(" -> Consumed NEWLINE\n");
    }

    printf("--- Choice List Parsed ---\n");
}