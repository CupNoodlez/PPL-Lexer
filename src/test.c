#include <stdio.h>
#include "tokens.h"

int main() {
    const char* testLexemes[] = {
        "scene", "action", "==", "and", "{", "hello", "42", NULL
    };

    for (int i = 0; testLexemes[i] != NULL; i++) {
        printf("%s is being checked\n", testLexemes[i]);
        const char* type = getTokenType(testLexemes[i]);
        printf("%s is a %s\n", testLexemes[i], type);
    }

    return 0;
}
