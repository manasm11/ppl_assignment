/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#ifndef _LOOKUPTABLEDEF_
#define _LOOKUPTABLEDEF_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexerDef.h"

struct node{
	Tokentype type;
	char* lexeme;
	struct node* next;
};

typedef struct node Node;

struct list{
	Node* head;
};

typedef struct list List;

struct lookUp{
	int no_slots;
	List** slots;
};	

typedef struct lookUp LookUpTable;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lookUpTableDef.h"



//Function Declarations
LookUpTable* newTable(int no_slots);
void addEntry(LookUpTable* lookUpTable, char* key, Tokentype tokentype);
int hash(char* key, int no_slots);
void populate_Table(LookUpTable* lookUpTable);
Node* getTokenType(char* key, LookUpTable* lookUpTable);

#endif
