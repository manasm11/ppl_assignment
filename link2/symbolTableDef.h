/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#ifndef _SYMBOLTABLEDEF_
#define _SYMBOLTABLEDEF_

#include <stdio.h>
#include <stdlib.h>

#define SYMBOL_TABLE_SLOTS 20

#include "astDef.h"


/***********************************************************************************************/

//Linked List of siblings
struct nested_scopes_linkedlist{
	struct symTableTreeNode* head;
	int no_siblings;
};
typedef struct nested_scopes_linkedlist nestedScopes;


struct symTableTreeNode{

	//Symbol Table corresponding to this procedure
	struct symbolTable* table;

	//The node itself is a part of some linked list
	struct symTableTreeNode* next; 

	//Children HashTree nodes
	nestedScopes* children;	

	//Pointer to parent node
	struct symTableTreeNode* parent;

	//Name of the function this node represents
	char* scope;


};

typedef struct symTableTreeNode SymTableTreeNode;

//Symbol Table Tree
struct symTableTree{
	SymTableTreeNode* root;		
};
typedef struct symTableTree SymTableTree;


/***********************************************************************************************/


/********Actual Symbol Structure*******/


struct symbol{

	//Lexeme will be in it only
	Token* lexicalTOKEN;

	int datatype;

	//Memory occupied by variable
	int width;

	int offset;	

	//Whether the declared variable has been assigned some value or not
	int assigned;

	//Corresponding AST node of the symbol
	struct astNode* astNode;

	//For matrices
	int dimension1, dimension2;

	//For sorting symbols to print them in order
	int number;

	//For Code Generation
	int* val;
	char* str;

};
typedef struct symbol Symbol;



/***********************************************************************************************/



/***** Symbol Table ******/

struct symbolNode{
	
	/***This has actual symbol information**/

	Symbol* symbol;

	struct symbolNode* next;
};

typedef struct symbolNode SymbolNode;

struct slots_list{
	SymbolNode* head;
	int total_symbols;
};

typedef struct slots_list slotsList;

struct symbolTable{
	int no_slots;
	slotsList** slots;
};	

typedef struct symbolTable SymbolTable;


#endif