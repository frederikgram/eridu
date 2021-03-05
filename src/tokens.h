enum TokenType {
    
    // Primitives
    TOK_INTEGER,
    TOK_FLOATING,
    TOK_STRING,

    // Keywords
    TOK_IF,
    TOK_WHILE,
    TOK_DEFINE,
    TOK_ELSE,
    TOK_FOR,
    TOK_RETURN,
    TOK_INT,
    TOK_FLOAT,
    TOK_STR,

    // SYMBOLS
    TOK_LPARENS,
    TOK_RPARENS,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_COMMA,
    TOK_SEMICOLON,

    // Operators
    TOK_PLUS,
    TOK_MINUS,
    TOK_DIVIDE,
    TOK_MULTIPLY,
    TOK_BIT_XOR,
    TOK_BIT_AND,
    TOK_BIT_OR,
    TOK_ASSIGN,
    TOK_LESS,
    TOK_GREAT,
    TOK_MOD,
    TOK_NOT,
    TOK_INCREMENT,
    TOK_DECREMENT,
    TOK_POWER,
    TOK_AND,
    TOK_OR,
    TOK_GEQ,
    TOK_LEQ,
    TOK_EQ,

    TOK_IDENTIFIER,

    // Error?
    ERROR

} TokenType;

typedef struct Token {
    
    char * value;
    int length;
    int start;
    int end;
    enum TokenType tokentype;

} Token;
