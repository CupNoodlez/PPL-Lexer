#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tokens.h"
#include "helpers.h"

typedef enum {
    START,          // starting state
    IN_IDENTIFIER,  // handles identifiers and keywords
    IN_NUMBER,      // handles numeric literals
    IN_OPERATOR,    // handles operators
    IN_COMMENT,     // handles comments
    IN_STRING,      // handles string literals
    IN_DELIM,       // handles delimiters
    IN_BLANK,       // handles whitespace
    DONE            // completion state
} LexerState;

void finalize_token(Token *tokens, int *token_count, char *lexeme_buffer, int *buffer_index);

int main() {
    char filename[256] = "input.st";
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

    LexerState state = START;

    while ((ch = fgetc(fp)) != EOF) {
        switch (state) {

            // --- START: decide what to do with the next character ---
            case START:
                if (isWhitespace(ch)) {
                    state = IN_BLANK;
                    // Don't buffer whitespace
                }
                else if (ch == '#') {
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
                    state = IN_STRING;
                    lexeme_buffer[buffer_index++] = ch;
                }
                else if (isOperator(ch)) {
                    state = IN_OPERATOR;
                    lexeme_buffer[buffer_index++] = ch;
                }
                else if (isDelimiter(ch)) {
                    state = IN_DELIM;
                    lexeme_buffer[buffer_index++] = ch;
                }
                else if (isDigit(ch)) {
                    lexeme_buffer[buffer_index++] = ch;
                    state = IN_NUMBER;
                }
                else if (isAlpha(ch)) {
                    // Identifiers must start with a letter (not underscore)
                    lexeme_buffer[buffer_index++] = ch;
                    state = IN_IDENTIFIER;
                }
                else {
                    lexeme_buffer[buffer_index++] = ch;
                    state = IN_IDENTIFIER;
                }
                break;

            // --- Whitespace ---
            case IN_BLANK:
                if (!isWhitespace(ch)) {
                    // Transition back to START to process this character
                    ungetc(ch, fp);
                    state = START;
                }
                // Continue consuming whitespace
                break;

            // --- Identifier or keyword ---
            case IN_IDENTIFIER:
                if (isWhitespace(ch) || isDelimiter(ch) || isOperator(ch)) {
                    ungetc(ch, fp);  // push back for next iteration
                    finalize_token(tokens, &token_count, lexeme_buffer, &buffer_index);
                    state = START;
                } else {
                    // After first character, underscores are allowed
                    lexeme_buffer[buffer_index++] = ch;
                }
                break;

            // --- Number ---
            case IN_NUMBER:
                if (isDigit(ch) || ch == '.') {
                    lexeme_buffer[buffer_index++] = ch;
                } else if (isWhitespace(ch) || isDelimiter(ch) || isOperator(ch)) {
                    ungetc(ch, fp);  // push back for next iteration
                    finalize_token(tokens, &token_count, lexeme_buffer, &buffer_index);
                    state = START;
                } else {
                    // Invalid character in number, but continue as identifier
                    lexeme_buffer[buffer_index++] = ch;
                    state = IN_IDENTIFIER;
                }
                break;

            // --- Delimiter ---
            case IN_DELIM:
                lexeme_buffer[buffer_index] = '\0';
                str_copy(tokens[token_count].type, getTokenType(lexeme_buffer));
                str_copy(tokens[token_count].lexeme, lexeme_buffer);
                token_count++;
                buffer_index = 0;
                ungetc(ch, fp);  // push back current character
                state = START;
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
                    state = START;
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
                state = START;
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
                    // Remove the last character if it's a newline
                    if (buffer_index > 0 && lexeme_buffer[buffer_index - 1] == '\n') {
                        buffer_index--;
                    }
                    lexeme_buffer[buffer_index] = '\0';
                    str_copy(tokens[token_count].type, COMMENT);
                    str_copy(tokens[token_count].lexeme, lexeme_buffer);
                    token_count++;
                    buffer_index = 0;
                    state = START;
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
                    state = START;
                }
                break;

            // --- Done state (should not be reached in normal operation) ---
            case DONE:
                break;
        }        
    }
    // Handle leftover lexeme at EOF
    finalize_token(tokens, &token_count, lexeme_buffer, &buffer_index);

    state = DONE;
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