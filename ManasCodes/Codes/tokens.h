// ID: 2017B5A70546P Name: Manas Mishra
// ID: 2017B4A70583P Name: Dhruv Patel
// ID: 2017B3A70783P Name: Bhavya Gera
// ID: 2017B3A70599P Name: Ayush Agrawal
#include <stdio.h>
typedef struct __token
{
    int line;
    int col;
    char str[64];
    struct __token *next;
} Token;

// int pToken(Token *t) { printf("Line: %d, Column: %d, Token: %s\n", t->line, t->col, t->str); }
int pToken(Token *t) { printf("Token: %s\n", t->str); }

int pTokens(Token *head) { head &&pToken(head) && pTokens(head->next); }

void add_token(Token **head_ref, int lno, int cno, char *str)
{
    /* 1. allocate node */
    Token *token = (Token *)malloc(sizeof(Token));
    /* 2. put in the data  */
    token->line = lno;
    token->col = cno;
    strcpy(token->str, str);

    /* 3. Make next of new node as head */
    token->next = (*head_ref);

    /* 4. move the head to point to the new node */
    (*head_ref) = token;
}
static void reverse_tokens(Token **head_ref)
{
    Token *prev = NULL;
    Token *current = *head_ref;
    Token *next = NULL;
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
