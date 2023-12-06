#include "differenciator.h"
#include "dump.h"
#include "dsl.h"

#include <random>
#include <time.h>

static void GraphNodeDump   (FILE* file, Node* node);
static void DrawConnections (FILE* file, Node* node);

void TexDump (Node* node_1, Node* node_2, const char* phraze, bool dump_derivative)
{
    assert (node_1);
    assert (node_2);
    assert (phraze);

    FILE* tex = fopen (OUTPUT_TEX, "a");
    fprintf (tex, "%s", phraze);
    fprintf (tex, "$");
    if (dump_derivative) fprintf (tex, "(");

    DumpNode (node_1, node_1, tex, true);
    if (dump_derivative) fprintf (tex, ")' = ");
    else fprintf (tex, " = ");

    DumpNode (node_2, node_2, tex, true);
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
    sprintf (dot_command, "dot -Tpng %s -o %s", OUTPUT_DOT, OUTPUT_TREE);
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

    _print ("\\begin{center} \\includegraphics[scale=0.6]{%s} \\end{center}\n", OUTPUT_TAYLOR);
    _print ("\\begin{center} \\includegraphics[scale=0.6]{%s} \\end{center}\n", OUTPUT_TANGENT);

    _print (R"(\end{flushleft}
               \end{document})");

    fclose (file);

    char command[MAX_COMMAND_LENGTH] = "";
    sprintf (command, "pdflatex >/dev/null %s", OUTPUT_TEX);
    system (command);
}

void DumpNode (Node* node, Node* main_node, FILE* file, bool tex_dump)
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
            if (tex_dump) _print ("\\frac{");

            DumpNode (node->left, main_node, file, tex_dump);

            if (tex_dump) _print ("}{");
            else _print ("/");

            DumpNode (node->right, main_node, file, tex_dump);
            if (tex_dump) _print ("}");

            break;

        case SIN:
            if (tex_dump) _print ("\\sin{(");
            else _print ("sin((");

            DumpNode (node->left, main_node, file, tex_dump);
            if (tex_dump) _print (")}");
            else _print ("))");

            break;

        case COS:
            if (tex_dump) _print ("\\cos{(");
            else _print ("cos((");

            DumpNode (node->left, main_node, file, tex_dump);
            if (tex_dump) _print (")}");

            else _print ("))");
            break;

        case LN:
            if (tex_dump) _print ("\\ln{(");
            else _print ("log((");

            DumpNode (node->left, main_node, file, tex_dump);

            if (tex_dump) _print (")}");
            else _print ("))");

            break;

        case POW:
            if (node != main_node) _print ("(");

            DumpNode (node->left, main_node, file, tex_dump);

            if (tex_dump) _print ("^");
            else _print ("**");

            DumpNode (node->right, main_node, file, tex_dump);

            if (node != main_node) _print (")");

            break;

        default:
            if (node != main_node) _print ("(");
            DumpNode (node->left, main_node, file, tex_dump);

            _print ("%s", OperationsArray[node->value.op]);

            DumpNode (node->right, main_node, file, tex_dump);
            if (node != main_node) _print (")");
        }
    }
}

void TaylorGraphic (Node* taylor, Node* main_node, double x)
{
    assert (taylor);
    assert (main_node);

    FILE* file = fopen (OUTPUT_GPI, "w");

    fprintf (file, R"(#! /usr/bin/gnuplot -persist
                    set terminal png size 800, 600)"
                    "\nset output \"%s\" \n"
                    R"(set xlabel "x"
                    set ylabel "y"
                    set grid
                    set title "Compare function with it's Taylor formula"
                    plot )", OUTPUT_TAYLOR);

    fprintf (file, "[%lf:%lf]", x - delta, x + delta);

    DumpNode (taylor->left, taylor->left, file, false);

    fprintf (file, R"(title "Taylor", )");

    DumpNode (main_node, main_node, file, false);

    fclose (file);

    char command[MAX_COMMAND_LENGTH] = "";
    sprintf (command, "chmod +x %s && ./%s", OUTPUT_GPI, OUTPUT_GPI);
    system (command);
}

void TangentGraphic (Node* node, Node* diff, double x)
{
    assert (node);

    Node* tangent = Tangent (node, diff, x);

    FILE* file = fopen (OUTPUT_GPI, "w");

    fprintf (file, R"(#! /usr/bin/gnuplot -persist
                    set terminal png size 800, 600)"
                    "\nset output \"%s\"\n"
                    R"(set xlabel "x"
                    set ylabel "y"
                    set grid)"
                    "\nset title \"Function and tangent at x = %.3lf\" \n"
                    "plot ", OUTPUT_TANGENT, x);

    DumpNode (node, node, file, false);
    fprintf (file, ", ");
    DumpNode (tangent, tangent, file, false);

    fclose (file);

    char command[MAX_COMMAND_LENGTH] = "";
    sprintf (command, "chmod +x %s && ./%s", OUTPUT_GPI, OUTPUT_GPI);
    system (command);

    TreeDtor (tangent);
}
