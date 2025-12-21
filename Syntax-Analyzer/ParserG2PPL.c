#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../Lexical-Analyzer/LexicalG2PPL.h"

FILE *outputFile;

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
    outputFile = fopen("SyntaxAnalysis.txt", "w");
    if (outputFile == NULL) {
        fprintf(stderr, "Error opening SyntaxAnalysis.txt for writing.\n");
        return 1;
    }

    char filetoken_name[256];
    printf("Enter the source filetoken_name to parse: ");
    scanf("%255s", filetoken_name);

    tokens = lex_all(filetoken_name, &token_count);
    if (tokens == NULL)
    {
        fprintf(outputFile, "Lexing failed.\n");
        fclose(outputFile);
        return 1;
    }
    // // test: print all tokens
    // for (int i = 0; i < token_count; i++) {
    //     fprintf(outputFile, "Token %d: token_name='%s', Lexeme='%s', Line=%d\n",
    //         i + 1, tokens[i].token_name, tokens[i].lexeme, tokens[i].lineNumber);
    // }

    fprintf(outputFile, "=== Parser Syntax Analysis Test ===\n");
    fprintf(outputFile, "Tokens loaded: %d. Starting parse.\n\n", token_count);

    fprintf(outputFile, "Next token is: %s  Next lexeme is: %s\n", tokens[current_pos].token_name, tokens[current_pos].lexeme);
    parse_Program();

    if (!isAtEnd()) {
        parseError("End-of-File (EOF)");
    }
    else
    {
        fprintf(outputFile, "\nSUCCESS: All statements consumed and stream is at EOF.\n");
    }

    printf("Parsing complete. See SyntaxAnalysis.txt for details.\n");
    free(tokens);
    fclose(outputFile);
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
            fprintf(outputFile, "Next token is: %s  Next lexeme is: %s\n", tokens[current_pos].token_name, tokens[current_pos].lexeme);
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
    fprintf(stderr, "-------------------\n");

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
            fprintf(outputFile, "Recovered. Next token is: %s  Next lexeme is: %s\n",
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
            fprintf(outputFile, "Next token is: %s  Next lexeme is: %s\n", tokens[current_pos].token_name, tokens[current_pos].lexeme);
        }
    }
}

void parse_Program() {
    fprintf(outputFile, "Enter <program>...\n");
    skip_noise_tokens();
    if(!match("START")){
        parseError("START keyword");
    }
    parse_StatementList();
    if(!match("END")){
        parseError("END keyword");
    }
    fprintf(outputFile, "<program> (done)\n");
 
}

void parse_StatementList() {
    fprintf(outputFile, "Enter <statement_list>...\n");
    skip_noise_tokens();
    while (!isAtEnd() && !check("END") && !check("DEDENT")) {
        parse_Statement();
        skip_noise_tokens();
    }
    fprintf(outputFile, "<statement_list> (done)\n");
}

void parse_Statement() {
    skip_noise_tokens();
    fprintf(outputFile, "\nEnter <statement>...\n");
   
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

    fprintf(outputFile, "<statement> (done) \n");
}

void parse_StatementBlock()
{
    skip_noise_tokens();
    fprintf(outputFile, "Enter <statement_block>\n");
    if (!match("INDENT"))
        parseError("an INDENT");

    parse_StatementList();

    if (!match("DEDENT")) 
        parseError("a DEDENT");

    fprintf(outputFile, "<statement_block> (done)\n");
}

void parse_IDList()
{
    fprintf(outputFile, "Enter <id_list>\n");

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
    fprintf(outputFile, "<id_list> (done)\n");
}

void parse_AttributeAccess()
{
    fprintf(outputFile, "Enter <attribute_access>\n");
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
    fprintf(outputFile, "<attribute_access> (done)\n");
}

void parse_Literal()
{
    fprintf(outputFile, "Enter <literal>\n");

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

        fprintf(outputFile, "<literal> (done)\n");
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

        fprintf(outputFile, "<literal> (done)\n");
        return;
    }

    parseError("a literal (INTEGER, FLOAT, STRING, CHAR, TRUE, or FALSE)");
}

/* ---- EXPRESSION PARSING ---- */

void parse_Expression()
{
    fprintf(outputFile, "Enter <expression> \n");

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
    fprintf(outputFile, "<expression> (done) \n");
}

void parse_AndExpr()
{
    fprintf(outputFile, "Enter <and_expr> \n");

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
    fprintf(outputFile, "<and_expr> (done) \n");
}

void parse_NotExpr()
{
    fprintf(outputFile, "Enter <not_expr> \n");

    while (check("NOT"))
    {
        if (!match("NOT"))
        {
            parseError("NOT");
        }
    }

    parse_RelationalExpr();

    skip_noise_tokens();
    fprintf(outputFile, "<not_expr> (done) \n");
}

void parse_RelationalExpr()
{
    fprintf(outputFile, "Enter <relational_expr> \n");

    parse_ArithmeticExpr();

    if ((match("LESS") || match("LESS_EQUAL") || match("GREATER") ||
         match("GREATER_EQUAL") || match("EQUAL_EQUAL") ||
         match("NOT_EQUAL") || match("IS")))
    {
        parse_ArithmeticExpr();
    }

    skip_noise_tokens();
    fprintf(outputFile, "<relational_expr> (done) \n");
}

void parse_ArithmeticExpr()
{
    fprintf(outputFile, "Enter <arithmetic_expr> \n");

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
    fprintf(outputFile, "<arithmetic_expr> (done) \n");
}

void parse_Term()
{
    fprintf(outputFile, "Enter <term> \n");

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
    fprintf(outputFile, "<term> (done) \n");
}

void parse_PowerExpr()
{
    fprintf(outputFile, "Enter <power_expr> \n");

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
    fprintf(outputFile, "<power_expr> (done) \n");
}

void parse_UnaryExpr()
{
    fprintf(outputFile, "Enter <unary_expr> \n");

    while (check("MINUS"))
    {
        if (!match("MINUS"))
        {
            parseError("'-'");
        }
    }

    parse_Factor();

    skip_noise_tokens();
    fprintf(outputFile, "<unary_expr> (done) \n");
}

void parse_Factor()
{
    fprintf(outputFile, "Enter <factor> \n");

    if (match("LPAREN"))
    {
        parse_Expression();
        if (!match("RPAREN"))
        {
            parseError("')");
        }

        skip_noise_tokens();
        fprintf(outputFile, "<factor> (done) \n");
        return;
    }

    if (check("IDENTIFIER"))
    {
        parse_AttributeAccess();
        skip_noise_tokens();
        fprintf(outputFile, "<factor> (done) \n");
        return;
    }

    parse_Literal();

    skip_noise_tokens();
    fprintf(outputFile, "<factor> (done) \n");
}

/* ---- EXPRESSION PARSING ---- */

/* ---- ASSIGNMENT STATEMENT ---- */
void parse_AssignmentStatement()
{
    fprintf(outputFile, "Enter <assignment_stmt> \n");
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
    fprintf(outputFile, "<assignment_statement> (done)\n");
}

/* ---- DECLARATION STATEMENT ---- */
void parse_DeclarationStatement()
{

    fprintf(outputFile, "Enter <declaration_stmt>\n");

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
    fprintf(outputFile, "<declaration_stmt> (done)\n");
}


void parse_TemplateDeclaration()
{
    fprintf(outputFile, "Enter <template_decl>\n");

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

    fprintf(outputFile, "<template_decl> (done)\n");
}


void parse_AttributeBlock()
{
    fprintf(outputFile, "Enter <attribute_block>\n");

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

    fprintf(outputFile, "Exit <attribute_block>\n");
}
void parse_AttributeList()
{
    fprintf(outputFile, "Enter <attribute_list>\n");

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

    fprintf(outputFile, "<attribute_list> (done)\n");
}

// character declaration
void parse_CharacterDeclaration()
{
    fprintf(outputFile, "Enter <character_decl>\n");

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

    fprintf(outputFile, "<character_decl> (done)\n");
}




// ------------- SCENE -----------------------
void parse_SceneDeclaration()
{
    fprintf(outputFile, "Enter <scene_decl>\n");

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

    fprintf(outputFile, "<scene_decl> (done)\n");
}

void parse_SceneEntry()
{
    fprintf(outputFile, "Enter <scene_entry>\n");

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

    fprintf(outputFile, "<scene_entry> (done)\n");
}

void parse_SceneList()
{
    fprintf(outputFile, "Enter <scene_list>\n");

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

    fprintf(outputFile, "<scene_list> (done)\n");
}

void parse_ScenesBlock()
{
    fprintf(outputFile, "Enter <scenes_block>\n");

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

    fprintf(outputFile, "<scenes_block> (done)\n");
}

/* ---- OUTPUT STATEMENT ---- */
void parse_OutputStatement() {
    skip_noise_tokens();
     fprintf(outputFile, "Enter <output_stmt>\n");
 
    // Parse output keyword
    parse_OutputKey();
 
    if(check("IDENTIFIER")) {  //optional attribute access
        parse_TargetID();
    }
 
    if(!match("COLON")) {
        parseError("a COLON"); //expects a colon
    }
 
    parse_OutputBody();
 
    fprintf(outputFile, "<output_stmt> (done) \n");
}

void parse_OutputKey(){
    fprintf(outputFile, "Enter <output_key>\n");

    if(match("NARRATE")){
        fprintf(outputFile, "<output_key> (done)\n");
        return;
    }
    if(match("DIALOGUE")){
        fprintf(outputFile, "<output_key> (done)\n");
        return;   
    }
    if(match("SHOW")){
        fprintf(outputFile, "<output_key> (done)\n");
        return;
    }

    parseError("NARRATE, DIALOGUE, or SHOW keyword");
}
 
void parse_OutputBody(){
 
    fprintf(outputFile, "Enter <output_body>\n");
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
 
    fprintf(outputFile, "<output_body> (done) \n");
}
 
void parse_OutputBlock(){
    fprintf(outputFile, "Enter <output_block>\n");
    if(!match("INDENT")){
        parseError("an INDENT"); 
    }
 
    parse_ContentItem();
    skip_noise_tokens();  

    if(!match("DEDENT")){
        parseError("a DEDENT"); 
    }        
    fprintf(outputFile, "<output_block> (done) \n");
   
}
 
 
void parse_ContentItem(){
    fprintf(outputFile, "Enter <content_item>...\n");
 
    parse_Concat();
 
    fprintf(outputFile, "<content_item> (done) \n");
}
 
 
void parse_Concat(){
    fprintf(outputFile, "Enter <concat>\n");
 
    parse_ConcatElement();
    while(!isAtEnd() && check("PLUS")){
        match("PLUS");
        parse_ConcatElement();
    }
 
    fprintf(outputFile, "<concat> (done) \n");
}
 
void parse_ConcatElement(){
    fprintf(outputFile, "Enter <concat_element>\n");
 
    if(check("IDENTIFIER")){
        parse_AttributeAccess();
    } else {
        parse_Literal();
    }
 
    fprintf(outputFile, "<concant_element> (done) \n");
}
 

/* ---- INPUT STATEMENT ---- */
void parse_InputStatement(){
 
    skip_noise_tokens();
    fprintf(outputFile, "Enter <input_stmt>\n");
 
 
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
 
    fprintf(outputFile, "<input_stmt> (done) \n");
}
 
void parse_TargetID(){
    fprintf(outputFile, "Enter <target_id>\n");
    parse_AttributeAccess();
 
    fprintf(outputFile, "<target_id> (done) \n");
}
 
void parse_PromptContent(){
    fprintf(outputFile, "Enter <prompt_content>\n");
    if(!match("STRING")){
        parseError("a STRING_LITERAL");
    }
    fprintf(outputFile, "<prompt_content> (done) \n");
}

void parse_ChoiceBlock(){
    skip_noise_tokens();
 
    fprintf(outputFile, "Enter <choice_block>\n");
    if(check("INDENT")){
        match("INDENT");
        fprintf(outputFile, " -> Consumed INDENT.\n");
       
        parse_ChoiceList();
 
        if(!match("DEDENT"))
            fprintf(outputFile, " -> Consumed DEDENT.\n");
        else parseError("a DEDENT"); //expects an DEDENT      
    } else {
        parseError("an INDENT"); //expects an "INDENT"
    }
   
    fprintf(outputFile, "<choice_block> (done) \n");
}
 
//<choice_list> ::=  { “[“ <string_literal> ":" <literal> “]” “,” "NEWLINE"}
void parse_ChoiceList() {
    fprintf(outputFile, "Enter <choice_list>\n");
 
    if(!match("LBRACKET")){
        parseError("expects a RBRACKET [") ;
    }
    fprintf(outputFile, " -> Consumed '['\n");
 
    if(!match("STRING")){
        parseError("expects a STRING");
    }
    fprintf(outputFile, " -> Consumed STRING_LITERAL: %s\n", currentToken()->lexeme);
 
    if(!match("COLON")) {
        parseError("a COLON");
    }
    fprintf(outputFile, " -> Consumed COLON\n");
 
    parse_Literal();
 
    if(!match("RBRACKET")){
        parseError("a BRACKET ]");
    }
    fprintf(outputFile, " -> Consumed RBRACKET ]\n");
 
    if(check("COMMA")){
        if(match("COMMA")){
            if(!match("NEWLINE")) parseError("expect a NEWLINE");
            fprintf(outputFile, " -> Consumed NEWLINE\n");
            parse_ChoiceList();
        }
    }
 
    fprintf(outputFile, "<choice_list> (done) \n");
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
    fprintf(outputFile, "\nEnter <condition_stmt>\n");
 
    if(!match("IF")){
        parseError("an IF keyword"); 
    } 
    parse_Expression();
    parse_ConditionalTail();
 
    fprintf(outputFile, "<condition_stmt> (done) \n");
}

void parse_ConditionalTail(){
    fprintf(outputFile, "\nEnter <conditional_tail>\n");
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
 
    fprintf(outputFile, "<conditional_tail> (done) \n");
}

void parse_ElifClause(){
    fprintf(outputFile, "\nEnter <elif_clause>\n");
    if(!match("ELIF")){
        parseError("an ELIF keyword"); 
    } 
    parse_Expression();
    if(!match("COLON"))
        parseError("a COLON"); 

    parse_StatementBlock();
    fprintf(outputFile, "<elif_clause> (done) \n");
}
void parse_ElseClause(){
    fprintf(outputFile, "\nEnter <else_clause>\n");
    if(!match("ELSE")){
        parseError("an ELSE keyword"); 
    } 
    if(!match("COLON"))
        parseError("a COLON"); 

    parse_StatementBlock();
    fprintf(outputFile, "<else_clause> (done) \n");
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
    fprintf(outputFile, "\nEnter <iterative_stmt>\n");

    if (match("FOR"))
        parse_ForStructure();
    else if (match("REPEAT"))
        parse_RepeatStructure();

    fprintf(outputFile, "<iterative_stmt> (done) \n");
}
void parse_ForStructure(){
    fprintf(outputFile, "\nEnter <for_structure>\n");

    parse_LoopVariable();

    if(!match("IN"))
        parseError("an IN keyword");
    parse_CollectionSource();
    if(!match("COLON"))
        parseError("a COLON");
    parse_StatementBlock();

    fprintf(outputFile, "<for_structure> (done) \n");
}
void parse_CollectionSource(){
    fprintf(outputFile, "\nEnter <collection_source>\n");
    if(check("NARRATE") || check("DIALOGUE") || check("SHOW")){
        parse_OutputStatement();
    } else {
        parse_Expression();
    }
    fprintf(outputFile, "<collection_source> (done) \n");
}
void parse_LoopVariable(){
    fprintf(outputFile, "\nEnter <loop_variable>\n");
    if (match("IDENTIFIER")) {
    } else if (match("CHARACTER") || match("SCENE") || match("TEMPLATE")) {
    } else {
        parseError("an IDENTIFIER or ENTITY_TYPE");
    }
    fprintf(outputFile, "<loop_variable> (done) \n");
}
void parse_RepeatStructure(){
    fprintf(outputFile, "\nEnter <repeat_structure>\n");

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

    fprintf(outputFile, "<repeat_structure> (done) \n");
}
