#ifndef DSL_H
#define DSL_H

#include "differenciator.h"

#define dL Diff (node->left, DumpFunction)
#define dR Diff (node->right, DumpFunction)
#define cL CopyNode (node->left)
#define cR CopyNode (node->right)

#define _NUM(val) CreateNumNode (nullptr, nullptr, val)
#define _VAR(var) CreateVarNode (nullptr, nullptr, var)

#define  _ADD(L, R)  CreateOpNode (L, R, ADD )
#define  _SUB(L, R)  CreateOpNode (L, R, SUB )
#define _MULT(L, R)  CreateOpNode (L, R, MULT)
#define  _DIV(L, R)  CreateOpNode (L, R, DIV )
#define  _SIN(L, R)  CreateOpNode (L, R, SIN )
#define  _COS(L, R)  CreateOpNode (L, R, COS )
#define  _POW(L, R)  CreateOpNode (L, R, POW )
#define   _LN(L, R)  CreateOpNode (L, R, LN  )

#define CUR_TOKEN  tokens[*cur_token]
#define NEXT_TOKEN tokens[*cur_token + 1]

#endif
