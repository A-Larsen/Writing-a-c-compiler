#include "lexer.h"

#define option_lex 1 << 0;
#define option_parse 1 << 1;
#define option_codegen 1 << 2;
static int options = 0;

int main(int argc, char **argv) {
    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--lex") == 0)
            options |= option_lex;
    }

    Lexer lexer;

    char *file_name = argv[argc-1];
    FILE *fp = fopen(file_name, "r");
    lexer_init(&lexer);
    lexer_run(&lexer, fp);

    fclose(fp);
    return 0;
}
