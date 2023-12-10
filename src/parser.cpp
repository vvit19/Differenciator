#include "differenciator.h"
#include "utils.h"
#include "dsl.h"

const int MAX_TOKENS = 1024;

static void  Lexer          (Token* tokens, char* buffer);
static void  AddVarToken    (Token* tokens, int cur_token, char* buffer, int* i);
static void  AddNumToken    (Token* tokens, int cur_token, char* buffer, int* i);
static void  AddOpToken     (Token* tokens, int cur_token, char* buffer, int* i);

static Node* GetExpression  (Token* tokens, int* cur_token, ExitCodes* exit_code); // +-
static Node* GetMult        (Token* tokens, int* cur_token, ExitCodes* exit_code); // */
static Node* GetPower       (Token* tokens, int* cur_token, ExitCodes* exit_code); // ^
static Node* GetUnary       (Token* tokens, int* cur_token, ExitCodes* exit_code); // unary: sin, cos, ln
static Node* GetBrackets    (Token* tokens, int* cur_token, ExitCodes* exit_code); // ()
static Node* GetVariable    (Token* tokens, int* cur_token, ExitCodes* exit_code); // variables
static Node* GetNumber      (Token* tokens, int* cur_token, ExitCodes* exit_code); // numbers

Node* GetGrammar (char* buffer)
{
    Token tokens[MAX_TOKENS] = {};

    Lexer (tokens, buffer);

    ExitCodes exit_code = NO_ERR;
    int cur_token = 0;
    Node* main_node = GetExpression (tokens, &cur_token, &exit_code);

    SYNT_ASSERT (&exit_code, tokens[cur_token].type == OP && tokens[cur_token].value.op == END);

    if (exit_code == ERR)
    {
        TreeDtor (main_node);
        return nullptr;
    }

    return main_node;
}

static Node* GetExpression (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    Node* main_node = GetMult (tokens, cur_token, exit_code);

    while (CUR_TOKEN.type == OP &&
          (CUR_TOKEN.value.op == SUB || CUR_TOKEN.value.op == ADD))
    {
        Operations op = CUR_TOKEN.value.op;
        *cur_token += 1;

        Node* sub_node = GetMult (tokens, cur_token, exit_code);

        main_node = CreateOpNode (main_node, sub_node, op);
    }

    return main_node;
}

static Node* GetMult (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    Node* main_node = GetPower (tokens, cur_token, exit_code);

    while (CUR_TOKEN.type == OP &&
          (CUR_TOKEN.value.op == MULT || CUR_TOKEN.value.op == DIV))
    {
        Operations op = CUR_TOKEN.value.op;
        *cur_token += 1;

        Node* sub_node = GetPower (tokens, cur_token, exit_code);

        main_node = CreateOpNode (main_node, sub_node, op);
    }

    return main_node;
}

static Node* GetPower (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    Node* main_node = GetUnary (tokens, cur_token, exit_code);

    while (CUR_TOKEN.type == OP && CUR_TOKEN.value.op == POW)
    {
        Operations op = CUR_TOKEN.value.op;
        *cur_token += 1;

        Node* sub_node = GetUnary (tokens, cur_token, exit_code);

        main_node = CreateOpNode (main_node, sub_node, op);
    }

    return main_node;
}


static Node* GetUnary (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    Node* main_node = GetBrackets (tokens, cur_token, exit_code);

    while (CUR_TOKEN.type == OP &&
          (CUR_TOKEN.value.op == SIN || CUR_TOKEN.value.op == COS || CUR_TOKEN.value.op == LN))
    {
        Operations op = CUR_TOKEN.value.op;

        *cur_token += 1;
        SYNT_ASSERT (exit_code, CUR_TOKEN.type == OP && CUR_TOKEN.value.op == OPEN_BRACKET);
        *cur_token += 1;

        main_node = GetExpression (tokens, cur_token, exit_code);

        SYNT_ASSERT (exit_code, CUR_TOKEN.type == OP && CUR_TOKEN.value.op == CLOSE_BRACKET);
        *cur_token += 1;

        main_node = CreateOpNode (main_node, nullptr, op);
    }

    return main_node;
}

static Node* GetBrackets (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);

    if (CUR_TOKEN.type == OP && CUR_TOKEN.value.op == OPEN_BRACKET)
    {
        *cur_token += 1;
        Node* main_node = GetExpression (tokens, cur_token, exit_code);

        SYNT_ASSERT (exit_code, CUR_TOKEN.type == OP && CUR_TOKEN.value.op == CLOSE_BRACKET);
        *cur_token += 1;

        return main_node;
    }
    else return GetVariable (tokens, cur_token, exit_code);
}

static Node* GetVariable (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);

    Node* main_node = GetNumber (tokens, cur_token, exit_code);

    if (CUR_TOKEN.type == VAR)
    {
        main_node = _VAR (CUR_TOKEN.value.var);
        *cur_token += 1;
    }

    return main_node;
}

static Node* GetNumber (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);

    Node* main_node = nullptr;

    if (CUR_TOKEN.type == OP && CUR_TOKEN.value.op == SUB && NEXT_TOKEN.type == NUM)
    {
        main_node = _NUM ((-1.0) * NEXT_TOKEN.value.num);
        *cur_token += 2;
    }
    if (CUR_TOKEN.type == NUM)
    {
        main_node = _NUM (CUR_TOKEN.value.num);
        *cur_token += 1;
    }

    return main_node;
}

static void Lexer (Token* tokens, char* buffer)
{
    int tokens_cnt = 0;

    for (int i = 0; buffer[i] != '\0'; tokens_cnt++)
    {
        i = SkipSpaces (buffer, i);
        if (buffer[i] == '\0') break;

        tokens[tokens_cnt].type = NO_TYPE;

        if (isdigit (buffer[i]))
        {
            AddNumToken (tokens, tokens_cnt, buffer, &i);
        }

        else
        {
            AddOpToken (tokens, tokens_cnt, buffer, &i);

            if (tokens[tokens_cnt].type != NO_TYPE) continue;

            AddVarToken (tokens, tokens_cnt, buffer, &i);
        }
    }

    tokens[tokens_cnt].type     = OP;
    tokens[tokens_cnt].value.op = END;
}

static void AddNumToken (Token* tokens, int cur_token, char* buffer, int* i)
{
    assert (tokens);
    assert (buffer);
    assert (i);

    tokens[cur_token].type = NUM;

    char* end_ptr = &buffer[*i];
    tokens[cur_token].value.num = strtod (&buffer[*i], &end_ptr);

    while (&buffer[*i] != end_ptr) *i += 1;
}

static void AddOpToken (Token* tokens, int cur_token, char* buffer, int* i)
{
    assert (tokens);
    assert (buffer);
    assert (i);

    for (size_t cnt = 0; cnt < sizeof (OperationsArray) / sizeof (const char*); cnt++)
    {
        int op_length = strlen (OperationsArray[cnt]);
        if (strncmp (OperationsArray[cnt], &buffer[*i], op_length) == 0)
        {
            tokens[cur_token].type = OP;

            *i += op_length;

            tokens[cur_token].value.op = (Operations) cnt;

            break;
        }
    }
}

static void AddVarToken (Token* tokens, int cur_token, char* buffer, int* i)
{
    assert (tokens);
    assert (buffer);
    assert (i);

    tokens[cur_token].type = VAR;
    char variable[MAX_VAR_LENGTH] = "";

    for (int k = 0; isalpha (buffer[*i]) || buffer[*i] == '_' || isdigit (buffer[*i]); *i += 1, k++)
    {
        variable[k] = buffer[*i];
    }

    strcpy (tokens[cur_token].value.var, variable);
}
