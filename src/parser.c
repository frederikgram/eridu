/**/

#include <stdio.h>
#include <stdlib.h>

#include "lexer.c"
#include "utils.c"


struct Type * eat_type(struct Token ** head, struct Token * token) {

    struct Type * type = (struct Type *) malloc(sizeof(struct Type));

    bool is_wrapped = false;

    /* Ignore '[' ']' encasing as it's purely aesthetic */
    if (kind(token, TK_LBRACKET)) {
        is_wrapped = true;
        skip(head, token);
    }

    type->token = consume(head, *head);

    switch (type->token->kind) {
        case TK_INT:  type->kind = TYK_INT;  break;
        case TK_CHAR: type->kind = TYK_CHAR; break;
        // @TODO ... bool, float, double, etc.

        case TK_ARRAY:
            type->kind = TYK_ARRAY;
            type->next = eat_type(head, *head);
            if (type->next == NULL) {
                fprintf(stderr, "Expected type: 'type' after 'array' type at position %i\n", type->token->start);
                exit(1);

            }
            break;
                
        case TK_ADDRESS:
            type->kind = TYK_ADDRESS;
            type->next = eat_type(head, *head);

            if (type->next == NULL) {
                fprintf(stderr, "Expected type: 'type' after 'address' type at position %i\n", type->token->start);
                exit(1);
            }

            break;

        case TK_ENUM:
            type->kind = TYK_ENUM;
            type->identifier = consume_if_kind(head, *head, TK_IDENTIFIER);
            break;        
    
        case TK_STRUCT:
            type->kind = TYK_STRUCT;
            type->identifier = consume_if_kind(head, *head, TK_IDENTIFIER);
            break;        
    }
    

    /* If the next token is ']' skip it */
    if (is_wrapped) {skip_if_kind(head, *head, TK_RBRACKET); }    

    return type;

}


/* Temporary for testing*/
void print_type(struct Type * type) {

    printf("%s ", type->token->value);
    if (type->kind == TYK_ADDRESS || type->kind == TYK_ARRAY) {
        print_type(type->next);
    } else {
        printf("\n");
    }
    

}

int main(int argc, char * argv[]){

    if (argc < 2) {
        exit(-1);
    }

    struct Token * head = lex(argv[1]);
  

    //============ Print Types
    struct Type * t1 = eat_type(&head, head);
    print_type(t1);
        
    struct Type * t2 = eat_type(&head, head);
    print_type(t2);
        
    struct Type * t3 = eat_type(&head, head);
    print_type(t3);

    struct Type * t4 = eat_type(&head, head);
    print_type(t4);

    skip(&head, head);
    skip(&head, head);

    struct Type * t5 = eat_type(&head, head);
    print_type(t5);

    skip(&head, head);
    skip(&head, head);

    struct Type * t6 = eat_type(&head, head);
    print_type(t6);

    skip(&head, head);
    skip(&head, head);

    struct Type * t7 = eat_type(&head, head);
    print_type(t7); 
};
