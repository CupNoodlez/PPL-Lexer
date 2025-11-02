#include <stdio.h>
#include <ctype.h>
#include "tokens.h"

#define TOKEN_SIZE 100

int main() {
    
    FILE *fp = fopen("test.txt", "r");
    if (!fp) {
        perror("fopen");
        printf("Error opening file 'test.txt'. Make sure the file is in the program's working directory.\n");
        return 1;
    }

    char ch, next;
    char word[TOKEN_SIZE];
    int count = 0;

    while ((ch = fgetc(fp)) != EOF) {
         // check if it's a comment
        if (ch == '/') {
            next = fgetc(fp);

            // single-line comment
            if (next == '/') {
                printf("Comment: //");
                while ((ch = fgetc(fp)) != EOF && ch != '\n') {
                    putchar(ch); // print the comment text
                    /* naisip ko kasi kung ilalagay siya sa array eh pwede namang infinite yung comment*/
                }
                printf("\n");
                continue; // skip to next iteration
            }

            // multi-line comment
            else if (next == '*') {
                printf("Comment: /*");
                char prev = 0;
                while ((ch = fgetc(fp)) != EOF) {
                    putchar(ch);
                    if (prev == '*' && ch == '/') break;
                    prev = ch;
                }
                printf("\n");
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
                printf("Word: %s\n", word);
                count = 0;
            }

            // also print the delimiter itself if needed
            if (isDelimiter(ch)) {
                printf("Delimiter: %c\n", ch);
            }
        } else {
            word[count++] = ch;
            if (count >= TOKEN_SIZE - 1) count = TOKEN_SIZE - 1; // prevent overflow
        }
    }

    // print last word if file didn’t end with delimiter
    if (count > 0) {
        word[count] = '\0';
        printf("Word: %s\n", word);
    }

    fclose(fp);
    return 0;
}
