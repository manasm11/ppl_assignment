#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
#define assert(condition, error_message) condition || printf("[-] ERROR: %s\n", error_message) && exitt(1)
int exitt(int n) { exit(n); }
char filename[] = "src_code.txt";
int main(int argc, char const *argv[])
{
    FILE *fp = fopen(filename, "r");
    assert(fp, "Code file not found");
    char *line = (char *)malloc(sizeof(char) * 128);
    int lno = 1, cno = 0;
    for (size_t l; getline(&line, &l, fp) != -1; fflush(fp), cno = 0, lno++)
    {
        for (char *t = strtok(line, " \n"); t; t = strtok(NULL, " \n"))
        {
            Token tkn;
            initialize_token(&tkn, lno, ++cno, t);
            pToken(tkn);
        }
    }
    return 0;
}
