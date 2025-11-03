#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tokens.h"
#include "helpers.h"

typedef enum {
    DEFAULT,        // default state (delimiters, unknowns)
    IN_IDENTIFIER,  // handles all alphanumeric tokens (keywords, identifiers, numbers)
    IN_STRING,      // handles string literals
    IN_OPERATOR,    // handles operators
    IN_COMMENT      // handles comments
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
                else if (ch == '#') {
                    finalize_token(tokens, &token_count, lexeme_buffer, &buffer_index);
                    state = IN_COMMENT;
                    lexeme_buffer[buffer_index++] = ch;
                    
                    // Check if it's a multi-line comment (##)
                    int next_ch = fgetc(fp);
                    if (next_ch == '#') {
                        lexeme_buffer[buffer_index++] = next_ch;
                    } else if (next_ch != EOF) {
                        ungetc(next_ch, fp);
                    }
                }
                else if (ch == '"') {
                    finalize_token(tokens, &token_count, lexeme_buffer, &buffer_index);
                    state = IN_STRING;
                    lexeme_buffer[buffer_index++] = ch;
                }
                else if (ch == '=' || ch == '!' || ch == '<' || ch == '>') {
                    finalize_token(tokens, &token_count, lexeme_buffer, &buffer_index);
                    state = IN_OPERATOR;
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

            // --- Comments ---
            case IN_COMMENT:
                lexeme_buffer[buffer_index++] = ch;
                
                // Check if it's a single-line comment (starts with single #)
                if (buffer_index == 2 && lexeme_buffer[0] == '#' && lexeme_buffer[1] != '#') {
                    // Single-line comment: read until newline
                    while (ch != '\n' && (ch = fgetc(fp)) != EOF) {
                        lexeme_buffer[buffer_index++] = ch;
                    }
                    lexeme_buffer[buffer_index] = '\0';
                    str_copy(tokens[token_count].type, COMMENT);
                    str_copy(tokens[token_count].lexeme, lexeme_buffer);
                    token_count++;
                    buffer_index = 0;
                    state = DEFAULT;
                }
                // Check if it's a multi-line comment (starts with ##)
                else if (buffer_index >= 2 && lexeme_buffer[0] == '#' && lexeme_buffer[1] == '#') {
                    // Multi-line comment: read until closing ##
                    bool found_closing = false;
                    while (!found_closing && (ch = fgetc(fp)) != EOF) {
                        lexeme_buffer[buffer_index++] = ch;
                        
                        // Check for closing ##
                        if (ch == '#') {
                            int next_ch = fgetc(fp);
                            if (next_ch == '#') {
                                lexeme_buffer[buffer_index++] = next_ch;
                                found_closing = true;
                            } else if (next_ch != EOF) {
                                ungetc(next_ch, fp);
                            }
                        }
                    }
                    lexeme_buffer[buffer_index] = '\0';
                    str_copy(tokens[token_count].type, COMMENT);
                    str_copy(tokens[token_count].lexeme, lexeme_buffer);
                    token_count++;
                    buffer_index = 0;
                    state = DEFAULT;
                }
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