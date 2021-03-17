#define MAXIMUM_NODE_VALUE_SIZE 1024
#define MAXIMUM_NODE_NUM_CHILDREN 64

typedef struct AST_NODE {
    struct AST_NODE * parent;
    struct AST_NODE * children[MAXIMUM_NODE_NUM_CHILDREN];
    int num_children;

    enum CT_Type type;
    char value [MAXIMUM_NODE_VALUE_SIZE];
    int length;
} AST_NODE;


typedef struct AST {
    struct AST_NODE root;
} AST;


