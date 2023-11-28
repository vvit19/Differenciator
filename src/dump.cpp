#include "tree.h"

static void NodeDump        (FILE* dot, Node* node);
static void DrawConnections (FILE* dot, Node* node);
static void PrintTabs       (FILE* file, int level);

const char* const dot_file = "dump.dot";

#define _print(...) fprintf (dot, __VA_ARGS__)

void TreeDump (Node* node)
{
    assert (node);

    FILE* dot = fopen (dot_file, "w");

    _print (R"(
            digraph g {
            rankdir   =  TB;
            graph[ranksep = 1.3, nodesep = 0.5, style = "rounded, filled"]
            )");

    NodeDump (dot, node);

    DrawConnections (dot, node);

    _print ("}\n");

    fclose (dot);

    system ("dot -Tpng dump.dot -o tree.png");
}

void NodeDump (FILE* dot, Node* node)
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

    NodeDump (dot, node->left);
    NodeDump (dot, node->right);
}

static void DrawConnections (FILE* dot, Node* node)
{
    if (node->left)
    {
        _print ("Node%p->Node%p\n", node, node->left);
        DrawConnections (dot, node->left);
    }

    if (node->right)
    {
        _print ("Node%p->Node%p\n", node, node->right);
        DrawConnections (dot, node->right);
    }
}
