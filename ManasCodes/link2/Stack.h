/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "StackDef.h"


//Function Declarations

Stack* initialize_stack();
StackNode* create_stack_node(TreeNode* treenode);
StackNode* pop(Stack* stk);
StackNode* top(Stack* stk);
void push(Stack* stk, TreeNode* treenode);
void push_children_on_the_stack(Stack* stk, TreeNode* treenode);
int isTopDOLLAR(Stack* stk);



