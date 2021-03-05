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

void _build_node(AST_NODE * this, AST_NODE * parent, Token * token, int prop_value){

    this->parent = parent;
    if (prop_value) {
        strncpy(this->value, token->value, token->length);
        this->value_len = token->length;
    }
}

void _eat_assignment(ParserStatus * parser, AST_NODE * this) {
    // Assume that *this has already been malloc'd

    this->children[0] = malloc(sizeof(AST_NODE));

    // Ensure that our declaration starts
    // with a literal token.
    switch(parser->current_token->tokentype) {
        case TOK_INT:
            this->children[0]->type = AST_INT;
        case TOK_STR:
            this->children[0]->type = AST_STR;
        case TOK_FLOAT:
            this->children[0]->type = AST_FLOAT;
            break;
        default:
            fprintf(stderr, "Variable declaration at position %d must start with a literal tag such as int or str\n", parser->current_token->start);
            exit(-1);
    }

    // This node represents an ASSIGN statement
    this->type = AST_ASSIGN;
    increment(parser);
    
    if (parser->current_token->tokentype != TOK_IDENTIFIER) { exit(-1); }

    // left-hand side of the ASSIGN node
    // needs to contain the identifier and
    // its expected type
    this->children[0]->children[0] = malloc(sizeof(AST_NODE));
    _build_node(this->children[0]->children[0], this->children[0], parser->current_token, 1);
    increment(parser);
    
    // Sanity check, we want the token between the identifier
    // and the following expression to me an equals sign
    if (parser->current_token->tokentype != TOK_ASSIGN) { exit(-1); }
    increment(parser);

    // Get right hand side of the expression
    this->children[1] = malloc(sizeof(AST_NODE));
     _eat_expr(parser, this->children[1]);
    increment(parser);

}

void _eat_statement_list(ParserStatus * parser, AST_NODE * this) {
    increment(parser);
}

void _eat_expression(ParserStatus * parser, AST_NODE * this) {
    // Assume that *this has already been malloc'd

    switch(parser->current_token->tokentype) {
        case INTEGER:
            this->type = AST_INTEGER;
            this->value = parser->current_token->value;
            this->value_len = parser->current_token->length;
        case STRING:
            this->type = AST_STRING;
            this->value = parser->current_token->value;
            this->value_len = parser->current_token->length;
        case FLOATING:
            this->type = AST_FLOATING;
            this->value = parser->current_token->value;
            this->value_len = parser->current_token->length;
        default:
            break;
    }
    
    if (parser->current_token->tokentype == TOK_LPARENS) {
        this->type = AST_PARENS_EXPR;
        
        this->children[0] = malloc(sizeof(AST_NODE));
        _eat_expression(parser, this->children[0]);
        if (parser->current_token->tokentype == TOK_RPARENS) { exit(-1); }
    }

    increment(parser);
}  

void _eat_condition(ParserStatus * parser, AST_NODE * this) {
    increment(parser);
}  

void _eat_statement(ParserStatus * parser, AST_NODE * this) {
    // Assume that *this has already been malloc'd
    
    // Sadly, we can not just check for the first
    // char in a token to identify the type of statement
    // as e..g int and if has the same first char
    // because of this, we need to use strncmp
    
    switch(parser->current_token->value[0]) {
    
        // Start of new block
        case '{':
            this->type = AST_BLOCK;
            this->children[0] = malloc(sizeof(AST_NODE));
            increment(parser);
            _eat_statement_list(parser, this->children[0]);
            if (parser->current_token->tokentype != TOK_RBRACKET) { exit(-1); }
            increment(parser);
            break;

        // Start of while loop
        case 'w':
            this->type = AST_WHILE;
            this->children[0] = malloc(sizeof(AST_NODE));
            increment(parser);
            _eat_condition(parser, this->children[0]);
            if (parser->current_token->tokentype != TOK_LBRACKET) { exit(-1); }
            increment(parser);
            this->children[1] = malloc(sizeof(AST_NODE));
            _eat_statement_list(parser, this->children[1]); 
            break;

        // Start of return statement
        case 'r':
            this->type = AST_RETURN;
            this->children[0] = malloc(sizeof(AST_NODE));
            increment(parser);
            _eat_expression(parser, this->children[0]);
            break;
        case 'i':
            // Start of assignment statement
            if(strncmp(parser->current_token->value, "if", 2) == 0) {
                this->children[0] = malloc(sizeof(AST_NODE));
                _eat_assignment(parser, this->children[0]);
                break;
            }
            // Start of if statement
            this->type = AST_IF;
            increment(parser);
            if (parser->current_token->tokentype != TOK_LPARENS) { exit(-1); }
            increment(parser);

            this->children[0] = malloc(sizeof(AST_NODE));
            _eat_expression(parser, this->children[0]);
            if (parser->current_token->tokentype != TOK_RPARENS) { exit(-1); }
            increment(parser);

            this->children[1] = malloc(sizeof(AST_NODE));
            _eat_statement(parser, this->children[1]);
            break;
    }

    // Semicolon statement, weird huh?
    if (parser->current_token->tokentype == TOK_SEMICOLON) {
        this->type = AST_SEMICOLON;
        increment(parser);
    
    // Plain expression statement
    } else {
        this->children[0] = malloc(sizeof(AST_NODE));
        _eat_expression(parser, this->children[0]);
        if (parser->current_token->tokentype != TOK_SEMICOLON) { exit(-1); }
        increment(parser);
    }
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
    AST_NODE root;
    root.type = AST_PROG;
    root.children[0] = malloc(sizeof(AST_NODE));
    _eat_assignment(&parser, root.children[0]);

    if (root.children[0] == NULL) {
        printf("Program was found to be empty");
        exit(0);
    }
}
