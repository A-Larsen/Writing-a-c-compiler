#define PCRE2_CODE_UNIT_WIDTH 8
#include <stdio.h>
#include <pcre2.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

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

// the brace_indicator will be incremented for every open brace and
// decremented for every closing brace, if this is not zero when the lexer
// finishs then an error occurs
static uint8_t brace_indicator = 0;

static int options = 0;

// return 1 if there is a match, 0 otherwise
int regex_match(char **found, char *str, const char *regex, bool capture) {
    PCRE2_SPTR pattern = (PCRE2_SPTR)regex;
    PCRE2_SPTR subject = (PCRE2_SPTR)str;
    int error_number;

    PCRE2_SIZE error_offset;
    PCRE2_SIZE subject_length = (PCRE2_SIZE)strlen((char *)subject);

    pcre2_code *re = pcre2_compile(
        pattern,
        PCRE2_ZERO_TERMINATED,
        0,
        &error_number,
        &error_offset,
        NULL
    );

    if (re == NULL) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(error_number, buffer, sizeof(buffer));
        fprintf(stderr, "PCRE2 compilation failed at offset %d: %s\n", 
                (int)error_offset, buffer);
        exit(1);
    }

    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re,
                                                                        NULL);
    int rc = pcre2_match(
        re,
        subject,
        subject_length,
        0,
        0,
        match_data,
        NULL
    );

    if (rc < 0) {
        switch (rc) {
            case PCRE2_ERROR_NOMATCH: return 0;
            default: fprintf(stderr, "Matching error %d\n", rc);
                     break;
        }
        pcre2_match_data_free(match_data);
        pcre2_code_free(re);
        exit(1);
    }

    PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);

    if (rc == 0)
        fprintf(stderr, "ovector was not big enough for all the"
                        "captured substrings\n");

    PCRE2_SPTR substring_start = subject + ovector[0];
    PCRE2_SIZE substring_length = ovector[1] - ovector[0];

    if (capture) {
        *found = malloc(substring_length + 1);
        memcpy(*found, (char *)substring_start, (int)substring_length);
        (*found)[substring_length] = '\0';
    }

    int len = strlen(str) - substring_length;
    memcpy(str, str + substring_length, len); 
    str[len] = '\0';

    pcre2_match_data_free(match_data);
    pcre2_code_free(re);
    
    return 1;
}

bool handle_comment(char *line, const char **token_regexs) {
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



void lexer(char *line, uint32_t line_number) {
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
            case TOKEN_OPEN_BRACE: brace_indicator++; break;
            case TOKEN_ClOSE_BRACE: brace_indicator--; break;
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

int main(int argc, char **argv) {
    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--lex") == 0)
            options |= option_lex;
    }

    char *file_name = argv[argc-1];
    FILE *fp = fopen(file_name, "r");
    uint32_t line_number = 1;
    char line[256];
    memset(line, 0, 256);
    int line_pos = 0;

    while (true) {
        int ch = getc(fp);
        line[line_pos] = ch;
        line_pos++;
        if (ch == '\n' || ch == EOF) {
            lexer(line, line_number);
            memset(line, 0, 255);
            line_pos = 0;
            line_number++;
        }
        if (ch == EOF)  break;
    }
    if (brace_indicator != 0) {
        fprintf(stderr, "ERROR!\n\tNo Closing brace!");
        return 1;
    }

    fclose(fp);
    return 0;
}
