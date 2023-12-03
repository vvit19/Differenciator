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
    fprintf (tex, "%s\n", phraze);
    fprintf (tex, "\\begin{equation}\n");

    fprintf (tex, "(");
    TexDumpNode (node, node, tex);
    fprintf (tex, ")' = ");

    TexDumpNode (diff_node, diff_node, tex);
    fprintf (tex, "\n\\end{equation}\n");

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
                 \usepackage[T2A]{fontenc}
                 \usepackage[utf8]{inputenc}
                 \usepackage[english,russian]{babel}
                 \author{Савченко Виталий, Б01-306}
                 \title{Учебник по матану}
                 \begin{document}
                 \maketitle )");
    _print ("\n");

    fclose (file);
}

void TexDumpEnd ()
{
    FILE* file = fopen (OUTPUT_TEX, "a");

    _print (R"(\end{document})");

    fclose (file);
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
            TexDumpNode (node->right, main_node, file);
            _print (")}");
            break;

        case COS:
            _print ("\\cos{(");
            TexDumpNode (node->right, main_node, file);
            _print (")}");
            break;

        case LN:
            _print ("\\ln{(");
            TexDumpNode (node->right, main_node, file);
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
