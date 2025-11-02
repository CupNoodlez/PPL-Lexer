#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "tokens.h"

#define TOKEN_SIZE 100
#define MAX_TOKENS 1000

int main() {
    
    FILE *fp = fopen("test.txt", "r");
    if (!fp) {
        perror("fopen");
        printf("Error opening file 'test.txt'. Make sure the file is in the program's working directory.\n");
        return 1;
    }

    Token tokens[MAX_TOKENS];
    int tokenCount = 0;

    char ch, next;
    char word[TOKEN_SIZE];
    int count = 0;

    while ((ch = fgetc(fp)) != EOF) {
         // check if it's a comment
        if (ch == '/') {
            next = fgetc(fp);

            // single-line comment
            if (next == '/') {
                char comment[TOKEN_SIZE] = "//";
                int i = 2;
                while ((ch = fgetc(fp)) != EOF && ch != '\n' && i < TOKEN_SIZE - 1) {
                    comment[i++] = ch;
                }
                comment[i] = '\0';

                strcpy(tokens[tokenCount].lexeme, comment);
                strcpy(tokens[tokenCount].type, COMMENT);
                tokenCount++;
                continue;
            }

            // multi-line comment
            else if (next == '*') {
                char comment[TOKEN_SIZE] = "/*";
                int i = 2;
                char prev = 0;
                while ((ch = fgetc(fp)) != EOF && i < TOKEN_SIZE - 1) {
                    comment[i++] = ch;
                    if (prev == '*' && ch == '/') break;
                    prev = ch;
                }
                comment[i] = '\0';

                strcpy(tokens[tokenCount].lexeme, comment);
                strcpy(tokens[tokenCount].type, COMMENT);
                tokenCount++;
                continue;
            }

            // if not a comment, treat '/' as normal character
            else {
                ungetc(next, fp); // put the read character back
            }
        }

        if (isWhiteSpace(ch) || isDelimiter(ch)) {
            // end of a word
            if (count > 0) {
                word[count] = '\0';
                strcpy(tokens[tokenCount].lexeme, word);
                strcpy(tokens[tokenCount].type, getTokenType(word));
                tokenCount++;
                count = 0;
            }

            // also print the delimiter itself if needed
            if (isDelimiter(ch)) {
                char delimStr[2] = { ch, '\0' };
                strcpy(tokens[tokenCount].lexeme, delimStr);
                strcpy(tokens[tokenCount].type, DELIMITER);
                tokenCount++;
            }
        } else {
            word[count++] = ch;
            if (count >= TOKEN_SIZE - 1) count = TOKEN_SIZE - 1; // prevent overflow
        }
    }

    // print last word if file didn’t end with delimiter
    if (count > 0) {
        word[count] = '\0';
        strcpy(tokens[tokenCount].lexeme, word);
        strcpy(tokens[tokenCount].type, getTokenType(word));
        tokenCount++;
    }

    fclose(fp);

        // --- Print all tokens ---
    printf("\n%-20s | %-10s\n", "LEXEME", "TYPE");
    printf("----------------------------------------\n");
    for (int i = 0; i < tokenCount; i++) {
        printf("%-20s | %-10s\n", tokens[i].lexeme, tokens[i].type);
    }


    return 0;
}
