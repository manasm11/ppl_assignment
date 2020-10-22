/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#include <stdio.h>
#include <stdlib.h>

#include "NaryTreeDef.h"


// Function Declarations
ParseTree* initialize_tree();
void add_root(ParseTree* tree);
void add_children(TreeNode* node, Rule* rule);
Children* initialize_children();
TreeNode* create_node(int term_or_non_term,	int enum_int, TreeNode* parent);






