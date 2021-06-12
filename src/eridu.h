/**/
enum TOKEN_KIND {
 
    TK_EQ,
    TK_IF,
    TK_OR,
    TK_AND,
    TK_FOR,
    TK_GEQ,
    TK_INT,
    TK_LEQ,
    TK_MOD,
    TK_NOT,
    TK_BYTE,
    TK_ELSE,
    TK_ERROR,
    TK_LESS,
    TK_PLUS,
    TK_TYPE,
    TK_ARRAY,
    TK_COMMA,
    TK_FLOAT,
    TK_GREAT,
    TK_MINUS,
    TK_POWER,
    TK_WHILE,
    TK_ASSIGN,
    TK_BIT_OR,
    TK_DEFINE,
    TK_DIVIDE,
    TK_DOUBLE,
    TK_LBRACE,
    TK_RBRACE,
    TK_RETURN,
    TK_STRING,
    TK_STRUCT,
    TK_ADDRESS,
    TK_BIT_AND,
    TK_BIT_XOR,
    TK_LPARENS,
    TK_RPARENS,
    TK_CALLABLE,
    TK_LBRACKET,
    TK_MULTIPLY,
    TK_RBRACKET,
    TK_DECREMENT,
    TK_INCREMENT,
    TK_SEMICOLON,
    TK_IDENTIFIER,

} TOKEN_KIND;

enum NODE_KIND {

  NK_ADD,       // +
  NK_ASSIGN,    // =
  NK_BITAND,    // &
  NK_BITNOT,    // ~
  NK_BITOR,     // |
  NK_BITXOR,    // ^
  NK_COMMA,     // ,
  NK_DIV,       // /
  NK_LT,        // <
  NK_MOD,       // %
  NK_MUL,       // *
  NK_NOT,       // !
  NK_SUB,       // -
  NK_COND,      // ?:
  NK_EQ,        // ==
  NK_LE,        // <=
  NK_LOGAND,    // &&
  NK_LOGOR,     // ||
  NK_NE,        // !=
  NK_DO,        // "do"
  NK_IF,        // "if"
  NK_CASE,      // "case"
  NK_ADDR,      // unary &
  NK_BLOCK,     // { ... }
  NK_DEREF,     // unary *
  NK_NEG,       // unary -
  NK_NUM,       // Integer
  NK_RETURN,    // "return"
  NK_SWITCH,    // "switch"
  NK_VAR,       // Variable
  NK_NULL_EXPR, // Do nothing
  NK_FUNCALL,   // Function call
  NK_FOR,       // "for" or "while"
  NK_EXPR_STMT, // Expression statement
  NK_STMT_EXPR, // Statement expression

} NODE_KIND;

enum TYPE_KIND {
    TYK_INT,
    TYK_BOOL,
    TYK_BYTE,
    TYK_CHAR,
    TYK_ENUM,
    TYK_NULL,
    TYK_TYPE,
    TYK_VOID,
    TYK_ARRAY,
    TYK_FLOAT,
    TYK_DOUBLE,
    TYK_STRING,
    TYK_STRUCT,
    TYK_ADDRESS,
    TYK_CALLABLE,

} TYPE_KIND;


typedef struct Token {
    
    char * value;

    int length;
    int start;
    int end;

    enum TOKEN_KIND kind;
    struct Token * next;
    
} Token;


typedef struct Node {


    enum NODE_KIND kind;
    enum TYPE_KIND tyk; 

    struct Token * token;
    struct Node * next;

    /* Fields used for
       specific NODE_KINDs
    */ 
    struct Node * conditional;
    struct Node * then;
    struct Node * els;
    struct Node * initial;
    struct Node * increment;
    struct Node * body;
    struct Node * function_type;
    struct Node * arguments;

} Node;

typedef struct Type {

    enum TYPE_KIND kind;
    int size;   
    struct Token  * token;
    
} Type;
