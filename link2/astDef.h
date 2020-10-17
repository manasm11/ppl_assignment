/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#ifndef _ASTDEF_
#define _ASTDEF_

#include <stdio.h>
#include <stdlib.h>

#include "NaryTreeDef.h"
#include "symbolTableDef.h"
#include "intermediateCodeDef.h"


//List of all possible Labels
typedef enum{
	MAINFUNCTION_NODE,
	FUNCTIONDEF_NODE,
	PARAMETER_NODE,
	DECLARATIONSTMT_NODE,
	ASSIGNMENTSTMT1_NODE,
	ASSIGNMENTSTMT2_NODE,
	FUNCTIONCALL_NODE,
	IF_NODE,
	ELSE_NODE,
	READ_NODE,
	PRINT_NODE,
	BOOLEANEXPRESSION1_NODE,
	BOOLEANEXPRESSION2_NODE,
	BOOLEANEXPRESSION3_NODE,
	CONSTRAINEDVARS_NODE,
	VAR_NODE,
	SIZEEXP_NODE,
	MATRIX_NODE,
	OPERATOR_NODE,
	ROW
}Labeltype;


//Linked List of siblings
struct astlinkedlist{
	struct astNode* head;
	int no_siblings;
	struct astNode* last;
};
typedef struct astlinkedlist astChildren;


struct astNode{

	//For Non-Leaf Nodes
	Labeltype label;

	//Whether leaf node or not
	int leaf;

	//Pointer to parent node
	struct astNode* parent;

	//Linked list of Children
	astChildren* children;

	//The node itself is a part of some linked list
	struct astNode* next; 

	//If it's a leaf node
	Token* lexicalTOKEN; 

	//For compressing list -- i.e concatenation purpose
	struct astNode* concat;

	//For symbol Table
	struct symTableTreeNode* current_scope;

	//For Type checking purposes
	Tokentype nodeType;

	//For funid node and size_exp node only
	struct typeList* return_parameters;

	//For printing errors
	int line_no;

	//For checking Recursion
	struct astNode* parent_function;

	//For string Length
	int str_length;

	//For Matrix dimensions
	int dimension1, dimension2;

	//For IR generation
	char* name;
	struct tuple* tuple;

	char* str;
	int* val;

};

typedef struct astNode ASTNode;


//AST
struct ast{
	ASTNode* root;		
};
typedef struct ast AST;


/////////////////////////////////////////////////////////////////////////////////////

struct typeList{
	struct typeListNode* head;
	struct typeListNode* last;
	int no_siblings;	
};
typedef struct typeList TypeList;

struct typeListNode{
	struct typeListNode* next;
	Tokentype type;
};
typedef struct typeListNode TypeNode;

#endif