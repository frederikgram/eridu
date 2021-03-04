/* Abstract Syntax Tree */

typedef struct AST_NODE {
    struct Token token;
    struct AST_NODE * parent;
    struct AST_NODE * children[16];
    int num_children;
} AST_NODE;


typedef struct AST {
    struct AST_NODE root;
} AST;


