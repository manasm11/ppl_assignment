/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "intermediateCodeDef.h"

TupleList* generateIR(AST* tree);
void generateIR_util(ASTNode* node, TupleList* list);
void checkNode(ASTNode* node, TupleList* list);

void process_conditional_statement(ASTNode* node, TupleList* list);
void process_var_node(ASTNode* node, TupleList* list);
void process_arithmetic_expression(ASTNode* node, TupleList* list);
void process_boolean_expression(ASTNode* node, TupleList* list, int num);
void process_io_statement(ASTNode* node, TupleList* list);
void process_assignment_statement(ASTNode* node, TupleList* list);

Tuple* make_tuple(Tokentype op,	char* arg1,	char* arg2,	char* result, ASTNode* node1, ASTNode* node2, ASTNode* node3, int process);
void add_tuple(TupleList* list, Tuple* tuple);
TupleList* initialize_tuples();

char* generate_name();
char* generate_label();
char* generate_matrix_str(ASTNode* temp);

void print_ir_code(TupleList* list);
void print_tuple(Tuple* temp);