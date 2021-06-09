#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tokens.h"

#define STD_CHARACTER_BUFFER_SIZE 1024
#define STD_ARRAY_OF_TOKENS_SIZE    102400

typedef struct LexerStatus {
    char last_char;
    char current_char;
    char next_char;
    int cursor;
    int is_inside_comment;
    int is_inside_string;

    char buffer[STD_CHARACTER_BUFFER_SIZE];
    int buffer_len;

    struct Token tokens[STD_ARRAY_OF_TOKENS_SIZE];
    int tokens_len;

} LexerStatus;


const char separators[] = {'(', ')', '{', '}', ';', '[', ']', ':', ','};
const int num_separators = sizeof(separators) / sizeof(char);

const char single_char_operators[] = {'+', '-', '/', '*', '^', '&', '|', '<', '>', '=', '%', '!'};
const int num_single_char_operators = sizeof(single_char_operators) / sizeof(char);

const char dual_char_operators[8][2] = {"++", "--", "**", "&&", "||", ">=", "<=", "=="} ;
const int num_dual_char_operators = 8;

/* Type Checkers */
int is_separator(char c) {
    for (int i = 0; i < num_separators; i++) {
        if (c == separators[i]) {
            return 1;
        } 
    }
    return 0;
}

int is_single_char_operator(char c) {
    for (int i = 0; i < num_single_char_operators; i++) {
        if (c == single_char_operators[i]) {
            return 1;
        }
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
    if(value_len >= 2 && value[0] == '"' && value[value_len-1] == '"'){
        return 1;
    }
    return 0;
}

enum TokenType find_single_char_operator_type(char operator){
    switch(operator){
        case '+':
            return PLUS;
        case '-':
            return MINUS;
        case '/':
            return DIVIDE;
        case '*':
            return MULTIPLY;
        case '^':
            return BIT_XOR;
        case '&':
            return BIT_AND;
        case '|':
            return BIT_OR;
        case '=':
            return ASSIGN;
        case '<':
            return LESS;
        case '>':
            return GREAT;
        case '%':
            return MOD;
        case '!':
            return NOT;
        default:
            return ERROR;
       
    }
}
enum TokenType find_dual_char_operator_type(char * value, int value_len) {
    
    if (value_len != 2) { return ERROR; }

    if (strncmp(value, "++", 2) == 0) { return INCREMENT; }
    if (strncmp(value, "--", 2) == 0) { return DECREMENT; }
    if (strncmp(value, "**", 2) == 0) { return POWER; }
    if (strncmp(value, "&&", 2) == 0) { return AND; }
    if (strncmp(value, "||", 2) == 0) { return OR; }
    if (strncmp(value, ">=", 2) == 0) { return GEQ; }
    if (strncmp(value, "<=", 2) == 0) { return LEQ; }
    if (strncmp(value, "==", 2) == 0) { return EQ; }

    return ERROR;

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

enum TokenType find_keyword_type(char * value, int value_len) {
 
    if (value_len == 2 && strncmp(value, "if", 2) == 0)       { return IF; }
    if (value_len == 3 && strncmp(value, "for", 3) == 0)      { return FOR; }
    if (value_len == 3 && strncmp(value, "int", 3) == 0)      { return INT; }
    if (value_len == 3 && strncmp(value, "str", 3) == 0)      { return STR; }
    if (value_len == 4 && strncmp(value, "else", 4) == 0)     { return ELSE; }
    if (value_len == 4 && strncmp(value, "type", 4) == 0)     { return TYPE; }
    if (value_len == 5 && strncmp(value, "byte", 4) == 0)     { return BYTE; }
    if (value_len == 5 && strncmp(value, "while", 5) == 0)    { return WHILE; }
    if (value_len == 5 && strncmp(value, "float", 5) == 0)    { return FLOAT; }
    if (value_len == 5 && strncmp(value, "array", 5) == 0)    { return ARRAY; }
    if (value_len == 6 && strncmp(value, "struct", 6) == 0)   { return STRUCT; }
    if (value_len == 6 && strncmp(value, "lambda", 6) == 0)   { return LAMBDA; }
    if (value_len == 6 && strncmp(value, "define", 6) == 0)   { return DEFINE; }
    if (value_len == 6 && strncmp(value, "return", 6) == 0)   { return RETURN; }
    if (value_len == 8 && strncmp(value, "callable", 8) == 0) { return CALLABLE; }

    return ERROR;

}


enum TokenType identify_token_type(char * value, int value_len) {

    enum TokenType type;

    // Types that only tokens of length two or more can be
    if (value_len >= 2) {
        if (is_string(value, value_len)) { return STR; }
        if (is_float(value, value_len))  { return FLOAT; }


        type = find_dual_char_operator_type(value, value_len);
        if (type != ERROR) { return type; }

        type = find_keyword_type(value, value_len);
        if (type != ERROR) { return type; }
    }

    if (is_integer(value, value_len)) { return INT; }

    type = find_separator_type(value[0]);
    if (type != ERROR) { return type; }

    type = find_single_char_operator_type(value[0]);
    if (type != ERROR) { return type; }

    /* Assume everything else is an identifier 
    as long as the first character is an alpha character */
    if (isalpha(value[0]) != 0) { return IDENTIFIER; }
    else { return ERROR; }
}

void _increment(LexerStatus * status, FILE * ptr) {

        status->last_char    = status->current_char;
        status->current_char = status->next_char;
        status->next_char    = fgetc(ptr);
        status->cursor++;
}

void _build_token_from_buffer(LexerStatus * status) {

    // Sanity Check
    if (status->buffer_len == 0){ return; }

    struct Token token;
    token.value = malloc(sizeof(char) * (status->buffer_len + 1));
    strncpy(token.value, status->buffer, status->buffer_len);
    token.length = status->buffer_len;
    token.start = status->cursor - status->buffer_len + 1;
    token.end = status->cursor;

    token.tokentype = identify_token_type(token.value, token.length);

    if (token.tokentype == ERROR) {
        fprintf(stderr, "Lexical Error: No type could be identified for Token '%s' at position '%d'\n", token.value, token.start);
        exit(1);
    }

    #ifdef LEXER_PRINT
        fprintf(stdout, "TOKEN: %s\n", token.value);
    #endif

    // Reset Character Buffer
    status->buffer_len = 0;

    status->tokens[status->tokens_len] = token;
    status->tokens_len++;

}

void _add_current_char_to_buffer(LexerStatus * status) {

        status->buffer[status->buffer_len] = status->current_char;
        status->buffer_len++;
}

struct LexerStatus lex(char * filename) {

    // Load input file
    FILE * inp_fptr;
    inp_fptr = fopen(filename, "r");
    if (inp_fptr == NULL){
        fprintf(stderr, "Could not open file: '%s'\n", filename);
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

    return status;
}


int main(int argc, char * argv[]) {
    if (argc != 2) {
        return -1;
    }
    printf("LEXER: Attempting to lex file '%s'\n", argv[1]);
    struct LexerStatus lexer = lex(argv[1]);
    for (int i = 0; i < lexer.tokens_len; i++) {
        printf("Token #%d:\t%s\n", i, lexer.tokens[i].value);
    }
    
    return 0;
}


