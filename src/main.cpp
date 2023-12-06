#include "differenciator.h"
#include "utils.h"
#include "dump.h"

int main (int argc, const char** argv)
{
    if (argc != 2)
    {
        fprintf (stderr, RED_COLOR "Неправильный запуск программы (ввод аргументов командной строки)\n" RESET_COLOR);
        return -1;
    }

    char* buffer = get_file_content (argv[1]);
    Node* main_node = GetGrammar (buffer);
    free (buffer);

    if (main_node == nullptr)
    {
        fprintf (stderr, RED_COLOR "Выход из программы (синтаксическая ошибка)\n" RESET_COLOR);
        return -1;
    }

    TexDumpBegin ();

    Node* aboba = Diff (main_node, &TexDump);
    Optimize (&aboba);
    TexDump (main_node, aboba, "Итого имеем: \\\\", true);
    GraphDump (aboba);

    Node* taylor = Taylor (main_node, 5, 0.0);
    Optimize (&taylor);
    TexDump (main_node, taylor,
            "Я в своем сознании настолько преисполнился, что вы можете называть меня Тейлором: \\\\", false);

    TaylorGraphic (taylor, main_node, 0.0);
    TangentGraphic (main_node, aboba, 0.0);

    TreeDtor (taylor);
    TreeDtor (aboba);
    TreeDtor (main_node);

    TexDumpEnd ();
}
