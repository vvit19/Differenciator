#include "tree.h"

Node* CreateNode (Node* left, Node* right, Types type, ...)
{
    Node* node = (Node*) calloc (1, sizeof (Node));
    assert (node);

    node->left   = left;
    node->right  = right;
    node->type = type;

    if (type == NO_TYPE) return node;

    va_list value = {};
    va_start (value, type);

    switch (type)
    {
    case OP:
        node->value.op = (Operations) va_arg (value, int);
        break;

    case NUM:
        node->value.num = va_arg (value, elem_t);
        break;

    case VAR:
        strcpy (node->value.var, va_arg (value, const char*));
        break;

    default:
        fprintf (stderr, "Default case reached in file: %s, function: %s, line: %d\n",
                          __FILE__, __PRETTY_FUNCTION__, __LINE__);
    }

    va_end (value);

    return node;
}

char* GetTree (Node* main_node, char* buffer)
{
    assert (main_node);
    assert (buffer);

    while (*buffer != '(')
    {
        if (*buffer == '\0') { return nullptr; }

        buffer++;
    }
    buffer++;

    char* return_value = buffer;

    Node* node = nullptr;

    int brackets_counter = 0;
    for ( ; *buffer != '\0'; buffer++)
    {
        if (isspace (*buffer)) continue;

        if (*buffer == '(') { brackets_counter++; continue; }

        if (*buffer == ')') { brackets_counter--; continue; }

        if (brackets_counter != 0) continue;

        if (*buffer >= '0' && *buffer <= '9')
        {
            elem_t num = 0;
            sscanf (buffer, "%lf", &num);

            while (*buffer >= '0' && *buffer <= '9') buffer++;

            node = CreateNode (nullptr, nullptr, NUM, num);

            if (main_node->left) { main_node->right = node; return return_value; }
            main_node->left = node;
            return GetTree (main_node, return_value);
        }

        for (int j = 0; j < sizeof (OperationsArray) / sizeof (CharOperation); j++)
        {
            if (*buffer == OperationsArray[j].op_char)
            {
                node = CreateNode (nullptr, nullptr, OP, OperationsArray[j].op_name);
                if (main_node->left) { main_node->right = node; return GetTree (node, return_value); } //для самой начальной ноды заполняю левого сына ей же
                main_node->left = node;
                buffer = GetTree (node, return_value);
                return GetTree (main_node, buffer);
            }
        }

        char variable[MAX_VAR_LENGTH] = "";
        for (int k = 0; k < MAX_VAR_LENGTH && !isspace (*buffer) && *buffer != '\0' && *buffer != ')'; k++)
        {
            variable[k] = *buffer;
            buffer++;
        }

        node = CreateNode (nullptr, nullptr, VAR, variable);
        if (main_node->left) { main_node->right = node; return return_value; }
        main_node->left = node;
        return GetTree (main_node, return_value);
    }

    return return_value;
}

void TreeDtor (Node* node)
{
    if (node == nullptr) return;

    TreeDtor (node->left);
    free (node->left);
    node->left = nullptr;

    TreeDtor (node->right);
    free (node->right);
    node->right = nullptr;
}

Node* CopyNode (Node* original_node)
{
    assert (original_node);

    Node* node = (Node*) calloc (1, sizeof (Node));
    assert (node);

    node->left   = original_node->left;
    node->right  = original_node->right;
    node->type   = original_node->type;
    node->value  = original_node->value;

    return node;
}
