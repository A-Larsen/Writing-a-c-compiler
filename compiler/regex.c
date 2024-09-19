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
#include "regex.h"

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
