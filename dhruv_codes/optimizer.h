/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "astDef.h"


void optimize_stmt(ASTNode* node);
void optimize_conditional_statement(ASTNode* node);
void optimize_assignment_statement(ASTNode* node);
void optimizer_util(ASTNode* node);
void optimizer(AST* ast);
void optimize_arithmetic_exp(ASTNode* node);
void optimize_boolean_exp(ASTNode* node);
void optimize_var_node(ASTNode* node);
void optimize_sizeexp(ASTNode* node);

