enum CT_Type {
    
    // Primitives
    CT_INTEGER,
    CT_FLOATING,
    CT_STRING,
    
    // Keywords
    CT_IF,
    CT_WHILE,
    CT_DEFINE,
    CT_ELSE,
    CT_FOR,
    CT_RETURN,
    CT_INT,
    CT_FLOAT,
    CT_STR,

    // Structural Types
    CT_PARENS_EXPR,
    CT_PROG,
    
    // Seperators
    CT_LPARENS,
    CT_RPARENS,
    CT_LBRACE,
    CT_RBRACE,
    CT_LBRACKET,
    CT_RBRACKET,
    CT_COMMA,
    CT_SEMICOLON,

    // Binary Operators
    CT_PLUS,
    CT_MINUS,
    CT_DIVIDE,
    CT_MULTIPLY,
    CT_POWER,
    CT_AND,
    CT_OR,
    CT_GEQ,
    CT_LEQ,
    CT_EQ,
    CT_MOD,
    CT_GREAT,
    CT_LESS,

    // CT_ASSIGN also functions
    // as a reserved keyword
    CT_ASSIGN,

    // Binary Bit Operators
    CT_BIT_XOR,
    CT_BIT_AND,
    CT_BIT_OR,

    // Unary operators
    CT_NOT,
    CT_INCREMENT,
    CT_DECREMENT,

    // Utils
    CT_IDENTIFIER,
    CT_ERROR,
    CT_NULL

} CT_Type;
