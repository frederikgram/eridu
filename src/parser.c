/**/

#include <stdio.h>
#include <stdlib.h>

#include "lexer.c"
#include "utils.c"


int main(int argc, char * argv[]){

    if (argc < 2) {
        exit(-1);
    }

    struct Token * head = lex(argv[1]);
    printf("Token: '%s'\n", head->value);
    
};
