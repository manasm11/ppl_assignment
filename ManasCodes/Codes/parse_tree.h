typedef struct __parse_table
{
    
} ParseTable;

#pragma once
#include <stdio.h>
#include "grammar.c"
#include "tokenizer.c"

typedef struct Node Node;
struct Node
{
    void *data;
    Node *next;
    Node *prev;
    Node *parent;
    Node *children;
};

Node *nodeNew(void *data)
{
    Node *n;

    n = (Node *)malloc(sizeof(Node));
    if (!n)
        return n;
    n->data = data;
    n->next = NULL;
    n->prev = NULL;
    n->parent = NULL;
    n->children = NULL;
    return n;
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
    printf("%s\n", root->data);
    if (root->next && printf("%s > ", root->data))
        print_tree(root->next);
    if (root->children && printf("%s\nV\n", root->data))
        print_tree(root->children);
}