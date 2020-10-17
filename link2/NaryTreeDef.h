/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/



#ifndef _NARYTREEDEF_
#define _NARYTREEDEF_

#include <stdio.h>
#include <stdlib.h>

#include "parserDef.h" 


//Information about the tree Node
struct treeNodeInfo{
	int term_or_nonterm;
	Symbol_node_type type;
};
typedef struct treeNodeInfo TreeNodeInfo;


//Linked List of siblings
struct linkedlist{
	struct treeNode* head;
	int no_siblings;
};
typedef struct linkedlist Children;



struct treeNode{
	struct treeNode* next; //The node itself is a part of some linked list

	TreeNodeInfo* info;
	Token* lexicalTOKEN; //If it's a leaf node

	//Linked list of Children
	Children* children;	

	struct treeNode* parent; //Pointer to parent node


	/******Other Fields are for AST******/
	struct astNode* addr;
	struct astNode* inh_addr;
	int rule_no;

};
typedef struct treeNode TreeNode;


//ParseTree
struct parseTree{
	TreeNode* root;		
};
typedef struct parseTree ParseTree;

#endif