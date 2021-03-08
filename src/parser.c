/* Parser section handling the construction of the initial Abstract Syntax Tree
 *
 * note: This could most definitely be implemented in way less code, using
 * regular expressions.  But I am trying to implement this using 
 * finite-state-machine like functionality, as I feel this will
 * give me a better understanding of the process behind Parsing
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Includes tokens.h
#include "lexer.h"
 
// Includes ast.h
#include "parser.h"

/* Helper functions for ParserStatus instances */

int increment(ParserStatus * parser) {

    // No more tokens to be found.
    if (parser->token_idx >= parser->lexer.tokens_len - 1) {
        return 0;
    } else if (parser->token_idx == parser->lexer.tokens_len -1) {
        parser->next_token = NULL;
    } else {
        parser->next_token = &parser->lexer.tokens[parser->token_idx + 2];
    }

    parser->current_token = &parser->lexer.tokens[parser->token_idx + 1];
    parser->token_idx++;

    return 1;
}

/* Helper functions for Abstract Syntax Tree Nodes */

AST_NODE build_node(AST_NODE * parent) {
    AST_NODE * this = malloc(sizeof(AST_NODE));
    if (parent != NULL) { this->parent = parent; }
    return *this;
}

int add_child(AST_NODE * child, AST_NODE * parent) {

    // @TODO Automatic resizing of AST_NODE children arrays
    if (parent->num_children == 64) { return -1; }

    child->parent = parent;

    parent->children[parent->num_children-1] = child;
    parent->num_children++;   
}


/* Functions for identification and conversion of TOKEN and AST types */

enum AST_NODE_TYPE binary_operator(enum TokenType type) {

    switch (type) {
        case TOK_MULTIPLY:
            return AST_MULT;
        case TOK_DIVIDE:
            return AST_DIV;
        case TOK_PLUS:
            return AST_PLUS;
        case TOK_MINUS:
            return AST_SUB;
        case TOK_EQ:
            return AST_EQ;
        case TOK_LESS:
            return AST_LESS;
        case TOK_GREAT:
            return AST_GREAT;
        default:
            return AST_NULL;
    }
}

/* Functions for Finite state machine-like behavior */

AST_NODE eat_expression(ParserStatus * parser, AST_NODE * parent) {
    
    AST_NODE this = build_node(parent);
    this.type = AST_NULL;

    // Shorthands
    enum TokenType type = parser->current_token->tokentype;
    char * value        = parser->current_token->value;
    int length          = parser->current_token->length;

    // Check for pre-fix Unary Operators
    if (type == TOK_NOT || type == TOK_INCREMENT  || TOK_DECREMENT) {
        this.type = (type == TOK_NOT ? (type == TOK_INCREMENT ? AST_INCREMENT : AST_DECREMENT) : AST_NOT);

        // Get the negated expression following the '!' operator
        increment(parser);
        AST_NODE expression = eat_expression(parser, parent);
        add_child(&expression, &this);
        
    // Check for parenthesis encased expression
    } else if (type == TOK_LPARENS) {
        this.type = AST_PARENS_EXPR;

        // Get the encased expression
        increment(parser);
        AST_NODE expression = eat_expression(parser, parent);
        add_child(&expression, &this);

        // Assert a right parenthesis terminator
        if (parser->current_token->tokentype != TOK_RPARENS) {
            fprintf(stderr, "Expected ')' at position %d\n", parser->current_token->start);
            exit(1);
        }

        increment(parser);

    // Check for constants
    } else if (type == TOK_INTEGER || type == TOK_STRING || type == TOK_FLOATING || type == TOK_IDENTIFIER) {
        

        // Get the AST type for the current token,
        // to avoid having to do this check
        // for both the case of binary and
        // constant expressions
        enum AST_NODE_TYPE this_tokens_ast_type;

        switch(type) {
            case TOK_INTEGER:
                this_tokens_ast_type = AST_INTEGER;
                break;
            case TOK_STRING:
                this_tokens_ast_type = AST_STRING;
                break;
            case TOK_FLOATING:
                this_tokens_ast_type = AST_FLOATING;
                break;
            case TOK_IDENTIFIER:
                this_tokens_ast_type = AST_IDENTIFIER;
                break;
            default:
                this_tokens_ast_type = AST_NULL;
        }

        // Check for binary operators
        enum AST_NODE_TYPE bot;
        if ((bot = binary_operator(parser->next_token->tokentype)) != AST_NULL) {
            this.type = bot;

            // Build left hand side of binary expression
            AST_NODE left_side = build_node(&this);
            left_side.type = this_tokens_ast_type;
            strncpy(left_side.value, parser->current_token->value, parser->current_token->length);
            left_side.length = parser->current_token->length;
            add_child(&left_side, &this);

            // Get right hand side of binary expression
            // we increment twice since we used the
            // parser->next_token to identify
            // if we had a binary operator or not
            increment(parser);
            increment(parser);
            AST_NODE right_side = eat_expression(parser, &this);
            add_child(&right_side, &this);

        } else {
            
            this.type = this_tokens_ast_type;
            strncpy(this.value, value, length);
            this.length = length;
            increment(parser);
        }
    }

    return this;
}  

AST_NODE eat_block(ParserStatus * parser, AST_NODE * parent) {
    increment(parser);
}

AST_NODE eat_assignment(ParserStatus * parser, AST_NODE * parent) {
    increment(parser);
}

AST_NODE eat_statement(ParserStatus * parser, AST_NODE * parent) {

    AST_NODE this = build_node(parent);
    this.type = AST_NULL;

    // Shorthands
    char * value  = parser->current_token->value;
    int length    = parser->current_token->length;

    if (strncmp(value, "if", 2) == 0) {

        // Setup AST_NODE attributes
        this.type = AST_IF;
        strncpy(this.value, value, length);
        this.length = length;
        
        // Assert left parenthases before 'if' conditional
        increment(parser);
        if (parser->current_token->tokentype != TOK_LPARENS) {
            fprintf(stderr, "Expected '(' at position %d\n", parser->current_token->start);
            exit(1);
        }

        // Get Conditional of 'if' statement
        increment(parser);
        AST_NODE conditional = eat_expression(parser, parent);
        add_child(&conditional, &this);
        
        // Assert right parenthases after 'if' conditional
        increment(parser);
        if (parser->current_token->tokentype != TOK_RPARENS) {
            fprintf(stderr, "Expected ';' at position %d, got value: %s\n", parser->current_token->start, parser->current_token->value);
            exit(1);
        }

        // Get block of statements
        increment(parser);
        AST_NODE block = eat_block(parser, parent);
        add_child(&block, &this);

    } else if (strncmp(value, "for", 3) == 0) {
            
            // Setup AST_NODE attributes
            this.type = AST_FOR;
            strncpy(this.value, value, length);
            this.length = length;
            
            // Assert left parenthases before 'for' header
            increment(parser);
            if (parser->current_token->tokentype != TOK_LPARENS) {
                fprintf(stderr, "Expected '(' at position %d\n", parser->current_token->start);
                exit(1);
            }

            // Get Assignment 
            increment(parser);
            AST_NODE assignment = eat_statement(parser, parent);
            if (assignment.type != AST_INT && assignment.type != AST_STR && assignment.type != AST_FLOAT) {
                fprintf(stderr, "Expected assignment inside of 'for' conditional\n");
                exit(1);
            }

            add_child(&assignment, &this);

            // There is no need to assert a proper closing of
            // the previous assignment using ';' as the
            // eat_statement section handling assignment
            // declarations asserts that itself
            
            // Get Conditional
            increment(parser);
            AST_NODE conditional = eat_expression(parser, parent);
            add_child(&conditional, &this);

            // Assert semicolon before incrementer declaration inside 'for' header
            increment(parser);
            if (parser->current_token->tokentype != TOK_SEMICOLON) {
                fprintf(stderr, "Expected ';' at position %d, got value: %s\n", parser->current_token->start, parser->current_token->value);
                exit(1);
            }
            
            // Get Incrementer
            increment(parser);
            AST_NODE incrementer = eat_expression(parser, parent);
            add_child(&incrementer, &this);
            
            // Assert right parenthases after 'for' header
            increment(parser);
            if (parser->current_token->tokentype != TOK_RPARENS) {
                fprintf(stderr, "Expected '()' at position %d\n", parser->current_token->start);
                exit(1);
            }

            // Get Block of statements
            increment(parser);
            AST_NODE block = eat_block(parser, parent);
            add_child(&block, &this);

        // The statement is an assigment 
        } else if (strncmp(value, "int", 3) == 0 || strncmp(value, "str", 3) == 0 || strncmp(value, "float", 5) == 0) {
            
            // Setup AST_NODE attributes
            // This nested ternary just finds the correct INT, STR, or FLOAT type.
            this.type = (parser->current_token->tokentype == TOK_INT ? (parser->current_token->tokentype == TOK_STR ? AST_STR : AST_FLOAT) : AST_INT);
            strncpy(this.value, value, length);
            this.length = length;

            // Get Identifier
            increment(parser);
            if (parser->current_token->tokentype != TOK_IDENTIFIER) {
                fprintf(stderr, "Expected identifier after type declaration at position %d\n", parser->current_token->start);
                exit(1);
            }

            AST_NODE identifier = build_node(&this);
            identifier.type = AST_IDENTIFIER;
            strncpy(identifier.value, parser->current_token->value, parser->current_token->length);
            identifier.length = parser->current_token->length;
            add_child(&identifier, &this);

            // Assert '=' operator before expression
            increment(parser);
            if (parser->current_token->tokentype != TOK_ASSIGN) {
                fprintf(stderr, "Expected '=' at position %d\n", parser->current_token->start);
                exit(1);
            }
            
            // Get Expression 
            increment(parser);
            AST_NODE expression = eat_expression(parser, parent);
            add_child(&expression, &this);

            // Ensure proper closing of identifier assigment
            if (parser->current_token->tokentype != TOK_SEMICOLON) {
                fprintf(stderr, "Expected ';' at position %d, got value: %s\n", parser->current_token->start, parser->current_token->value);
                exit(1);
            }
            increment(parser);
        }

    return this;
}

AST_NODE eat_statement_list(ParserStatus * parser, AST_NODE * parent) {
    AST_NODE stmt;
    while((stmt = eat_statement(parser, parent)).type != AST_NULL) {
        add_child(&stmt, parent);
    }
}  

void main(int argc, char * argv[]) {
    
    // Validate Arguments
    if (argv[1] == NULL) {
        fprintf(stderr, "No input file was given\n");
        exit(1);
    }

    // Initialize Parser
    ParserStatus parser;

    parser.lexer = lex(argv[1]);
    parser.token_idx = 0;
    
    if (parser.lexer.tokens_len < 2) {
        fprintf(stderr, "Not enough tokens were found after perfoming lexical analysis");
        exit(1);
    }

    // Initialize Parser Instance
    parser.current_token = &parser.lexer.tokens[0];
    parser.next_token = &parser.lexer.tokens[1];

    // Build Abstract Syntax Tree Stems
    AST_NODE root = build_node(NULL);
    root.type = AST_PROG;
    root.num_children = 0;
    parser.ast.root = root; 

    AST_NODE statement_list = eat_statement_list(&parser, &parser.ast.root);
    add_child(&statement_list, &parser.ast.root);

    // Visualize the final Abstract Syntax Tree
    
}

