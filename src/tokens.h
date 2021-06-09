enum TokenType {
    
    // Keywords
    IF,
    WHILE,
    DEFINE,
    ELSE,
    FOR,
    RETURN,

    /* Types */
    INT,
    STR,
    BYTE,
    TYPE,
    FLOAT,
    ARRAY,
    LAMBDA,
    STRUCT,
    CALLABLE,

    // Seperators
    LPARENS,
    RPARENS,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    COMMA,
    SEMICOLON,

    // Binary Operators
    PLUS,
    MINUS,
    DIVIDE,
    MULTIPLY,
    POWER,
    AND,
    OR,
    GEQ,
    LEQ,
    EQ,
    MOD,
    GREAT,
    LESS,
    ASSIGN,

    // Binary Bit Operators
    BIT_XOR,
    BIT_AND,
    BIT_OR,

    // Unary operators
    NOT,
    INCREMENT,
    DECREMENT,

    // Utils
    IDENTIFIER,
    ERROR

} TokenType;

typedef struct Token {
    
    char * value;
    int length;
    int start;
    int end;
    enum TokenType tokentype;

} Token;

