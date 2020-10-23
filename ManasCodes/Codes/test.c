#include "grammar.c"
#include "tokenizer.c"
#define debug(message) printf("[*] %s", message)

typedef struct __stack
{
    int top;
    Symbol stack[256];
} Stack;

void copy_stack(Stack *dest, Stack *src)
{
    for (int i = 0; i < src->top + 1; i++)
    {
        // debug("YAHA PAHUCH GYA !!!\n");
        dest->stack[i].is_terminal = src->stack[i].is_terminal;
        dest->stack[i].next = src->stack[i].next;
        strcpy(dest->stack[i].str, src->stack[i].str);
        // pSymbol(&dest->stack[i]);
        // printf
    }
    dest->top = src->top;
}
// Stack stack;
int print_stack(Stack s)
{
    for (int i = 0; i < s.top + 1; i++)
    {
        pSymbol(&s.stack[i]);
    }
}
Symbol pop(Stack *s)
{
    if (s->top >= 0)
    {
        return s->stack[s->top--];
    }
}

void push(Stack *s, Symbol sym)
{
    s->top++;
    s->stack[s->top].is_terminal = sym.is_terminal;
    s->stack[s->top].next = sym.next;
    strcpy(s->stack[s->top].str, sym.str);
}

int parse(Grammar grammars[], Token *head, Stack stack)
{
    // debug();
    if (!head)
        return 1;
    debug("PARSING HEAD ") && pToken(head);
    debug("PARSING STACK\n") && print_stack(stack);
    // debug("TERMINAL: ") && printf("head: %s, top: %s\n", head->str, stack.stack[stack.top].str);
    if (stack.stack[stack.top].is_terminal)
    {
        // printf("YAHA TAK AA GYA !!!\n");
        if (!strcmp(head->str, stack.stack[stack.top].str))
        {
            // debug("JAI HIND !!!\n");
            // pToken(head);
            stack.top--;
            head = head->next;
            return parse(grammars, head, stack);
        }
        else
        {
            return 0;
        }
    }
    else
    {
        for (int i = 0; i < NO_OF_GRAMMAR_RULES; i++)
        {
            if (!strcmp(grammars[i].lhs.str, stack.stack[stack.top].str))
            {
                Stack temp;
                temp.top = -1;
                copy_stack(&temp, &stack);
                // printf("BEFORE POPING TEMP ") && print_stack(temp);
                // printf("BEFORE POPING STACK ") && print_stack(stack);
                pop(&temp);
                // printf("AFTER POPING ");
                // print_stack(temp);
                // TO ADD EXPANDED FORM IN TEMP STACK.
                for (Symbol *s = grammars[i].rhs_head; s != NULL; s = s->next)
                {
                    push(&temp, *s);
                    // pSymbol(&temp.stack[temp.top]);
                }
                debug("AFTER PUSHING ") && print_stack(temp);
                // print_stack(stack);
                if (parse(grammars, head, temp))
                {

                    return 1;
                }
            }
        }
        return 0;
    }
}

int main(int argc, char const *argv[])
{
    initialize_grammar("grammar_test.txt");
    // pGrammars(grammars);
    initialize_token_stream("src_code_test.txt");
    // pTokens(head);
    Stack stack;
    strcpy(stack.stack[0].str, "<S>");
    stack.stack[0].is_terminal = 0;
    stack.top = 0;
    int val = parse(grammars, head, stack);
    val ? debug("SUCCUSEES\n") : debug("UNSUCCESSFUL\n");

    return 0;
}
