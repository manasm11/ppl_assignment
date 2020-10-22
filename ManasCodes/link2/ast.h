/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/

#include <stdio.h>
#include <stdlib.h>

#include "astDef.h"


//Function Declarations

AST* generateAST(ParseTree* tree);
AST* initialize_AST();
void add_ast_root(AST* tree, ASTNode* root);

void generateAST_UTIL(TreeNode* node);
void process_node(TreeNode* node);


ASTNode* makeNode(Labeltype label, int leaf, ASTNode* parent, astChildren* children, ASTNode* next, Token* lexicalTOKEN);
ASTNode* makeLeaf(Token* lexicalTOKEN);

astChildren* initialize_ast_children();
void add_to_children(astChildren* children, ASTNode* node);
void add_parent_pointers(ASTNode* parent, astChildren* children);


void printAST(AST* ast);
void printAST_util(ASTNode* node, int tabs);
void printASTNode(ASTNode* node, int tabs);

void l_attributes(TreeNode* node, TreeNode* prev_sibling);

TypeList* initialize_type_list();
void add_to_return_parameters(TypeList* list, Tokentype type);

void print_compression_ratio(ParseTree* tree1, AST* tree2);
void countNodesAST_util(ASTNode* node, int* num, int* size);
void countNodesParseTree_util(TreeNode* node, int* num, int* size);