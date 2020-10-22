/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Stack.h"

Stack* initialize_stack(){
	Stack* stk = (Stack*)malloc(sizeof(Stack));
	stk->size=0;
	stk->list = (StackList*)malloc(sizeof(StackList));
	stk->list->head = NULL;

	return stk;
}

StackNode* create_stack_node(TreeNode* treenode){

	StackNode* node = (StackNode*)malloc(sizeof(StackNode));	
	node->treeNodePointer = treenode;
	node->next = NULL;

	return node;
}

void push(Stack* stk, TreeNode* treenode){
	StackNode* node = create_stack_node(treenode);
	
	node->next = stk->list->head;
	stk->list->head = node;

	stk->size++;
}

StackNode* pop(Stack* stk){
	if(stk->size==0){
		printf("Stack is Empty\n");
		return NULL;
	}
	else{
		StackNode* temp = stk->list->head;
		stk->list->head = stk->list->head->next;
		stk->size--;

		return temp;
	}
}

StackNode* top(Stack* stk){
	if(stk->size==0){
		printf("Stack is Empty\n");
		return NULL;
	}

	else{
		StackNode* temp = stk->list->head;
		return temp;
	}
}

void push_children_on_the_stack(Stack* stk, TreeNode* treenode){
	Children* children = treenode->children;
	TreeNode* temp = children->head;

	//DUMMY NODE
	StackNode* head = create_stack_node(NULL);
	StackNode* temp_stack = head;
	
	//Don't Push EPSILON on the stack
	if(temp->info->term_or_nonterm == 0 && temp->info->type.terminal == EPSILON){
		return;
	}

	while(temp!=NULL){

		//Create a Stack Node
		StackNode* node = create_stack_node(temp);
		
		temp_stack->next = node;
		temp_stack = temp_stack->next;

		temp = temp->next;
	}

	temp_stack->next = stk->list->head;
	stk->list->head = head->next;

	stk->size += treenode->children->no_siblings;
}

//Whether $ is on the top of the stack or not
int isTopDOLLAR(Stack* stk){
	if(stk->size==1){
		TreeNodeInfo* info = stk->list->head->treeNodePointer->info;
		if(info->term_or_nonterm==0 && info->type.terminal==DOLLAR)
			return 1;
	}		
	return 0;
}


