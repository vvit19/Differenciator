#include "tree.h"
#include "dsl.h"
#include "utils.h"

int main ()
{
    char* buffer = get_file_content ("input.txt");
    Node* main_node = CreateNode (nullptr, nullptr, NO_TYPE);
    main_node->left = main_node;
    GetTree (main_node, buffer);
    TreeDump (main_node->right);
    Node* aboba = Diff (main_node->right);
    // TreeDump (aboba);
}

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
        return _DIV (_SUB (_MULT (dL, cR), _MULT (cL, dR)), _MULT (cR, cR));

    case SIN:
        return _MULT (_SIN (cL, nullptr), dL);

    case COS:
        return _MULT (_MULT (_COS (cL, nullptr), dL), CreateNode (nullptr, nullptr, NUM, -1));

    default:
        fprintf (stderr, "Default case reached in file: %s, function: %s, line: %d\n",
                          __FILE__, __PRETTY_FUNCTION__, __LINE__);
    }

    return nullptr;
}
