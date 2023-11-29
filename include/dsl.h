#ifndef DSL_H
#define DSL_H

#include "differenciator.h"

#define dL Diff (node->left)
#define dR Diff (node->right)
#define cL CopyNode (node->left)
#define cR CopyNode (node->right)

#define  _ADD(L, R)  CreateNode (L, R, OP, ADD )
#define  _SUB(L, R)  CreateNode (L, R, OP, SUB )
#define _MULT(L, R)  CreateNode (L, R, OP, MULT)
#define  _DIV(L, R)  CreateNode (L, R, OP, DIV )
#define  _SIN(L, R)  CreateNode (L, R, OP, SIN )
#define  _COS(L, R)  CreateNode (L, R, OP, COS )
#define  _POW(L, R)  CreateNode (L, R, OP, POW )
#define   _LN(L, R)  CreateNode (L, R, OP, LN  )

#endif
