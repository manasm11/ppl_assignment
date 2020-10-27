#include <stdio.h>
#include "colors.h"
int main(int argc, char const *argv[])
{
    char *string = "HELLO";
    printf("%s %s\n", string, string);
    BOLD_YELLOW;
    printf("%-12s %s\n", string, NULL);
    CLEAR_COLORS;
    return 0;
}
