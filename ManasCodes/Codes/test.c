#include "grammar.c"
#include "tokenizer.c"
#include <ctype.h>
#define STACK_SIZE 512
#define debug(message) printf("[*] %s", message)
#define NO_OF_KEYWORDS 23
#define NO_OF_PRINTING_COLUMNS 100
#define elif else if
// #define perror(line, stmt_type, operator, lexeme1, id_type1, lexeme2, id_type2, depth) \
//     printf("%-10d\n", line);                                                             \
//     printf(" %-10s\n", stmt_type);                                                       \
//     printf(" %-10s\n", operator);                                                        \
//     printf(" %-10s\n", lexeme1);                                                         \
//     printf(" %-10d\n", id_type1);                                                        \
//     printf(" %-10s\n", lexeme2);                                                         \
//     printf(" %-10d\n", id_type2);                                                        \
//     printf(" %-10d\n", depth);
#define perror(line, stmt_type, operator, lexeme1, id_type1, lexeme2, id_type2, depth, message) printf("%-5d %-15s %-10s %-15s %-15s %-15s %-15s %d\n", line, stmt_type, operator, lexeme1, id_type1, lexeme2, id_type2, depth) && BOLD_RED &&printf("ERROR: ") && printf(message) && CLEAR_COLORS &&NEWLINE;
int is_keyword(char *str)
{
    char *keywords[NO_OF_KEYWORDS] = {
#include "keywords.h"
    };
    for (int i = 0; i < NO_OF_KEYWORDS; i++)
    {
        if (!strcmp(str, keywords[i]))
        {
            return 1;
        }
    }
    return 0;
}

typedef struct __parse_table
{

} ParseTable;

#pragma once
#include <stdio.h>

Node *root;

typedef struct __st_nodes
{
    int top;
    Node **nodes;

} Stack_Of_Nodes;

Stack_Of_Nodes global_nodes;
Stack_Of_Nodes type_nodes;

void initialise_stack_of_nodes(Stack_Of_Nodes *s)
{
    s->nodes = (Node **)malloc(sizeof(Node *) * STACK_SIZE);
    // printf("NODES IS INITIALISED !!!\n");
}

typedef struct __stack
{
    int top;
    Symbol *stack;
} Stack;

void initialise_stack(Stack *s)
{
    s->stack = (Symbol *)malloc(sizeof(Symbol) * STACK_SIZE);
}

// Stack global_stack[STACK_SIZE];
void copy_stack(Stack *dest, Stack *src)
{
    for (int i = 0; i < src->top + 1; i++)
    {
        dest->stack[i].is_terminal = src->stack[i].is_terminal;
        dest->stack[i].next = src->stack[i].next;
        strcpy(dest->stack[i].str, src->stack[i].str);
        dest->stack[i].node = src->stack[i].node;
    }
    dest->top = src->top;
}
// Stack stack;
int print_stack(Stack s)
{
    for (int i = 0; i < s.top + 1; i++)
    {
        pSymbol(&s.stack[i]);
    }
    printf("\n");
}
Symbol pop(Stack *s)
{
    if (s->top >= 0)
    {
        return s->stack[s->top--];
    }
}

int print_global_nodes(Stack_Of_Nodes s)
{
    printf("PRINTING GLOBAL STACK\n");
    for (int i = 0; i < s.top + 1; i++)
    {
        printf("%s ", s.nodes[i]->data.str);
    }
    printf("\n");
}

int free_children(Node *parent)
{
    if (!parent)
        return -1;
    Node *child = parent->children;
    // printf("PRIND %s\n", child->data.str);
    while (child)
    {
        printf("FREEING %s\n", child->data.str);
        printf("HEALO !!!\n");
        Node *next = child->next;
        free_children(child);
        child = next;
    }
    if (parent)
        free(parent);
    return 0;
}

void push(Stack *s, Symbol sym)
{
    s->top++;
    s->stack[s->top].is_terminal = sym.is_terminal;
    s->stack[s->top].next = sym.next;
    strcpy(s->stack[s->top].str, sym.str);
    s->stack[s->top].node = nodeNew(s->stack[s->top]);
}

int is_bool_operator(Token *t)
{
    // printf("GOING IN IS_BOOL with %s \n", t->str);
    return !(strcmp(t->str, "&&&") && strcmp(t->str, "|||"));
}
int is_arithematic_operator(Token *t)
{
    return !(strcmp(t->str, "+") && strcmp(t->str, "-") && strcmp(t->str, "*") && strcmp(t->str, "/") && strcmp(t->str, "="));
}

char *id2str(int id)
{
    if (id == INTEGER_ID)
        return "integer";
    if (id == BOOL_ID)
        return "Boolean";
    if (id == REAL_ID)
        return "real";
    if (id == JAGGED_ARR_ID)
        return "jagged";
    if (id == RECT_ARR_ID)
        return "array";
    return "NONE!!!";
}

int get_id_type(Token *t)
{
    // printf("TOP OF TYPE NODES: %d\n", type_nodes.top);
    for (int i = 0; i < type_nodes.top + 1; i++)
    {
        // printf(type_nodes.nodes[i]->data.str) && NEWLINE;
        if (!strcmp(type_nodes.nodes[i]->data.str, t->str))
        {
            // printf("ID TYPE OF %s is %d", type_nodes.nodes[i]->data.str, type_nodes.nodes[i]->data.id_type);
            return type_nodes.nodes[i]->data.id_type;
        }
    }
    return -1;
}

int parse(Grammar grammars[], Token *head, Stack stack)
{
    if (!head)
        return 1;
    // debug("PARSING HEAD ") && pToken(head);
    // debug("PARSING STACK\n") && print_stack(stack);
    // for (int i = 0; i < global_nodes.top + 1; i++)
    // {
    //     printf("%s ", global_nodes.nodes[i]->data.str);
    // }
    // printf("\n");
    if (stack.stack[stack.top].is_terminal)
    {
        global_nodes.nodes[global_nodes.top]->data.line_no = head->line;
        int is_id = !strcmp(stack.stack[stack.top].str, "id");
        int is_num = !strcmp(stack.stack[stack.top].str, "num");
        // if(is_id){}
        // printf("[*] stack[top].str = %s\n[*] is_id = %d\n", stack.stack[stack.top].str, is_id);
        // Checking if it is an identifier

        if (!is_keyword(head->str))
        {
            // FIXME Will except numbers with first character as digit.
            // Eg: 1ab is a num
            // if (!strcmp(stack.stack[stack.top].str, "e"))
            // {
            //     stack.top--;
            //     global_nodes.top--;
            //     return parse(grammars, head, stack);
            // }
            if (is_num && isdigit(head->str[0]))
            {
                strcpy(global_nodes.nodes[global_nodes.top]->data.str, head->str);
                global_nodes.nodes[global_nodes.top]->data.type = INT;
                stack.top--;
                global_nodes.top--;
                head = head->next;
                // printf("HEAD = %s\n", head->str);
                return parse(grammars, head, stack);
            }
            if (is_id && !isdigit(head->str[0]))
            {
                Token *temp = head;
                Node *temp_ref = global_nodes.nodes[global_nodes.top];
                strcpy(global_nodes.nodes[global_nodes.top]->data.str, head->str);
                global_nodes.nodes[global_nodes.top]->data.type = ID;
                stack.top--;
                global_nodes.top--;

                head = head->next;
                // printf("HEAD = %s\n", head->str);
                if (parse(grammars, head, stack))
                {
                    int count = 5;
                    while (temp && strcmp(temp->str, ":") && count--)
                        temp = temp->next;
                    if (!temp)
                        return 1;
                    if (strcmp(temp->str, ":"))
                        return 1;
                    temp = temp->next;
                    if (!temp)
                        return 1;
                    // printf("%s\n", temp->str);
                    // pTokens(temp);
                    if (!strcmp(temp->str, "integer"))
                    {

                        // printf("is_type = %d\n", temp_ref->data.id_type);
                        temp_ref->data.id_type = INTEGER_ID;
                        type_nodes.nodes[++type_nodes.top] = temp_ref;
                    }
                    elif (!strcmp(temp->str, "Boolean"))
                    {
                        // printf("is_type = %d\n", temp_ref->data.id_type);
                        temp_ref->data.id_type = BOOL_ID;
                        type_nodes.nodes[++type_nodes.top] = temp_ref;
                    }
                    elif (!strcmp(temp->str, "real"))
                    {
                        // printf("is_type = %d\n", temp_ref->data.id_type);
                        temp_ref->data.id_type = REAL_ID;
                        type_nodes.nodes[++type_nodes.top] = temp_ref;
                    }
                    elif (!strcmp(temp->str, "jagged"))
                    {
                        // printf("is_type = %d\n", temp_ref->data.id_type);
                        temp_ref->data.id_type = JAGGED_ARR_ID;
                        temp = temp->next->next;
                        // printf("HEALLO \n");
                        // pToken(temp);
                        while (strcmp(temp->str, "of"))
                        {
                            strcat(temp_ref->data.dimensions, temp->str);
                            strcat(temp_ref->data.dimensions, " ");
                            // pToken(temp);
                            temp = temp->next;
                        }
                        // printf(temp_ref->data.dimensions);
                        // NEWLINE;
                        type_nodes.nodes[++type_nodes.top] = temp_ref;
                    }
                    elif (!strcmp(temp->str, "array"))
                    {
                        // printf("is_type = %d\n", temp_ref->data.id_type);
                        temp_ref->data.id_type = RECT_ARR_ID;
                        if (isdigit(temp->next->next->str[0]))
                        {
                            temp_ref->data.is_static = 1;
                        }
                        else
                        {
                            temp_ref->data.is_static = 0;
                        }
                        temp = temp->next;
                        // printf("HEALLO \n");
                        // pToken(temp);
                        // printf("%d\n", temp_ref->data.id_type);
                        while (strcmp(temp->str, "of"))
                        {
                            strcat(temp_ref->data.dimensions, temp->str);
                            // strcat(temp_ref->data.dimensions, " ");
                            // pToken(temp);
                            temp = temp->next;
                        }
                        // printf("%d\n", temp_ref->data.id_type);
                        // printf("%s\n", temp_ref->data.dimensions);
                        NEWLINE;
                        type_nodes.nodes[++type_nodes.top] = temp_ref;
                    }
                    // printf("KEYWORD VALUE: %s !!!\n", head->next->str);
                    // printf("HELLO !!!\n");
                    return 1;
                }
            }
            // printf("[-] ERROR in line: %d\n", head->line);
        }

        // if (is_id && !is_keyword(head->str))
        // {
        //     strcpy(global_nodes.nodes[global_nodes.top]->data.str, head->str);
        //     stack.top--;
        //     global_nodes.top--;
        //     head = head->next;
        //     // printf("HEAD = %s\n", head->str);
        //     return parse(grammars, head, stack);
        // }
        if (!strcmp(head->str, stack.stack[stack.top].str))
        {
            stack.top--;
            global_nodes.top--;
            head = head->next;
            // printf("HEAD = %s\n", head->str);
            return parse(grammars, head, stack);
        }
        else
        {
            return 0;
        }
    }
    else
    {
        Node *terminal = nodeNew(stack.stack[stack.top]);
        for (int i = 0; i < NO_OF_GRAMMAR_RULES; i++)
        {
            Stack_Of_Nodes temp_global_nodes;
            initialise_stack_of_nodes(&temp_global_nodes);
            if (!strcmp(grammars[i].lhs.str, stack.stack[stack.top].str))
            {
                global_nodes.nodes[global_nodes.top]->data.grammar_rule_no = i;
                Stack temp;
                initialise_stack(&temp);
                Stack temp2;
                initialise_stack(&temp2);
                temp2.top = -1;
                temp.top = -1;
                copy_stack(&temp, &stack);
                pop(&temp);
                for (Symbol *s = grammars[i].rhs_head; s != NULL; s = s->next)
                {
                    push(&temp, *s);
                    push(&temp2, *s);
                }
                for (int i = 0; i < global_nodes.top + 1; i++)
                {
                    temp_global_nodes.nodes[i] = global_nodes.nodes[i];
                }
                temp_global_nodes.top = global_nodes.top;

                Node *parent = global_nodes.nodes[global_nodes.top--];
                for (int i = 0; i < temp2.top + 1; i++)
                {
                    copy_symbol(&temp2.stack[i].node->data, &temp2.stack[i]);
                    Node *child_node = nodeNew(temp2.stack[i]);
                    add_child(parent, child_node);
                    global_nodes.nodes[++global_nodes.top] = child_node;
                    // global_nodes.nodes[global_nodes.top-1]->data.depth = depth;
                }

                if (parse(grammars, head, temp))
                {
                    return 1;
                }
                for (int i = temp_global_nodes.top; i > -1; i--)
                {
                    global_nodes.nodes[i] = temp_global_nodes.nodes[i];
                }
                global_nodes.top = temp_global_nodes.top;
                for (Node *t = parent->children; t; t = t->next)
                {
                    // printf("JIA MANAT DI %s\n", t->data.str);
                    free_children(t);
                }
                parent->children = NULL;
            }
        }
        return 0;
    }
}
static void reverse(Node **head_ref)
{
    struct Node *prev = NULL;
    struct Node *current = *head_ref;
    struct Node *next = NULL;
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

void reverse_children(Node **head_ref)
{
    if (*head_ref)
    {
        reverse(&(*head_ref)->children);
        for (Node *n = (*head_ref)->children; n; n = n->next)
        {
            reverse_children(&n);
        }
    }
}

int print_heading(char heading[])
{
    // char *heading = "PRINTING TYPE EXPRESSION TABLE";
    BOLD_MAGENTA;
    for (int i = 0; i < NO_OF_PRINTING_COLUMNS; i++)
        printf("*");
    NEWLINE;
    for (int i = 0; i < (NO_OF_PRINTING_COLUMNS - strlen(heading)) / 2 - 1; i++)
    {
        printf("*");
    }
    BOLD_CYAN &&printf(" %s ", heading) && BOLD_MAGENTA;
    for (int i = 0; i < (NO_OF_PRINTING_COLUMNS - strlen(heading)) / 2 - 1; i++)
    {
        printf("*");
    }
    NEWLINE;
    for (int i = 0; i < NO_OF_PRINTING_COLUMNS; i++)
        printf("*");
    NEWLINE;
    CLEAR_COLORS;
}

void print_type_nodes(Stack_Of_Nodes s)
{
    // print_heading("PRINTING PARSE TABLE");
    print_heading("PRINTING TYPE EXPRESSION TABLE");
    // BOLD_YELLOW;
    // printf("");
    // CLEAR_COLORS;
    BOLD_YELLOW &&printf("%-10s %-10s %-22s %-10s", "VARIABLE", "IS_ARRAY", "IS_STATIC / DIMS", "DATA_TYPE") && CLEAR_COLORS &&NEWLINE;
    for (int i = 0; i < s.top + 1; i++)
    {
        char temp[64];
        strcpy(temp, "");
        strcat(temp, s.nodes[i]->data.id_type != 3 ? "-" : s.nodes[i]->data.is_static ? "Static" : "Dynamic");
        strcat(temp, s.nodes[i]->data.dimensions);
        printf("%-10s %-10s %-22s %-10s\n", s.nodes[i]->data.str, s.nodes[i]->data.id_type == 3 || s.nodes[i]->data.id_type == 4 ? id2str(s.nodes[i]->data.id_type) : "-", temp, (s.nodes[i]->data.id_type == INTEGER_ID || s.nodes[i]->data.id_type == REAL_ID || s.nodes[i]->data.id_type == BOOL_ID) ? id2str(s.nodes[i]->data.id_type) : "-");
    }
}

// int get_token_depth(int line, char *operator)
int get_token_depth(Token *root)
{
    // static int i = 0;
    // int i = 0;
    // printf("Token: %s\n", t->str);
    // for (; i < type_nodes.top + 1; i++)
    // {
    //     // printf("Checking %s\n", type_nodes.nodes[i]->data.str);
    //     if (!strcmp(type_nodes.nodes[i]->data.str, t->str))
    //     {
    //         return type_nodes.nodes[i]->data.depth;
    //     }
    // }
    // if (!root)
    //     return 1;
    return 0;
}

int type_check(Token *head)
{
    print_heading("PRINTING TYPE ERRORS !!!");
    BOLD_YELLOW &&printf("%-5s %-15s %-10s %-15s %-15s %-15s %-15s %s\n", "LINE", "STATEMENT TYPE", "OPERATOR", "LEFT LEXEME", "L-LEXEME TYPE", "RIGHT LEXEME", "R-LEXEME TYPE", "DEPTH") && CLEAR_COLORS;
    int prev_line = -1;
    Token *prev = NULL;

    while (head)
    {
        if (!strcmp(head->str, "R1"))
        {
            head = head->next->next->next->next->next->next;
            int n = head->str[0] - '0';
            int sc_count = 0;
            while (strcmp(head->str, "}"))
            {
                if (!strcmp(head->str, ";"))
                    sc_count++;
                head = head->next;
            }
            // printf("SEMICOLON %d\n", sc_count);
            if (sc_count + 1 != n)
                perror(head->line, "DECLARATION", "R1", "{", "size", "}", "jagged", get_token_depth(head), "JAGGED ARRAY DECLARATION");
        }
        int temp_line = head->line;
        if (head->next && !is_keyword(head) && !isdigit(head->str[0]) && head->line != prev_line)
        {
            if (is_bool_operator(head->next))
            {
                if (get_id_type(head) != BOOL_ID)
                {
                    // printf("ERROR in line %d: Boolean operator can be applied to boolean variables.\n", head->line);
                    // char err_msg[64];
                    // strcpy(err_msg, "");
                    // strcat(err_msg, head->str);
                    // strcat(err_msg, " IS NOT BOOLEAN");
                    head->next && head->next->next &&perror(head->line, "ASSIGNMENT", head->next->str, head->str, id2str(get_id_type(head)), head->next->next->str, id2str(get_id_type(head->next->next)), get_token_depth(head), "TYPES NOT COMPATIBLE");
                    prev_line = head->line;
                }
                elif (BOOL_ID != get_id_type(head->next->next))
                {
                    // printf("ERROR in line %d: Type mismach\n", head->line);
                    // char err_msg[64];
                    // strcpy(err_msg, "");
                    // strcat(err_msg, head->str);
                    // strcat(err_msg, " IS NOT BOOLEAN");

                    head->next && head->next->next &&perror(head->line, "ASSIGNMENT", head->next->str, head->str, id2str(get_id_type(head)), head->next->next->str, id2str(get_id_type(head->next->next)), get_token_depth(head), "TYPES NOT COMPATIBLE");
                    prev_line = head->line;
                }
            }
            int flag1 = 0;
            int flag2 = 0;
            char id_name1[32];
            char id_name2[32];
            Token *tmp = NULL;
            if (!strcmp(head->next->str, "["))
            {
                strcpy(id_name1, head->str);
                flag1 = 1;
                while (head && strcmp(head->str, "]"))
                {
                    // printf(head->str) && NEWLINE;
                    head = head->next;
                }
            }
            tmp = head->next->next;
            if (tmp && tmp->next && !strcmp(tmp->next->str, "["))
            {
                // printf("ME YAHA AA GYA !!!\n");
                strcpy(id_name2, tmp->str);
                flag2 = 1;
            }
            if (is_arithematic_operator(head->next))
            {
                // printf("ME YAHA HU !!!\n");
                int head_id = -1;
                if (!flag1)
                    head_id = get_id_type(head);

                // head_id = head_id == JAGGED_ARR_ID ? INTEGER_ID : head_id;
                // head_id = head_id == RECT_ARR_ID ? INTEGER_ID : head_id;
                else
                    (head_id = INTEGER_ID);
                head_id = !strcmp(prev->str, "/") ? REAL_ID : head_id;
                int head_next_next_id = -1;
                if (!flag2)
                    head_next_next_id = get_id_type(head->next->next);

                // head_next_next_id = head_next_next_id == JAGGED_ARR_ID ? INTEGER_ID : head_next_next_id;
                // head_next_next_id = head_next_next_id == JAGGED_ARR_ID && !strcmp(head->next->next->next->str, "[") ? INTEGER_ID : head_next_next_id;
                // head_next_next_id = head_next_next_id == RECT_ARR_ID && !strcmp(head->next->next->next->str, "[") ? INTEGER_ID : head_next_next_id;
                // head_next_next_id = head_next_next_id == RECT_ARR_ID ? INTEGER_ID : head_next_next_id;
                else
                    (head_next_next_id = INTEGER_ID);
                head->next->next->next && (head_next_next_id = !strcmp(head->next->next->next->str, "/") ? REAL_ID : head_next_next_id);
                if ((head_id != -1) && (head_next_next_id != -1) && head_id != head_next_next_id)
                {
                    // char err_msg[64];
                    // strcpy(err_msg, "");
                    // strcat(err_msg, head->str);
                    // strcat(err_msg, " and ");
                    // strcat(err_msg, head->next->next->str);
                    // strcat(err_msg, " are not of same type.");

                    head->next && head->next->next &&perror(head->line, "ASSIGNMENT", head->next->str, flag1 ? id_name1 : head->str, flag1 ? "integer" : id2str(head_id), flag2 ? id_name2 : head->next->next->str, flag2 ? "integer" : id2str(head_next_next_id), get_token_depth(head), "TYPES NOT COMPATIBLE");
                    // head->next && head->next->next &&perror(head->line, "ASSIGNMENT", head->next->str, flag1 ? id_name1 : head->str, flag1 ? "integer" : id2str(get_id_type(head)), flag2 ? id_name2 : head->next->next->str, flag2 ? "integer" : id2str(get_id_type(head->next->next)), get_token_depth(head));
                    // printf("line:%d\n", head->line);
                    //  get_token_depth(head->next->next));
                    prev_line = head->line;
                }
            }
        }
        prev = head;
        head = head->next;
    }
}

int main(int argc, char const *argv[])
{
    // initialize_grammar("grammar_test.txt");
    initialize_grammar("grammar_copy_ki_copy.txt");
    // pGrammars(grammars);
    // initialize_token_stream("src_code.txt");
    initialize_token_stream("src_code_test.txt");
    Token *temp_head = head;
    // pTokens(head);
    Stack stack;
    initialise_stack(&stack);
    initialise_stack_of_nodes(&global_nodes);
    initialise_stack_of_nodes(&type_nodes);
    strcpy(stack.stack[0].str, "<start>");
    stack.stack[0].is_terminal = 0;
    stack.top = 0;
    Node *n = nodeNew(stack.stack[0]);
    global_nodes.nodes[0] = n;
    // printf("NEW GLOBAL NODES ACESSED !!!\n");
    global_nodes.top = 0;
    type_nodes.top = -1;

    stack.stack[0].node = root;
    strcpy(stack.stack[0].str, "<start>");
    stack.stack[0].is_terminal = 0;
    stack.top = 0;

    root = nodeNew(stack.stack[0]);
    stack.stack[0].node = root;
    global_nodes.nodes[0] = root;
    int val = parse(grammars, head, stack);
    // val ? debug("SUCCESS\n") : debug("UNSUCCESSFUL\n");
    // print_tree(root);
    reverse_children(&root);
    // printf("---------------------------------------------------\n");
    get_depth(root, -1);

    // print_heading("")
    // BOLD_BLUE;
    // // printf("\n*******************************");
    // // printf("\n***** PRINTING PARSE TREE *****");
    // // printf("\n*******************************\n\033[0m");
    // CLEAR_COLORS;
    print_tree(root);
    print_type_nodes(type_nodes);
    type_check(temp_head);
    return 0;
}
