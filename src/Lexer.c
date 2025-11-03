#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tokens.h"
#include "helpers.h"

typedef enum {
    START,          // starting state (beginning of token recognition)
    IN_IDENTIFIER,  // handles identifiers and keywords
    IN_NUMBER,      // handles numeric literals
    IN_OPERATOR,    // handles operators
    IN_COMMENT,     // handles comments
    IN_STRING,      // handles string literals
    IN_DELIM,       // handles delimiters
    IN_BLANK,       // handles whitespace
} LexerState;

void finalize_token(Token *tokens, int *token_count, char *lexeme_buffer, int *buffer_index);
void emit_token(Token *tokens, int *token_count, const char *type, const char *lexeme);
void handle_indentation(Token *tokens, int *token_count, int new_indent, int *indent_stack, int *stack_size);
void flush_token(Token *tokens, int *token_count, char *lexeme_buffer, int *buffer_index, int ch, FILE *fp, LexerState *state);

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

    // Indentation tracking
    int indent_stack[100];  // Stack of indentation levels
    int stack_size = 0;
    indent_stack[stack_size++] = 0;  // Initialize with base level 0
    int current_indent = 0;          // Current line's indentation
    bool at_line_start = true;       // Are we at the start of a line?

    LexerState state = START;

    while ((ch = fgetc(fp)) != EOF) {
        switch (state) {

            // --- START: decide what to do with the next character ---
            case START:
                // Handle indentation at line start
                if (at_line_start) {
                    if (ch == ' ' || ch == '\t') {
                        // Count indentation (each tab = 4 spaces for consistency)
                        current_indent += (ch == '\t') ? 4 : 1;
                        break;  // Stay in START, continue counting
                    } else if (ch == '\n') {
                        // Empty line - skip, stay at line start
                        break;
                    } else if (ch == '#') {
                        // Comment line - ignore indentation
                        current_indent = 0;
                        at_line_start = false;
                        // Continue to process '#' below
                    } else {
                        // Real content - handle indentation changes
                        handle_indentation(tokens, &token_count, current_indent, indent_stack, &stack_size);
                        at_line_start = false;
                        current_indent = 0;
                        // Continue to process character below
                    }
                }

                // Normal token processing
                if (ch == '\n') {
                    // Emit NEWLINE token
                    emit_token(tokens, &token_count, NEWLINE, "\\n");
                    at_line_start = true;
                    current_indent = 0;
                }
                else if (ch == ' ' || ch == '\t') {
                    state = IN_BLANK;
                }
                else if (ch == '#') {
                    state = IN_COMMENT;
                    lexeme_buffer[buffer_index++] = ch;
                }
                else if (ch == '"' || ch == '\'') {
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
                if (ch == '\n') {
                    // Newline - transition back to START at line start
                    emit_token(tokens, &token_count, NEWLINE, "\\n");
                    at_line_start = true;
                    current_indent = 0;
                    state = START;
                } else if (ch != ' ' && ch != '\t') {
                    ungetc(ch, fp);
                    state = START;
                }
                break;

            // --- Identifier or keyword ---
            case IN_IDENTIFIER:
                if (isSeparator(ch)) {
                    flush_token(tokens, &token_count, lexeme_buffer, &buffer_index, ch, fp, &state);
                } else {
                    lexeme_buffer[buffer_index++] = ch;
                }
                break;

            // --- Number ---
            case IN_NUMBER:
                if (isDigit(ch) || ch == '.') {
                    lexeme_buffer[buffer_index++] = ch;
                } else if (isSeparator(ch)) {
                    flush_token(tokens, &token_count, lexeme_buffer, &buffer_index, ch, fp, &state);
                } else {
                    lexeme_buffer[buffer_index++] = ch;
                    state = IN_IDENTIFIER;
                }
                break;

            // --- Delimiter ---
            case IN_DELIM:
                flush_token(tokens, &token_count, lexeme_buffer, &buffer_index, ch, fp, &state);
                break;

            // --- String literal ---
            case IN_STRING:
                lexeme_buffer[buffer_index++] = ch;
                if (ch == '"' || ch == '\'') {
                    finalize_token(tokens, &token_count, lexeme_buffer, &buffer_index);
                    state = START;
                }
                break;

            // --- Operators (e.g. ==, <=, !=, //) ---
            case IN_OPERATOR:
                if (ch == '=') {
                    lexeme_buffer[buffer_index++] = ch;
                } else if (lexeme_buffer[0] == '/' && ch == '/') {
                    lexeme_buffer[buffer_index++] = ch;
                } else {
                    ungetc(ch, fp);
                }
                finalize_token(tokens, &token_count, lexeme_buffer, &buffer_index);
                state = START;
                break;

            // --- Comments ---
            case IN_COMMENT:
                // Check if it's a multi-line comment (##)
                int next_ch = fgetc(fp);
                if (next_ch == '#') {
                    // Multi-line comment: read until closing ##
                    lexeme_buffer[buffer_index++] = ch;      // first #
                    lexeme_buffer[buffer_index++] = next_ch; // second #
                    
                    bool found_closing = false;
                    while (!found_closing && (ch = fgetc(fp)) != EOF) {
                        lexeme_buffer[buffer_index++] = ch;
                        
                        // Check for closing ##
                        if (ch == '#') {
                            int peek = fgetc(fp);
                            if (peek == '#') {
                                lexeme_buffer[buffer_index++] = peek;
                                found_closing = true;
                            } else if (peek != EOF) {
                                ungetc(peek, fp);
                            }
                        }
                    }
                    finalize_token(tokens, &token_count, lexeme_buffer, &buffer_index);
                    state = START;
                } else {
                    // Single-line comment: read until newline
                    lexeme_buffer[buffer_index++] = ch; // the #
                    if (next_ch != EOF && next_ch != '\n') {
                        ungetc(next_ch, fp);
                    }
                    
                    while ((ch = fgetc(fp)) != EOF && ch != '\n') {
                        lexeme_buffer[buffer_index++] = ch;
                    }
                    
                    finalize_token(tokens, &token_count, lexeme_buffer, &buffer_index);
                    
                    // After single-line comment, handle the newline
                    if (ch == '\n') {
                        emit_token(tokens, &token_count, NEWLINE, "\\n");
                        at_line_start = true;
                        current_indent = 0;
                    }
                    state = START;
                }
                break;
        }        
    }
    
    // Handle leftover lexeme at EOF
    finalize_token(tokens, &token_count, lexeme_buffer, &buffer_index);
    
    // Emit remaining DEDENT tokens to return to base level
    while (stack_size > 1) {
        emit_token(tokens, &token_count, DEDENT, "");
        stack_size--;
    }

    outputTokens(tokens);
    fclose(fp);
    return 0;
}


void emit_token(Token *tokens, int *token_count, const char *type, const char *lexeme) {
    str_copy(tokens[*token_count].type, type);
    str_copy(tokens[*token_count].lexeme, lexeme);
    (*token_count)++;
}

void handle_indentation(Token *tokens, int *token_count, int new_indent, int *indent_stack, int *stack_size) {
    int current_level = indent_stack[*stack_size - 1];
    
    if (new_indent > current_level) {
        // Increased indentation - emit INDENT
        indent_stack[(*stack_size)++] = new_indent;
        emit_token(tokens, token_count, INDENT, "");
    } else if (new_indent < current_level) {
        // Decreased indentation - emit DEDENT(s)
        while (*stack_size > 1 && indent_stack[*stack_size - 1] > new_indent) {
            (*stack_size)--;
            emit_token(tokens, token_count, DEDENT, "");
        }
        
        // Check for indentation error (dedent to non-existent level)
        if (*stack_size > 0 && indent_stack[*stack_size - 1] != new_indent) {
            // Indentation error - dedented to a level that wasn't used before
            // For now, we'll just accept it and add it to the stack
            indent_stack[(*stack_size)++] = new_indent;
        }
    }
    // If new_indent == current_level, no token needed
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

void flush_token(Token *tokens, int *token_count, char *lexeme_buffer, int *buffer_index, int ch, FILE *fp, LexerState *state) {
    finalize_token(tokens, token_count, lexeme_buffer, buffer_index);
    ungetc(ch, fp);  // push back current character
    *state = START;
}