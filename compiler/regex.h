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
#ifndef _REGEX_H_
#define _REGEX_H_
#define PCRE2_CODE_UNIT_WIDTH 8
#include <stdio.h>
#include <pcre2.h>
#include <string.h>
#include <stdbool.h>

int regex_match(char **found, char *str, const char *regex, bool capture);

#endif // _REGEX_H_
