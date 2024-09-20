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
    if (!fp) {
        fprintf(stderr, "File does not exist\n");
        return 1;
    }
    lexer_init(&lexer);
    lexer_run(&lexer, fp);
    lexer_free(&lexer);

    fclose(fp);
    return 0;
}
