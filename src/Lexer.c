#include <stdio.h>
#include "tokens.h"
#include "helpers.h"

int main() { 

    Token tokens[1000], buffer;

    FILE *fp = fopen("sample.stel", "r");
    if (!fp) {
        printf("Error opening file.\n");
        return 1;
    }

    char ch;
    while ((ch = fgetc(fp)) != EOF) {      
        // forward scan
        // if delimiter/whitespace (DW), scan backward until DW to form lexeme
        // store lexeme in buffer.lexeme

        // tokens[i].lexeme = buffer.lexeme;
        // tokens[i].type = getTokenType(buffer.lexeme);
    }


    // outputTokens(tokens);

    fclose(fp);
    return 0;
}