#include "ast.h"

typedef struct ParserStatus {

    LexerStatus lexer;
    
    Token * current_token;
    Token * next_token;
    int token_idx;


    struct AST ast;


} ParserStatus;

int increment(ParserStatus * parser);
AST_NODE build_node(AST_NODE * parent);
void add_child(AST_NODE * child, AST_NODE * parent);
void eat_statement_list(ParserStatus * parser, AST_NODE * parent);
