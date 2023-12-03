#include "differenciator.h"
#include "dsl.h"
#include "dump.h"
#include "utils.h"

#include <cmath>

Node* Diff (Node* node)
{
    assert (node);

    if (node->type == NUM) { return CreateNode (nullptr, nullptr, NUM, 0.0); }
    if (node->type == VAR) { return CreateNode (nullptr, nullptr, NUM, 1.0); }

    Node* diff_node = nullptr;

    switch (node->value.op)
    {
    case ADD:
    {
        diff_node = _ADD (dL, dR);
        break;
    }

    case SUB:
    {
        diff_node = _SUB (dL, dR);
        break;
    }

    case MULT:
    {
        diff_node = _ADD (_MULT (dL, cR), _MULT (cL, dR));
        break;
    }

    case DIV:
    {
        diff_node = _DIV (_SUB (_MULT (dL, cR), _MULT (cL, dR)), _POW (cR, CreateNode (nullptr, nullptr, NUM, 2.0)));
        break;
    }

    case SIN:
    {
        diff_node = _MULT (_COS (cL, nullptr), dL);
        break;
    }

    case COS:
    {
        diff_node = _MULT (_MULT (_SIN (cL, nullptr), dL), CreateNode (nullptr, nullptr, NUM, -1.0));
        break;
    }

    case LN:
    {
        diff_node = _MULT (_DIV (CreateNode (nullptr, nullptr, NUM, 1.0), cR), dR);
        break;
    }

    case POW:
    {
        if (node->left->type != NUM && node->right->type != NUM)
        {
            fprintf (stderr, "f(x)^g(x) format isn't supported\n");
            return nullptr;
        }

        if (node->right->type == NUM)
            diff_node = _MULT (_MULT (cR, _POW (cL, CreateNode (nullptr, nullptr, NUM, node->right->value.num - 1))), dL);
        else
            diff_node = _MULT (_MULT (_POW (cL, cR), CreateNode (nullptr, nullptr, NUM, log (node->left->value.num))), dR);

        break;
    }

    default:
        fprintf (stderr, "Default case reached in file: %s, function: %s, line: %d\n",
                          __FILE__, __PRETTY_FUNCTION__, __LINE__);
        return nullptr;
    }

    TexDumpDerivative (node, diff_node, GetRandomPhraze ());

    return diff_node;
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

Node* CreateNode (Node* left, Node* right, Types type, ...)
{
    Node* node = (Node*) calloc (1, sizeof (Node));
    assert (node);

    node->left   = left;
    node->right  = right;
    node->type = type;

    if (type == NO_TYPE) return node;

    va_list value = {};
    va_start (value, type);

    switch (type)
    {
    case OP:
        node->value.op = (Operations) va_arg (value, int);
        break;

    case NUM:
        node->value.num = va_arg (value, elem_t);
        break;

    case VAR:
        strcpy (node->value.var, va_arg (value, const char*));
        break;

    default:
        fprintf (stderr, "Default case reached in file: %s, function: %s, line: %d\n",
                          __FILE__, __PRETTY_FUNCTION__, __LINE__);
    }

    va_end (value);

    return node;
}

void TreeDtor (Node* node)
{
    if (!node) return;

    if (node->left)  TreeDtor (node->left);
    if (node->right) TreeDtor (node->right);

    free (node);
    node = nullptr;
}

Node* CopyNode (Node* original_node)
{
    assert (original_node);

    Node* node = (Node*) calloc (1, sizeof (Node));
    assert (node);

    *node = *original_node;

    if (original_node->left)  node->left  = CopyNode (original_node->left);
    if (original_node->right) node->right = CopyNode (original_node->right);

    return node;
}
