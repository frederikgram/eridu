/**/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool equal(Token * token, char * string) {
    return memcmp(token->value, string, token->length) == 0 && string[token->length] == '\0';
}

bool kind(Token * token, enum TOKEN_KIND kind) {
    return token->kind == kind;
}

void assert_equal(Token * token, char * string) {
    if (!equal(token, string)) {
        fprintf(stderr, "Expected Token with value '%s' at position '%i' but received '%s'\n",
                string, token->start, token->value);
        exit(1);
    }
}


void assert_kind(Token * token, enum TOKEN_KIND kind) {
    if (token->kind != kind) {
        fprintf(stderr, "Expected Token with kind '%i' at position '%i' but received '%i'\n",
                kind, token->start, token->kind);
        exit(1);
    }
}

struct Token * consume_if_equal(struct Token ** head, struct Token * token, char * string) {
    assert_equal(token, string);
    *head = token->next;
    return token;
}


struct Token * consume_if_kind(struct Token ** head, struct Token * token, enum TOKEN_KIND kind) {

    assert_kind(token, kind);
    *head = token->next;
    return token;
}

struct Token * consume(struct Token ** head, struct Token * token) {
    *head = token->next;
    return token;
}

void skip_if_kind(struct Token ** head, struct Token * token, enum TOKEN_KIND kind) {

    assert_kind(token, kind);
    *head = token->next;
}

void skip_if_equal(struct Token ** head, struct Token * token, char * string) {

    assert_equal(token, string);
    *head = token->next;
}


void skip(struct Token ** head, struct Token * token) {
    *head = token->next;
}

struct Node * _build_node() {

    struct Node * node = (struct Node *) malloc(sizeof(struct Node *));

    node->kind;
    node->tyk;

    node->next  = NULL;
    node->type  = NULL;
    node->token = NULL;
    node->scope = NULL;

    node->els           = NULL;
    node->body          = NULL;
    node->then          = NULL;
    node->initial       = NULL;
    node->arguments     = NULL;
    node->increment     = NULL;
    node->conditional   = NULL;
    node->function_type = NULL;


}
