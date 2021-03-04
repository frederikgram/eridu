#include "ast.h"

typedef struct ParserStatus {

    LexerStatus lexer;
    
    Token * current_token;
    Token * next_token;
    int token_idx;
    struct AST ast;


} ParserStatus;
