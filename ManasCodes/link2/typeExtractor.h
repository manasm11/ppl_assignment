/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#include <stdio.h>
#include <stdlib.h>

#include "typeExtractorDef.h"

// Function Declarations

void get_type_function_call(ASTNode* node, int* undeclared_variable, ErrorList* errors);
void get_type_sizeexp(ASTNode* node, int* undeclared_variable, ErrorList* errors);
void get_type_var_node(ASTNode* node, int* undeclared_variable, ErrorList* errors);
void get_type_arithmetic_exp(ASTNode* node, int* undeclared_variable, ErrorList* errors);
void get_type_boolean_exp(ASTNode* node, int* undeclared_variable, ErrorList* errors);
void get_dimensions_matrix(ASTNode* node, ErrorList* errors);