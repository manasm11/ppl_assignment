#include <stdio.h>
typedef struct __Token
{
    int data;
    struct __Token *next;
} Token;

Token *head = NULL;
Token *current = NULL;

//display the list
void printList()
{

    Token *ptr = head;

    printf("\n[head] =>");
    //start from the beginning
    while (ptr != NULL)
    {
        printf(" %d =>", ptr->data);
        ptr = ptr->next;
    }

    printf(" [null]\n");
}

//insert link at the first location
void insert(int data)
{
    //create a link
    Token *link = (Token *)malloc(sizeof(Token));

    //link->key = key;
    link->data = data;

    //point it to old first Token
    link->next = head;

    //point first to new first Token
    head = link;
}

int main()
{
    insert(10);
    insert(20);
    insert(30);
    insert(1);
    insert(40);
    insert(56);

    printList();
    return 0;
}