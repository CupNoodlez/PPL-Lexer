#include <stdio.h>
// declaration of syntax and defintion


int isAlpha(char ch){
  return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z' ));
}

int isDigit(char ch){
  return (ch == '0' || ch == '1' || ch == '2' || ch == '3'||
          ch == '4' || ch == '5' || ch == '6' || ch == '7' ||
          ch == '8' || ch == '9');
}

//check if Valid Identifier
int isAlphaNumeric(char lexeme[]) {
    int length = 0;
    while (lexeme[length] != '\0') length++;

    for (int i = 0; i < length; i++) {
        if (!(isAlpha(lexeme[i]) || isDigit(lexeme[i]))) {
            return 1; // invalid character
        }
    }

    return 0; // valid
}


int isIdentifier(char lexeme[]){

  if (isAlpha(lexeme[0]) || lexeme[0] == '_'){
    if(isAlpha(lexeme[0])){
       // check if it is a keyword
    } 

    if(lexeme[0] == '_'){
      isAlphaNumeric(lexeme);
    }

  } else {
    printf("Invalid Syntax");
    return 0;
  }

}


int isKeyword(char lexeme[]){
  //check if it is a keyword
  return;

}


int main(){

  char lexeme[50];

  //filehandling
  printf("Enter a word: ");
  scanf("%s", lexeme);

  if(isIdentifier(lexeme)){
    printf("\nvalid!");
  } else{
    printf("invalid");
  }

  return 0;
}