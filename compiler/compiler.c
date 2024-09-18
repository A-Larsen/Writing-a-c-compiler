#define PCRE2_CODE_UNIT_WIDTH 8
#include <stdio.h>
#include <pcre2.h>
#include <stdint.h>
#include <string.h>

#define option_lex 1 << 0;
#define option_parse 1 << 1;
#define option_codegen 1 << 2;

static int options = 0;

int regex_match(char *str, char *regex)
    {
    pcre2_code *re;
    PCRE2_SPTR pattern = (PCRE2_SPTR)regex;
    PCRE2_SPTR subject = (PCRE2_SPTR)str;
    int error_number;
    int i;
    int rc;

    PCRE2_SIZE error_offset;
    PCRE2_SIZE *ovector;
    PCRE2_SIZE subject_length = (PCRE2_SIZE)strlen((char *)subject);
    pcre2_match_data *match_data;

    re = pcre2_compile(
        pattern,
        PCRE2_ZERO_TERMINATED,
        0,
        &error_number,
        &error_offset,
        NULL
        );

    if (re == NULL)
        {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(error_number, buffer, sizeof(buffer));
        fprintf(stderr, "PCRE2 compilation failed at offset %d: %s\n", 
                (int)error_offset, buffer);
        exit(1);
        }

    match_data = pcre2_match_data_create_from_pattern(re, NULL);
    rc = pcre2_match(
        re,
        subject,
        subject_length,
        0,
        0,
        match_data,
        NULL
        );

    if (rc < 0)
        {
        switch (rc)
            {
            case PCRE2_ERROR_NOMATCH: return 1;
            default: fprintf(stderr, "Matching error %d\n", rc);
                     break;
            }
        pcre2_match_data_free(match_data);
        pcre2_code_free(re);
        exit(1);
        }

    ovector = pcre2_get_ovector_pointer(match_data);
    printf("Match succeded at offset %d\n", (int)ovector[0]);

    if (rc == 0)
        fprintf(stderr, "ovector was not big enough for all the"
                        "captured substrings\n");

    for (i = 0; i < rc; i++)
        {
        PCRE2_SPTR substring_start = subject + ovector[2*i];
        PCRE2_SIZE substring_length = ovector[2*i+1] - ovector[2*i];
        printf("%2d: %.*s\n", i, (int)substring_length,
               (char *)substring_start);
        }
    return 0;

    }

int main(int argc, char **argv)
    {
    for (int i = 0; i < argc; ++i)
        {
        if (strcmp(argv[i], "--lex") == 0)
            {
            options |= option_lex;
            }
        }

    char *file_name = argv[argc-1];
    FILE *fp = fopen(file_name, "r");
    int ch = 0;
    char line[255];
    memset(line, 0, 255);
    int line_pos = 0;

    while ((ch = fgetc(fp)) != EOF)
        {
        line[line_pos] = ch;
        line_pos++;
        if (ch == '\n')
            {
            regex_match(line, "[a-zA-Z_]\\w*\\b");
            regex_match(line, "[0-9]+\\b");
            regex_match(line, "(int\\b|void\\b|return\\b)");
            regex_match(line, "\\)");
            regex_match(line, "\\(");
            regex_match(line, "\\{");
            regex_match(line, "\\}");
            regex_match(line, "\\;");
            memset(line, 0, 255);
            line_pos = 0;
            }
        }
    printf("options: %d\n", options);
    printf("file name: %s\n", file_name);
    return 0;
    }
