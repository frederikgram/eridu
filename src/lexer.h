#include "tokens.h"

#define CHARACTER_BUFFER_SIZE 1024
#define ARRAY_OF_TOKENS_SIZE    102400


/* Lexer Utils */
typedef struct LexerStatus {
    char last_char;
    char current_char;
    char next_char;
    int cursor;
    int is_inside_comment;
    int is_inside_string;

    char buffer[CHARACTER_BUFFER_SIZE];
    int buffer_len;

    struct Token tokens[ARRAY_OF_TOKENS_SIZE];
    int tokens_len;

} LexerStatus;

struct LexerStatus lex(char * filename);
