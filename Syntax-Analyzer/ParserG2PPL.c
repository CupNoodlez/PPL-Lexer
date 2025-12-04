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
void parse_AssignmentOperator();
void parse_Program();
void parse_IDList();
void parse_AttributeAccess();
void parse_BooleanLiteral();
void parse_Literal();
void parse_Expression();
void parse_AndExpr();
void parse_NotExpr();
void parse_RelationalExpr();
void parse_ArithmeticExpr();
void parse_Term();
void parse_PowerExpr();
void parse_UnaryExpr();
void parse_Factor();

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
    parse_AssignmentStatement();
    
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

void parse_AttributeAccess() {
    printf("Enter <attribute_access>\n");
    // id is ALWAYS required
    if (!match("IDENTIFIER")) {
        parseError("IDENTIFIER");
    }
 
    // Optional: .attribute or .scenario
    if (match("DOT")) {
        if (!match("IDENTIFIER")) {
            parseError("ATTRIBUTE or SCENARIO IDENTIFIER");
        }
    }
 
    skip_noise_tokens();
    printf("<attribute_access> (done)\n");
}

void parse_BooleanLiteral(){
    printf("Enter <boolean_literal>\n");

    if (!check("TRUE") && !check("FALSE")){
        parseError("Boolean Literal");
    }
    advance();

    skip_noise_tokens();
    printf("<boolean_literal> (done)\n");
}

// ...existing code...
void parse_Literal(){
    printf("Enter <literal>\n");

    // Booleans
    if (check("TRUE") || check("FALSE")) {
        parse_BooleanLiteral();
        return;
    }

    // Numbers
    if (check("INTEGER") || check("FLOAT")) {
        if (check("INTEGER")) {
            if (!match("INTEGER")) parseError("INTEGER");
        } else {
            if (!match("FLOAT")) parseError("FLOAT");
        }

        skip_noise_tokens();
        printf("<literal> (done)\n");
        return;
    }

    // Strings / Chars
    if (check("STRING") || check("CHAR")) {
        if (check("STRING")) {
            if (!match("STRING")) parseError("STRING");
        } else {
            if (!match("CHAR")) parseError("CHAR");
        }

        skip_noise_tokens();
        printf("<literal> (done)\n");
        return;
    }

    parseError("a literal (INTEGER, FLOAT, STRING, CHAR, TRUE, or FALSE)");
}

/* ---- EXPRESSION PARSING ---- */
 
void parse_Expression() {
    printf("Enter <expression> \n");
 
    parse_AndExpr();
 
    while (check("OR")) {
        if (!match("OR")){
            parseError("OR");
        }
        parse_AndExpr();
    }
 
    skip_noise_tokens();
    printf("<expression> (done) \n");
}
 
void parse_AndExpr() {
    printf("Enter <and_expr> \n");
 
    parse_NotExpr();
   
    while (check("AND")) {
        if(!match("AND")){
            parseError("AND");
        }
        parse_NotExpr();
    }
   
    skip_noise_tokens();
    printf("<and_expr> (done) \n");
}
 
void parse_NotExpr() {
    printf("Enter <not_expr> \n");
 
    while (check("NOT")) {
        if(!match("NOT")){
            parseError("NOT");
        }
    }
   
    parse_RelationalExpr();
 
    skip_noise_tokens();
    printf("<not_expr> (done) \n");    
}
 
void parse_RelationalExpr() {
    printf("Enter <relational_expr> \n");
 
    parse_ArithmeticExpr();
 
    if ((match("LESS") || match("LESS_EQUAL") || match("GREATER") ||
        match("GREATER_EQUAL") || match("EQUAL_EQUAL") ||
        match("NOT_EQUAL") || match("IS"))) {
            parse_ArithmeticExpr();
    }
 
    skip_noise_tokens();
    printf("<relational_expr> (done) \n");
 
}
 
void parse_ArithmeticExpr() {
    printf("Enter <arithmetic_expr> \n");
 
    parse_Term();
 
    while(check("PLUS") || check("MINUS")) {
        if (!(match("PLUS") || match("MINUS"))) {
            parseError("ADDITIVE_OP");
        }
        parse_Term();
    }
 
    skip_noise_tokens();
    printf("<arithmetic_expr> (done) \n");
}
 
void parse_Term() {
    printf("Enter <term> \n");
 
    parse_PowerExpr();
 
    while(check("MULTIPLY") || check("DIVIDE") || check("MODULUS")) {
        if (!(match("MULTIPLY") || match("DIVIDE") || match("MODULUS"))){
            parseError("MULTIPLICATIVE_OP");
        }
        parse_PowerExpr();
    }
 
    skip_noise_tokens();
    printf("<term> (done) \n");
}
 
void parse_PowerExpr() {
    printf("Enter <power_expr> \n");
 
    parse_UnaryExpr();
 
    while(check("EXPONENT")) {
        if (!match("EXPONENT")){
            parseError("'^'");
        }
        parse_UnaryExpr();
    }
 
    skip_noise_tokens();
    printf("<power_expr> (done) \n");
}
 
void parse_UnaryExpr() {
    printf("Enter <unary_expr> \n");
 
    while(check("MINUS")){
        if(!match("MINUS")){
            parseError("'-'");
        }
    }
 
    parse_Factor();
 
    skip_noise_tokens();
    printf("<unary_expr> (done) \n");
}
 
void parse_Factor() {
    printf("Enter <factor> \n");
 
    if (match("LPAREN")) {
        parse_Expression();
        if (!match("RPAREN")){
            parseError("')");
        }
 
        skip_noise_tokens();
        printf("<factor> (done) \n");
        return;
    }
 
    if(check("IDENTIFIER")) {
        parse_AttributeAccess();
        skip_noise_tokens();
        printf("<factor> (done) \n");
        return;
    }
 
    parse_Literal();
 
    skip_noise_tokens();
    printf("<factor> (done) \n");
}
 
/* ---- EXPRESSION PARSING ---- */

/* ---- ASSIGNMENT STATEMENT ---- */
void parse_AssignmentStatement()
{
    printf("Parsing Assignment Statement...\n");
 
    parse_AttributeAccess();
    parse_AssignmentOperator();
    parse_Expression();
 
    printf("<assignment_statement> (done)\n");
}

void parse_AssignmentOperator()
{
    printf("Enter <assignment_operator>\n");
 
    if (match("ASSIGN"))
    {
        printf("-> ASSIGN\n");
    }
    else if (match("PLUS_ASSIGN"))
    {
        printf("-> PLUS_ASSIGN\n");
    }
    else if (match("MINUS_ASSIGN"))
    {
        printf("-> MINUS_ASSIGN\n");
    }
    else if (match("MULT_ASSIGN"))
    {
        printf("-> MULT_ASSIGN\n");
    }
    else if (match("DIV_ASSIGN"))
    {
        printf("-> DIV_ASSIGN\n");
    }
    else if (match("MOD_ASSIGN"))
    {
        printf("-> MOD_ASSIGN\n");
    }
    else
    {
        parseError("assignment operator (=, +=, -=, *=, /=, %=)");
    }
 
    printf("<assignment_operator> done\n");
}