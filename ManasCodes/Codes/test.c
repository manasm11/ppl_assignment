#include "grammar.c"
#include "tokenizer.c"
#include <ctype.h>
#define STACK_SIZE 1024
#define debug(message) printf("[*] %s", message)
#define NO_OF_KEYWORDS 13
int is_keyword(char *str)
{
    char *keywords[NO_OF_KEYWORDS] = {
#include "keywords.h"
    };
    for (int i = 0; i < NO_OF_KEYWORDS; i++)
    {
        if (!strcmp(str, keywords[i]))
        {
            return 1;
        }
    }
    return 0;
}

typedef struct __parse_table
{

} ParseTable;

#pragma once
#include <stdio.h>

Node *root;
Node *current;

typedef struct __st_nodes
{
    int top;
    Node *nodes[STACK_SIZE];

} Stack_Of_Nodes;

Stack_Of_Nodes global_nodes;

typedef struct __stack
{
    int top;
    Symbol stack[STACK_SIZE];
} Stack;

Stack global_stack[STACK_SIZE];
int global_count = 0;
void copy_stack(Stack *dest, Stack *src)
{
    for (int i = 0; i < src->top + 1; i++)
    {
        dest->stack[i].is_terminal = src->stack[i].is_terminal;
        dest->stack[i].next = src->stack[i].next;
        strcpy(dest->stack[i].str, src->stack[i].str);
        dest->stack[i].node = src->stack[i].node;
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
    printf("\n");
}
Symbol pop(Stack *s)
{
    if (s->top >= 0)
    {
        return s->stack[s->top--];
    }
}

int print_global_nodes(Stack_Of_Nodes s)
{
    printf("PRINTING GLOBAL STACK\n");
    for (int i = 0; i < s.top + 1; i++)
    {
        printf("%s ", s.nodes[i]->data.str);
    }
    printf("\n");
}

void push(Stack *s, Symbol sym)
{
    s->top++;
    s->stack[s->top].is_terminal = sym.is_terminal;
    s->stack[s->top].next = sym.next;
    strcpy(s->stack[s->top].str, sym.str);
    s->stack[s->top].node = nodeNew(s->stack[s->top]);
}
int parse(Grammar grammars[], Token *head, Stack stack)
{
    if (!head)
        return 1;
    debug("PARSING HEAD ") && pToken(head);
    debug("PARSING STACK\n") && print_stack(stack);
    // for (int i = 0; i < global_nodes.top + 1; i++)
    // {
    //     printf("%s ", global_nodes.nodes[i]->data.str);
    // }
    // printf("\n");
    if (stack.stack[stack.top].is_terminal)
    {
        global_nodes.nodes[global_nodes.top]->data.line_no = head->line;
        int is_id = !strcmp(stack.stack[stack.top].str, "id");
        int is_num = !strcmp(stack.stack[stack.top].str, "num");
        // if(is_id){}
        // printf("[*] stack[top].str = %s\n[*] is_id = %d\n", stack.stack[stack.top].str, is_id);
        // Checking if it is an identifier
        if (!is_keyword(head->str))
        {
            // FIXME Will except numbers with first character as digit.
            // Eg: 1ab is a num
            // if (!strcmp(stack.stack[stack.top].str, "e"))
            // {
            //     stack.top--;
            //     global_nodes.top--;
            //     return parse(grammars, head, stack);
            // }
            if (is_num && isdigit(head->str[0]))
            {
                strcpy(global_nodes.nodes[global_nodes.top]->data.str, head->str);
                global_nodes.nodes[global_nodes.top]->data.type = INT;
                stack.top--;
                global_nodes.top--;
                head = head->next;
                // printf("HEAD = %s\n", head->str);
                return parse(grammars, head, stack);
            }
            if (is_id && !isdigit(head->str[0]))
            {
                strcpy(global_nodes.nodes[global_nodes.top]->data.str, head->str);
                global_nodes.nodes[global_nodes.top]->data.type = ID;
                stack.top--;
                global_nodes.top--;
                head = head->next;
                // printf("HEAD = %s\n", head->str);
                return parse(grammars, head, stack);
            }
            // printf("[-] ERROR in line: %d\n", head->line);
        }
        // if (is_id && !is_keyword(head->str))
        // {
        //     strcpy(global_nodes.nodes[global_nodes.top]->data.str, head->str);
        //     stack.top--;
        //     global_nodes.top--;
        //     head = head->next;
        //     // printf("HEAD = %s\n", head->str);
        //     return parse(grammars, head, stack);
        // }
        if (!strcmp(head->str, stack.stack[stack.top].str))
        {
            stack.top--;
            global_nodes.top--;
            head = head->next;
            // printf("HEAD = %s\n", head->str);
            return parse(grammars, head, stack);
        }
        else
        {
            return 0;
        }
    }
    else
    {
        Node *terminal = nodeNew(stack.stack[stack.top]);
        for (int i = 0; i < NO_OF_GRAMMAR_RULES; i++)
        {
            Stack_Of_Nodes temp_global_nodes;
            if (!strcmp(grammars[i].lhs.str, stack.stack[stack.top].str))
            {
                global_nodes.nodes[global_nodes.top]->data.grammar_rule_no = i;
                Stack temp;
                Stack temp2;
                temp2.top = -1;
                temp.top = -1;
                copy_stack(&temp, &stack);
                pop(&temp);
                for (Symbol *s = grammars[i].rhs_head; s != NULL; s = s->next)
                {
                    push(&temp, *s);
                    push(&temp2, *s);
                }
                for (int i = 0; i < global_nodes.top + 1; i++)
                {
                    temp_global_nodes.nodes[i] = global_nodes.nodes[i];
                }
                temp_global_nodes.top = global_nodes.top;

                Node *parent = global_nodes.nodes[global_nodes.top--];
                for (int i = 0; i < temp2.top + 1; i++)
                {
                    copy_symbol(&temp2.stack[i].node->data, &temp2.stack[i]);
                    Node *child_node = nodeNew(temp2.stack[i]);
                    add_child(parent, child_node);
                    global_nodes.nodes[++global_nodes.top] = child_node;
                    // global_nodes.nodes[global_nodes.top-1]->data.depth = depth;
                }

                if (parse(grammars, head, temp))
                {
                    return 1;
                }
                parent->children = NULL;
                for (int i = temp_global_nodes.top; i > -1; i--)
                {
                    global_nodes.nodes[i] = temp_global_nodes.nodes[i];
                }
                global_nodes.top = temp_global_nodes.top;
            }
        }
        return 0;
    }
}
static void reverse(Node **head_ref)
{
    struct Node *prev = NULL;
    struct Node *current = *head_ref;
    struct Node *next = NULL;
    while (current != NULL)
    {
        // Store next
        next = current->next;

        // Reverse current node's pointer
        current->next = prev;

        // Move pointers one position ahead.
        prev = current;
        current = next;
    }
    *head_ref = prev;
}

void reverse_children(Node **head_ref)
{
    if (*head_ref)
    {
        reverse(&(*head_ref)->children);
        for (Node *n = (*head_ref)->children; n; n = n->next)
        {
            reverse_children(&n);
        }
    }
}

int main(int argc, char const *argv[])
{
    initialize_grammar("grammar_copy_ki_copy.txt");
    // pGrammars(grammars);
    initialize_token_stream("src_code_test.txt");
    // pTokens(head);
    Stack stack;
    strcpy(stack.stack[0].str, "<start>");
    stack.stack[0].is_terminal = 0;
    stack.top = 0;

    global_nodes.nodes[0] = nodeNew(stack.stack[0]);
    global_nodes.top = 0;

    stack.stack[0].node = root;
    strcpy(stack.stack[0].str, "<start>");
    stack.stack[0].is_terminal = 0;
    stack.top = 0;

    root = nodeNew(stack.stack[0]);
    stack.stack[0].node = root;
    global_nodes.nodes[0] = root;
    int val = parse(grammars, head, stack);
    val ? debug("SUCCUSS\n") : debug("UNSUCCESSFUL\n");
    // print_tree(root);
    reverse_children(&root);
    // printf("---------------------------------------------------\n");
    get_depth(root, -1);

    BOLD_BLUE;
    printf("\n*******************************");
    printf("\n***** PRINTING PARSE TREE *****");
    printf("\n*******************************\n\033[0m");
    CLEAR_COLORS;
    print_tree(root);
    return 0;
}
