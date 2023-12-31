#include "differenciator.h"
#include "dsl.h"
#include "dump.h"
#include "utils.h"

#include <cmath>

static bool FindVariables (Node* node);

Node* Diff (Node* node, void (*DumpFunction) (Node*, Node*, const char*, bool))
{
    assert (node);

    if (node->type == NUM) { return _NUM (0.0); }
    if (node->type == VAR) { return _NUM (1.0); }

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
            diff_node = _DIV (_SUB (_MULT (dL, cR), _MULT (cL, dR)), _POW (cR, _NUM(2.0)));
            break;
        }

    case SIN:
        {
            diff_node = _MULT (_COS (cL), dL);
            break;
        }

    case COS:
        {
            diff_node = _MULT (_MULT (_SIN (cL), dL), _NUM(-1.0));
            break;
        }

    case LN:
        {
            diff_node = _MULT (_DIV (_NUM(1.0), cL), dL);
            break;
        }

    case POW:
        {
            bool left_var  = FindVariables (node->left);
            bool right_var = FindVariables (node->right);

            if (left_var && right_var)
            {
                Node* sub_node =  Diff (_MULT (_LN (cL), cR), DumpFunction);
                diff_node = _MULT (_POW (cL, cR), sub_node);
                free (sub_node);
            }
            else
            {
                if (left_var) diff_node = _MULT (_MULT (cR, _POW (cL, _NUM(node->right->value.num - 1))), dL);
                else if (right_var) diff_node = _MULT (_MULT (_POW (cL, cR), _LN(cL)), dR);
            }

            break;
        }

    default:
        {
        NO_PROPER_CASE_FOUND;

        return nullptr;
        }
    }

    if (DumpFunction) DumpFunction (node, diff_node, GetRandomPhraze(), true);

    return diff_node;
}

static bool FindVariables (Node* node)
{
    assert (node);

    bool var = false;

    if (node->left)  var = FindVariables (node->left);
    if (var) return var;

    if (node->right) var = FindVariables (node->right);
    if (var) return var;

    if (node->type == VAR) var = true;

    return var;
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
    case SIN:  return sin (left_value);
    case COS:  return cos (left_value);
    case POW:  return pow (left_value, right_value);
    case LN:   return log (left_value);

    default: printf ("Default case reached in file: %s, function: %s, line: %d",
                     __FILE__, __PRETTY_FUNCTION__, __LINE__);
    }

    return NAN;
}

Node* CreateOpNode (Node* left, Node* right, Operations op)
{
    Node* node = (Node*) calloc (1, sizeof (Node));
    assert (node);

    node->left     = left;
    node->right    = right;

    node->type     = OP;
    node->value.op = op;

    return node;
}

Node* CreateVarNode (Node* left, Node* right, const char* var)
{
    Node* node = (Node*) calloc (1, sizeof (Node));
    assert (node);

    node->left     = left;
    node->right    = right;

    node->type     = VAR;
    strcpy (node->value.var, var);

    return node;
}

Node* CreateNumNode (Node* left, Node* right, elem_t num)
{
    Node* node = (Node*) calloc (1, sizeof (Node));
    assert (node);

    node->left      = left;
    node->right     = right;

    node->type      = NUM;
    node->value.num = num;

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

Node* GetTaylorFormula (Node* main_node, int degree, elem_t x)
{
    assert (main_node);

    Node* cur_node = main_node;
    Node* diff = main_node;
    elem_t val = 0;

    Node* taylor = _ADD (nullptr, nullptr);
    taylor->left = _NUM (Eval (main_node, x));

    for (int i = 1; i <= degree; i++)
    {
        diff = Diff (cur_node, nullptr);

        if (cur_node != main_node) TreeDtor (cur_node);
        cur_node = diff;

        Optimize (&diff);
        val = Eval (diff, x);

        taylor->right = _DIV (_MULT (_NUM (val),
        _POW (_SUB (_VAR ("x"), _NUM (x)), _NUM ((elem_t) i))), _NUM ((elem_t) Factorial (i)));

        taylor = _ADD (taylor, nullptr);
    }

    TreeDtor (diff);

    char o[MAX_VAR_LENGTH] = "";
    if (x > 0) sprintf (o, "o((x-%.3lf)^%d)", x, degree);
    else if (x < 0) sprintf (o, "o((x+%.3lf)^%d)", -x, degree);
    else sprintf (o, "o(x^%d)", degree);

    taylor->right = _VAR (o);

    return taylor;
}

Node* GetTangent (Node* node, Node* diff, double x)
{
    assert (node);

    Node* diff_copy = diff;

    if (!diff) diff = Diff (node, TexDump);
    Optimize (&diff);

    elem_t deriv_val = Eval (diff, x);
    elem_t func_val  = Eval (node, x);

    Node* tangent = CreateOpNode (_NUM (func_val), _MULT (_NUM (deriv_val), _SUB (_VAR ("x"), _NUM (x))), ADD);
    Optimize (&tangent);

    if (!diff_copy) TreeDtor (diff);

    return tangent;
}
