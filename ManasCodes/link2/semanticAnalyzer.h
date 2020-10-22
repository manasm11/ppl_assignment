/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "semanticAnalyzerDef.h"

//Function Declarations

ErrorList* initialize_errors();
void add_error(ErrorList* list, char* str, int line_no);
void sort_errors(ErrorList* list);
void sort_errors_util(Error* temp, int count);
void print_errors(ErrorList* errors);



void check_stmt(ASTNode* node, ErrorList* errors);

void check_assignment_statement(ASTNode* node, ErrorList* errors);
void check_conditional_statement(ASTNode* node, ErrorList* errors);
void check_function_call_statement(ASTNode* node, ErrorList* errors);
void check_input_parameters(ASTNode* node, ErrorList* errors);
void check_io_stmt(ASTNode* node, ErrorList* errors);


void semantic_analyzer(AST* ast, ErrorList* errors, SymTableTree* tree);
void semantic_analyzer_util(ASTNode* node, ErrorList* errors);
void check_other_errors(ASTNode* node, ErrorList* errors);
void check_for_assigned_parameters(ASTNode* node, ErrorList* errors);
void check_assigned(ASTNode* node, ErrorList* errors);

void set_offsets(SymTableTreeNode* node);
void set_offset(SymTableTreeNode* node);

