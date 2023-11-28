#ifndef TREE_H
#define TREE_H

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <cctype>

typedef double elem_t;

const int MAX_VAR_LENGTH = 10;

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
    POW
};

struct CharOperation
{
    Operations op_name;
    char op_char;
};

const CharOperation OperationsArray[] =
{
    {ADD, '+'},
    {SUB, '-'},
    {MULT, '*'},
    {DIV, '/'},
    {SIN, 'S'},
    {COS, 'C'},
    {POW, '^'}
};

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

Node* Diff       (Node* node);
Node* CreateNode (Node* left, Node* right, Types type, ...);
Node* CopyNode   (Node* original_node);
void  TreeDtor   (Node* node);
char* GetTree    (Node* node, char* buffer);
void  TreeDump   (Node* node);

#endif
