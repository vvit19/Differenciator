#include "differenciator.h"
#include "dsl.h"
#include "utils.h"

#include <cmath>

Node* Diff (Node* node)
{
    assert (node);

    if (node->type == NUM) { return CreateNode (nullptr, nullptr, NUM, 0.0); }
    if (node->type == VAR) { return CreateNode (nullptr, nullptr, NUM, 1.0); }

    switch (node->value.op)
    {
    case ADD:
        return _ADD (dL, dR);

    case SUB:
        return _SUB (dL, dR);

    case MULT:
        return _ADD (_MULT (dL, cR), _MULT (cL, dR));

    case DIV:
        return _DIV (_SUB (_MULT (dL, cR), _MULT (cL, dR)), _POW (cR, CreateNode (nullptr, nullptr, NUM, 2.0)));

    case SIN:
        return _MULT (_SIN (cL, nullptr), dL);

    case COS:
        return _MULT (_MULT (_COS (cL, nullptr), dL), CreateNode (nullptr, nullptr, NUM, -1.0));

    case POW:
        if (node->left->type != NUM && node->right->type != NUM)
        {
            fprintf (stderr, "f(x)^g(x) format isn't supported\n");
            return nullptr;
        }

        if (node->right->type == NUM)
        {
            return _MULT (_MULT (cR, _POW (cL, CreateNode (nullptr, nullptr, NUM, node->right->value.num - 1))), dL);
        }

        return _MULT (_MULT (_POW (cL, cR), CreateNode (nullptr, nullptr, NUM, log (node->left->value.num))), dR);

    default:
        fprintf (stderr, "Default case reached in file: %s, function: %s, line: %d\n",
                          __FILE__, __PRETTY_FUNCTION__, __LINE__);
    }

    return nullptr;
}

elem_t Eval (Node* node, elem_t var_value)
{
    if (!node) return NAN;

    if (node->type == NUM) return node->value.num;
    if (node->type == VAR) return var_value;

    elem_t left_value  = Eval (node->left,  var_value);
    elem_t right_value = Eval (node->right, var_value);

    switch (node->value.op)
    {
    case ADD:  return left_value + right_value;
    case SUB:  return left_value - right_value;
    case MULT: return left_value * right_value;
    case DIV:  return left_value / right_value;
    case SIN:  return sin (right_value);
    case COS:  return cos (right_value);
    case POW:  return pow (left_value, right_value);
    case LN:   return log (right_value);

    default: printf ("Default case reached in file: %s, function: %s, line: %d",
                     __FILE__, __PRETTY_FUNCTION__, __LINE__);
    }

    return NAN;
}
