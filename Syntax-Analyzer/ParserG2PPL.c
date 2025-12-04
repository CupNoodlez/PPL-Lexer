
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
void parse_AssignmentOperator();
void parse_Program();
void parse_InputStatement();
void parse_PromptContent();
void parse_TargetID();
void parse_AttributeAccess();
void parse_ChoiceBlock();
void parse_ChoiceList();
void parse_Attribute();
void parse_Scenario();
void parse_OutputStatement();
void parse_OutputBody();
void parse_ContentItem();
void parse_OutputBlock();
void parse_Content();
void parse_ConcatElement();
void parse_Concat();
void parse_IDList();
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

void parse_AssignmentStatement()
{
    printf("\nEnter <assignment_stmt> \n");
 
    parse_AttributeAccess();
    parse_AssignmentOperator();
    parse_Expression();
 
    printf("<assignment_statement> (done)\n");
}

void parse_AssignmentOperator()
{
    printf("\nEnter <assignment_operator>\n");
 
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

void parse_Program() {
    skip_noise_tokens();
    printf("\nEnter <program>...\n");
    
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
            parseError("an IDENTIFIER or EOF (unhandled statement token_name)");
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
     printf("\nEnter <output_stmt>...\n");

    // Parse output keyword
    if(check("NARRATE") || check("DIALOGUE") || check("SHOW")) {
        if(match("NARRATE")) printf(" -> Consumed NARRATE keyword\n");
        else if(match("DIALOGUE")) printf(" -> Consumed DIALOGUE keyword\n");
        else if(match("SHOW")) printf(" -> Consumed SHOW keyword\n");
    } else {
        parseError("a NARRATE, DIALOGUE, or SHOW");
    }

    if(check("IDENTIFIER")) {  //optional attribute access
        parse_TargetID();
    }

    if(match("COLON")) {
        printf(" -> Consumed COLON\n");
    } else {
        parseError("a COLON"); //expects a colon
    }

    parse_OutputBody();

    printf("<outuput_stmt> (done) \n");
}

void parse_OutputBody(){

    printf("\nEnter <output_body>...\n");
   if(check("INDENT")){
        parse_OutputBlock();
   } 
   else if (check("STRING") || check("IDENTIFIER")) {
        parse_ContentItem();
   } 
   else {
        parseError("an INDENT, STRING, or IDENTIFIER"); //expect an indent, string, or identifier
   }

    printf("<output_body> (done) \n");
}

void parse_OutputBlock(){
    printf("\nEnter <output_block>...\n");
    if(!match("INDENT")){
        parseError("an INDENT"); //expect indent
    }
    printf(" -> Consumed INDENT\n");

    parse_ContentItem();

    if(!match("DEDENT")){
        parseError("a DEDENT"); // expect dedent
    }
    printf(" -> Consumed DEDENT\n");

    printf("<output_block> (done) \n");
    
}


void parse_ContentItem(){
    printf("\nEnter <content_item>...\n");

    parse_Concat();

    printf("<content_item> (done) \n");
}


void parse_Concat(){
    printf("\nEnter <concat>...\n");

    parse_ConcatElement();
    while(!isAtEnd() && check("PLUS")){
        match("PLUS");
        parse_ConcatElement();
    }

    printf("<concat> (done) \n");
}

void parse_ConcatElement(){
    printf("\nEnter <concat_element>...\n");

    if(check("IDENTIFIER")){
        parse_AttributeAccess();
    } else {
        parse_Literal();
    }

    printf("<concant_element> (done) \n");
}



void parse_InputStatement(){

    skip_noise_tokens();
    printf("\nEnter <input_stmt>...\n");


    if(check("ASK")){
        match("ASK");
        printf(" -> Consumed ASK keyword.\n");

        if(check("IDENTIFIER")){
            parse_TargetID();
        }

        parse_PromptContent();

        if(!match("AS")){
            parseError(" AS Keyword");  //expect an "as" keyword
        }
        printf(" -> Consumed AS keyword.\n");

        if(!match("IDENTIFIER")){
            parseError("an IDENTIFIER"); //expect an "identifier" 
        }
        printf(" -> Consumed IDENTIFIER.\n");

    } else if (check("CHOICE")){
        match("CHOICE");
        printf(" -> Consumed CHOICE keyword.\n");

        if(!match("AS")){
            parseError("AS Keyword"); //expect an "as" keyword
        }
        printf(" -> Consumed AS keyword.\n");

        if(!match("IDENTIFIER")){
            parseError("an IDENTIFIER"); //expect an "identifier" 
        }
        printf(" -> Consumed IDENTIFIER.\n");

        if(!match("COLON")){
            parseError("a COLON"); //expect a "colon" 
        }
        printf(" -> Consumed COLON.\n");

        if(!match("NEWLINE")){
            parseError("a NEWLINE"); //expect a "newLine" 
        }

        parse_ChoiceBlock();

    } else {
        parseError("a CHOICE Keyword"); //expects an "CHOICe" keyword
    } 

    printf("<input_stmt> (done) \n");
}

void parse_TargetID(){
    printf("\nEnter <target_id>...\n");
    parse_AttributeAccess();

    printf("<target_id> (done) \n");
}

void parse_PromptContent(){
    printf("\nEnter <prompt_content>...\n");
    if(!match("STRING")){
        parseError("a STRING_LITERAL");
    }
    printf(" -> Consumed STRING.\n");
    printf("<prompt_content> (done) \n");
}

void parse_AttributeAccess() {

   printf("\nEnter <attribute_access>...\n");
    // id is ALWAYS required
    if (!match("IDENTIFIER")) {
        parseError("an IDENTIFIER");
    }
    printf(" -> Consumed IDENTIFIER.\n");

    // Optional: .attribute or .scenario
    if (match("DOT")) {
        printf(" -> Consumed DOT.\n");
        if (!match("IDENTIFIER")) {
            parseError("an ATTRIBUTE or SCENARIO IDENTIFIER");
        }
        printf(" -> Consumed ATTRIBUTE/SCENARIO IDENTIFIER.\n");
    }

    printf("<attribute_access> (done) \n");
}

void parse_Attribute(){
    printf("\nEnter <attribute>...\n");
    if(!match("IDENTIFIER")){
        parseError("an IDENTIFIER"); //expects an "Identifier"
    } 
    printf(" -> Consumed ATTRIBUTE.\n");

    printf("<attribute_id> (done) \n");
}

void parse_Scenario(){
    printf("\nEnter <scenario>...\n");
    if(!match("IDENTIFIER")){
        parseError("an IDENTIFIER"); //expects an "Identifier"
    } 
    printf(" -> Consumed SCENARIO.\n");
    printf("<scenario_id> (done) \n");
}

void parse_ChoiceBlock(){
    skip_noise_tokens();

    printf("\nEnter <choice_block>...\n");
    if(check("INDENT")){
        match("INDENT");
        printf(" -> Consumed INDENT.\n");
        
        parse_ChoiceList();

        if(!match("DEDENT"))
            printf(" -> Consumed DEDENT.\n");
        else parseError("a DEDENT"); //expects an DEDENT      
    } else {
        parseError("an INDENT"); //expects an "INDENT"
    }
   
    printf("<choice_block> (done) \n");
}

//<choice_list> ::=  { “[“ <string_literal> ":" <literal> “]” “,” "NEWLINE"}
void parse_ChoiceList() {
    printf("\nEnter <choice_list>...\n");

    // Keep parsing until we reach a token that is not part of a choice
    while(!isAtEnd() && check("LBRACKET")) { // Each choice starts with “[“
        if(match("LBRACKET")) {
            printf(" -> Consumed '['\n");

            // Expect string literal
            if(match("STRING")) {
                printf(" -> Consumed STRING_LITERAL: %s\n", get_currentToken()->lexeme);
            } else {
                parseError("a STRING_LITERAL");
            }

            // Expect colon
            if(match("COLON")) {
                printf(" -> Consumed ':'\n");
            } else {
                parseError("a COLON");
            }

            // Expect literal
            if(match("STRING")) {
                printf(" -> Consumed LITERAL: %s\n", get_currentToken()->lexeme);
            } else {
                parseError("a STRING_LITERAL");
            }

            // Expect closing bracket
            if(match("RBRACKET")) {
                printf(" -> Consumed ']'\n");
            } else {
                parseError("a BRACKET ]");
            }

            // Expect comma
            if(match("COMMA")) {
                printf(" -> Consumed ','\n");
            } else {
                parseError("a COMMA");
            }

            // Expect newline
            if(match("NEWLINE")) {
                printf(" -> Consumed NEWLINE\n");
            } else {
                parseError("a NEWLINE");
            }
        }
    }

    printf("<choice_list> (done) \n");
}

void parse_IDList(){
    printf("\nEnter <id_list>\n");

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


void parse_BooleanLiteral(){
    printf("\nEnter <boolean_literal>\n");

    if (!check("TRUE") && !check("FALSE")){
        parseError("a Boolean Literal");
    }
    advance();

    skip_noise_tokens();
    printf("<boolean_literal> (done)\n");
}

// ...existing code...
void parse_Literal(){
    printf("\nEnter <literal>\n");

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
    printf("\nEnter <expression> \n");
 
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
    printf("\nEnter <and_expr> \n");
 
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
    printf("\nEnter <not_expr> \n");
 
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
    printf("\nEnter <relational_expr> \n");
 
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
    printf("\nEnter <arithmetic_expr> \n");
 
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
    printf("\nEnter <term> \n");
 
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
    printf("\nEnter <power_expr> \n");
 
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
    printf("\nEnter <unary_expr> \n");
 
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
    printf("\nEnter <factor> \n");
 
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