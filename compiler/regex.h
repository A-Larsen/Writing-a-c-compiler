#ifndef _REGEX_H_
#define _REGEX_H_
#define PCRE2_CODE_UNIT_WIDTH 8
#include <stdio.h>
#include <pcre2.h>
#include <string.h>
#include <stdbool.h>

int regex_match(char **found, char *str, const char *regex, bool capture);

#endif // _REGEX_H_
