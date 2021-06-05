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
    STRUCT,

    // Structural Types
    PARENS_EXPR,
    PROG,
    
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

    // ASSIGN also functions
    // as a reserved keyword
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

} Type;
