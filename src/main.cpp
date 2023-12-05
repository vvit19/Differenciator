#include "differenciator.h"
#include "utils.h"
#include "dump.h"

int main ()
{
    char* buffer = get_file_content ("input.txt");
    Node* main_node = GetGrammar (buffer);
    free (buffer);

    TexDumpBegin ();

    Node* aboba = Diff (main_node, &TexDumpDerivative);
    Optimize (aboba);
    GraphDump (aboba);
    TexDumpDerivative (main_node, aboba, "Итого имеем:\\\\ \n");

    DumpTaylor (main_node, 5, 0.0);

    TexDumpEnd ();
    TreeDtor (aboba);
    TreeDtor (main_node);
}
