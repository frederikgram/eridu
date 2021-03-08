/* Abstract Syntax Tree */

enum AST_NODE_TYPE {

    AST_ASSIGN,
    AST_EXPR,
    AST_STMT,
    AST_IF,
    AST_FOR,
    AST_WHILE,
    AST_FACTOR,
    AST_STRING,
    AST_INTEGER,
    AST_FLOATING,
    AST_IDENTIFIER,
    AST_MULT,
    AST_PLUS,
    AST_MINUS,
    AST_SUB,
    AST_DIV,
    AST_LESS,
    AST_GREAT,
    AST_EQ,
    AST_ARG,
    AST_COMMA,
    AST_SEMICOLON,
    AST_STR,
    AST_INT,
    AST_FLOAT,
    AST_RETURN,
    AST_PROG,
    AST_DEFINE,
    AST_PARAMS,
    AST_PARENS_EXPR,
    AST_BLOCK,
    AST_EOF

} AST_NODE_TYPE;


typedef struct AST_NODE {
    struct AST_NODE * parent;
    struct AST_NODE * children[64];
    int num_children;

    enum AST_NODE_TYPE type;
    char value [1024];
    int length;
} AST_NODE;


typedef struct AST {
    struct AST_NODE root;
} AST;


