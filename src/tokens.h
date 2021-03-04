enum TokenType {
    
    // Primitives
    INTEGER,
    FLOATING,
    STRING,

    // Keywords
    IF,
    WHILE,
    DEFINE,
    ELSE,
    FOR,
    RETURN,
    INT,
    FLOAT,
    STR,
    VAR,

    // SYMBOLS
    LPARENS,
    RPARENS,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    COMMA,
    SEMICOLON,

    // Operators
    PLUS,
    MINUS,
    DIVIDE,
    MULTIPLY,
    BIT_XOR,
    BIT_AND,
    BIT_OR,
    ASSIGN,
    LESS,
    GREAT,
    MOD,
    NOT,
    INCREMENT,
    DECREMENT,
    POWER,
    AND,
    OR,
    GEQ,
    LEQ,
    EQ,

    IDENTIFIER,

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
