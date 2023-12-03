#ifndef DIFF_H
#define DIFF_H

#include "utils.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <cctype>

typedef double elem_t;

const int MAX_TEXT_LENGTH = 100;

enum Ways
{
    LEFT,
    RIGHT,
};

enum Types
{
    NO_TYPE,
    OP,
    NUM,
    VAR,
};

enum Operations
{
    ADD,
    SUB,
    MULT,
    DIV,
    SIN,
    COS,
    POW,
    LN,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    END
};

struct CharOperation
{
    Operations op_name;
    char op_char;
};

const char* const OperationsArray[] = { "+", "-", "*", "/", "sin", "cos", "^", "ln" };

union Value
{
    elem_t num;
    Operations op;
    char var[MAX_TEXT_LENGTH];
};

struct Node
{
    Node* left;
    Node* right;

    Types type;
    Value value;
};

struct Token
{
    Types type;
    Value value;
};

Node*  Diff       (Node* node);
elem_t Eval       (Node* node, elem_t var_value);
Node*  CreateNode (Node* left, Node* right, Types type, ...);
Node*  CopyNode   (Node* original_node);
void   TreeDtor   (Node* node);
Node*  GetGrammar (char* buffer);
void   Optimize   (Node* node);

#endif
