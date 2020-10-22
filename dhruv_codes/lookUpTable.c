/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lookUpTable.h"

LookUpTable* newTable(int no_slots){
	
	LookUpTable* lookUpTable = (LookUpTable*)malloc(sizeof(LookUpTable));

	lookUpTable->no_slots = no_slots;
	
	lookUpTable->slots = (List**)malloc(no_slots*sizeof(List*));
	
	for(int i=0;i<no_slots;i++){
		lookUpTable->slots[i] = (List*)malloc(sizeof(List));
		lookUpTable->slots[i]->head = NULL;
	}

	//Adding the entries to the LookUpTable initially
	populate_Table(lookUpTable);

	return lookUpTable;
}

void addEntry(LookUpTable* lookUpTable, char* key, Tokentype tokentype){
	int hashvalue =  hash(key,lookUpTable->no_slots);

	Node* entry = (Node*)malloc(sizeof(Node));
	
	entry->type = tokentype;
	
	entry->lexeme = (char*)malloc(sizeof(char)*(strlen(key)+1));
	strcpy(entry->lexeme,key);
	
	entry->next = lookUpTable->slots[hashvalue]->head;
	
	//Adding the entry at the beginning of the slot
	lookUpTable->slots[hashvalue]->head = entry;
}

int hash(char* key, int no_slots){
	int sum = 0;

	for(int i=0;i<strlen(key);i++){
		sum = sum*10 + (key[i] - '0');
		if(sum<0)
			sum = 0;
	}

	int hashvalue = sum%no_slots;

	return hashvalue;
}

//Populating Lookup Table with all the keywords
void populate_Table(LookUpTable* lookUpTable){
	addEntry(lookUpTable, "declare", DECLARE);
	addEntry(lookUpTable, "integer", INTEGER);
	addEntry(lookUpTable, "real", REAL);
	addEntry(lookUpTable, "Boolean", BOOLEAN);
	addEntry(lookUpTable, "size", SIZE);
	addEntry(lookUpTable, "list", LIST);
	addEntry(lookUpTable, "of", OF);
	addEntry(lookUpTable, "variables", VARIABLES);
	addEntry(lookUpTable, "jagged", JAGGED);
	addEntry(lookUpTable, "array", ARRAY);
	addEntry(lookUpTable, "values", VALUES);
	addEntry(lookUpTable, "R1", R1);
}



Node* getTokenType(char* key, LookUpTable* lookUpTable){
	int hashvalue = hash(key, lookUpTable->no_slots);

	Node* temp = lookUpTable->slots[hashvalue]->head;

	while(temp!=NULL){
		//If the key exists in the table, return the node
		if(strcmp(temp->lexeme,key)==0)
			return temp;

		temp = temp->next;
	}
	
	//Else return Null
	return NULL;
}



/***************************************************

//Function to check the working of lookUpTable

int main(){
	LookUpTable* lookUpTable = newTable(20);
	Node* temp = getTokenType("matrix",lookUpTable);
	if(temp!=NULL){
		printf("%d\n",temp->type);
	}
	return 0;
}

*****************************************************/
