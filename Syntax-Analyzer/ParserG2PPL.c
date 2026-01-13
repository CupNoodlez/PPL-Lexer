#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../Lexical-Analyzer/LexicalG2PPL.h"

FILE *outputFile;

Token *tokens;
int token_count;
int current_pos = 0;
int indentLevel = 0;

// Panic-mode state
static bool recovering = false;
static bool errorOccurred = false;

void printIndent();
void beginScope(const char* token_name);
void endScope();
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

    fprintf(outputFile, "=== Parser Syntax Analysis S-Expression Output ===\n");
    // fprintf(outputFile, "Tokens loaded: %d. Starting parse.\n\n", token_count);

    // fprintf(outputFile, "Next token is: %s  Next lexeme is: %s\n", tokens[current_pos].token_name, tokens[current_pos].lexeme);
    parse_Program();

    // Skip any trailing noise tokens (comments, newlines) after END
    skip_noise_tokens();
    
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

void printIndent() {
    for (int i = 0; i < indentLevel; i++) {
        fprintf(outputFile, "  ");
    }
}

void beginScope(const char* token_name){
    printIndent();
    fprintf(outputFile, "(%s\n", token_name);
    indentLevel++;
}

void endScope(){
    indentLevel--;
    printIndent();
    fprintf(outputFile, ")\n");
}

bool match(const char* token_name) {
    if (check(token_name)) {
        printIndent();
        fprintf(outputFile, "(%s %s)\n", tokens[current_pos].token_name, tokens[current_pos].lexeme);
        advance();
        // if(!isAtEnd()){
        //     fprintf(outputFile, "Next token is: %s  Next lexeme is: %s\n", tokens[current_pos].token_name, tokens[current_pos].lexeme);
        // }
        return true;
    }
    return false;
}

void parseError(const char *expected)
{
    errorOccurred = true;  // Set error flag
    
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

// Recover by finding a good synchronization point (statement boundary or block end)
static void recover_to_newline(void)
{
    recovering = true;
    int indentDepth = 0;

    // Skip until we find a good synchronization point
    while (!isAtEnd()) {
        // Track INDENT/DEDENT to skip entire malformed blocks
        if (check("INDENT")) {
            indentDepth++;
            advance();
            continue;
        }
        
        if (check("DEDENT")) {
            if (indentDepth > 0) {
                // We're inside a malformed block, consume this DEDENT
                indentDepth--;
                advance();
                if (indentDepth == 0) {
                    // We've exited the malformed block, now look for next statement
                    fprintf(stderr, "Recovered: exited malformed block\n");
                }
                continue;
            } else {
                // Not inside a malformed block, DEDENT is a good sync point
                fprintf(stderr, "Recovered at DEDENT (block end)\n");
                recovering = false;
                return;
            }
        }
        
        // END keyword - always a good recovery point
        if (check("END")) {
            fprintf(stderr, "Recovered at END keyword\n");
            recovering = false;
            return;
        }
        
        // Found NEWLINE - check if next token starts a statement (but only if not in a malformed block)
        if (check("NEWLINE") && indentDepth == 0) {
            advance(); // consume the NEWLINE
            
            // Skip any additional noise tokens after NEWLINE
            while ((check("COMMENT") || check("COMMENT_MULTI") || check("NEWLINE")) && !isAtEnd()) {
                advance();
            }
            
            // Check for statement-starting keywords or tokens
            if (isAtEnd() || 
                check("DEDENT") ||
                check("END") ||
                check("CHARACTER") || 
                check("SCENE") || 
                check("TEMPLATE") ||
                check("IF") || 
                check("ELIF") ||
                check("ELSE") ||
                check("FOR") || 
                check("REPEAT") ||
                check("ASK") ||
                check("CHOICE") ||
                check("NARRATE") ||
                check("DIALOGUE") ||
                check("SHOW") ||
                check("IDENTIFIER")) {
                fprintf(stderr, "Recovered at statement boundary (after NEWLINE, before %s)\n",
                       isAtEnd() ? "EOF" : tokens[current_pos].token_name);
                recovering = false;
                return;
            }
            // Not a statement start; keep searching
            continue;
        }
        
        advance();
    }

    recovering = false;
}

// Optional: keep skipping noise when not recovering
void skip_noise_tokens()
{
    while (check("COMMENT") || check("COMMENT_MULTI") || check("NEWLINE"))
    {
        advance();
        // if(!isAtEnd()){
        //     fprintf(outputFile, "Next token is: %s  Next lexeme is: %s\n", tokens[current_pos].token_name, tokens[current_pos].lexeme);
        // }
    }
}

void parse_Program() {
    beginScope("Program");
    
    skip_noise_tokens();
    if(!match("START")){
        parseError("START keyword");
    }
    parse_StatementList();
    
    // After parsing statement list (possibly with errors), consume everything until END
    // This handles stray DEDENT/INDENT tokens from malformed blocks
    while (!isAtEnd() && !check("END")) {
        // Check if this looks like a valid top-level statement
        if (!check("DEDENT") && !check("INDENT") && !check("NEWLINE") && 
            !check("COMMENT") && !check("COMMENT_MULTI")) {
            // Found something that looks like a statement - try to parse it
            if (check("CHARACTER") || check("SCENE") || check("TEMPLATE") ||
                check("IF") || check("FOR") || check("REPEAT") ||
                check("ASK") || check("CHOICE") ||
                check("NARRATE") || check("DIALOGUE") || check("SHOW") ||
                check("IDENTIFIER")) {
                // Reset error flag and try to parse
                errorOccurred = false;
                parse_Statement();
                skip_noise_tokens();
                if (errorOccurred) {
                    // If parsing this statement failed, continue consuming tokens
                    continue;
                }
            } else {
                // Unknown token, consume it
                advance();
            }
        } else {
            // Consume noise/structural tokens
            advance();
        }
    }
    
    skip_noise_tokens();
    
    if(!match("END")){
        parseError("END keyword");
    }
    
    endScope();
}

void parse_StatementList() {
    beginScope("StatementList");
    
    skip_noise_tokens();
    while (!isAtEnd() && !check("END")) {
        // If we hit DEDENT, we're at the end of a block, not the program
        if (check("DEDENT")) {
            break;
        }
        
        parse_Statement();
        skip_noise_tokens();
    }
    
    endScope();
}

void parse_Statement() {
    
    skip_noise_tokens();
    beginScope("Statement");
    
    // Reset error flag for this statement
    errorOccurred = false;
    
    // Parse exactly ONE statement (no while loop)
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
        // After skipping noise, check if there's an actual statement to parse
        if (!check("END") && !check("DEDENT") && !isAtEnd()) {
            endScope();
            parse_Statement(); // Recursively parse the actual statement
            return;
        }
    }
    else if (!check("END") && !check("DEDENT") && !isAtEnd()) {
        // Only report error if we're not at a natural boundary
        parseError("a valid statement (IDENTIFIER, ASK, CHOICE, NARRATE, DIALOGUE, SHOW, IF, FOR, REPEAT, CHARACTER, SCENE, or TEMPLATE)");
    }

    endScope();
}

void parse_StatementBlock()
{
    skip_noise_tokens();
    beginScope("StatementBlock");

    if (!match("INDENT"))
        parseError("an INDENT");

    parse_StatementList();

    if (!match("DEDENT")) 
        parseError("a DEDENT");

    endScope();
}

void parse_IDList()
{
    beginScope("IDList");

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
    endScope();
}

void parse_AttributeAccess()
{
    beginScope("AttributeAccess");
    
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
    endScope();
}

void parse_Literal()
{
    beginScope("Literal");

    // Booleans
    if (check("TRUE") || check("FALSE"))
    {
        if (check("TRUE")) match("TRUE");
        else match("FALSE");
    }
    // Numbers
    else if (check("INTEGER") || check("FLOAT"))
    {
        if (check("INTEGER")) match("INTEGER");
        else match("FLOAT");
    }
    // Strings / Chars
    else if (check("STRING") || check("CHAR"))
    {
        if (check("STRING")) match("STRING");
        else match("CHAR");
    }
    else {
        parseError("a literal (INTEGER, FLOAT, STRING, CHAR, TRUE, or FALSE)");
    }
    
    endScope();
}

/* ---- EXPRESSION PARSING ---- */

void parse_Expression()
{
    beginScope("Expression");

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
    endScope();
}

void parse_AndExpr()
{
    beginScope("AndExpr");

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
    endScope();
}

void parse_NotExpr()
{
    beginScope("NotExpr");

    while (check("NOT"))
    {
        if (!match("NOT"))
        {
            parseError("NOT");
        }
    }

    parse_RelationalExpr();

    skip_noise_tokens();
    endScope();
}

void parse_RelationalExpr()
{
    beginScope("RelationalExpr");

    parse_ArithmeticExpr();

    if ((match("LESS") || match("LESS_EQUAL") || match("GREATER") ||
         match("GREATER_EQUAL") || match("EQUAL_EQUAL") ||
         match("NOT_EQUAL") || match("IS")))
    {
        parse_ArithmeticExpr();
    }

    skip_noise_tokens();
    endScope();
}

void parse_ArithmeticExpr()
{
    beginScope("ArithmeticExpr");

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
    endScope();
}

void parse_Term()
{
    beginScope("Term");

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
    endScope();
}

void parse_PowerExpr()
{
    beginScope("PowerExpr");

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
    endScope();
}

void parse_UnaryExpr()
{
    beginScope("UnaryExpr");

    while (check("MINUS"))
    {
        if (!match("MINUS"))
        {
            parseError("'-'");
        }
    }

    parse_Factor();

    skip_noise_tokens();
    endScope();
}

void parse_Factor()
{
    beginScope("Factor");

    if (match("LPAREN"))
    {
        parse_Expression();
        if (!match("RPAREN"))
        {
            parseError("')");
        }

        skip_noise_tokens();
        endScope();
        return;
    }

    if (check("IDENTIFIER"))
    {
        parse_AttributeAccess();
        skip_noise_tokens();
        endScope();
        return;
    }

    parse_Literal();

    skip_noise_tokens();
    endScope();
}

/* ---- EXPRESSION PARSING ---- */

/* ---- ASSIGNMENT STATEMENT ---- */
void parse_AssignmentStatement()
{
    beginScope("AssignmentStmt");
    
    parse_AttributeAccess();

    if (
        match("ASSIGN") || match("PLUS_ASSIGN") || match("MINUS_ASSIGN") || match("MULT_ASSIGN") || match("DIV_ASSIGN") || match("MOD_ASSIGN"))
    {
    }
    else
    {
        parseError("assignment operator (=, +=, -=, *=, /=, %=)");
        endScope();
        return;  // Don't continue parsing after error
    }

    parse_Expression();
    endScope();
}

/* ---- DECLARATION STATEMENT ---- */
void parse_DeclarationStatement()
{

    beginScope("DeclarationStmt");

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
    
    endScope();
}


void parse_TemplateDeclaration()
{
    beginScope("TemplateDecl");

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
    
    endScope();
}


void parse_AttributeBlock()
{
    beginScope("AttributeBlock");

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

    endScope();
}
void parse_AttributeList()
{
    beginScope("AttributeList");

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
    
    endScope();
}

// character declaration
void parse_CharacterDeclaration()
{
    beginScope("CharacterDecl");

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

    if (check("IDENTIFIER") && nextToken() != NULL && strcmp(nextToken()->token_name, "COLON") == 0)
    {
        /* Rule: character <id_list> */
        match("IDENTIFIER");   // consume id
        match("COLON");        // consume ':'
        parse_AttributeBlock();        // this consumes IDENTIFIER itself
        
    }
    else
    {
        /* Rule: character id : <attribute_block> */
        parse_IDList();
    }
    
    endScope();
}




// ------------- SCENE -----------------------
void parse_SceneDeclaration()
{
    beginScope("SceneDecl");

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

    endScope();
}

void parse_SceneEntry()
{
    beginScope("SceneEntry");

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

    endScope();
}

void parse_SceneList()
{
    beginScope("SceneList");

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

    endScope();
}

void parse_ScenesBlock()
{
    beginScope("ScenesBlock");

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

    endScope();
}

/* ---- OUTPUT STATEMENT ---- */
void parse_OutputStatement() {
    skip_noise_tokens();
    beginScope("OutputStmt");
 
    // Parse output keyword
    parse_OutputKey();
 
    if(check("IDENTIFIER")) {  //optional attribute access
        parse_TargetID();
    }
 
    if(!match("COLON")) {
        parseError("a COLON"); //expects a colon
    }
 
    parse_OutputBody();
 
    endScope();
}

void parse_OutputKey(){
    beginScope("OutputKey");

    if(match("NARRATE")){
    }
    else if(match("DIALOGUE")){
    }
    else if(match("SHOW")){
    }
    else {
        parseError("NARRATE, DIALOGUE, or SHOW keyword");
    }
    
    endScope();
}
 
void parse_OutputBody(){
    beginScope("OutputBody");
    
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
 
    endScope();
}
 
void parse_OutputBlock(){
    beginScope("OutputBlock");

    if(!match("INDENT")){
        parseError("an INDENT"); 
    }
 
    parse_ContentItem();
    skip_noise_tokens();  

    if(!match("DEDENT")){
        parseError("a DEDENT"); 
    }        
    
    endScope();
}
 
 
void parse_ContentItem(){
    beginScope("ContentItem");
 
    parse_Concat();
 
    endScope();
}
 
 
void parse_Concat(){
    beginScope("Concat");
 
    parse_ConcatElement();
    while(!isAtEnd() && check("PLUS")){
        match("PLUS");
        parse_ConcatElement();
    }
 
    endScope();
}
 
void parse_ConcatElement(){
    beginScope("ConcatElement");
 
    if(check("IDENTIFIER")){
        parse_AttributeAccess();
    } else {
        parse_Literal();
    }
 
    endScope();
}

/* ---- INPUT STATEMENT ---- */
void parse_InputStatement(){
 
    skip_noise_tokens();
    beginScope("InputStmt");
 
    if(check("ASK")){
        match("ASK");
 
        if(check("IDENTIFIER")){
            parse_TargetID();
        }
 
        parse_PromptContent();
        if (errorOccurred) {
            endScope();
            return;  // Exit early after error
        }
 
        if(!match("AS")){
            parseError(" AS Keyword");  //expect an "as" keyword
            endScope();
            return;  // Exit early after error
        }
 
        if(!match("IDENTIFIER")){
            parseError("an IDENTIFIER"); //expect an "identifier"
            endScope();
            return;  // Exit early after error
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
 
    endScope();
}
 
void parse_TargetID(){
    beginScope("TargetID");
    
    parse_AttributeAccess();
 
    endScope();
}
 
void parse_PromptContent(){
    beginScope("PromptContent");

    if(!match("STRING")){
        parseError("a STRING_LITERAL");
    }
    
    endScope();
}

void parse_ChoiceBlock(){
    skip_noise_tokens();
 
    beginScope("ChoiceBlock");

    if(check("INDENT")){
        match("INDENT");
       
        parse_ChoiceList();
 
        if(!match("DEDENT")) { }
        else parseError("a DEDENT"); //expects an DEDENT      
    } else {
        parseError("an INDENT"); //expects an "INDENT"
    }
   
    endScope();
}
 
//<choice_list> ::=  { “[“ <string_literal> ":" <literal> “]” “,” "NEWLINE"}
void parse_ChoiceList() {
    beginScope("ChoiceList");
 
    if(!match("LBRACKET")){
        parseError("expects a RBRACKET [") ;
    }
 
    if(!match("STRING")){
        parseError("expects a STRING");
    }
 
    if(!match("COLON")) {
        parseError("a COLON");
    }
 
    parse_Literal();
 
    if(!match("RBRACKET")){
        parseError("a BRACKET ]");
    }
 
    if(check("COMMA")){
        if(match("COMMA")){
            if(!match("NEWLINE")) parseError("expect a NEWLINE");
            parse_ChoiceList();
        }
    }
 
    endScope();
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
    beginScope("ConditionStmt");
 
    if(!match("IF")){
        parseError("an IF keyword"); 
    } 
    parse_Expression();
    parse_ConditionalTail();
 
    endScope();
}

void parse_ConditionalTail(){
    beginScope("ConditionalTail");

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
 
    endScope();
}

void parse_ElifClause(){
    beginScope("ElifClause");

    if(!match("ELIF")){
        parseError("an ELIF keyword"); 
    } 
    parse_Expression();
    if(!match("COLON"))
        parseError("a COLON"); 
 
    parse_StatementBlock();
    endScope();
}
void parse_ElseClause(){
    beginScope("ElseClause");
    
    if(!match("ELSE")){
        parseError("an ELSE keyword"); 
    } 
    if(!match("COLON"))
        parseError("a COLON"); 
 
    parse_StatementBlock();
    
    endScope();
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
    beginScope("IterativeStmt");
 
    if (match("FOR"))
        parse_ForStructure();
    else if (match("REPEAT"))
        parse_RepeatStructure();
 
    endScope();
}
void parse_ForStructure(){
    beginScope("ForStructure");
 
    parse_LoopVariable();
 
    if(!match("IN"))
        parseError("an IN keyword");
    parse_CollectionSource();
    if(!match("COLON"))
        parseError("a COLON");
    parse_StatementBlock();
 
    endScope();
}
void parse_CollectionSource(){
    beginScope("CollectionSource");

    if(check("NARRATE") || check("DIALOGUE") || check("SHOW")){
        parse_OutputStatement();
    } else {
        parse_Expression();
    }
    
    endScope();
}
void parse_LoopVariable(){
    beginScope("LoopVariable");

    if (match("IDENTIFIER")) {
    } else if (match("CHARACTER") || match("SCENE") || match("TEMPLATE")) {
    } else {
        parseError("an IDENTIFIER or ENTITY_TYPE");
    }
    
    endScope();
}
void parse_RepeatStructure(){
    beginScope("RepeatStructure");
 
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
 
    endScope();
}