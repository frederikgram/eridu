#include "enums.h"

typedef struct Token {
    
    char * value;
    int length;
    int start;
    int end;
    enum CT_Type tokentype;

} Token;
