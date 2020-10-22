/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "symbolTable.h"
#include "semanticAnalyzer.h"

extern char* LabelMap[];
extern char* TerminalMap[];
extern char* NonTerminalMap[];



// Complete make Symbol 
// currentScope of funid


/**** Functions to initialize symbol table and add entries to it ******/

SymbolTable* newSymbolTable(int no_slots){
	
	SymbolTable* symbolTable = (SymbolTable*)malloc(sizeof(SymbolTable));

	symbolTable->no_slots = no_slots;
	
	symbolTable->slots = (slotsList**)malloc(no_slots*sizeof(slotsList*));
	
	for(int i=0;i<no_slots;i++){
		symbolTable->slots[i] = (slotsList*)malloc(sizeof(slotsList));
		symbolTable->slots[i]->head = NULL;
		symbolTable->slots[i]->total_symbols = 0;
	}

	return symbolTable;
}

void addSymbol(SymbolTable* symbolTable, char* key, Symbol* symbol){
	int hashvalue =  findHash(key,symbolTable->no_slots);

	SymbolNode* entry = (SymbolNode*)malloc(sizeof(SymbolNode));
	
	entry->symbol = symbol;
	
	entry->next = symbolTable->slots[hashvalue]->head;
	
	//Adding the entry at the beginning of the slot
	symbolTable->slots[hashvalue]->head = entry;

	symbolTable->slots[hashvalue]->total_symbols++;
}

int findHash(char* key, int no_slots){
	int sum = 0;

	for(int i=0;i<strlen(key);i++){
		sum = sum*10 + (key[i] - '0');
		sum = sum%no_slots;
	}

	int hashvalue = sum%no_slots;

	return hashvalue;
}



SymbolNode* getSymbolInfo(char* key, SymbolTable* symbolTable){
	int hashvalue = findHash(key, symbolTable->no_slots);

	SymbolNode* temp = symbolTable->slots[hashvalue]->head;

	
	while(temp!=NULL){
		//If the key exists in the table, return the node
		
		if(strcmp(temp->symbol->lexicalTOKEN->lexeme,key)==0)
			return temp;

		temp = temp->next;
		
	}
		
	//Else return Null
	return NULL;
}


/*************************************************************************************************/

SymTableTree* initialize_SymbolTableTree(){
	SymTableTree* tree = (SymTableTree*)malloc(sizeof(SymTableTree));
	tree->root = NULL;	

	return tree;
}

SymTableTreeNode* make_symTableTreeNode(SymTableTreeNode* parent, char* scope){

	SymTableTreeNode* node = (SymTableTreeNode*)malloc(sizeof(SymTableTreeNode));
	node->table = newSymbolTable(SYMBOL_TABLE_SLOTS);
	node->next = NULL;
	node->children = initialize_nested_scopes();

	node->parent = parent;

	node->scope = scope;

	if(parent!=NULL)
		add_to_nested_scopes(parent,node);

	return node;
}

void add_to_nested_scopes(SymTableTreeNode* parent, SymTableTreeNode* node){
	
	node->next = parent->children->head;
	parent->children->head = node;

	parent->children->no_siblings++;
}

nestedScopes* initialize_nested_scopes(){
	nestedScopes* children = (nestedScopes*)malloc(sizeof(nestedScopes));
	children->head = NULL;
	children->no_siblings = 0;
	
	return children;
}



/************************************************************************************************/

SymTableTree* buildAllSymbolTables(AST* ast, ErrorList* errors){

	SymTableTree* tree = initialize_SymbolTableTree();

	//Add root to the tree
	char* _main = (char*)malloc(sizeof(char)*10);
	strcpy(_main,"_main");
	tree->root = make_symTableTreeNode(NULL, _main);

	int number = 0;
	traverseAST(ast->root, tree->root, errors, &number);

	return tree;

}


void traverseAST(ASTNode* node, SymTableTreeNode* current_scope, ErrorList* errors, int* number){

	if(node==NULL)
		return;

	node->current_scope = current_scope;

	SymTableTreeNode* new_scope = current_scope;
	

	/*** if Node is FUNCTIONDEF_NODE ***/
	if(node->leaf== 0 && node->label == FUNCTIONDEF_NODE){
		new_scope = make_symTableTreeNode(current_scope, node->children->head->next->lexicalTOKEN->lexeme);
	}



	/******** Now which entries have to be added to symbol Table ******/

	//Function Definition Encountered
	if(node->leaf == 1 && node->lexicalTOKEN->type==FUNID && node->parent->label == FUNCTIONDEF_NODE){

		//Look into current scope for same definition
		SymbolNode* info = getSymbolInfo(node->lexicalTOKEN->lexeme, node->parent->current_scope->table);

		//If  function overloading
		if(info!=NULL){
			char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
			sprintf(str,"Error: Function overloading for function '%s()'",node->lexicalTOKEN->lexeme);
			add_error(errors,str,node->lexicalTOKEN->line_no);
			//REPORT ERROR

		}
		else{
			*number = *number+1;
			Symbol* symbol = makeSymbol(node, *number);
			addSymbol(node->parent->current_scope->table,node->lexicalTOKEN->lexeme, symbol);
		}

		set_parent_function(node);
	}

	//Function Call Encountered
	else if(node->leaf == 1 && node->lexicalTOKEN->type==FUNID){

		//First set the static parent
		set_parent_function(node);
		
		SymbolNode* info = getSymbolInfo(node->lexicalTOKEN->lexeme, current_scope->table);

		int recursion = check_recursion(node);

		if(recursion==1){

			node->nodeType = ERROR;
			
			char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
			sprintf(str,"Error: Recursive Function Call '%s()' encountered",node->lexicalTOKEN->lexeme);
			add_error(errors,str,node->lexicalTOKEN->line_no);
		}

		//If the function definition is not there in the current scope
		else if(info==NULL){

			node->nodeType = ERROR;
			
			char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
			sprintf(str,"Error: Undefined Function '%s()' Called",node->lexicalTOKEN->lexeme);
			add_error(errors,str,node->lexicalTOKEN->line_no);			
			//REPORT ERROR

		}

	}

	//Variable declaration Encountered
	else if(node->leaf == 1 && (node->parent->label == DECLARATIONSTMT_NODE || node->parent->label == PARAMETER_NODE) && (node->lexicalTOKEN->type < 8 || node->lexicalTOKEN->type > 11)){
		
		//Look into current scope for same declaration
		SymbolNode* info = getSymbolInfo(node->lexicalTOKEN->lexeme, current_scope->table);		

		//If multiple declaration case
		if(info!=NULL){

			char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
			sprintf(str,"Error: Multiple Declaration for Variable '%s'",node->lexicalTOKEN->lexeme);
			add_error(errors,str,node->lexicalTOKEN->line_no);
			//REPORT Variable Declaration with a same name already done
		}

		else{
			*number = *number+1;
			Symbol* symbol = makeSymbol(node, *number);
			addSymbol(node->current_scope->table,node->lexicalTOKEN->lexeme, symbol);
		}
		
	}


	//Variable use Encountered
	else if(node->leaf == 1 && node->lexicalTOKEN->type==ID){

		//Look into the entire scope hierarchy for this variable declaration
		SymbolNode* info = findInfo(node);

		 //If the variable is declared earlier
		if(info!=NULL){
			node->nodeType = info->symbol->datatype;
		}
		else{
			node->nodeType = ERROR;

			char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
			sprintf(str,"Error: Undeclared Variable '%s' Used",node->lexicalTOKEN->lexeme);
			add_error(errors,str,node->lexicalTOKEN->line_no);
			//REPORT ERROR

		}
	}



	/*** Now go to the children ***/

	astChildren* children = node->children;
	ASTNode* temp;

	/*** Children in the new scope ***/
	if(children!=NULL){

		temp = children->head;

		while(temp!=NULL){
			traverseAST(temp,new_scope,errors,number);
			temp = temp->next;
		}
	}


	
	//If its a compresses concatenated node
	temp = node->concat;

	//Concatenated entities in the same scope
	traverseAST(temp,current_scope,errors,number);	

}

Symbol* makeSymbol(ASTNode* node, int number){

	Symbol* symbol = (Symbol*)malloc(sizeof(Symbol));
	symbol->lexicalTOKEN = node->lexicalTOKEN;
	symbol->width = 0;
	symbol->offset = 0;
	symbol->number = number;

	/**Pointer to corresponding AST code**/
	symbol->astNode = node;

	//For parameters and declarations
	if(symbol->lexicalTOKEN->type!=FUNID){
		symbol->datatype = node->parent->children->head->lexicalTOKEN->type;
	}
	else{
		symbol->datatype = FUNID;		
	}

	/**** Deciding Width if Symbol ****/

	if(symbol->datatype==INT){
		symbol->width = 2;
	}
	
	else if(symbol->datatype==STRING){
		symbol->width = 0;
	}
	
	else if(symbol->datatype==REAL){
		symbol->width = 4;
	}
	else if(symbol->datatype==MATRIX){
		symbol->width = 0;
	}

	symbol->assigned = 0;
	symbol->dimension1 = 0;
	symbol->dimension2 = 0;

	symbol->val = NULL;
	symbol->str = NULL;

	return symbol;

}

/**********************************************************************************************/

void printSymbolTableTree(SymTableTree* tree){
	printf("\n\n-------------------------------------Printing Symbol Table Tree--------------------------------------\n\n");

	printf("%-20s %-20s %-15s %-20s %-10s %-5s %s\n\n", "Identifier name","Scope","Nesting level","Static parent","Type","Width","Offset");

	printSymbolTableTree_util(tree->root, 1);

	printf("\n\n-----------------------------------------------------------------------------------------------------\n\n");
}

void printSymbolTableTree_util(SymTableTreeNode* node, int nested){

	/****** Printing the current table ******/

	printSymbolTable(node, nested);

	/****** Now go to the children ********/

	nestedScopes* children = node->children;

	SymTableTreeNode* temp;

	if(children!=NULL){
		temp = children->head;

		while(temp!=NULL){
			printSymbolTableTree_util(temp, nested+1);
			temp = temp->next;			
		}
		
	}
}

void printSymbol(Symbol* symbol, int nested, char* scope, char* static_parent){

	if(symbol->lexicalTOKEN->type==FUNID)
		return;

	char* null = "NULL";
	if(static_parent!=NULL){
		printf("%-20s %-20s %-15d %-20s %-10s %-5d %d\n", symbol->lexicalTOKEN->lexeme,scope,nested,static_parent,TerminalMap[symbol->datatype],symbol->width,symbol->offset);
	}
	else{
		printf("%-20s %-20s %-15d %-20s %-10s %-5d %d\n", symbol->lexicalTOKEN->lexeme,scope,nested,null,TerminalMap[symbol->datatype],symbol->width,symbol->offset);
	}	

}


void printSymbolTable(SymTableTreeNode* node, int nested){
	char* scope = node->scope;
	char* static_parent;

	if(node->parent!=NULL){
		static_parent = node->parent->scope;
	}
	else{
		static_parent=NULL;
	}

	SymbolTable* table = node->table;

	slotsList* symbolList = (slotsList*)malloc(sizeof(slotsList));
	symbolList->head = NULL;
	SymbolNode* temp;

	//Collecting symbols
	for(int i=0;i<SYMBOL_TABLE_SLOTS;i++){
		SymbolNode* n = table->slots[i]->head;

		while(n!=NULL){
			temp = (SymbolNode*)malloc(sizeof(SymbolNode));
			temp->symbol = n->symbol;

			temp->next = symbolList->head;
			symbolList->head = temp;

			n = n->next;
		}
	}

	sort_symbols(symbolList);

	temp = symbolList->head;

	while(temp!=NULL){
		printSymbol(temp->symbol, nested, scope, static_parent);
		temp = temp->next;
	}

	printf("\n\n");
}


void sort_symbols(slotsList* list){
	if(list->head==NULL){
		return;
	}
	sort_symbols_util(list->head,list->total_symbols);
}


void sort_symbols_util(SymbolNode* temp, int count){
	if(temp->next==NULL){
		return;
	}	
	sort_symbols_util(temp->next,count--);	
	
	// insert_in_sorted_order
	Symbol* symbol = temp->symbol;
	int number = temp->symbol->number;
	
	SymbolNode* pretemp = temp;
	temp = temp->next;
	while(temp!=NULL){

		if(temp->symbol->number > number){
			break;
		}		

		pretemp->symbol = temp->symbol;
				
		temp = temp->next;
		pretemp = pretemp->next;	
	}

	pretemp->symbol = symbol;
	
}


SymbolNode* findInfo(ASTNode* node){	
	SymbolNode* info = NULL;

	SymTableTreeNode* temp_scope = node->current_scope;

	while(temp_scope!=NULL){

		info = getSymbolInfo(node->lexicalTOKEN->lexeme, temp_scope->table);

		if(info!=NULL){
			break;
		}

		temp_scope = temp_scope->parent;
	}

	return info;
}

void set_parent_function(ASTNode* node){

	//Skip 1 parent
	ASTNode* temp = node->parent->parent;

	while(temp!=NULL){
		if(temp->label==FUNCTIONDEF_NODE || temp->label==MAINFUNCTION_NODE){
			break;
		}
		temp = temp->parent;
	}

	if(temp->label==FUNCTIONDEF_NODE){
		node->parent_function = temp->children->head->next;
	}

	//If the function call is inside the main function
	if(temp->label==MAINFUNCTION_NODE){
		node->parent_function = NULL;		
	}
}

int check_recursion(ASTNode* node){
		
	if(node->parent_function==NULL){
		return 0;
	}

	 if(strcmp(node->lexicalTOKEN->lexeme,node->parent_function->lexicalTOKEN->lexeme)==0){
	 	return 1;
	 }

	 return 0;	
}
