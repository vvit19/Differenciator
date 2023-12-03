#include "differenciator.h"
#include "utils.h"
#include "dump.h"

int main ()
{
    TexDumpBegin ();

    char* buffer = get_file_content ("input.txt");

    Node* main_node = CreateNode (nullptr, nullptr, NO_TYPE);
    main_node->left = main_node;
    main_node->right = GetGrammar (buffer);
    free (buffer);

    Node* aboba  = Diff (main_node->right);
    Optimize (aboba);
    GraphDump (aboba);
    TexDumpDerivative (main_node->right, aboba, "Итого имеем:\n");

    // elem_t a = Eval (aboba, 1.0);
    // printf ("%lf\n", a);

    TexDumpEnd ();

    TreeDtor (aboba);
    TreeDtor (main_node->right);
    free (main_node);
}
