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

static void parser(Lexer *lexer, char *line, uint32_t line_number) {
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

    static uint8_t second_token_check_type = 0;


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
            case TOKEN_OPEN_BRACE: lexer->brace_indicator++; break;
            case TOKEN_ClOSE_BRACE: lexer->brace_indicator--; break;
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
                free(match);
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
    lexer->brace_indicator = 0;
    lexer->parenthesis_indicator = 0;
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
            parser(lexer, line, line_number);
            memset(line, 0, 255);
            line_pos = 0;
            line_number++;
        }
        if (ch == EOF)  break;
    }

    if (lexer->brace_indicator != 0) {
        fprintf(stderr, "ERROR!\n\tNo Closing brace!");
        exit(1);
    }
}

