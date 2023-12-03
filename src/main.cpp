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

    Node* aboba  = main_node->right;
    GraphDump (aboba);
    Node* abobus = NULL;
    for (int i = 0; i < 1; i++)
    {
        abobus = aboba;
        aboba = Diff (aboba);
        Optimize (aboba);
        TexDumpDerivative (abobus, aboba, "Итого получается: \n");
        TreeDtor (abobus);
        GraphDump (aboba);
    }

    TreeDtor (aboba);


    // elem_t a = Eval (aboba, 1.0);
    // printf ("%lf\n", a);

    TexDumpEnd ();
    free (main_node);
}
