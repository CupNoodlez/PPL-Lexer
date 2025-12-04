#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../Lexical-Analyzer/LexicalG2PPL.h"

Token *tokens;
int token_count;
int current_pos = 0;

bool isAtEnd();
Token *currentToken();
Token *nextToken();
bool check(const char *token_name);
void advance();
bool match(const char *token_name);
void parseError(const char *expected);
void skip_noise_tokens();
void parse_AssignmentStatement();
void parse_Program();
void parse_Statement();
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
// void parse_DeclarationStatement();

// void parse_CharacterDeclaration();

// void parse_SceneDeclaration();

// void parse_TemplateDeclaration();

void parse_AttributeBlock();

void parse_AttributeList();

void parse_InputStatement();
void parse_TargetID();
void parse_PromptContent();
void parse_ChoiceBlock();
void parse_ChoiceList();

void parse_OutputStatement();
void parse_Content();
void parse_OutputBody();
void parse_OutputBlock();
void parse_ContentItem();
void parse_Concat();
void parse_ConcatElement();


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
    // test: print all tokens
    for (int i = 0; i < token_count; i++) {
        printf("Token %d: token_name='%s', Lexeme='%s', Line=%d\n",
            i + 1, tokens[i].token_name, tokens[i].lexeme, tokens[i].lineNumber);
    }

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
    fprintf(stderr, "------------------\n");
    free(tokens);
    exit(1);
}

void skip_noise_tokens()
{
    while (check("COMMENT") || check("COMMENT_MULTI") || check("NEWLINE"))
    {
        advance();
    }
}

void parse_Program() {
    printf("Enter <program>...\n");
    skip_noise_tokens();
    if(!match("START")){
        parseError("START keyword");
    }
    parse_Statement();
    if(!match("END")){
        parseError("END keyword");
    }
    printf("<program> (done)\n");
 
}
void parse_Statement() {
    skip_noise_tokens();
    printf("\nEnter <statement>...\n");
   
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
    }
 
    while (check("DEDENT") || check("NEWLINE")) {
        advance();
    }
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

// ...existing code...
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
// void parse_DeclarationStatement()
// {

//     printf("Enter <declaration_stmt>\n");

//     if (check("CHARACTER"))
//     {
//         parse_CharacterDeclaration();
//     }
//     else if (check("SCENE"))
//     {
//         parse_SceneDeclaration();
//     }
//     else if (check("TEMPLATE"))
//     {
//         parse_TemplateDeclaration();
//     }
//     else
//     {
//         parseError("Expected 'character', 'scene', or 'template'");
//     }
//     printf("Exit <declaration_stmt>\n");
// }

// void parse_CharacterDeclaration()
// {
//     printf("Enter <character_decl>\n");

//     // 1. Consume "character"
//     if (!match("CHARACTER"))
//     {
//         parseError("Expected 'character'");
//         return;
//     }

//     // 2. Consume the first 'id' (Common to both rules)
//     parse_IDList();

//     if (match("COLON"))
//     {
//     }

//     printf("Exit <character_decl>\n");
// }

// void parse_SceneDeclaration()
// {
//     printf("Enter <scene_decl>\n");

//     if (match("SCENE"))
//     {
//         if (match("ID"))
//         {
//             if (match("COLON"))
//             {
//                 parse_ScenesBlock();
//             }
//             else
//             {
//                 parseError("Expected ':' after scene ID");
//             }
//         }
//         else
//         {
//             parseError("Expected identifier for scene");
//         }
//     }

//     printf("Exit <scene_decl>\n");
// }

// void parse_TemplateDeclaration()
// {
//     printf("Enter <template_decl>\n");

//     match("TEMPLATE");
//     match("ID");

//     // Parse Parameters: "(" <param> {"," <param>} ")"
//     if (match("LPAREN"))
//     {
//         // Parse first param (assuming parse_Param exists)
//         parse_Param();

//         // Handle EBNF curly braces { "," <param> } using a while loop
//         while (strcmp(currentToken, "COMMA") == 0)
//         {
//             match("COMMA");
//             parse_Param();
//         }

//         if (!match("RPAREN"))
//             parseError("Expected ')'");
//     }
//     else
//     {
//         parseError("Expected '(' after template ID");
//     }

//     if (!match("COLON"))
//         parseError("Expected ':'");

//     // Parse the blocks required by the grammar
//     parse_StatementBlock(); // Assuming this function exists from your other code
//     parse_AttributeBlock();

//     printf("Exit <template_decl>\n");
// }

// void parse_AttributeBlock()
// {
//     printf("Enter <attribute_block>\n");

//     if (match("NEWLINE"))
//     {
//         if (match("INDENT"))
//         {

//             parse_AttributeList();

//             if (!match("DEDENT"))
//             {
//                 parseError("Expected 'DEDENT' at end of attribute block");
//             }
//         }
//         else
//         {
//             parseError("Expected 'INDENT' after newline");
//         }
//     }
//     else
//     {
//         parseError("Expected 'NEWLINE' before attribute block");
//     }
//     printf("<attribute_block> (done)\n");
// }

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
