/**/

#include <stdio.h>
#include <stdlib.h>

#include "lexer.c"
#include "ast.h"


/* Forward Declarations */
struct AST_NODE * _eat_statement_block(struct LexerStatus * lexer);
struct AST_NODE * _eat_type(struct LexerStatus * lexer);

/* Returns a pointer to a newly malloced bare-bones AST_NODE */
struct AST_NODE * _build_ast_node() {

    struct AST_NODE * node = (struct AST_NODE *) malloc(sizeof(struct AST_NODE *));
    node->max_num_children = STD_AST_NODE_NUM_CHILDREN;
    node->num_children = 0; 
    return node;

}

/* Exits with error-code if the given type was not found as the next token */
void assert(struct LexerStatus * lexer, enum TOKEN_TYPE type) {
    
    if (current_token(lexer)->type != type) {
        printf("PARSER: Expected type '%i' at token #%i but received '%i'\n", type, lexer->current_token_index, current_token(lexer)->type);
        exit(-1);
    } 

}

/* Exits with error-code if the given type was not found as the next token, increments */
void expect(struct LexerStatus * lexer, enum TOKEN_TYPE type) {
    
    if (current_token(lexer)->type != type) {
        printf("PARSER: Expected type '%i' at token #%i but received '%i'\n", type, lexer->current_token_index, current_token(lexer)->type);
        exit(-1);
    } 

    increment(lexer);
}

/* 2x'es the given AST_NODE's node->children array */
void increase_capacity(struct AST_NODE * node) {

    if (node->num_children >= node->max_num_children) {
        realloc(node->children, sizeof(struct AST_NODE *) * node->max_num_children * 2);
        node->max_num_children *= 2;
    }
}


/* Safely adds a child to a AST_NODE, ensuring capacity */
void add_child(struct AST_NODE * parent, struct AST_NODE * child) {
    increase_capacity(parent);
    parent->children[parent->num_children] = child;
    parent->num_children++;
}


/* ===== Finite State Machine ===== */






struct AST_NODE * _eat_wrapped_type(struct LexerStatus * lexer) {

    struct AST_NODE * this = _build_ast_node();
    this->type = AST_WRAPPED_TYPE;
    expect(lexer, LBRACKET);

    
    struct AST_NODE * child; 

    while((child = _eat_type(lexer)) != NULL) {

        add_child(this, child);
    }

    expect(lexer, RBRACKET);

    return this;

}

struct AST_NODE * _eat_type(struct LexerStatus * lexer) {

    struct AST_NODE * this = _build_ast_node();

    switch (current_token(lexer)->type) {

        case STR:
        case INT:
        case BYTE:
        case TYPE:
        case FLOAT:
        case ADDRESS:
        case CALLABLE:
            this->type = AST_TYPE;
            break;
        case IDENTIFIER:
            this->type = AST_TYPE_IDENTIFIER;
            this->token = current_token(lexer);
            break;

        default:
            free(this);
            return NULL;

    }

    this->token = current_token(lexer);
    increment(lexer);
    return this;
    
}

struct AST_NODE * _eat_any_type(struct LexerStatus * lexer) {

    struct AST_NODE * this = _build_ast_node();
    if (current_token(lexer)->type == LBRACKET) {
        this = _eat_wrapped_type(lexer);
    } else {
        this = _eat_type(lexer);
    }

    return this;
}


struct AST_NODE * _eat_argument(struct LexerStatus * lexer) {

    struct AST_NODE * this = _build_ast_node();
    this->type = AST_ARGUMENT;
     
    struct AST_NODE * type = _eat_any_type(lexer);
    add_child(this, type);

    assert(lexer, IDENTIFIER);
    struct AST_NODE * identifier = _build_ast_node();
    identifier->type = AST_IDENTIFIER;
    identifier->token = current_token(lexer);
    add_child(this, identifier); 
    increment(lexer);

    return this; 

}

struct AST_NODE * _eat_arguments(struct LexerStatus * lexer) {
    struct AST_NODE * this = _build_ast_node();
    this->type = AST_ARGUMENTS;
         

    struct AST_NODE * child; 

    // Try to eat statements until no more can be eaten
    while((child = _eat_argument) != NULL) {
        add_child(this, child);

        if (current_token(lexer)->type == COMMA) { increment(lexer); }
        else if (current_token(lexer)->type == RPARENS) { break; }
    }

    return this;
}


struct AST_NODE * _eat_expression(struct LexerStatus * lexer) {
}


struct AST_NODE * _eat_statement(struct LexerStatus * lexer) {

    struct AST_NODE * this = _build_ast_node();
    struct AST_NODE * statement_block;

    switch(current_token(lexer)->type) {

        case IF:
            this->type = AST_IF_STATEMENT;
            this->token = current_token(lexer);
            increment(lexer); 
            expect(lexer, LPARENS);

            struct AST_NODE * conditional = _eat_expression(lexer);
            add_child(this, conditional);
            expect(lexer, RPARENS);
            
            statement_block = _eat_statement_block(lexer);
            add_child(this, statement_block); 

            return this;

        case DEFINE:
            this->token = current_token(lexer);
            this->type = AST_DEFINE_STATEMENT;
            increment(lexer);
            
            struct AST_NODE * type = _eat_any_type(lexer);
            add_child(this, type); 

            assert(lexer, IDENTIFIER);
            struct AST_NODE * identifier = _build_ast_node();
            identifier->type = AST_IDENTIFIER;
            identifier->token = current_token(lexer);
            add_child(this, identifier); 
            increment(lexer);
        
            struct AST_NODE * arguments = _eat_arguments(lexer);
            add_child(this, arguments); 
            
            statement_block = _eat_statement_block(lexer); 
            add_child(this, statement_block); 
            
            return this;

        case ELSE:

            // AST_ELSE_IF_STATEMENT
            if (next_token(lexer)->type == IF) {
                increment(lexer);
                increment(lexer);
                this->type = AST_ELSE_IF_STATEMENT;
                expect(lexer, LPARENS);
                struct AST_NODE * conditional = _eat_expression(lexer);
                add_child(this, conditional);
                
            // AST_ELSE_STATEMENT
            } else {
                increment(lexer);
                this->type = AST_ELSE_STATEMENT;
            }

            struct AST_NODE * statement_block = _eat_statement_block(lexer);
            add_child(this, statement_block); 

            return this;

        case FOR:
            break;         
        case WHILE:
            break;         
        case RETURN:
            break;         
        default:
            break;

    }

    return NULL;

}

struct AST_NODE * _eat_statement_block(struct LexerStatus * lexer) {

    struct AST_NODE * this = _build_ast_node(); 
    this->type = AST_STATEMENT_BLOCK;
    
    expect(lexer, LBRACE);

    struct AST_NODE * child; 

    // Try to eat statements until no more can be eaten
    while((child = _eat_statement(lexer)) != NULL) {
        add_child(this, child);
    }

    expect(lexer, RBRACE);

    return this; 

}

struct AST_NODE * _eat_statement_list(struct LexerStatus * lexer) {
    
    struct AST_NODE * this = _build_ast_node(); 
    this->type = AST_STATEMENT_LIST;

    struct AST_NODE * child; 

    // Try to eat statements until no more can be eaten
    while((child = _eat_statement(lexer)) != NULL) {
        add_child(this, child);
    }

    return this; 
 
}



void dfs(struct AST_NODE * node, int depth)  {
    
    for (int i = 0; i < depth; i++) {
        printf("-");
    }

    if (node->token == NULL) {
        printf("TYPE: %i\n", node->type);
    } else {
        printf("%s\n", node->token->value);
    }

    for (int i = 0; i < node->num_children; i++){
        dfs(node->children[i], depth + 1);
    }

}

int main(int argc, char * argv[]){

    if (argc < 2) {
        exit(-1);
    }


    struct LexerStatus lexer = lex(argv[1]);

    printf("PARSER: Initiaizing Parsing Structures\n");
    struct AST_NODE * program = _build_ast_node();
    program->type = AST_PROGRAM;

    add_child(program, _eat_statement_list(&lexer));
    
    dfs(program, 0);
     
    
};
