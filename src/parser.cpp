#include "differenciator.h"
#include "utils.h"
#include "dsl.h"

const int MAX_TOKENS = 1024;

static void  Lexer          (Token* tokens, char* buffer);

static Node* GetExpression  (Token* tokens, int* cur_token, ExitCodes* exit_code); // +-
static Node* GetMult        (Token* tokens, int* cur_token, ExitCodes* exit_code); // */
static Node* GetPower       (Token* tokens, int* cur_token, ExitCodes* exit_code); // ^
static Node* GetUnary       (Token* tokens, int* cur_token, ExitCodes* exit_code); // unary: sin, cos, ln
static Node* GetBrackets    (Token* tokens, int* cur_token, ExitCodes* exit_code); // ()
static Node* GetVariable    (Token* tokens, int* cur_token, ExitCodes* exit_code); // variables
static Node* GetNumber      (Token* tokens, int* cur_token, ExitCodes* exit_code); // numbers

static Node* GetExpression (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);
    assert (exit_code);

    if (*exit_code == ERR) return nullptr;

    Node* main_node = GetMult (tokens, cur_token, exit_code);

    while (tokens[*cur_token].type == OP &&
          (tokens[*cur_token].value.op == SUB || tokens[*cur_token].value.op == ADD))
    {
        Operations op = tokens[*cur_token].value.op;
        *cur_token += 1;

        Node* sub_node = GetMult (tokens, cur_token, exit_code);

        main_node = CreateNode (main_node, sub_node, OP, op);
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

    while (tokens[*cur_token].type == OP &&
          (tokens[*cur_token].value.op == MULT || tokens[*cur_token].value.op == DIV))
    {
        Operations op = tokens[*cur_token].value.op;
        *cur_token += 1;

        Node* sub_node = GetPower (tokens, cur_token, exit_code);

        main_node = CreateNode (main_node, sub_node, OP, op);
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

    while (tokens[*cur_token].type == OP && tokens[*cur_token].value.op == POW)
    {
        Operations op = tokens[*cur_token].value.op;
        *cur_token += 1;

        Node* sub_node = GetUnary (tokens, cur_token, exit_code);

        main_node = CreateNode (main_node, sub_node, OP, op);
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

    while (tokens[*cur_token].type == OP &&
          (tokens[*cur_token].value.op == SIN || tokens[*cur_token].value.op == COS || tokens[*cur_token].value.op == LN))
    {
        Operations op = tokens[*cur_token].value.op;

        *cur_token += 1;
        SYNT_ASSERT (exit_code, tokens[*cur_token].type == OP && tokens[*cur_token].value.op == OPEN_BRACKET);
        *cur_token += 1;

        main_node = GetExpression (tokens, cur_token, exit_code);

        SYNT_ASSERT (exit_code, tokens[*cur_token].type == OP && tokens[*cur_token].value.op == CLOSE_BRACKET);
        *cur_token += 1;

        main_node = CreateNode (main_node, nullptr, OP, op);
    }

    return main_node;
}

static Node* GetBrackets (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);

    if (tokens[*cur_token].type == OP && tokens[*cur_token].value.op == OPEN_BRACKET)
    {
        *cur_token += 1;
        Node* main_node = GetExpression (tokens, cur_token, exit_code);

        SYNT_ASSERT (exit_code, tokens[*cur_token].type == OP && tokens[*cur_token].value.op == CLOSE_BRACKET);
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

    if (tokens[*cur_token].type == VAR)
    {
        main_node = CreateNode (nullptr, nullptr, VAR, tokens[*cur_token].value.var);
        *cur_token += 1;
    }

    return main_node;
}

static Node* GetNumber (Token* tokens, int* cur_token, ExitCodes* exit_code)
{
    assert (tokens);
    assert (cur_token);

    Node* main_node = nullptr;

    if (tokens[*cur_token].type == OP && tokens[*cur_token].value.op == SUB && tokens[*cur_token + 1].type == NUM)
    {
        main_node = _NUM((-1.0) * tokens[*cur_token + 1].value.num);
        *cur_token += 2;
    }
    if (tokens[*cur_token].type == NUM)
    {
        main_node = _NUM(tokens[*cur_token].value.num);
        *cur_token += 1;
    }

    return main_node;
}

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

static void Lexer (Token* tokens, char* buffer)
{
    int tokens_cnt = 0;

    for (int i = 0; buffer[i] != '\0'; i++, tokens_cnt++)
    {
        i = SkipSpaces (buffer, i);
        if (buffer[i] == '\0') break;

        tokens[tokens_cnt].type = NO_TYPE;

        if (isdigit (buffer[i]))
        {
            tokens[tokens_cnt].type = NUM;

            char* end_ptr = &buffer[i];
            tokens[tokens_cnt].value.num = strtod (&buffer[i], &end_ptr);

            while (&buffer[i] != end_ptr) i++;
            i--;
        }

        else
        {
            for (size_t cnt = 0; cnt < sizeof (OperationsArray) / sizeof (const char*); cnt++)
            {
                int op_length = strlen (OperationsArray[cnt]);
                if (strncmp (OperationsArray[cnt], &buffer[i], op_length) == 0)
                {
                    tokens[tokens_cnt].type = OP;

                    i += op_length - 1;

                    tokens[tokens_cnt].value.op = (Operations) cnt;

                    break;
                }
            }

            if (tokens[tokens_cnt].type != NO_TYPE) continue;

            tokens[tokens_cnt].type = VAR;
            char variable[MAX_VAR_LENGTH] = "";
            for (int k = 0; isalpha (buffer[i]) || buffer[i] == '_' || isdigit (buffer[i]); i++, k++) // do not forget to do i--
            {
                variable[k] = buffer[i];
            }
            i--;

            strcpy (tokens[tokens_cnt].value.var, variable);
        }
    }

    tokens[tokens_cnt].type     = OP;
    tokens[tokens_cnt].value.op = END;
}
