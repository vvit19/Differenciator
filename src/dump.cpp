#include "differenciator.h"

static void NodeDump        (FILE* file, Node* node);
static void DrawConnections (FILE* file, Node* node);

const char* const dot_file = "dump.dot";

#define _print(...) fprintf (file, __VA_ARGS__)

void TreeDump (Node* node)
{
    assert (node);

    FILE* file = fopen (dot_file, "w");

    _print (R"(
            digraph g {
            rankdir   =  TB;
            graph[ranksep = 1.3, nodesep = 0.5, style = "rounded, filled"]
            )");

    NodeDump (file, node);

    DrawConnections (file, node);

    _print ("}\n");

    fclose (file);

    system ("dot -Tpng dump.dot -o tree.png");
}

void NodeDump (FILE* file, Node* node)
{
    if (node == nullptr) return;

    if (node->type == VAR)
    {
        _print ("Node%p[shape=rectangle, color=\"red\", width=0.2, style=\"filled\","
                "fillcolor=\"lightblue\", label=\"%s\"] \n \n",
                node, node->value.var);
    }
    else if (node->type == OP)
    {
        _print ("Node%p[shape=rectangle, color=\"red\", width=0.2, style=\"filled\","
                "fillcolor=\"lightblue\", label=\"%c\"] \n \n",
                node, OperationsArray[node->value.op].op_char);
    }
    else if (node->type == NUM)
    {
        _print ("Node%p[shape=rectangle, color=\"red\", width=0.2, style=\"filled\","
                "fillcolor=\"lightblue\", label=\"%lf\"] \n \n",
                node, node->value.num);
    }

    NodeDump (file, node->left);
    NodeDump (file, node->right);
}

static void DrawConnections (FILE* file, Node* node)
{
    if (node->left)
    {
        _print ("Node%p->Node%p\n", node, node->left);
        DrawConnections (file, node->left);
    }

    if (node->right)
    {
        _print ("Node%p->Node%p\n", node, node->right);
        DrawConnections (file, node->right);
    }
}

void PrintTree (Node* node)
{
    if (!node) return;
}
