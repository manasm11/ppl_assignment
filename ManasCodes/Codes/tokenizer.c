#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
#define assert(condition, error_message) !condition && printf("[-] ERROR: %s\n", error_message) ? exit(1) : 1
Token *head = NULL;
Token *initialize_token_stream(char filename[])
{
    FILE *fp = fopen(filename, "r");
    assert(fp, "Code file not found");
    char *line = (char *)malloc(sizeof(char) * 128);
    int lno = 1, cno = 0;
    for (size_t l; getline(&line, &l, fp) != -1; fflush(fp), cno = 0, lno++)
    {
        for (char *t = strtok(line, " \n\t"); t; t = strtok(NULL, " \n\t"))
        {
            add_token(&head, lno, ++cno, t);
        }
    }
    reverse_tokens(&head);
    return head;
}
// int main(int argc, char const *argv[])
// {
//     initialize_token_stream("src_code.txt");
//     pTokens(head);
//     return 0;
// }
