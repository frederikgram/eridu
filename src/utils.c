/**/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool equal(Token * token, char * string) {
    return memcmp(token->value, string, token->length) == 0 && string[token->length] == '\0';
}

void assert_equality(Token * token, char * string) {
    if (!equal(token, string)) {
        fprintf(stderr, "Expected Token with value '%s' at position '%i->%li' but received '%s'\n",
                string, token->start, token->start + strlen(string), token->value);
        exit(1);
    }
}

struct Token * skip_if_equal(struct Token * token, char * string) {
    assert_equality(token, string);
    return token->next; 
}

