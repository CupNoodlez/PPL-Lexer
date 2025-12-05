#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../Lexical-Analyzer/LexicalG2PPL.h"

Token *tokens;
int token_count;
int current_pos = 0;

// Panic-mode state
static bool recovering = false;

bool isAtEnd();
Token *currentToken();
Token *nextToken();
bool check(const char *token_name);
void advance();
bool match(const char *token_name);
void parseError(const char *expected);
void skip_noise_tokens();

// NEW: recovery helper
static void recover_to_newline(void);

void parse_AssignmentStatement();
void parse_Program();
void parse_StatementList();
void parse_Statement();
void parse_StatementBlock();
void parse_IDList();
void parse_AttributeAccess();
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

void parse_DeclarationStatement();
void parse_CharacterDeclaration();
void parse_ScenesBlock();
void parse_SceneDeclaration();
void parse_SceneEntry();
void parse_SceneList();
void parse_TemplateDeclaration();

void parse_AttributeBlock();

void parse_AttributeList();

void parse_InputStatement();
void parse_TargetID();
void parse_PromptContent();
void parse_ChoiceBlock();
void parse_ChoiceList();


void parse_InputStatement();
void parse_TargetID();
void parse_PromptContent();
void parse_ChoiceBlock();
void parse_ChoiceList();

void parse_OutputStatement();
void parse_OutputKey();
void parse_Content();
void parse_OutputBody();
void parse_OutputBlock();
void parse_ContentItem();
void parse_Concat();
void parse_ConcatElement();

void parse_ConditionStatement();
void parse_ConditionalTail();
void parse_ElifClause();
void parse_ElseClause();

void parse_IterativeStatement();
void parse_ForStructure();
void parse_CollectionSource();
void parse_LoopVariable();
void parse_RepeatStructure();

int main() {
    char filetoken_name[256];
    printf("Enter the source filetoken_name to parse: ");
    scanf("%255s", filetoken_name);

    tokens = lex_all(filetoken_name, &token_count);
    if (tokens == NULL)
    {
        printf("Lexing failed.\n");
        return 1;
    }
    // // test: print all tokens
    // for (int i = 0; i < token_count; i++) {
    //     printf("Token %d: token_name='%s', Lexeme='%s', Line=%d\n",
    //         i + 1, tokens[i].token_name, tokens[i].lexeme, tokens[i].lineNumber);
    // }

    printf("=== Parser Syntax Analysis Test ===\n");
    printf("Tokens loaded: %d. Starting parse.\n\n", token_count);

    printf("Next token is: %s  Next lexeme is: %s\n", tokens[current_pos].token_name, tokens[current_pos].lexeme);
    parse_Program();

    if (!isAtEnd()) {
        parseError("End-of-File (EOF)");
    }
    else
    {
        printf("\nSUCCESS: All statements consumed and stream is at EOF.\n");
    }

    free(tokens);
    return 0;
}

bool isAtEnd()
{
    return current_pos >= token_count;
}

Token *currentToken()
{
    if (isAtEnd())
        return NULL;
    return &tokens[current_pos];
}

Token *nextToken()
{
    if (current_pos + 1 >= token_count)
        return NULL;
    return &tokens[current_pos + 1];
}

bool check(const char *token_name)
{
    if (isAtEnd())
        return false;
    return strcmp(tokens[current_pos].token_name, token_name) == 0;
}

void advance()
{
    if (!isAtEnd())
        current_pos++;
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

void parseError(const char *expected)
{
    if (isAtEnd())
    {
        fprintf(stderr, "\n--- SYNTAX ERROR ---\n");
        fprintf(stderr, "Unexpected end of input. Expected '%s'.\n", expected);
    }
    else
    {
        Token *curr = currentToken();
        fprintf(stderr, "\n--- SYNTAX ERROR ---\n");
        fprintf(stderr, "Line %d: Expected '%s', but found token [%s] with lexeme '%s'.\n",
                curr->lineNumber, expected, curr->token_name, curr->lexeme);
    }
    fprintf(stderr, "Attempting panic-mode recovery to next NEWLINE...\n");
    fprintf(stderr, "------------------\n");

    // PANIC MODE: do not exit; resync and continue
    recover_to_newline();
}

// Recover by skipping until NEWLINE; consume exactly one NEWLINE and resume
static void recover_to_newline(void)
{
    recovering = true;

    while (!isAtEnd() && !check("NEWLINE")) {
        advance();
    }

    // Consume exactly one NEWLINE to start next statement cleanly
    if (check("NEWLINE")) {
        advance();
        if(!isAtEnd()){
            printf("Recovered. Next token is: %s  Next lexeme is: %s\n",
                   tokens[current_pos].token_name, tokens[current_pos].lexeme);
        }
    }

    recovering = false;
}

// Optional: keep skipping noise when not recovering
void skip_noise_tokens()
{
    while (check("COMMENT") || check("COMMENT_MULTI") || check("NEWLINE"))
    {
        advance();
        if(!isAtEnd()){
            printf("Next token is: %s  Next lexeme is: %s\n", tokens[current_pos].token_name, tokens[current_pos].lexeme);
        }
    }
}

void parse_Program() {
    printf("Enter <program>...\n");
    skip_noise_tokens();
    if(!match("START")){
        parseError("START keyword");
    }
    parse_StatementList();
    if(!match("END")){
        parseError("END keyword");
    }
    printf("<program> (done)\n");
 
}

void parse_StatementList() {
    printf("Enter <statement_list>...\n");
    skip_noise_tokens();
    while (!isAtEnd() && !check("END") && !check("DEDENT")) {
        parse_Statement();
        skip_noise_tokens();
    }
    printf("<statement_list> (done)\n");
}

void parse_Statement() {
    skip_noise_tokens();
    printf("\nEnter <statement>...\n");
   
    while (!check("END") && !check("DEDENT")) {
       
        if (check("IDENTIFIER")) {
            parse_AssignmentStatement();
        }
        else if (check("ASK") || check("CHOICE")) {
            parse_InputStatement();
        }
        else if (check("NARRATE") || check("DIALOGUE") || check("SHOW")){
            parse_OutputStatement();
        }
        else if (check("IF")) {
            parse_ConditionStatement();
        }
        else if (check("FOR") || check("REPEAT")) {
            parse_IterativeStatement();
        }
        else if(check("CHARACTER") || check("SCENE") || check("TEMPLATE")){
            parse_DeclarationStatement();
        }
        else if (check("NEWLINE") || check("COMMENT") || check("COMMENT_MULTI")) {
            skip_noise_tokens();
            continue;
        }
        else {
             parseError("an IDENTIFIER or EOF (unhandled statement token_name)");
        }  
    }

    printf("<statement> (done) \n");
}

void parse_StatementBlock()
{
    skip_noise_tokens();
    printf("Enter <statement_block>\n");
    if (!match("INDENT"))
        parseError("an INDENT");

    parse_StatementList();

    if (!match("DEDENT")) 
        parseError("a DEDENT");

    printf("<statement_block> (done)\n");
}

void parse_IDList()
{
    printf("Enter <id_list>\n");

    if (!match("IDENTIFIER"))
    {
        parseError("an IDENTIFIER");
    }

    while (check("COMMA"))
    {
        match("COMMA");
        if (!match("IDENTIFIER"))
        {
            parseError("an IDENTIFIER");
        }
    }

    skip_noise_tokens();
    printf("<id_list> (done)\n");
}

void parse_AttributeAccess()
{
    printf("Enter <attribute_access>\n");
    // id is ALWAYS required
    if (!match("IDENTIFIER"))
    {
        parseError("IDENTIFIER");
    }

    // Optional: .attribute or .scenario
    if (match("DOT"))
    {
        if (!match("IDENTIFIER"))
        {
            parseError("ATTRIBUTE or SCENARIO IDENTIFIER");
        }
    }

    skip_noise_tokens();
    printf("<attribute_access> (done)\n");
}

void parse_Literal()
{
    printf("Enter <literal>\n");

    // Booleans
    if (check("TRUE") || check("FALSE"))
    {
        if (check("TRUE"))
        {
            if (!match("TRUE"))
                parseError("TRUE");
        }
        else
        {
            if (!match("FALSE"))
                parseError("FALSE");
        }
        return;
    }

    // Numbers
    if (check("INTEGER") || check("FLOAT"))
    {
        if (check("INTEGER"))
        {
            if (!match("INTEGER"))
                parseError("INTEGER");
        }
        else
        {
            if (!match("FLOAT"))
                parseError("FLOAT");
        }

        printf("<literal> (done)\n");
        return;
    }

    // Strings / Chars
    if (check("STRING") || check("CHAR"))
    {
        if (check("STRING"))
        {
            if (!match("STRING"))
                parseError("STRING");
        }
        else
        {
            if (!match("CHAR"))
                parseError("CHAR");
        }

        printf("<literal> (done)\n");
        return;
    }

    parseError("a literal (INTEGER, FLOAT, STRING, CHAR, TRUE, or FALSE)");
}

/* ---- EXPRESSION PARSING ---- */

void parse_Expression()
{
    printf("Enter <expression> \n");

    parse_AndExpr();

    while (check("OR"))
    {
        if (!match("OR"))
        {
            parseError("OR");
        }
        parse_AndExpr();
    }

    skip_noise_tokens();
    printf("<expression> (done) \n");
}

void parse_AndExpr()
{
    printf("Enter <and_expr> \n");

    parse_NotExpr();

    while (check("AND"))
    {
        if (!match("AND"))
        {
            parseError("AND");
        }
        parse_NotExpr();
    }

    skip_noise_tokens();
    printf("<and_expr> (done) \n");
}

void parse_NotExpr()
{
    printf("Enter <not_expr> \n");

    while (check("NOT"))
    {
        if (!match("NOT"))
        {
            parseError("NOT");
        }
    }

    parse_RelationalExpr();

    skip_noise_tokens();
    printf("<not_expr> (done) \n");
}

void parse_RelationalExpr()
{
    printf("Enter <relational_expr> \n");

    parse_ArithmeticExpr();

    if ((match("LESS") || match("LESS_EQUAL") || match("GREATER") ||
         match("GREATER_EQUAL") || match("EQUAL_EQUAL") ||
         match("NOT_EQUAL") || match("IS")))
    {
        parse_ArithmeticExpr();
    }

    skip_noise_tokens();
    printf("<relational_expr> (done) \n");
}

void parse_ArithmeticExpr()
{
    printf("Enter <arithmetic_expr> \n");

    parse_Term();

    while (check("PLUS") || check("MINUS"))
    {
        if (!(match("PLUS") || match("MINUS")))
        {
            parseError("ADDITIVE_OP");
        }
        parse_Term();
    }

    skip_noise_tokens();
    printf("<arithmetic_expr> (done) \n");
}

void parse_Term()
{
    printf("Enter <term> \n");

    parse_PowerExpr();

    while (check("MULTIPLY") || check("DIVIDE") || check("MODULUS"))
    {
        if (!(match("MULTIPLY") || match("DIVIDE") || match("MODULUS")))
        {
            parseError("MULTIPLICATIVE_OP");
        }
        parse_PowerExpr();
    }

    skip_noise_tokens();
    printf("<term> (done) \n");
}

void parse_PowerExpr()
{
    printf("Enter <power_expr> \n");

    parse_UnaryExpr();

    while (check("EXPONENT"))
    {
        if (!match("EXPONENT"))
        {
            parseError("'^'");
        }
        parse_UnaryExpr();
    }

    skip_noise_tokens();
    printf("<power_expr> (done) \n");
}

void parse_UnaryExpr()
{
    printf("Enter <unary_expr> \n");

    while (check("MINUS"))
    {
        if (!match("MINUS"))
        {
            parseError("'-'");
        }
    }

    parse_Factor();

    skip_noise_tokens();
    printf("<unary_expr> (done) \n");
}

void parse_Factor()
{
    printf("Enter <factor> \n");

    if (match("LPAREN"))
    {
        parse_Expression();
        if (!match("RPAREN"))
        {
            parseError("')");
        }

        skip_noise_tokens();
        printf("<factor> (done) \n");
        return;
    }

    if (check("IDENTIFIER"))
    {
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
    printf("Enter <assignment_stmt> \n");
    parse_AttributeAccess();

    if (
        match("ASSIGN") || match("PLUS_ASSIGN") || match("MINUS_ASSIGN") || match("MULT_ASSIGN") || match("DIV_ASSIGN") || match("MOD_ASSIGN"))
    {
    }
    else
    {
        parseError("assignment operator (=, +=, -=, *=, /=, %=)");
    }

    parse_Expression();
    printf("<assignment_statement> (done)\n");
}

/* ---- DECLARATION STATEMENT ---- */
void parse_DeclarationStatement()
{

    printf("Enter <declaration_stmt>\n");

    if (check("CHARACTER"))
    {
        parse_CharacterDeclaration();
    }
    else if (check("SCENE"))
    {
        parse_SceneDeclaration();
    }
    else if (check("TEMPLATE"))
    {
        parse_TemplateDeclaration();
    }
    else
    {
        parseError("Expected 'character', 'scene', or 'template'");
    }
    printf("<declaration_stmt> (done)\n");
}


void parse_TemplateDeclaration()
{
    printf("Enter <template_decl>\n");

    match("TEMPLATE");
    match("IDENTIFIER");

    // Parse Parameters: "(" <param> {"," <param>} ")"
    if (match("LPAREN"))
    {
        if(!match("IDENTIFIER")){
            parseError("IDENTIFIER");
        }

        // Handle EBNF curly braces { "," <param> } using a while loop
        while (check("COMMA"))
        {
            match("COMMA");
            if(!match("IDENTIFIER")){
            parseError("IDENTIFIER");
            }
        }

        if (!match("RPAREN"))
            parseError(")");
    }
    else
    {
        parseError("( after template ID");
    }

    if (!match("COLON"))
        parseError(":");

    // Parse the blocks required by the grammar
    parse_StatementBlock(); 

    printf("<template_decl> (done)\n");
}


void parse_AttributeBlock()
{
    printf("Enter <attribute_block>\n");

    if (!match("NEWLINE"))
    {
        parseError("Expected 'NEWLINE' before attribute block");
        return; // Stop parsing this function to avoid cascading errors
    }

    if (!match("INDENT"))
    {
        parseError("Expected 'INDENT' after newline");
        return;
    }

    parse_AttributeList();

    if (!match("DEDENT"))
    {
        parseError("Expected 'DEDENT' at end of attribute block");
    }

    printf("Exit <attribute_block>\n");
}
void parse_AttributeList()
{
    printf("Enter <attribute_list>\n");

    parse_AttributeAccess();

    if (!match("ASSIGN"))
    {
        parseError("Expected '=' in attribute assignment");
    }

    parse_Literal();

    while (match("NEWLINE"))
    {
        if(check("DEDENT")){
            break;
        }
        
        parse_AttributeAccess();

        if (!match("ASSIGN"))
        {
            parseError("Expected '=' in attribute assignment");
        }

        parse_Literal();
    }

    printf("<attribute_list> (done)\n");
}

// character declaration
void parse_CharacterDeclaration()
{
    printf("Enter <character_decl>\n");

    /* Match 'character' keyword */
    if (!match("CHARACTER"))
    {
        parseError("Expected 'character'");
    }

    /* Both rules start with an identifier */
    if (!check("IDENTIFIER"))
    {
        parseError("Identifier");
    }

    /* 
        Look ahead:

        character id : attribute_block
        character <id_list>

        If next token after IDENTIFIER is COLON → attribute rule
    */

    if (check("IDENTIFIER") && (strcmp(nextToken()->token_name, "COMMA")))
    {
        /* Rule: character id : <attribute_block> */

        match("IDENTIFIER");   // consume id
        match("COLON");        // consume ':'

        parse_AttributeBlock();
    }
    else
    {
        /* Rule: character <id_list> */
        parse_IDList();        // this consumes IDENTIFIER itself
    }

    printf("<character_decl> (done)\n");
}




// ------------- SCENE -----------------------
void parse_SceneDeclaration()
{
    printf("Enter <scene_decl>\n");

    if (match("SCENE"))
    {
        parse_AttributeAccess();
        if (match("COLON"))
        {
            parse_ScenesBlock();
        }
        else
        {
            parseError("Expected ':' after scene ID");
        }
    }
    else
    {
        parseError("Expected identifier");
    }

    printf("<scene_decl> (done)\n");
}

void parse_SceneEntry()
{
    printf("Enter <scene_entry>\n");

    // <scenario>
    parse_AttributeAccess();

    // must have a comma
    if (!match("COMMA"))
        parseError("',' after scenario");

    // must have string literal
    if (check("STRING"))
        match("STRING");
    else if (check("CHAR"))
        match("CHAR");
    else
        parseError("STRING or CHAR literal");

    printf("<scene_entry> (done)\n");
}

void parse_SceneList()
{
    printf("Enter <scene_list>\n");

    /* Parse the first scene entry */
    parse_SceneEntry();

    /* Continue as long as NEWLINE appears */
    while (match("NEWLINE"))
    {
        /* If DEDENT is the next token, stop the loop (don't consume it) */
        if (check("DEDENT"))
        {
            break;
        }

        /* Parse the next scene entry */
        parse_SceneEntry();
    }

    printf("<scene_list> (done)\n");
}

void parse_ScenesBlock()
{
    printf("Enter <scenes_block>\n");

    /* Must begin with NEWLINE */
    if (!match("NEWLINE"))
        parseError("Expected NEWLINE before scenes block");

    /* Must be followed by INDENT */
    if (!match("INDENT"))
        parseError("Expected INDENT at start of scenes block");

    /* Now parse the scene list */
    parse_SceneList();

    /* Must end with DEDENT */
    if (!match("DEDENT"))
        parseError("Expected DEDENT after scenes block");

    printf("<scenes_block> (done)\n");
}

/* ---- OUTPUT STATEMENT ---- */
void parse_OutputStatement() {
    skip_noise_tokens();
     printf("Enter <output_stmt>\n");
 
    // Parse output keyword
    parse_OutputKey();
 
    if(check("IDENTIFIER")) {  //optional attribute access
        parse_TargetID();
    }
 
    if(!match("COLON")) {
        parseError("a COLON"); //expects a colon
    }
 
    parse_OutputBody();
 
    printf("<output_stmt> (done) \n");
}

void parse_OutputKey(){
    printf("Enter <output_key>\n");

    if(match("NARRATE")){
        printf("<output_key> (done)\n");
        return;
    }
    if(match("DIALOGUE")){
        printf("<output_key> (done)\n");
        return;   
    }
    if(match("SHOW")){
        printf("<output_key> (done)\n");
        return;
    }

    parseError("NARRATE, DIALOGUE, or SHOW keyword");
}
 
void parse_OutputBody(){
 
    printf("Enter <output_body>\n");
    skip_noise_tokens();
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
    printf("Enter <output_block>\n");
    if(!match("INDENT")){
        parseError("an INDENT"); 
    }
 
    parse_ContentItem();
    skip_noise_tokens();  

    if(!match("DEDENT")){
        parseError("a DEDENT"); 
    }        
    printf("<output_block> (done) \n");
   
}
 
 
void parse_ContentItem(){
    printf("Enter <content_item>...\n");
 
    parse_Concat();
 
    printf("<content_item> (done) \n");
}
 
 
void parse_Concat(){
    printf("Enter <concat>\n");
 
    parse_ConcatElement();
    while(!isAtEnd() && check("PLUS")){
        match("PLUS");
        parse_ConcatElement();
    }
 
    printf("<concat> (done) \n");
}
 
void parse_ConcatElement(){
    printf("Enter <concat_element>\n");
 
    if(check("IDENTIFIER")){
        parse_AttributeAccess();
    } else {
        parse_Literal();
    }
 
    printf("<concant_element> (done) \n");
}
 

/* ---- INPUT STATEMENT ---- */
void parse_InputStatement(){
 
    skip_noise_tokens();
    printf("Enter <input_stmt>\n");
 
 
    if(check("ASK")){
        match("ASK");
 
        if(check("IDENTIFIER")){
            parse_TargetID();
        }
 
        parse_PromptContent();
 
        if(!match("AS")){
            parseError(" AS Keyword");  //expect an "as" keyword
        }
 
        if(!match("IDENTIFIER")){
            parseError("an IDENTIFIER"); //expect an "identifier"
        }
 
    } else if (check("CHOICE")){
        match("CHOICE");
 
        if(!match("AS")){
            parseError("AS Keyword"); //expect an "as" keyword
        }
 
        if(!match("IDENTIFIER")){
            parseError("an IDENTIFIER"); //expect an "identifier"
        }
 
        if(!match("COLON")){
            parseError("a COLON"); //expect a "colon"
        }
 
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
    printf("Enter <target_id>\n");
    parse_AttributeAccess();
 
    printf("<target_id> (done) \n");
}
 
void parse_PromptContent(){
    printf("Enter <prompt_content>\n");
    if(!match("STRING")){
        parseError("a STRING_LITERAL");
    }
    printf("<prompt_content> (done) \n");
}

void parse_ChoiceBlock(){
    skip_noise_tokens();
 
    printf("Enter <choice_block>\n");
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
    printf("Enter <choice_list>\n");
 
    if(!match("LBRACKET")){
        parseError("expects a RBRACKET [") ;
    }
    printf(" -> Consumed '['\n");
 
    if(!match("STRING")){
        parseError("expects a STRING");
    }
    printf(" -> Consumed STRING_LITERAL: %s\n", currentToken()->lexeme);
 
    if(!match("COLON")) {
        parseError("a COLON");
    }
    printf(" -> Consumed COLON\n");
 
    parse_Literal();
 
    if(!match("RBRACKET")){
        parseError("a BRACKET ]");
    }
    printf(" -> Consumed RBRACKET ]\n");
 
    if(check("COMMA")){
        if(match("COMMA")){
            if(!match("NEWLINE")) parseError("expect a NEWLINE");
            printf(" -> Consumed NEWLINE\n");
            parse_ChoiceList();
        }
    }
 
    printf("<choice_list> (done) \n");
}
/*
---- CONDITION STATEMENT ----
<condition_stmt>       ::= "if" <expression> <conditional_tail>
<conditional_tail>      ::= ":" <statement_block> { <elif_clause> } [ <else_clause> ]
                | "then" <statement>

<elif_clause>               ::= "elif" <expression> ":" <statement_block> 

<else_clause>             ::= "else" ":" <statement_block>
*/

void parse_ConditionStatement(){
    printf("\nEnter <condition_stmt>\n");
 
    if(!match("IF")){
        parseError("an IF keyword"); 
    } 
    parse_Expression();
    parse_ConditionalTail();
 
    printf("<condition_stmt> (done) \n");
}

void parse_ConditionalTail(){
    printf("\nEnter <conditional_tail>\n");
    if(!match("COLON"))
        parseError("a COLON"); 

    parse_StatementBlock();
    while(check("ELIF")){
        parse_ElifClause();
    }
    if(check("ELSE")){
        match("ELSE");
        if(!match("COLON"))
            parseError("a COLON"); 
        parse_StatementBlock();
    }
 
    printf("<conditional_tail> (done) \n");
}

void parse_ElifClause(){
    printf("\nEnter <elif_clause>\n");
    if(!match("ELIF")){
        parseError("an ELIF keyword"); 
    } 
    parse_Expression();
    if(!match("COLON"))
        parseError("a COLON"); 

    parse_StatementBlock();
    printf("<elif_clause> (done) \n");
}
void parse_ElseClause(){
    printf("\nEnter <else_clause>\n");
    if(!match("ELSE")){
        parseError("an ELSE keyword"); 
    } 
    if(!match("COLON"))
        parseError("a COLON"); 

    parse_StatementBlock();
    printf("<else_clause> (done) \n");
}

/* LOOP STATEMENT 

<iterative_stmt>        ::= "for" <for_structure> | "repeat" <repeat_structure>

<for_structure>         ::= <loop_variable> "in" <collection_source> ":" <statement_block>
<collection_source>     ::= <expression> | <output_stmt>
<loop_variable>         ::= <identifier> | <entity_type>

<repeat_structure>      ::= <expression> "times" ":" <statement_block> | 
   "until" <expression> ":" <statement_block>
*/

void parse_IterativeStatement(){
    printf("\nEnter <iterative_stmt>\n");

    if (match("FOR"))
        parse_ForStructure();
    else if (match("REPEAT"))
        parse_RepeatStructure();

    printf("<iterative_stmt> (done) \n");
}
void parse_ForStructure(){
    printf("\nEnter <for_structure>\n");

    parse_LoopVariable();

    if(!match("IN"))
        parseError("an IN keyword");
    parse_CollectionSource();
    if(!match("COLON"))
        parseError("a COLON");
    parse_StatementBlock();

    printf("<for_structure> (done) \n");
}
void parse_CollectionSource(){
    printf("\nEnter <collection_source>\n");
    if(check("NARRATE") || check("DIALOGUE") || check("SHOW")){
        parse_OutputStatement();
    } else {
        parse_Expression();
    }
    printf("<collection_source> (done) \n");
}
void parse_LoopVariable(){
    printf("\nEnter <loop_variable>\n");
    if (match("IDENTIFIER")) {
    } else if (match("CHARACTER") || match("SCENE") || match("TEMPLATE")) {
    } else {
        parseError("an IDENTIFIER or ENTITY_TYPE");
    }
    printf("<loop_variable> (done) \n");
}
void parse_RepeatStructure(){
    printf("\nEnter <repeat_structure>\n");

    if(match("UNTIL")){
        parse_Expression();
        if(!match("COLON"))
            parseError("a COLON");
        parse_StatementBlock();
    } else {
        parse_Expression();
        if(!match("TIMES"))
            parseError("a TIMES keyword");
        if(!match("COLON"))
            parseError("a COLON");
        parse_StatementBlock();
    }

    printf("<repeat_structure> (done) \n");
}
