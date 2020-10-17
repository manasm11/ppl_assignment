/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#ifndef _INTERMEDIATECODEDEF_
#define _INTERMEDIATECODEDEF_

#include "semanticAnalyzerDef.h"
#include "astDef.h"


struct tuple{
	Tokentype op;
	char* arg1;
	char* arg2;
	char* result;

	struct tuple* next;

	//For getting offsets
	struct astNode *node1,*node2,*node3;

	//For Code Generation
	int process;
};
typedef struct tuple Tuple;





struct tupleList{
	Tuple* head;
	Tuple* last;
	int total_tuples;	
};
typedef struct tupleList TupleList;


#endif