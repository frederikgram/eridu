#include <stdio.h>                                                              
#include <stdlib.h>                                                             
#include <ctype.h>                                                              
#include <string.h>                                                             
#include <stdarg.h>


#define CHARACTER_BUFFER_SIZE   1024
#define ARRAY_OF_TOKENS_SIZE   102400

/* Token Utils */
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

    // SYMBOLS
    LPARENS,
    RPARENS,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    COMMA,
    SEMICOLON,

    IDENTIFIER,

    // Placeholder for all operators
    OPERATOR,

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



/* Reserved Keywords, operators & separator definitions */
const char separators[] = {'(', ')', '{', '}', ';', '[', ']', ':', ','};
const int num_separators = sizeof(separators) / sizeof(char);

const char single_char_operators[] = {'+', '-', '/', '*', '^', '&', '|', '<', '>', '=', '%', '!'};
const int num_single_char_operators = sizeof(single_char_operators) / sizeof(char);

const char dual_char_operators[8][2] = {"++", "--", "**", "&&", "||", ">=", "<=", "=="} ;
const int num_dual_char_operators = 8;

/* Type Identifiers */
int is_separator(char c) {
    
    for (int i = 0; i < num_separators; i++) {
        if (c == separators[i]) {
            return 1;
        } 
    }
    return 0;
}

enum TokenType find_separator_type(char separator) {
    
    switch(separator){
        case '(':
            return LPARENS;
        case ')':
            return RPARENS;
        case '[':
            return LBRACKET;
        case ']':
            return RBRACKET;
        case '{':
            return LBRACE;
        case '}':
            return RBRACE;
        case ',':
            return COMMA;
        case ';':
            return SEMICOLON;
        default:
            return ERROR;
       
    }

}

int is_single_char_operator(char c) {

    for (int i = 0; i < num_single_char_operators; i++) {
        if (c == single_char_operators[i]) { return 1; }
    }
    return 0;
}

int is_dual_char_operator(char a, char b) {

    for (int i = 0; i < num_dual_char_operators; i++) {
        if (a == dual_char_operators[i][0] && b == dual_char_operators[i][1]) { 
            return 1;
        }
    }
    return 0;

}

int is_float(char * value, int value_len){

    int has_radix_point = 0;

    for (int i = 0; i < value_len; i++){
        if (isdigit(value[i]) == 0) {
            if (value[i] == '.' && has_radix_point == 0) {
                if (i == 0 || i == value_len-1) {
                    return 0;
                }

                has_radix_point = 1;
            } else {
                return 0;
            }
        }
    }
    return 1;
}

int is_integer(char * value, int value_len) {
    for (int i = 0; i < value_len; i++){
        if (isdigit(value[i]) == 0) {
            return 0;
        }
    }
    return 1;
}

int is_string(char * value, int value_len) {

    // Check if value starts end ends with quotation marks
    if(value_len >= 2 && value[0] == '"' && value[value_len-1] == '"'){
        return 1;
    }
    return 0;
}

enum TokenType find_keyword_type(char * value, int value_len) {

    if (strncmp(value, "if", 2) == 0) { return IF; }
    if (strncmp(value, "for", 3) == 0) { return FOR; }
    if (strncmp(value, "int", 3) == 0) { return INT; }
    if (strncmp(value, "str", 3) == 0) { return STR; }
    if (strncmp(value, "else", 4) == 0) { return ELSE; }
    if (strncmp(value, "while", 5) == 0) { return WHILE; }
    if (strncmp(value, "float", 5) == 0) { return FLOAT; }
    if (strncmp(value, "define", 6) == 0) { return DEFINE; }

    return ERROR;

}


enum TokenType identify_token_type(char * value, int value_len) {

    // Types that only tokens of length two or more can be
    if (value_len >= 2) {
        if (is_string(value, value_len))               { return STRING; }
        if (is_dual_char_operator(value[0], value[1])) { return OPERATOR; }
        if (is_float(value, value_len))                { return FLOATING; }

        enum TokenType type = find_keyword_type(value, value_len);
        if (type != ERROR) {
            return type;
        }
    }

    // Types that tokens of any non-empty length can be
    if (is_integer(value, value_len))              { return INTEGER; }
    if (is_separator(value[0]))                    { return find_separator_type(value[0]); }
    if (is_single_char_operator(value[0]))         { return OPERATOR; }

    // Assume everything else is an identifier
    return IDENTIFIER;
}

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

void _increment(LexerStatus * status, FILE * ptr) {

        status->last_char    = status->current_char;
        status->current_char = status->next_char;
        status->next_char    = fgetc(ptr);
        status->cursor++;
}

void _build_token_from_buffer(LexerStatus * status) {

    if (status->buffer_len == 0){
        return;
    }

    struct Token token;
    token.value = malloc(sizeof(char) * (status->buffer_len + 1));
    strncpy(token.value, status->buffer, status->buffer_len);
    token.length = status->buffer_len;
    token.start = status->cursor - status->buffer_len + 1;
    token.end = status->cursor;

    token.tokentype = identify_token_type(token.value, token.length);

    // Reset Character Buffer
    status->buffer_len = 0;
    status->tokens[status->tokens_len] = token;
    status->tokens_len++;

}

void _add_current_char_to_buffer(LexerStatus * status) {

        // If we're not inside a comment block, we can safely add the current
        // character to our character buffer, to be used next time we build a token.
        status->buffer[status->buffer_len] = status->current_char;
        status->buffer_len++;
}

void main(int argc, char *argv[]){

    // Load input file
    FILE * inp_fptr;
    inp_fptr = fopen(argv[1], "r");
    if (inp_fptr == NULL){
        fprintf(stderr, "Could not open file: '%s'\n", argv[1]);
    }

    // Initialize Tracker
    struct LexerStatus status;
    status.last_char = '\0';
    status.current_char = fgetc(inp_fptr);
    status.next_char    = fgetc(inp_fptr);

    status.cursor            = 0;
    status.is_inside_comment = 0;
    status.is_inside_string  = 0;
    status.buffer_len        = 0;
    status.tokens_len        = 0;

    char c;

    while (status.current_char != EOF){
            
        /* Handle Comment blocks */
        if (!status.is_inside_comment) {
        
            // We're now inside a comment block
            if (status.current_char == '/' && status.next_char == '*') {
                status.is_inside_comment = 1; 
                _increment(&status, inp_fptr);
                continue;
            }

        } else {
       
            // We've now espaced the comment block 
            if (status.last_char == '*' && status.current_char == '/') {
                status.is_inside_comment = 0;
            }
            
            _increment(&status, inp_fptr);
            continue;
        }

        /* Token Detection */
        if (isspace(status.current_char) && !status.is_inside_string && !status.is_inside_comment) {
        
            // Assume that our buffer represents a token
            // and try to build it
            if (status.buffer_len > 0) {
                _build_token_from_buffer(&status);        
            }

            _increment(&status, inp_fptr);
            continue;
        }

        // We've exited a string
        if (status.is_inside_string && status.last_char != '\\' && status.current_char == '\"' && !status.is_inside_comment) {
            status.is_inside_string = 0;
            _add_current_char_to_buffer(&status);
            _build_token_from_buffer(&status);
            _increment(&status, inp_fptr);
            continue;

        // We've entered a string
        } else if (!status.is_inside_string && status.last_char != '\\' && status.current_char == '\"' && !status.is_inside_comment){
            status.is_inside_string = 1;
            _build_token_from_buffer(&status);
            _add_current_char_to_buffer(&status);
            _increment(&status, inp_fptr);
            continue;
        }
        
        // Check if the current token is a separator
        // if it is, we convert both the current buffer and it
        // into two seperate tokens, then increment the lexer status.
        if (is_separator(status.current_char) && !status.is_inside_string && !status.is_inside_comment) {
            _build_token_from_buffer(&status);
            _add_current_char_to_buffer(&status);
            _build_token_from_buffer(&status);
            _increment(&status, inp_fptr);
            continue;
        }

        // If the current char could combine with the next char
        // to create a two-character operator such as ==, handle it.
        if (is_dual_char_operator(status.current_char, status.next_char) && !status.is_inside_string && !status.is_inside_comment){
            _build_token_from_buffer(&status);
            _add_current_char_to_buffer(&status);
            _increment(&status, inp_fptr);
            _add_current_char_to_buffer(&status);
            _build_token_from_buffer(&status);
            _increment(&status, inp_fptr);
            continue;
        
        }

        // This could be combined with the is_separator check
        // but this feels more explicit and readable
        if (is_single_char_operator(status.current_char) && !status.is_inside_string && !status.is_inside_comment) {
            _build_token_from_buffer(&status);
            _add_current_char_to_buffer(&status);
            _build_token_from_buffer(&status);
            _increment(&status, inp_fptr);
            continue;
        
        }

       _add_current_char_to_buffer(&status); 
       _increment(&status, inp_fptr);
    }

    for (int i = 0; i < status.tokens_len; i++) {
        printf("value\t%s\tstart\t%d\tend\t%d\ttype\t%d\n", status.tokens[i].value, status.tokens[i].start, status.tokens[i].end, status.tokens[i].tokentype);
    }
    
}
