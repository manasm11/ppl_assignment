/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#include <stdio.h>
#include <stdlib.h>

#include "symbolTableDef.h"
#include "semanticAnalyzerDef.h"


// Function Declarations

SymbolTable* newSymbolTable(int no_slots);
void addSymbol(SymbolTable* symbolTable, char* key, Symbol* symbol);
int findHash(char* key, int no_slots);

/** Returns the symbol node if the symbol is present, otherwise return NULL**/
SymbolNode* getSymbolInfo(char* key, SymbolTable* lookUpTable);



SymTableTree* initialize_SymbolTableTree();
SymTableTreeNode* make_symTableTreeNode(SymTableTreeNode* parent, char* scope);
void add_to_nested_scopes(SymTableTreeNode* parent, SymTableTreeNode* node);
nestedScopes* initialize_nested_scopes();



SymTableTree* buildAllSymbolTables(AST* ast, ErrorList* errors);
void traverseAST(ASTNode* node, SymTableTreeNode* current_scope, ErrorList* errors, int* number);
Symbol* makeSymbol(ASTNode* node, int number);


void printSymbolTableTree(SymTableTree* tree);
void printSymbolTableTree_util(SymTableTreeNode* node, int nested);
void printSymbol(Symbol* symbol, int nested, char* scope, char* static_parent);
void printSymbolTable(SymTableTreeNode* node, int nested);
void sort_symbols(slotsList* list);
void sort_symbols_util(SymbolNode* temp, int count);


SymbolNode* findInfo(ASTNode* node);
void set_parent_function(ASTNode* node);
int check_recursion(ASTNode* node);