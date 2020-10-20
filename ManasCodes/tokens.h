#include <stdio.h>
typedef struct __token
{
    int line;
    int col;
    char str[16];
} Token;

void pToken(Token t) { printf("Line: %d, Column: %d, Token: %s\n", t.line, t.col, t.str); }

int initialize_token(Token *t, int line, int col, char *str)
{
    t->line = line;
    t->col = col;
    strcpy(t->str, str);
}

typedef struct{
    KeyWordEnum type;
    char *value;
}KeyWord;