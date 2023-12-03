#ifndef UTILS_H
#define UTILS_H

#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cctype>

char* get_file_content(const char* filename);
int   calc_nlines(char* buffer);
int   get_file_size(FILE* file);
bool  is_equal(double a, double b);
int   SkipSpaces (char* buffer, int i);
void  ClearBuffer ();

#endif
