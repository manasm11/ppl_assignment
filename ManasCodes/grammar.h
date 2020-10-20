#include <stdio.h>
#define NO_OF_GRAMMAR_RULES 31
// <start> pro
typedef struct __symbol
{
    char str[16];
    int terminal;
    struct __symbol *next;
} Symbol;


typedef struct __grammar
{
    Symbol lhs;
    Symbol *rhs_head;
} Grammar;


// typedef Grammars[NO_OF_GRAMMAR_RULES] Grammar;