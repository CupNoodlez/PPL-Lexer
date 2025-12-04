
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
void parse_OutputStatement();
void parse_OutputBody();
void parse_ContentItem();
void parse_OutputBlock();

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
    
    printf("\n=== Parser Syntax Analysis Test ===\n");
    printf("Tokens loaded: %d. Starting parse.\n", token_count);

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
    printf("Parsing Assignment Statement...\n\n");
    
    if (!match("IDENTIFIER")) {
        parseError("IDENTIFIER (for variable token_name)");
    }
    printf(" -> Consumed IDENTIFIER.\n");

    if (!(match("ASSIGN") || match("PLUS_ASSIGN") || match("MINUS_ASSIGN") ||
          match("MULT_ASSIGN") || match("DIV_ASSIGN") || match("MOD_ASSIGN"))) {
        parseError("an assignment operator (=, +=, etc.)");
    }
    printf(" -> Consumed Assignment Operator.\n");

    //parseLiteral ata to pero parang ginagawa na naman na
    if (!(match("INTEGER") || match("STRING") || match("FLOAT") || match("CHAR") )){
        parseError("must be literal"); //expect a literal
    }
    printf(" -> Consumed LITERAL.\n");
    
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
        else if (check("NARRATE") || check("DIALOGUE") || check("SHOW")){
            parse_OutputStatement();
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

/*

<output_stmt> ::= <output_keyword> [target_ID] ":" <ouput_body>
<output_body> ::= "STRING" | <attribute_access>

*/

void parse_OutputStatement() {
    skip_noise_tokens();
    printf("\nParsing Output Statement...\n\n");

    // Parse output keyword
    if(check("NARRATE") || check("DIALOGUE") || check("SHOW")) {
        if(match("NARRATE")) printf(" -> Consumed NARRATE keyword\n");
        else if(match("DIALOGUE")) printf(" -> Consumed DIALOGUE keyword\n");
        else if(match("SHOW")) printf(" -> Consumed SHOW keyword\n");
    } else {
        parseError("NARRATE, DIALOGUE, or SHOW");
    }

    if(check("IDENTIFIER")) {  //optional attribute access
        parse_TargetID();
        printf(" -> Parsed Attribute Access\n");
    }

    if(match("COLON")) {
        printf(" -> Consumed COLON\n");
    } else {
        parseError("COLON"); //expects a colon
    }

    parse_OutputBody();

    printf("--- Output Statement Parsed OK ---\n");
}

void parse_OutputBody(){

    printf("\nParsing Output Body...\n\n");
   if(check("INDENT")){
        parse_OutputBlock();
   } 
   else if (check("STRING") || check("IDENTIFIER")) {
        parse_ContentItem();
   } 
   else {
        parseError("///"); //expect an indent, string, or identifier
   }

    printf("--- Output Body Parsed OK ---\n");
}

void parse_OutputBlock(){
    printf("\nParsing Output Block...\n\n");
    if(!match("INDENT")){
        parseError("///"); //expect indent
    }
    printf(" -> Consumed INDENT\n");

    parse_ContentItem();

    if(!match("DEDENT")){
        parseError("///"); // expect dedent
    }
    printf(" -> Consumed DEDENT\n");

    printf("--- Output Block Parsed OK ---\n");
    
}

void parse_ContentItem(){
    printf("\nParsing Content Item...\n\n");
    if(match("STRING")){
        printf(" -> Consumed STRING\n");
    } 
    else if (check("IDENTIFIER")){
        parse_AttributeAccess();
    } 
    else {
        parseError("STRING or IDENTIFIER"); //expects a string or identifier
    }

    printf("--- Content Item Parsed OK ---\n");
}


void parse_InputStatement(){

    skip_noise_tokens();
    printf("\nParsing Input Statement...\n\n");


    if(check("ASK")){
        match("ASK");
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

    } else if (check("CHOICE")){
        match("CHOICE");
        printf(" -> Consumed CHOICE keyword.\n");

        if(!match("AS")){
            parseError("///"); //expect an "as" keyword
        }
        printf(" -> Consumed AS keyword.\n");

        if(!match("IDENTIFIER")){
            parseError("///"); //expect an "identifier" 
        }
        printf(" -> Consumed IDENTIFIER.\n");

        if(!match("COLON")){
            parseError("///"); //expect a "colon" 
        }
        printf(" -> Consumed COLON.\n");

        if(!match("NEWLINE")){
            parseError("///"); //expect a "newLine" 
        }

        parse_ChoiceBlock();

    } else {
        parseError("///"); //expects an "AS" keyword
    } 

    printf("--- Input Statement Parsed OK ---\n");
}

void parse_TargetID(){
    printf("\nParsing Target ID...\n\n");
    parse_AttributeAccess();
    printf(" -> Consumed Target_ID.\n");

    printf("--- TargetID Parsed OK ---\n");
}

void parse_PromptContent(){
    printf("\nParsing Prompt Content...\n\n");
    if(!match("STRING")){
        parseError("///");
    }
    printf(" -> Consumed STRING.\n");
    printf("--- Prompt Content Parsed OK ---\n");
}

void parse_AttributeAccess() {

    printf("\nParsing Attribute Access...\n\n");
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

    printf("--- Attribute Access Parsed OK ---\n");
}

void parse_Attribute(){
    printf("\nParsing Attribute...\n\n");
    if(!match("IDENTIFIER")){
        parseError("///"); //expects an "Identifier"
    } 
    printf(" -> Consumed ATTRIBUTE.\n");

    printf("--- Attribute Parsed OK ---\n");
}

void parse_Scenario(){
    printf("\nParsing Scenario...\n\n");
    if(!match("IDENTIFIER")){
        parseError("///"); //expects an "Identifier"
    } 
    printf(" -> Consumed SCENARIO.\n");
    printf("--- Scenario Parsed OK ---\n");
}

void parse_ChoiceBlock(){
    skip_noise_tokens();

     printf("\nParsing Choice Block...\n\n");
    if(check("INDENT")){
        match("INDENT");
        printf(" -> Consumed INDENT.\n");
        
        parse_ChoiceList();

        if(!match("DEDENT"))
            printf(" -> Consumed DEDENT.\n");
        else parseError("///"); //expects an DEDENT      
    } else {
        parseError("///"); //expects an "INDENT"
    }
   
    printf("--- Choice Block Parsed OK ---\n");
}

//<choice_list> ::=  { “[“ <string_literal> ":" <literal> “]” “,” "NEWLINE"}
void parse_ChoiceList() {
    printf("\nParsing Choice List...\n\n");

    // Keep parsing until we reach a token that is not part of a choice
    while(!isAtEnd() && check("LBRACKET")) { // Each choice starts with “[“
        if(match("LBRACKET")) {
            printf(" -> Consumed '['\n");

            // Expect string literal
            if(match("STRING")) {
                printf(" -> Consumed STRING_LITERAL: %s\n", get_currentToken()->lexeme);
            } else {
                parseError("STRING_LITERAL");
            }

            // Expect colon
            if(match("COLON")) {
                printf(" -> Consumed ':'\n");
            } else {
                parseError(":");
            }

            // Expect literal
            if(match("STRING")) {
                printf(" -> Consumed LITERAL: %s\n", get_currentToken()->lexeme);
            } else {
                parseError("LITERAL");
            }

            // Expect closing bracket
            if(match("RBRACKET")) {
                printf(" -> Consumed ']'\n");
            } else {
                parseError("]");
            }

            // Expect comma
            if(match("COMMA")) {
                printf(" -> Consumed ','\n");
            } else {
                parseError(",");
            }

            // Expect newline
            if(match("NEWLINE")) {
                printf(" -> Consumed NEWLINE\n");
            } else {
                parseError("NEWLINE");
            }
        }
    }

    printf("--- Choice List Parsed OK ---\n");
}