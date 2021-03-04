#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Includes tokens.h
#include "lexer.h"

// Includes ast.h
#include "parser.h"


// Returns 1 on success, 0 on EOF
int increment(ParserStatus * status) {

    // No more tokens to be found.
    if (status->token_idx >= status->lexer.tokens_len - 1) {
        return 0;
    } else if (status->token_idx == status->lexer.tokens_len -1) {
        status->next_token = NULL;
    } else {
        status->next_token = &status->lexer.tokens[status->token_idx + 2];
    }

    status->current_token = &status->lexer.tokens[status->token_idx + 1];
    status->token_idx++;

    return 1;
}

int expect(struct Token * token, enum TokenType expected_type) {
    return token->tokentype == expected_type;
}

AST_NODE * _build_node(AST_NODE * parent, Token * token){

    AST_NODE astn;
    astn.parent = parent;
    astn.token = *token;
    astn.num_children = 0;
    return &astn;
}


AST_NODE * _eat_expr(ParserStatus * parser) {
    return;
}

AST_NODE * _eat_statement(ParserStatus * parser) {
        
    AST_NODE this;

    switch(parser->current_token->value[0]) {
        
        // Handle compound statements
        case '{':
            this.children[0] = _build_node(&this, parser->current_token);
            increment(parser);
            this.children[1] = _eat_statement(parser);
            
            if (!expect(parser->current_token, RBRACKET)) {
                fprintf(stderr, "Expected } at position %d\n", parser->current_token->start);
                exit(1);
            }

            this.children[2] = _build_node(&this, parser->current_token);
            this.num_children = 3;
            break; 

        // Handle return statements
        case 'r':
            this.children[0] = _build_node(&this, parser->current_token);

            increment(parser);
            this.children[1] = _eat_expr(parser);

            if (!expect(parser->current_token, SEMICOLON)) {
                fprintf(stderr, "Expected ; at position %d\n", parser->current_token->start);
                exit(1);
            }

            this.children[2] = _build_node(&this, parser->current_token);
            this.num_children = 3;
            break;

        // Handle if and while statements
        case 'i':
        case 'w':
            this.children[0] = _build_node(&this, parser->current_token);
            increment(parser);
            
            if (!expect(parser->current_token, LPARENS)) {
                fprintf(stderr, "Expected ( at position %d\n", parser->current_token->start);
                exit(1);
            }

            this.children[1] = _build_node(&this, parser->current_token);
            increment(parser);
            this.children[2] = _eat_expr(parser);

            if (!expect(parser->current_token, RPARENS)) {
                fprintf(stderr, "Expected ) at position %d\n", parser->current_token->start);
                exit(1);
            }

            this.children[3] = _build_node(&this, parser->current_token);
            increment(parser);
            this.children[4] = _eat_statement(parser);
            this.num_children = 5;
            break;

        // Handle variable definitions
        case 'v':
            this.children[0] = _build_node(&this, parser->current_token);
            increment(parser);

            if (!expect(parser->current_token, INT) || !expect(parser->current_token, FLOAT) || !expect(parser->current_token, STR)) {
                fprintf(stderr, "Expected type declaration at position %d\n", parser->current_token->start);
                exit(1);
            }

            this.children[1] = _build_node(&this, parser->current_token);

            increment(parser);
            if (!expect(parser->current_token, IDENTIFIER)) {
                fprintf(stderr, "Expected type identifier for variable declaration at position %d\n", parser->current_token->start);
                exit(1);
            }
            this.children[2] = _build_node(&this, parser->current_token);

            increment(parser);
            if (!expect(parser->current_token, ASSIGN)) {
                fprintf(stderr, "Expected assignment operator '=' after identifier at position %d\n", parser->current_token->start);
                exit(1);
            }

            this.children[3] = _build_node(&this, parser->current_token);

            increment(parser);
            this.children[4] = _eat_expr(parser);
            
            if (!expect(parser->current_token, SEMICOLON)) {
                fprintf(stderr, "Expected ; after variable declaration at position %d\n", parser->current_token->start);
                exit(1);
            }
            this.children[5] = _build_node(&this, parser->current_token);
            this.num_children = 6; 
            break;

    }   

    increment(parser);
    return &this;

}

void main(int argc, char * argv[]) {
    
    // Initialize Parser
    ParserStatus parser;
    parser.lexer = lex(argv[1]);
    parser.token_idx = 0;
    
    if (parser.lexer.tokens_len < 2) {
        fprintf(stderr, "Not enough tokens were found after perfoming lexical analysis");
        exit(1);
    }

    parser.current_token = &parser.lexer.tokens[0];
    parser.next_token = &parser.lexer.tokens[1];

    // Setup Abstract Syntax Tree 
    AST ast;
    AST_NODE root;
    parser.ast = ast;
    parser.ast.root = root;
    parser.ast.root.num_children = 0;

    int c;
    while((c = increment(&parser) != 0)){
        printf("%s\n", parser.current_token->value);
        _eat_statement(&parser);
    }
}
