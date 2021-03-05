#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"

/* Operator & Separator definitions.
 * Keyword definitions are hardcoded into the
 * find_keyword_type() function
 */
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

    // Check if value starts end ends with quotation marks
    if(value_len >= 2 && value[0] == '"' && value[value_len-1] == '"'){
        return 1;
    }
    return 0;
}

/* Token Type Identifiers 
 * @TODO This seems so hardcoded and ugly... 
 * maybe I could implement a hashmap style solution instead
 */
enum TokenType find_single_char_operator_type(char operator){

    switch(operator){
        case '+':
            return TOK_PLUS;
        case '-':
            return TOK_MINUS;
        case '/':
            return TOK_DIVIDE;
        case '*':
            return TOK_MULTIPLY;
        case '^':
            return TOK_BIT_XOR;
        case '&':
            return TOK_BIT_AND;
        case '|':
            return TOK_BIT_OR;
        case '=':
            return TOK_ASSIGN;
        case '<':
            return TOK_LESS;
        case '>':
            return TOK_GREAT;
        case '%':
            return TOK_MOD;
        case '!':
            return TOK_NOT;
        default:
            return ERROR;
       
    }
}
enum TokenType find_dual_char_operator_type(char * value, int value_len) {
    
    if (value_len != 2) { return ERROR; }

    if (strncmp(value, "++", 2) == 0) { return TOK_INCREMENT; }
    if (strncmp(value, "--", 2) == 0) { return TOK_DECREMENT; }
    if (strncmp(value, "**", 2) == 0) { return TOK_POWER; }
    if (strncmp(value, "&&", 2) == 0) { return TOK_AND; }
    if (strncmp(value, "||", 2) == 0) { return TOK_OR; }
    if (strncmp(value, ">=", 2) == 0) { return TOK_GEQ; }
    if (strncmp(value, "<=", 2) == 0) { return TOK_LEQ; }
    if (strncmp(value, "==", 2) == 0) { return TOK_EQ; }

    return ERROR;

}

// @TODO Seems hardcoded and ugly
enum TokenType find_separator_type(char separator) {
    
    switch(separator){
        case '(':
            return TOK_LPARENS;
        case ')':
            return TOK_RPARENS;
        case '[':
            return TOK_LBRACKET;
        case ']':
            return TOK_RBRACKET;
        case '{':
            return TOK_LBRACE;
        case '}':
            return TOK_RBRACE;
        case ',':
            return TOK_COMMA;
        case ';':
            return TOK_SEMICOLON;
        default:
            return ERROR;
       
    }
}

// @TODO this is _really_ ugly.
enum TokenType find_keyword_type(char * value, int value_len) {
 
    if (value_len == 2 && strncmp(value, "if", 2) == 0)     { return TOK_IF; }
    if (value_len == 3 && strncmp(value, "for", 3) == 0)    { return TOK_FOR; }
    if (value_len == 3 && strncmp(value, "int", 3) == 0)    { return TOK_INT; }
    if (value_len == 3 && strncmp(value, "str", 3) == 0)    { return TOK_STR; }
    if (value_len == 4 && strncmp(value, "else", 4) == 0)   { return TOK_ELSE; }
    if (value_len == 5 && strncmp(value, "while", 5) == 0)  { return TOK_WHILE; }
    if (value_len == 5 && strncmp(value, "float", 5) == 0)  { return TOK_FLOAT; }
    if (value_len == 6 && strncmp(value, "define", 6) == 0) { return TOK_DEFINE; }
    if (value_len == 6 && strncmp(value, "return", 6) == 0) { return TOK_RETURN; }

    return ERROR;

}


enum TokenType identify_token_type(char * value, int value_len) {

    enum TokenType type;

    // Types that only tokens of length two or more can be
    if (value_len >= 2) {
        if (is_string(value, value_len)) { return TOK_STRING; }
        if (is_float(value, value_len))  { return TOK_FLOATING; }


        type = find_dual_char_operator_type(value, value_len);
        if (type != ERROR) { return type; }

        type = find_keyword_type(value, value_len);
        if (type != ERROR) { return type; }
    }

    if (is_integer(value, value_len)) { return TOK_INTEGER; }

    type = find_separator_type(value[0]);
    if (type != ERROR) { return type; }

    type = find_single_char_operator_type(value[0]);
    if (type != ERROR) { return type; }

    // Assume everything else is an identifier
    // if the first char is an alphabet
    if (isalpha(value[0]) != 0) { return TOK_IDENTIFIER; }
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
    if (token.tokentype == ERROR) {
        fprintf(stderr, "Lexical Error: No type could be identified for Token '%s'\n", token.value);
        exit(1);
    }

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
    //
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
