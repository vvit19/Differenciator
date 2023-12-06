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

const int MAX_VAR_LENGTH  = 20;
const int MAX_COMMAND_LENGTH = 100;
const int delta = 3;

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

const char* const OperationsArray[] = { "+", "-", "*", "/", "sin", "cos", "^", "ln", "(", ")" };

union Value
{
    elem_t num;
    Operations op;
    char var[MAX_VAR_LENGTH];
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

Node*  Diff       (Node* node, void (*DumpFunction) (Node*, Node*, const char*, bool));
elem_t Eval       (Node* node, elem_t var_value);
Node*  CreateNode (Node* left, Node* right, Types type, ...);
Node*  CopyNode   (Node* original_node);
void   TreeDtor   (Node* node);
Node*  GetGrammar (char* buffer);
void   Optimize   (Node** node);
Node*  Taylor     (Node* main_node, int degree, elem_t var_value);
Node*  Tangent    (Node* node, Node* diff, double x);

#endif
