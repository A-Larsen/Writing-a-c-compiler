#ifndef _LEXER_H_
#define _LEXER_H_
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "regex.h"

#define option_lex 1 << 0;
#define option_parse 1 << 1;
#define option_codegen 1 << 2;

#define TOKEN_KEYWORD 0
#define TOKEN_IDENTIFIER 1
#define TOKEN_INTEGER_CONSTANT 2
#define TOKEN_OPEN_PARANTHESIS 3
#define TOKEN_CLOSE_PARANTHESIS 4
#define TOKEN_OPEN_BRACE 5
#define TOKEN_ClOSE_BRACE 6
#define TOKEN_SEMICOLON 7
#define TOKEN_MULTILINE_COMMENT_START 8
#define TOKEN_MULTILINE_COMMENT_END 9
#define TOKEN_COUNT 10

typedef struct _Lexer {
    // the brace_indicator will be incremented for every open brace and
    // decremented for every closing brace, if this is not zero when the lexer
    // finishs then an error occurs
    uint8_t brace_indicator;
    uint8_t parenthesis_indicator;
} Lexer;

void lexer_init(Lexer *lexer);
void lexer_run(Lexer *lexer, FILE *fp);

#endif // _LEXER_H_
