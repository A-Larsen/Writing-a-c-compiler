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
#include "lexer.h"

static bool handle_comment(char *line, const char **token_regexs) {
    while(!regex_match(NULL, line,
                       token_regexs[TOKEN_MULTILINE_COMMENT_END],
                       false)) {
        if (line[0] == '\n') return true;
        int len = strlen(line) - 1;
        memcpy(line, line + 1, len);
        line[len] = '\0';
    }
    return false;
}

static void tokenizer(Lexer *lexer, char *line, uint32_t line_number) {
    static uint8_t second_token_check_type = 0;

    const char *token_regexs[TOKEN_COUNT] = {
        [TOKEN_KEYWORD] = "^(int\\b|void\\b|return\\b)",
        [TOKEN_IDENTIFIER] = "^[a-zA-Z_]\\w*\\b",
        [TOKEN_INTEGER_CONSTANT] = "^[0-9]+\\b",
        // opening tokens should be checked fist
        [TOKEN_OPEN_PARANTHESIS] = "^\\(",
        [TOKEN_OPEN_BRACE] = "^{",
        [TOKEN_SEMICOLON] = "^;",
        [TOKEN_MULTILINE_COMMENT_START] = "^/\\*",
        // The closing tokens should be last so they are checked last, and most
        // likely not checked at all in the 1st token check
        [TOKEN_CLOSE_PARANTHESIS] ="^\\)",
        [TOKEN_ClOSE_BRACE] = "^}",
        [TOKEN_MULTILINE_COMMENT_END] = "^\\*/",
    };

    while (true) {
REGEX_FOUND:
        // 2nd token check
        // ===============
        //     This second token check it to process syntax within an opening
        //     and closing delimiter
        switch(second_token_check_type) {
            case TOKEN_MULTILINE_COMMENT_START: {
                if (handle_comment(line, token_regexs)) return;
                break;
            }
            case TOKEN_OPEN_BRACE: {
                lexer->brace_depth++;
                break;
            }
            case TOKEN_ClOSE_BRACE: {
                lexer->brace_depth--;
                break;
            }
            case TOKEN_OPEN_PARANTHESIS: {
                lexer->parenthesis_depth++;
                break;
            };
            case TOKEN_CLOSE_PARANTHESIS: {
                lexer->parenthesis_depth--;
                break;
            }
        }

        second_token_check_type = 0;

        if (regex_match(NULL, line, "^\\n*$", false) || line[0] == EOF) break;
        regex_match(NULL, line, "^\\s+", false);

        // 1st token check
        // ===============
        //     This fist token check is to check high level tokens and see if a
        //     second token check is necisarry
        for (uint8_t i = 0; i < TOKEN_COUNT; ++i) {
            char *match = NULL;
            if (regex_match(&match, line, token_regexs[i], true)) {
                printf("%s\n", match);
                lexer->tokens[lexer->token_count++] = match;
                second_token_check_type = i;
                goto REGEX_FOUND;
            }
        }
        fprintf(stderr, "Error on line %d\n"
                        ">> %s",
                line_number, line);
        exit(1);
    }
}

void lexer_init(Lexer *lexer) {
    memset(lexer, 0, sizeof(Lexer));
}

void lexer_run(Lexer *lexer, FILE *fp) {
    uint32_t line_number = 1;
    char line[256];
    memset(line, 0, 256);
    int line_pos = 0;

    while (true) {
        int ch = getc(fp);
        line[line_pos] = ch;
        line_pos++;
        if (ch == '\n' || ch == EOF) {
            tokenizer(lexer, line, line_number);
            memset(line, 0, 255);
            line_pos = 0;
            line_number++;
        }
        if (ch == EOF)  break;
    }

    if (lexer->brace_depth != 0) {
        fprintf(stderr, "ERROR!\n\tNo Closing brace!");
        exit(1);
    }
    if (lexer->parenthesis_depth != 0) {
        fprintf(stderr, "ERROR!\n\tNo Closing paranthesis!");
        exit(1);
    }
}

void lexer_free(Lexer *lexer) {
    for (uint64_t i = 0; i < lexer->token_count; ++i) {
        free(lexer->tokens[i]);
    }
}

