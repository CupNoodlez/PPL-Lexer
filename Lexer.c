#include <stdio.h>
#include "tokens.h"
#include "helpers.h"

int main() { 

    Token tokens[1000];
    Token buffer;

    while (FILE *fp = fopen("sample.stel", "r")) {

        // forward scan
        // if delimiter or whitespace, scan backward to form lexeme
        // store lexeme in buffer.lexeme


        getTokenType(buffer.lexeme);
    }




    outputTokens(tokens);

    return 0;
}