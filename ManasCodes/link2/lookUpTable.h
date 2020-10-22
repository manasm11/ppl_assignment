/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


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



