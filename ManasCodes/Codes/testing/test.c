#include "grammar.c"
#include "tokenizer.c"
// #include "parse_tree.c"
#define debug(message) printf("[*] %s", message)

typedef struct __parse_table
{

} ParseTable;

#pragma once
#include <stdio.h>
// #include "grammar.c"
// #include "tokenizer.c"

// typedef struct Node Node;
// struct Node
// {
//     void *data;
//     Node *next;
//     Node *prev;
//     Node *parent;
//     Node *children;
// };

// Node *nodeNew(void *data)
// {
//     Node *n;

//     n = (Node *)malloc(sizeof(Node));
//     if (!n)
//         return n;
//     n->data = data;
//     n->next = NULL;
//     n->prev = NULL;
//     n->parent = NULL;
//     n->children = NULL;
//     return n;
// }

// int delete_node(Node *node)
// {
//     for (Node *temp = node->children; temp; temp = temp->next)
//     {
//         free(temp);
//     }
// }

// Node *add_child(Node *parent, Node *node)
// {
//     if (!(parent && node))
//         return NULL;
//     node->parent = parent;
//     Node *sibling;
//     if (parent->children)
//     {
//         sibling = parent->children;
//         while (sibling->next)
//             sibling = sibling->next;
//         node->prev = sibling;
//         sibling->next = node;
//     }
//     else
//         node->parent->children = node;
//     return node;
// }

// int print_tree(Node *root)
// {
//     if (!root)
//         return 0;
//     printf("%s\n", root->data);
//     if (root->next && printf("%s > ", root->data))
//         print_tree(root->next);
//     if (root->children && printf("%s\nV\n", root->data))
//         print_tree(root->children);
// }

Node *root;

typedef struct __stack
{
    int top;
    Symbol stack[256];
} Stack;

Stack global[16];
int global_count = 0;
void copy_stack(Stack *dest, Stack *src)
{
    for (int i = 0; i < src->top + 1; i++)
    {
        // debug("YAHA PAHUCH GYA !!!\n");
        dest->stack[i].is_terminal = src->stack[i].is_terminal;
        dest->stack[i].next = src->stack[i].next;
        strcpy(dest->stack[i].str, src->stack[i].str);
        dest->stack[i].node = src->stack[i].node;
        // pSymbol(&dest->stack[i]);
        // printf
    }
    dest->top = src->top;
}
// Stack stack;
int print_stack(Stack s)
{
    // printf("%s\n", s.called_from.str);
    for (int i = s.top; i > -1; i--)
    {
        pSymbol(&s.stack[i]);
    }
    printf("\n");
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
    s->stack[s->top].node = nodeNew(&s->stack[s->top]);
}

int parse(Grammar grammars[], Token *head, Stack stack)
{
    // debug();
    if (!head)
        return 1;
    // debug("PARSING HEAD ") && pToken(head);
    // debug("PARSING STACK\n") && print_stack(stack);
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
        Node *terminal = nodeNew(&stack.stack[stack.top]);
        for (int i = 0; i < NO_OF_GRAMMAR_RULES; i++)
        {
            // delete_node(pparent);
            if (!strcmp(grammars[i].lhs.str, stack.stack[stack.top].str))
            {
                Stack temp;
                Stack temp2;
                temp2.top = -1;
                temp.top = -1;
                copy_stack(&temp, &stack);
                // printf("BEFORE POPING TEMP ") && print_stack(temp);
                // printf("BEFORE POPING STACK ") && print_stack(stack);
                // Symbol *parent = (Symbol *)malloc(sizeof(Symbol));
                // strcpy(parent->str, pop(&temp).str);
                // printf("AFTER POPING ");
                // print_stack(temp);
                // TO ADD EXPANDED FORM IN TEMP STACK.
                for (Symbol *s = grammars[i].rhs_head; s != NULL; s = s->next)
                {
                    push(&temp, *s);
                    push(&temp2, *s);
                    // pSymbol(&temp.stack[temp.top]);
                }
                for (int i = 0; i < temp2.top; i++)
                {
                    add_child(terminal, temp2.stack[i].node);
                }
                // debug("AFTER PUSHING ") && print_stack(temp);
                // print_stack(stack);
                if (parse(grammars, head, temp))
                {
                    // strcpy(temp2.called_from.str, parent.str);
                    // copy_stack(&global[global_count++], &temp2);
                    // pSymbol(&parent);
                    // printf("\n");
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
    strcpy(stack.stack[0].str, "<start>");
    stack.stack[0].is_terminal = 0;
    stack.top = 0;
    root = nodeNew(&stack.stack[0]);
    stack.stack[0].node = root;
    int val = parse(grammars, head, stack);
    val ? debug("SUCCUSEES\n") : debug("UNSUCCESSFUL\n");
    // for (int i = global_count; global_count > -1; global_count--)
    //     print_stack(global[i]);
    print_tree(root);
    return 0;
}
