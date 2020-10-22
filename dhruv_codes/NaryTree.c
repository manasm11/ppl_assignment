/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "NaryTree.h"

ParseTree* initialize_tree(){
	ParseTree* tree = (ParseTree*)malloc(sizeof(ParseTree));
	tree->root = NULL;

	return tree;	
}

void add_root(ParseTree* tree){
	//Create starting nonterminal as the root of the tree
	tree->root = create_node(1,mainFunction,NULL);
	
}

void add_children(TreeNode* node, Rule* rule){

	node->rule_no = rule->rule_no;

	Children* children = initialize_children();

	TreeNode* temp = children->head;

	Symbol_list* symbols = rule->symbols;
	Symbol_node* temp2 = symbols->head;

	for(int k=0;k<symbols->length;k++){
		
		TreeNode* new_node;

		//Terminal
		if(temp2->term_or_nonterm == 0){
			new_node = create_node(temp2->term_or_nonterm, temp2->type.terminal, node);
		}
		//Non Terminal
		else{
			new_node = create_node(temp2->term_or_nonterm, temp2->type.nonterminal, node);
		}
		
		if(temp==NULL){
			temp = new_node;
			children->head = temp;
		}
		else{
			temp->next = new_node;
			temp = temp->next;
		}		
		
		//Go to the next symbol in the rule
		temp2 = temp2->next;

		//Increase the number of siblings
		children->no_siblings++;
	}

	//Assigning the children to the current node
	node->children = children;

}

Children* initialize_children(){
	Children* children = (Children*)malloc(sizeof(Children));
	children->head = NULL;
	children->no_siblings = 0;

	return children;
}


TreeNode* create_node(int term_or_nonterm,	int enum_int, TreeNode* parent){
	TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
	node->children = NULL;
	node->lexicalTOKEN = NULL;
	node->next = NULL;

	node->info = (TreeNodeInfo*)malloc(sizeof(TreeNodeInfo));

	node->info->term_or_nonterm = term_or_nonterm;


	//Storing which terminal or non-terminal the node represents
	Symbol_node_type type;
	
	//Terminal
	if(term_or_nonterm==0)
		type.terminal = (Tokentype)enum_int;
	
	//Non Terminal
	else
		type.nonterminal = (NonTerminal)enum_int;

	node->info->type = type;


	node->parent = parent;


	/***For AST**/

	node->addr = NULL;
	node->inh_addr = NULL;
	node->rule_no = 0;

	return node;
}







