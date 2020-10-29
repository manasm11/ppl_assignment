// ID: 2017B5A70546P Name: Manas Mishra
// ID: 2017B4A70583P Name: Dhruv Patel
// ID: 2017B3A70783P Name: Bhavya Gera
// ID: 2017B3A70599P Name: Ayush Agrawal
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grammar.h"
#define assert(condition, error_message) !(condition) && printf("[-] %s\n\tERROR in file %s: line %d\n", error_message, __FILE__, __LINE__) ? exitt(1) : 1
#define debug() printf("[*] RAN TILL File: %s, Line: %d\n", __FILE__, __LINE__)
int exitt(int n) { exit(n); }
Grammar grammars[NO_OF_GRAMMAR_RULES];
int readGrammar(char filename[])
{
    FILE *fp = fopen(filename, "r");
    assert(fp, "Grammar file not found");
    char *line = (char *)malloc(sizeof(char) * 128);
    int lno = 0, cno = 0, is_first;
    for (size_t l; getline(&line, &l, fp) != -1; fflush(fp), cno = 0, lno++)
    {
        is_first = 1;
        for (char *t = strtok(line, " \n"); t; t = strtok(NULL, " \n"))
        {
            assert(lno < NO_OF_GRAMMAR_RULES, "Change NO_OF_GRAMMAR_RULES according to grammar.txt");
            Symbol *new_symbol = (Symbol *)malloc(sizeof(Symbol));
            strcpy(new_symbol->str, t);
            if (is_first)
            {
                is_first = 0;
                new_symbol->is_terminal = 0;
                new_symbol->next = NULL;
                grammars[lno].lhs = *new_symbol;
                grammars[lno].rhs_head = NULL;
                continue;
            }

            if (++cno == 1)
                continue;

            new_symbol->is_terminal = !(t[0] == '<');
            new_symbol->next = grammars[lno].rhs_head;
            grammars[lno].rhs_head = new_symbol;
        }
    }
}
