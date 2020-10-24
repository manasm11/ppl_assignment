#include <stdio.h>
#define NO_OF_GRAMMAR_RULES 10
// #define NO_OF_TERMINALS 3
// #define NO_OF_NON_TERMINALS 2
// #define NO_OF_GRAMMAR_RULES 47
// <start> pro
typedef struct Node Node;
typedef enum __type
{
    KEYWORD,
    ID,
    INT,
    FLOAT
} Type;

typedef struct __symbol
{
    char str[16];
    int is_terminal;
    struct __symbol *next;
    Type type;
    Node *node;
} Symbol;

int copy_symbol(Symbol *dest, Symbol *src)
{
    dest->is_terminal = src->is_terminal;
    dest->next = src->next;
    strcpy(dest->str, src->str);
}

struct Node
{
    Symbol data;
    Node *next;
    Node *prev;
    Node *parent;
    Node *children;
};

Node *nodeNew(Symbol data)
{
    Node *n;

    n = (Node *)malloc(sizeof(Node));
    if (!n)
        return n;
    copy_symbol(&(n->data), &data);
    n->next = NULL;
    n->prev = NULL;
    n->parent = NULL;
    n->children = NULL;
    return n;
}

int delete_node(Node *node)
{
    for (Node *temp = node->children; temp; temp = temp->next)
    {
        free(temp);
    }
}

Node *add_child(Node *parent, Node *node)
{
    if (!(parent && node))
        return NULL;
    node->parent = parent;
    Node *sibling;
    if (parent->children)
    {
        sibling = parent->children;
        while (sibling->next)
            sibling = sibling->next;
        node->prev = sibling;
        sibling->next = node;
    }
    else
        node->parent->children = node;
    return node;
}

int print_tree(Node *root)
{
    if (!root)
        return 0;
    printf("%s\n", root->data.str);
    for (Node *child = root->children; child; child = child->next)
    {
        printf("%s -> ", child->data.str);
    }
    printf("\n");
    // for (Node *node = root; node; node = node->next)
    // {
    //     printf("%s -> ", node->data.str);
    // }
    // printf("\n");
    for (Node *child = root->children; child; child = child->next)
    {
        print_tree(child);
    }
}
//     if (root->next && printf("%s > ", root->data.str))
//         print_tree(root->next);
//     if (root->children && printf("%s\nV\n", root->data.str))
//         print_tree(root->children);
// }

typedef struct __grammar
{
    Symbol lhs;
    Symbol *rhs_head;
} Grammar;

int pSymbol(Symbol *s) { printf(" %s ", s->str); }
// int pSymbol(Symbol *s) { printf("Str: %s, is_terminal: %s\n", s->str, s->is_terminal ? "true" : "false"); }
int pSymbols(Symbol *head) { head &&printf("\t") && pSymbol(head) && pSymbols(head->next); }
int pGrammar(Grammar *g) { printf("LHS: ") && pSymbol(&(g->lhs)) && printf("RHS:\n") && pSymbols(g->rhs_head) && printf("\n"); }
int pGrammars(Grammar head[NO_OF_GRAMMAR_RULES])
{
    for (int i = 0; i < NO_OF_GRAMMAR_RULES; i++)
        pGrammar(head + i) || printf("------------------------------------------------------------------\n");
}

static void reverse_grammar(Symbol **head_ref)
{
    Symbol *prev = NULL;
    Symbol *current = *head_ref;
    Symbol *next = NULL;
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
int symEqual(Symbol *s1, Symbol *s2) { return !(strcmp(s1->str, s2->str)); }