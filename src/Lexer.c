#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tokens.h"
#include "helpers.h"

typedef enum {
    DEFAULT,
    IN_IDENTIFIER,
    IN_STRING,
    IN_OPERATOR
} LexerState;

void finalize_token(Token *tokens, int *token_count, char *lexeme_buffer, int *buffer_index);

int main() {
    char filename[256] = "src/test.st";
    if (!read_file_ext(filename)) {
        printf("Please enter a valid .st file format.\n");
        return 1;
    }

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error opening file.\n");
        return 1;
    }

    printf("File opened successfully!\n");

    Token tokens[1000];
    char lexeme_buffer[100] = "";
    int buffer_index = 0, token_count = 0;
    int ch;

    LexerState state = DEFAULT;

    while ((ch = fgetc(fp)) != EOF) {
        switch (state) {

            // --- Default: decide what to do with the next character ---
            case DEFAULT:
                if (isWhitespace(ch)) {
                    finalize_token(tokens, &token_count, lexeme_buffer, &buffer_index);
                }
                else if (ch == '"') {
                    finalize_token(tokens, &token_count, lexeme_buffer, &buffer_index);
                    state = IN_STRING;
                    lexeme_buffer[buffer_index++] = ch;
                }
                else if (isSeparator(ch)) {
                    finalize_token(tokens, &token_count, lexeme_buffer, &buffer_index);
                    lexeme_buffer[0] = ch;
                    lexeme_buffer[1] = '\0';
                    str_copy(tokens[token_count].type, getTokenType(lexeme_buffer));
                    str_copy(tokens[token_count].lexeme, lexeme_buffer);
                    token_count++;
                }
                else if (ch == '=' || ch == '!' || ch == '<' || ch == '>') {
                    finalize_token(tokens, &token_count, lexeme_buffer, &buffer_index);
                    state = IN_OPERATOR;
                    lexeme_buffer[buffer_index++] = ch;
                }
                else {
                    // start of identifier or keyword
                    lexeme_buffer[buffer_index++] = ch;
                    state = IN_IDENTIFIER;
                }
                break;

            // --- Identifier or keyword ---
            case IN_IDENTIFIER:
                if (isWhitespace(ch) || isSeparator(ch) || ch == '=' || ch == '!' || ch == '<' || ch == '>') {
                    ungetc(ch, fp);  // push back for next iteration
                    finalize_token(tokens, &token_count, lexeme_buffer, &buffer_index);
                    state = DEFAULT;
                } else {
                    lexeme_buffer[buffer_index++] = ch;
                }
                break;

            // --- String literal ---
            case IN_STRING:
                lexeme_buffer[buffer_index++] = ch;
                if (ch == '"') {
                    // string closed
                    lexeme_buffer[buffer_index] = '\0';
                    str_copy(tokens[token_count].type, STRING_LITERAL);
                    str_copy(tokens[token_count].lexeme, lexeme_buffer);
                    token_count++;
                    buffer_index = 0;
                    state = DEFAULT;
                }
                break;

            // --- Operators (e.g. ==, <=, !=) ---
            case IN_OPERATOR:
                if (ch == '=') {
                    lexeme_buffer[buffer_index++] = ch;
                } else {
                    ungetc(ch, fp);
                }
                lexeme_buffer[buffer_index] = '\0';
                str_copy(tokens[token_count].type, getTokenType(lexeme_buffer));
                str_copy(tokens[token_count].lexeme, lexeme_buffer);
                token_count++;
                buffer_index = 0;
                state = DEFAULT;
                break;
        }
    }

    // Handle leftover lexeme at EOF
    finalize_token(tokens, &token_count, lexeme_buffer, &buffer_index);

    outputTokens(tokens);
    fclose(fp);
    return 0;
}


void finalize_token(Token *tokens, int *token_count, char *lexeme_buffer, int *buffer_index) {
    if (*buffer_index == 0) return;
    lexeme_buffer[*buffer_index] = '\0';

    str_copy(tokens[*token_count].type, getTokenType(lexeme_buffer));
    str_copy(tokens[*token_count].lexeme, lexeme_buffer);
    (*token_count)++;

    *buffer_index = 0;
    lexeme_buffer[0] = '\0';
}