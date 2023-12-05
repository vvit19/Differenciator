#include "differenciator.h"
#include "dump.h"

#include <random>
#include <time.h>

static void GraphNodeDump   (FILE* file, Node* node);
static void DrawConnections (FILE* file, Node* node);

void TexDumpDerivative (Node* node, Node* diff_node, const char* phraze)
{
    assert (node);
    assert (diff_node);
    assert (phraze);

    FILE* tex = fopen (OUTPUT_TEX, "a");
    fprintf (tex, "%s", phraze);
    fprintf (tex, "$(");

    TexDumpNode (node, node, tex);
    fprintf (tex, ")' = ");

    TexDumpNode (diff_node, diff_node, tex);
    fprintf (tex, "$ \\\\ \n");

    fclose (tex);
}

const char* GetRandomPhraze ()
{
    tm time = {};
    size_t phraze_num = (size_t) ((rand () - time.tm_sec) % (sizeof (PHRAZES) / sizeof (const char*)));
    return PHRAZES[phraze_num];
}

#define _print(...) fprintf (file, __VA_ARGS__)

void GraphDump (Node* node)
{
    assert (node);

    FILE* file = fopen (OUTPUT_DOT, "w");

    _print (R"(
            digraph g {
            rankdir   =  TB;
            graph[ranksep = 1.3, nodesep = 0.5, style = "rounded, filled"]
            )");

    GraphNodeDump (file, node);

    DrawConnections (file, node);

    _print ("}\n");

    fclose (file);

    char dot_command[150] = "";
    sprintf (dot_command, "dot -Tpng %s -o %s", OUTPUT_DOT, OUTPUT_PNG);
    system (dot_command);
}

static void GraphNodeDump (FILE* file, Node* node)
{
    if (node == nullptr) return;

    if (node->type == VAR)
    {
        _print ("Node%p[shape=rectangle, width=0.2, style=\"filled\","
                "fillcolor=\"lightblue\", label=\"%s\"] \n \n",
                node, node->value.var);
    }
    else if (node->type == OP)
    {
        switch (node->value.op)
        {
        case SIN:
            _print ("Node%p[shape=rectangle, width=0.2, style=\"filled\","
            "fillcolor=\"yellow\", label=\"sin\"] \n \n", node);
            break;

        case COS:
            _print ("Node%p[shape=rectangle, width=0.2, style=\"filled\","
            "fillcolor=\"yellow\", label=\"cos\"] \n \n", node);
            break;

        case LN:
            _print ("Node%p[shape=rectangle, width=0.2, style=\"filled\","
            "fillcolor=\"yellow\", label=\"ln\"] \n \n", node);
            break;

        default:
            _print ("Node%p[shape=rectangle, width=0.2, style=\"filled\","
                    "fillcolor=\"yellow\", label=\"%s\"] \n \n",
                    node, OperationsArray[node->value.op]);
        }
    }
    else if (node->type == NUM)
    {
        _print ("Node%p[shape=rectangle, width=0.2, style=\"filled\","
                "fillcolor=\"orange\", label=\"%lf\"] \n \n",
                node, node->value.num);
    }

    GraphNodeDump (file, node->left);
    GraphNodeDump (file, node->right);
}

static void DrawConnections (FILE* file, Node* node)
{

    if (node->left && node->left->type != NO_TYPE)
    {
        _print ("Node%p->Node%p\n", node, node->left);
        DrawConnections (file, node->left);
    }

    if (node->right && node->right->type != NO_TYPE)
    {
        _print ("Node%p->Node%p\n", node, node->right);
        DrawConnections (file, node->right);
    }
}

void TexDumpBegin ()
{
    FILE* file = fopen (OUTPUT_TEX, "w");

    _print ( R"( \documentclass[a4paper,12pt]{article}
                 \usepackage[T1]{fontenc}
                 \usepackage[utf8]{inputenc}
                 \usepackage[english,russian]{babel}
                 \usepackage{pdfpages}
                 \usepackage{ragged2e}
                 \author{Савченко Виталий, Б01-306}
                 \title{Учебник по матану}
                 \begin{document}
                 \maketitle
                 \begin{flushleft} )");
    _print ("\n");

    fclose (file);
}

void TexDumpEnd ()
{
    FILE* file = fopen (OUTPUT_TEX, "a");

    _print (R"(\end{flushleft}
               \end{document})");

    fclose (file);

    char command[MAX_COMMAND_LENGTH] = "";
    sprintf (command, "pdflatex >/dev/null %s", OUTPUT_TEX);
    system (command);
}

void TexDumpNode (Node* node, Node* main_node, FILE* file)
{
    if (!node) return;

    if (node->type == NUM)
    {
        if (node->value.num < 0) _print ("(");

        if (is_equal (floor (node->value.num), node->value.num)) _print ("%d", (int) node->value.num);
        else _print ("%.3lf", node->value.num);

        if (node->value.num < 0) _print (")");
    }

    if (node->type == VAR) _print ("%s", node->value.var);

    if (node->type == OP)
    {
        switch (node->value.op)
        {
        case DIV:
            _print ("\\frac{");
            TexDumpNode (node->left, main_node, file);
            _print ("}{");
            TexDumpNode (node->right, main_node, file);
            _print ("}");
            break;

        case SIN:
            _print ("\\sin{(");
            TexDumpNode (node->left, main_node, file);
            _print (")}");
            break;

        case COS:
            _print ("\\cos{(");
            TexDumpNode (node->left, main_node, file);
            _print (")}");
            break;

        case LN:
            _print ("\\ln{(");
            TexDumpNode (node->left, main_node, file);
            _print (")}");
            break;

        default:
            if (node != main_node) _print ("(");
            TexDumpNode (node->left, main_node, file);
            _print ("%s", OperationsArray[node->value.op]);
            TexDumpNode (node->right, main_node, file);
            if (node != main_node) _print (")");
        }
    }
}

void DumpTaylor (Node* main_node, int degree, double var)
{
    FILE* tex = fopen (OUTPUT_TEX, "a");

    Node* diff_node = nullptr;
    elem_t val = Eval (main_node, var);

    fprintf (tex, "Если что, я настолько в своем сознании преисполнился,"
                  "что вы можете называть меня Тейлором\\\\ \n$");
    TexDumpNode (main_node, main_node, tex);

    if (!is_equal (val, 0.0)) fprintf (tex, " = %lf + ", val);
    else fprintf (tex, " = ");

    for (int i = 1; i <= degree; i++)
    {
        diff_node = Diff (main_node, nullptr);
        Optimize (diff_node);

        val = Eval (diff_node, var);

        if (is_equal (var, 0.0)) fprintf (tex, "\\frac{%.3lf}{%d} x^%d + ", val, Factorial (i), i);
        else if (var < 0) fprintf (tex, "\\frac{%.3lf}{%d} (x + %.3lf)^%d + ", val, Factorial (i), -var, i);
        else fprintf (tex, "\\frac{%.3lf}{%d} (x - %.3lf)^%d + ", val, Factorial (i), var, i);

        if (i != 1) TreeDtor (main_node);
        main_node = diff_node;
    }

    TreeDtor (diff_node);

    if (is_equal (var, 0.0)) fprintf (tex, "o(x^%d), x \\to 0 $", degree);
    else if (var < 0) fprintf (tex, "o((x + %.3lf)^%d), x \\to %.3lf $", -var, degree, var);
    else fprintf (tex, "o((x - %.3lf)^%d), x \\to %.3lf $", var, degree, var);

    fclose (tex);
}
