#include <stdio.h>
#define NO_OF_GRAMMAR_RULES 49
#include "colors.h"
// #define NO_OF_TERMINALS 3
// #define NO_OF_NON_TERMINALS 2
// #define NO_OF_GRAMMAR_RULES 47
// <start> pro
#define assert(condition, error_message) condition || RED &&printf("[-] %s !!!\n", error_message) && CLEAR_COLORS
#include "colors.h"
#define NEWLINE printf("\n")
typedef struct Node Node;
typedef enum __type
{
    KEYWORD,
    ID,
    INT,
    REAL,
} Type;

typedef enum __id_type
{
    INTEGER_ID,
    REAL_ID,
    BOOL_ID,
    RECT_ARR_ID,
    JAGGED_ARR_ID
} Id_type;

typedef struct __symbol
{
    char str[32];
    int is_terminal;
    struct __symbol *next;
    int is_static;
    char dimensions[32];
    Type type;
    Id_type id_type;
    int grammar_rule_no;
    int line_no;
    int depth;
    Node *node;
} Symbol;

int copy_symbol(Symbol *dest, Symbol *src)
{
    dest->is_terminal = src->is_terminal;
    dest->next = src->next;
    dest->line_no = src->line_no;
    dest->grammar_rule_no = src->grammar_rule_no;
    dest->id_type = src->id_type;
    dest->depth = src->depth;
    dest->is_static = src->is_static;
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
    {
        assert(n, "NODE NOT CREATED !!!");
        return n;
    }
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
    static int first = 1;
    static int count = 1;
    if (first)
    {
        first = 0;
        BOLD_YELLOW &&printf("%-20s %-10s %-10s %-10s", "SYMBOL NAME", "DEPTH", "TERMINAL", "LEXEME") && CLEAR_COLORS &&NEWLINE;
    }
    if (!root)
        return 0;
    char sym_name[16];
    if (root->data.type == ID)
    {
        strcpy(sym_name, "id");
    }
    else if (root->data.type == INT)
    {
        strcpy(sym_name, "num");
    }
    else
    {
        strcpy(sym_name, root->data.str);
    }

    printf("%d.%-20s %-10d %-10s %-10s", count++, sym_name, root->data.depth, root->data.is_terminal ? "TRUE" : "FALSE", root->data.is_terminal ? root->data.str : "***") && NEWLINE;
    // printf("[⭐] %s\n\t", root->data.str);
    // printf("CHILDREN OF: %s\n\t", root->data.str);
    // root->children || printf("**");
    // for (Node *child = root->children; child; child = child->next)
    // {
    //     // printf("%s, %d, \t", child->data.str, child->data.is_terminal ? child->data.line_no : -1);
    //     // printf("%s, %d\t", child->data.str, child->data.is_terminal ? -1 : child->data.grammar_rule_no);
    //     // printf("%s, %d\t", child->data.str, child->data.depth);
    //     // printf("%s  ", child->data.str);
    // }
    // printf("\n");
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
        pGrammar(head + i) || printf("\n------------------------------------------------------------------\n");
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
int get_depth(Node *node, int depth)
{
    if (!node)
        return 0;
    // static int depth = 0;
    depth++;
    node->data.depth = depth;
    for (Node *n = node->children; n; n = n->next)
    {
        // pSymbol(&n->data);
        get_depth(n, depth);
    }
    return 1;
}