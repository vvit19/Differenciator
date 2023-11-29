#include "differenciator.h"
#include "utils.h"

int main ()
{
    char* buffer = get_file_content ("input.txt");
    Node* main_node = CreateNode (nullptr, nullptr, NO_TYPE);
    main_node->left = main_node;
    GetTree (main_node, buffer);
    TreeDump (main_node->right);
    Node* aboba = Diff (main_node->right);
    Optimize (aboba);
    TreeDump (aboba);
    elem_t a = Eval (aboba, 1.0);
    printf ("%lf\n", a);
    // PrintTree (main_node->right);
    // PrintTree (aboba);
    TreeDtor (aboba);
    TreeDtor (main_node->right);
    free (main_node);
}
