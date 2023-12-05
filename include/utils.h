#ifndef UTILS_H
#define UTILS_H

#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cctype>

#define RED_COLOR   "\033[91m"
#define RESET_COLOR "\x1b[0m"

#define SYNT_ASSERT(...) \
    if (!(__VA_ARGS__)) { \
        fprintf (stderr, "%s""Syntax assertion failed in file: %s:%d, function: %s\n" \
                         "Statement (" #__VA_ARGS__ ") is false\n\n %s",  \
                         RED_COLOR, __FILE__, __LINE__, __PRETTY_FUNCTION__, RESET_COLOR); }

char*  get_file_content(const char* filename);
int    calc_nlines(char* buffer);
int    get_file_size(FILE* file);
bool   is_equal(double a, double b);
int    SkipSpaces (char* buffer, int i);
int    Factorial (int n);

#endif
