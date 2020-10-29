
// ID: 2017B5A70546P Name: Manas Mishra
// ID: 2017B4A70583P Name: Dhruv Patel
// ID: 2017B3A70783P Name: Bhavya Gera
// ID: 2017B3A70599P Name: Ayush Agrawal
#include "parser.c"

Stack stack;
Token *temp_head;

void initialize_driver(char *grammar_file, char *source_code_file)
{
    readGrammar(grammar_file);
    tokeniseSourcecode(source_code_file);
    temp_head = head;
    initialise_stack(&stack);
    initialise_stack_of_nodes(&global_nodes);
    initialise_stack_of_nodes(&type_nodes);
    strcpy(stack.stack[0].str, "<start>");
    stack.stack[0].is_terminal = 0;
    stack.top = 0;
    Node *n = nodeNew(stack.stack[0]);
    global_nodes.nodes[0] = n;
    global_nodes.top = 0;
    type_nodes.top = -1;
    stack.stack[0].node = root;
    strcpy(stack.stack[0].str, "<start>");
    stack.stack[0].is_terminal = 0;
    stack.top = 0;
    root = nodeNew(stack.stack[0]);
    stack.stack[0].node = root;
    global_nodes.nodes[0] = root;
}

int main(int argc, char *argv[])
{
    int is_initialised_called = 0;
    int exit_loop = 0;
    char *grammar_file = "Grammar.txt";
    char *src_code_file = "src_code.txt";
    if (argc != 3)
    {
        RED &&printf("Grammar and source code files not specified properly, using %s and %s\N", grammar_file, src_code_file) && CLEAR_COLORS;
        initialize_driver(grammar_file, src_code_file);
    }
    else
    {
        initialize_driver(argv[1], argv[2]);
    }

    parse(grammars, head, stack);
    reverse_children(&root);
    get_depth(root, -1);
    while (!exit_loop)
    {
        printf("\nEnter the task you want to perform:\n\n");
        printf("0: For exiting from loop\n");
        printf("1: For creating parse tree\n");
        printf("2: Traverse the parse tree to construct typeExpressionTable. Also print the type errors while traversing the parse tree and accessing the typeExpressionTable \n");
        printf("3: For printing the parse tree in the specified format \n");
        printf("4: For printing the TypeExpressionTable in the specified format\n");

        printf("\n\nYour Input: ");
        int choice;
        scanf("%d", &choice);

        switch (choice)
        {

        case 0:
        {
            exit_loop = 1;
        }
        break;

        case 1:
        {
            createParseTree(root);
        }
        break;

        case 2:
        {
            type_check(temp_head);
            BOLD_GREEN &&printf("TYPE ERRORS PRINTED SUCCESSFULLY !!!\n") && CLEAR_COLORS;
        }
        break;

        case 3:
        {
            printParseTree(root);
        }
        break;

        case 4:
        {
            print_type_nodes(type_nodes);
            BOLD_GREEN &&printf("TYPE EXPRESSION TABLE PRINTED SUCCESSFULLY !!!\n") && CLEAR_COLORS;
        }
        break;
        default:
        {
            BOLD_RED &&printf("PLEASE ENTER VALID INPUT\n") && CLEAR_COLORS;
        }
        break;
        }
    }
    return 0;
}