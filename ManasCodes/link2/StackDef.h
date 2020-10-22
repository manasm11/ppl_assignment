/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#ifndef _STACKDEF_
#define _STACKDEF_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NaryTreeDef.h"

struct stackNode{
	TreeNode* treeNodePointer;
	struct stackNode* next;
};

typedef struct stackNode StackNode;


struct stackList{
	StackNode* head;
};

typedef struct stackList StackList;

struct stack{
	StackList* list;
	int size;
};

typedef struct stack Stack;

#endif
