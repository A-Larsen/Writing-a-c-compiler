/* 
 * Copyright (C) 2024  Austin Larsen
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
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
    // the brace_depth will be incremented for every open brace and
    // decremented for every closing brace, if this is not zero when the lexer
    // finishs then an error occurs
    uint8_t brace_depth;
    uint8_t parenthesis_depth;
    uint64_t token_count;
    char *tokens[2048];
} Lexer;

void lexer_init(Lexer *lexer);
void lexer_run(Lexer *lexer, FILE *fp);
void lexer_free(Lexer *lexer);

#endif // _LEXER_H_
