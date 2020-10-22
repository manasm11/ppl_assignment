#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grammar.h"
#define assert(condition, error_message) !(condition) && printf("[-] %s\n\tERROR in file %s: line %d\n", error_message, __FILE__, __LINE__) ? exit(1) : void
#define debug() printf("[*] RAN TILL File: %s, Line: %d\n", __FILE__, __LINE__)
Grammar grammars[NO_OF_GRAMMAR_RULES];
int initialize_grammar(char filename[])
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
                // pSymbol(new_symbol);
                continue;
            }

            if (++cno == 1)
                continue;

            new_symbol->is_terminal = !(t[0] == '<');
            new_symbol->next = grammars[lno].rhs_head;
            grammars[lno].rhs_head = new_symbol;
            // printf("%s\n", t);
        }
        reverse_grammar(&(grammars[lno].rhs_head));
    }
}
// int main(int argc, char const *argv[])
// {
//     initialize_grammar("grammar.txt");
//     pGrammars(grammars);
//     return 0;
// }