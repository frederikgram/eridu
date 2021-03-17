/**/
/**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"
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

int decrement(ParserStatus * parser) {

    // No more tokens to be found.
    if (parser->token_idx <= parser->lexer.tokens_len - 1) {
        return 0;
    } else {
        parser->next_token = parser->current_token;
        parser->current_token = &parser->lexer.tokens[parser->token_idx];
    }

    parser->token_idx--;

    return 1;
}

int expect(ParserStatus * parser, enum CT_Type type) {

    if (parser->current_token->tokentype != type) {
        fprintf(stderr, "Expected 'CT_#%d' at position %d but got 'CT_#%d'\n", type, parser->current_token->start, parser->current_token->tokentype);
        exit(1);
    }

    increment(parser);
    return 1;

}

/* Helper functions for Abstract Syntax Tree Nodes */
AST_NODE build_node(AST_NODE * parent) {
    AST_NODE * this = malloc(sizeof(AST_NODE));
    this->type = CT_NULL;
    if (parent != NULL) { this->parent = parent; }
    return *this;
}

void add_child(AST_NODE * child, AST_NODE * parent) {
    child->parent = parent;
    parent->children[parent->num_children-1] = child;
    parent->num_children++; 
}


/* Functions for identification and conversion of CT types */
int is_binary_operator(enum CT_Type type) {

    switch (type) {
        case CT_MULTIPLY:
        case CT_DIVIDE:
        case CT_PLUS:
        case CT_MINUS:
        case CT_EQ:
        case CT_LESS:
        case CT_GREAT:
        case CT_POWER:
        case CT_MOD:
        case CT_LEQ:
        case CT_GEQ:
        case CT_OR:
        case CT_ASSIGN:
            return 1;
        default:
            return 0;
    }
}

int is_unary_operator(enum CT_Type type) {

    switch (type) {
        case CT_INCREMENT:
        case CT_DECREMENT:
        case CT_NOT:
        case CT_MINUS:
            return 1;
        default:
            return 0;
    }
}

int is_constant(enum CT_Type type) {

    switch (type) {
        case CT_STRING:
        case CT_FLOATING:
        case CT_INTEGER:
        case CT_IDENTIFIER:
            return 1;
        default:
            return 0;
    }
}

/* Functions for Finite state machine-like behavior */
AST_NODE eat_expression(ParserStatus * parser, AST_NODE * parent) {
    
    AST_NODE this = build_node(parent);
    this.type = CT_NULL;

    // Check for parenthesis encased expression
    if (parser->current_token->tokentype == CT_LPARENS) {
        this.type = CT_PARENS_EXPR;

        // Get the encased expression
        increment(parser);
        AST_NODE expression = eat_expression(parser, parent);
        add_child(&expression, &this);


        // Assert a right parenthesis terminator
        expect(parser, CT_RPARENS);

    // Check for Unary Operators
    } else if (is_unary_operator(parser->current_token->tokentype) == 1) {

        this.type = parser->current_token->tokentype;
        increment(parser);

        // Get following Expression
        AST_NODE expression = eat_expression(parser, parent);
        add_child(&expression, &this);

    
    // Check for constants
    } else if (is_constant(parser->current_token->tokentype) == 1) {
        
        // Check for binary operators
        if (is_binary_operator(parser->next_token->tokentype) == 1) {
            this.type = parser->next_token->tokentype;

            // Build left hand side of binary expression
            AST_NODE left_side = build_node(&this);
            left_side.type = parser->current_token->tokentype;
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
            printf("Found constant with value\t'%s'\twith type\t'#%d'\n", parser->current_token->value, parser->current_token->tokentype);
            this.type = parser->current_token->tokentype;
            strncpy(this.value, parser->current_token->value, parser->current_token->length);
            this.length = parser->current_token->length;
            increment(parser);
        }

    } else {
        printf("PARENT VALUE %s TYPE %d, THIS VALUE %s, THIS TYPE %d\n", parent->value, parent->type, parser->current_token->value, parser->current_token->tokentype);
        fprintf(stderr, "Tried to eat expression but no valid pattern was found at position %d\n", parser->current_token->start);
    }

    return this;
}  

AST_NODE eat_block(ParserStatus * parser, AST_NODE * parent) {
    AST_NODE block = build_node(parent);

    // Assert start of block
    expect(parser, CT_LBRACE);
    printf("---found start of block\n");

    // Eat all statements inside block
    eat_statement_list(parser, &block);

    // Assert end of block
    expect(parser, CT_RBRACE);
    printf("---found end of block\n");

    return block;
}


AST_NODE eat_parameters(ParserStatus * parser, AST_NODE * parent) {

    AST_NODE parameters = build_node(parent);
    while (parser->current_token->tokentype != CT_RPARENS) {
        AST_NODE param = build_node(&parameters);

        // Get type definition
        switch (parser->current_token->tokentype) {
            case CT_INT:
            case CT_FLOAT:
            case CT_STR:
                param.type = parser->current_token->tokentype;
                break;
            default:
                fprintf(stderr, "Expected type after define keyword at position %d\n", parser->current_token->start);
                exit(1);
            }

        // Setup AST_NODE attributes,
        // its not neccesary since its a structural
        // node but we do it for debug purposes
        strncpy(param.value, parser->current_token->value, parser->current_token->length);
        param.length = parser->current_token->length;

        // Get identifier
        increment(parser);
        AST_NODE identifier = build_node(&param);
        identifier.type = CT_IDENTIFIER;
        strncpy(identifier.value, parser->current_token->value, parser->current_token->length);
        identifier.length = parser->current_token->length;
        add_child(&identifier, &param);
        
        // If this was not the final parameter
        // increment past the expected comma
        increment(parser);
        if (parser->current_token->tokentype == CT_COMMA) {
            increment(parser);
        }
    }

    return parameters;
}

AST_NODE eat_statement(ParserStatus * parser, AST_NODE * parent) {

    AST_NODE this = build_node(parent);
    this.type = CT_NULL;

    // Shorthands
    enum CT_Type type = parser->current_token->tokentype;
    char * value      = parser->current_token->value;
    int length        = parser->current_token->length;
        
    if (type == CT_IF) {

        // Setup AST_NODE attributes
        this.type = CT_IF;
        strncpy(this.value, parser->current_token->value, parser->current_token->length);
        this.length = length;
        
        // Assert left parenthases before 'if' conditional
        increment(parser);
        expect(parser, CT_LPARENS);

        // Get Conditional of 'if' statement
        AST_NODE conditional = eat_expression(parser, parent);
        add_child(&conditional, &this);

        // Assert right parenthases after 'if' conditional
        expect(parser, CT_RPARENS);

        // Get block of statements
        AST_NODE block = eat_block(parser, parent);
        add_child(&block, &this);
        

    } else if (type == CT_FOR) {
            
        // Setup AST_NODE attributes
        this.type = CT_FOR;
        strncpy(this.value, parser->current_token->value, parser->current_token->length);
        this.length = parser->current_token->length;
        
        // Assert left parenthases before 'for' header
        increment(parser);
        expect(parser, CT_LPARENS);

        // Get Assignment 
        AST_NODE assignment = eat_statement(parser, parent);
        if (assignment.type != CT_INT && assignment.type != CT_STR && assignment.type != CT_FLOAT) {
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
        expect(parser, CT_SEMICOLON);

        // Get Incrementer
        AST_NODE incrementer = eat_expression(parser, parent);
        add_child(&incrementer, &this);
        
        // Assert right parenthases after 'for' header
        increment(parser);
        expect(parser, CT_RPARENS);

        // Get Block of statements
        AST_NODE block = eat_block(parser, parent);
        add_child(&block, &this);

    // The statement is an assigment to a newly defined variable
    } else if (type == CT_INT || type == CT_STR || type == CT_FLOAT) {

        // Setup AST_NODE attributes
        // This nested ternary just finds the correct INT, STR, or FLOAT type.
        this.type = type;
        strncpy(this.value, parser->current_token->value, parser->current_token->length);
        this.length = parser->current_token->length;

        // Get Identifier
        increment(parser);
        if (parser->current_token->tokentype != CT_IDENTIFIER) {
            fprintf(stderr, "Expected identifier after type declaration at position %d\n", parser->current_token->start);
            exit(1);
        }

        AST_NODE identifier = build_node(&this);
        identifier.type = CT_IDENTIFIER;
        strncpy(identifier.value, parser->current_token->value, parser->current_token->length);
        identifier.length = parser->current_token->length;
        add_child(&identifier, &this);

        // Assert '=' operator before expression
        increment(parser);
        expect(parser, CT_ASSIGN);

        // Get Expression 
        AST_NODE expression = eat_expression(parser, parent);
        add_child(&expression, &this);

        // Ensure proper closing of identifier assigment
        expect(parser, CT_SEMICOLON);
    
    // The statement defines a new function
    } else if (type == CT_DEFINE) {
    
        // Setup AST_NODE attributes
        this.type = CT_DEFINE;
        strncpy(this.value, parser->current_token->value, parser->current_token->length);
        this.length = parser->current_token->length;

        // Get type definition
        AST_NODE rettype = build_node(&this);
        increment(parser);
        switch (parser->current_token->tokentype) {
            case CT_INT:
            case CT_FLOAT:
            case CT_STR:
                rettype.type == parser->current_token->tokentype;
                break;
            default:
                fprintf(stderr, "Expected type after define keyword at position %d\n", parser->current_token->start);
                exit(1);
        }

        add_child(&rettype, &this);

        // Get Identifier
        increment(parser);
        if (parser->current_token->tokentype != CT_IDENTIFIER) {
            fprintf(stderr, "Expected identifier after define keyword in function definition at position %d\n", parser->current_token->start);
            exit(1);
        }

        AST_NODE identifier = build_node(&this);
        identifier.type = CT_IDENTIFIER;
        strncpy(identifier.value, parser->current_token->value, parser->current_token->length);
        identifier.length = parser->current_token->length;
        add_child(&identifier, &this);

        // Ensure left parenthesis after identifier in function definiton
        increment(parser);
        expect(parser, CT_LPARENS);

        // Get Parameters
        AST_NODE parameters = eat_parameters(parser, &this); 
        add_child(&parameters, &this);

        // Ensure right parentheses after parameters
        // technically redundant, but we do it for
        // consistency
        expect(parser, CT_RPARENS);
        
        // Get block of statements
        // that the function will perform
        AST_NODE block = eat_block(parser, &this);
        add_child(&block, &this);

    } else if (type == CT_RETURN) {
    
        // Setup AST_NODE attributes
        this.type = CT_RETURN;
        strncpy(this.value, parser->current_token->value, parser->current_token->length);
        this.length = parser->current_token->length;

        // Get expression to return
        increment(parser);
        AST_NODE return_value = eat_expression(parser, &this);
        add_child(&return_value, &this);
    
        printf("Returning value %s\n", return_value.value);
        // Assert ';' at the end of statement
        expect(parser, CT_SEMICOLON);
    
    } else if (type == CT_WHILE) {

        // Setup AST_NODE attributes
        this.type = CT_WHILE;
        strncpy(this.value, parser->current_token->value, parser->current_token->length);
        this.length = parser->current_token->length;
        
        // Assert left parenthases before 'while' header
        increment(parser);
        expect(parser, CT_LPARENS);

        // Get Conditional
        AST_NODE conditional = eat_expression(parser, parent);
        add_child(&conditional, &this);
        
        // Assert right parenthases after 'while' header
        expect(parser, CT_RPARENS);

        // Get Block of statements
        AST_NODE block = eat_block(parser, parent);
        add_child(&block, &this);

    /* 
        We assume that anything that doesn't fit
        into the patterns as defined above, is stand-alone
        expressions such as "a = 5" and so forth
    */
    } else {
        this = eat_expression(parser, parent);
        if (this.type == CT_NULL) { return this; }
        expect(parser, CT_SEMICOLON);
    }

    return this;
}

void eat_statement_list(ParserStatus * parser, AST_NODE * parent) {
    AST_NODE stmt_list = build_node(parent);
    
    while (parser->current_token->tokentype != CT_RBRACE) {
        AST_NODE stmt = eat_statement(parser, &stmt_list);
        add_child(&stmt, &stmt_list);

    }

}  


void draw(AST_NODE * node, int depth) {

    if (node == NULL) {
        return;
    } else if (node->length > 0) {
        printf("%s\n", node->value);
    } else {
        printf("CT_#%d\n", node->type);
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
    root.type = CT_PROG;
    root.num_children = 0;
    parser.ast.root = root; 

    AST_NODE statement_list = build_node(&parser.ast.root);
    eat_statement_list(&parser, &statement_list);
    add_child(&statement_list, &parser.ast.root);

    // Visualize the AST
    draw(&parser.ast.root, 0);
    draw(parser.ast.root.children[0], 1);
}

