typedef struct __parse_table
{

} ParseTable;

#pragma once
#include <stdio.h>
// #include "grammar.c"
// #include "tokenizer.c"

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
    printf("%s\n", root->data);
    if (root->next && printf("%s > ", root->data))
        print_tree(root->next);
    if (root->children && printf("%s\nV\n", root->data))
        print_tree(root->children);
}
// typedef struct __test
// {
//     char str[16];
// } Test;

// int main(int argc, char const *argv[])
// {
//     Test *root_test = (Test *)malloc(sizeof(Test));
//     strcpy(root_test->str, "ROOT");
//     Test *c1_test = (Test *)malloc(sizeof(Test));
//     strcpy(c1_test->str, "C1");
//     Test *c2_test = (Test *)malloc(sizeof(Test));
//     strcpy(c2_test->str, "C2");
//     Test *c11_test = (Test *)malloc(sizeof(Test));
//     strcpy(c11_test->str, "C11");
//     Node *root_node = nodeNew(root_test);
//     Node *c11_node = nodeNew(c11_test);
//     Node *c1_node = nodeNew(c1_test);
//     Node *c2_node = nodeNew(c2_test);
//     add_child(root_node, c1_node);
//     add_child(root_node, c2_node);
//     add_child(c1_node, c11_node);
//     // add_child(root_node, c1_node);
//     print_tree(root_node);
//     return 0;
// }
