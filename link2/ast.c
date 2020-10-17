/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "ast.h"
#include "parser.h"
#include "Stack.h"
#include "NaryTree.h"
#include "lookUpTable.h"
#include "lexer.h"



extern char* TerminalMap[];
extern char* NonTerminalMap[];

char* LabelMap[] = {
	"MAINFUNCTION_NODE",
	"FUNCTIONDEF_NODE",
	"PARAMETER_NODE",
	"DECLARATIONSTMT_NODE",
	"ASSIGNMENTSTMT1_NODE",
	"ASSIGNMENTSTMT2_NODE",
	"FUNCTIONCALL_NODE",
	"IF_NODE",
	"ELSE_NODE",
	"READ_NODE",
	"PRINT_NODE",
	"BOOLEANEXPRESSION1_NODE",
	"BOOLEANEXPRESSION2_NODE",
	"BOOLEANEXPRESSION3_NODE",
	"CONSTRAINEDVARS_NODE",
	"VAR_NODE",
	"SIZEEXP_NODE",
	"MATRIX_NODE",
	"OPERATOR_NODE",
	"ROW"};


AST* generateAST(ParseTree* tree){

	if(tree->root == NULL)
		return NULL;

	//Generate the AST
	generateAST_UTIL(tree->root);

	AST* ast = initialize_AST();
	add_ast_root(ast, tree->root->addr);

	return ast;

}

AST* initialize_AST(){
	AST* tree = (AST*)malloc(sizeof(AST));
	tree->root = NULL;	

	return tree;
}

void add_ast_root(AST* tree, ASTNode* root){
	//Create starting nonterminal as the root of the tree
	tree->root = root;	
}


/*************************************************************************************************************************************/

//Do a postorder traversal of the parse tree and generate the AST
void generateAST_UTIL(TreeNode* node){

	if(node==NULL){
		return;
	}

	Children* children = node->children;

	//If not a leaf node
	if(children != NULL){

		TreeNode* temp = children->head;			
		
		while(temp!=NULL){
			generateAST_UTIL(temp);
			temp = temp->next;
		}		
	}

	//After all children are done

	//printf("%d\n",node->rule_no);
	//Leaf Node
	if(node->rule_no==0){
		//printf("%d\n", node->info->term_or_nonterm);
		//printf("%s\n", TerminalMap[node->info->type.terminal]);
		return;
	}

	//printf("%s\n", NonTerminalMap[node->info->type.nonterminal]);
	process_node(node);	
	
}


void process_node(TreeNode* node){

	int rule_no = node->rule_no;

	switch(rule_no){

		/*********Pass the address upward******/
		case 3:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 22:
		case 23:
		case 28:
		case 29:
		case 30:
		case 31:
		case 49:{
			node->addr = node->children->head->addr;
		}break;

		/********Address is NULL********/
		case 4:
		case 20:
		case 34:
		case 37:
		case 39:
		case 68:
		case 79:
		case 83:
		case 86:
		case 88:{
			node->addr = NULL;
		}break;


		/**********Make Leaf*********/
		case 15:
		case 16:
		case 17:
		case 18:
		case 26:
		case 50:
		case 51:
		case 52:
		case 53:
		case 69:
		case 70:
		case 71:
		case 72:
		case 73:
		case 74:
		case 75:
		case 76:
		{
			node->addr = makeLeaf(node->children->head->lexicalTOKEN);
		}break;


		/***** 50 rules done*****/

		case 1:{
			astChildren* children = initialize_ast_children(); 

			add_to_children(children, node->children->head->next->next->next->addr);

			node->addr = makeNode(MAINFUNCTION_NODE,0,NULL,children,NULL,NULL); 

			add_parent_pointers(node->addr,children);
		}break; 

		case 13:{
			astChildren* children = initialize_ast_children(); 

			add_to_children(children, node->children->head->next->next->addr);
			add_to_children(children, makeLeaf(node->children->head->next->next->next->next->next->lexicalTOKEN));
			add_to_children(children, node->children->head->next->next->next->next->next->next->next->addr);
			add_to_children(children, node->children->head->next->next->next->next->next->next->next->next->next->addr);

			node->addr = makeNode(FUNCTIONDEF_NODE,0,NULL,children,NULL,NULL); 

			add_parent_pointers(node->addr,children);
		}break;

		case 21:{
			astChildren* children = initialize_ast_children(); 

			add_to_children(children, node->children->head->addr);
			add_to_children(children, node->children->head->next->addr);

			node->addr = makeNode(DECLARATIONSTMT_NODE,0,NULL,children,NULL,NULL); 

			add_parent_pointers(node->addr,children);
		}break; 

		case 24:{
			astChildren* children = initialize_ast_children(); 

			add_to_children(children, node->children->head->addr);
			add_to_children(children, node->children->head->next->next->addr);

			node->addr = makeNode(ASSIGNMENTSTMT1_NODE,0,NULL,children,NULL,NULL); 

			add_parent_pointers(node->addr,children);
		}break; 

		case 25:{
			astChildren* children = initialize_ast_children(); 

			add_to_children(children, node->children->head->addr);
			add_to_children(children, node->children->head->next->next->addr);

			node->addr = makeNode(ASSIGNMENTSTMT2_NODE,0,NULL,children,NULL,NULL); 

			add_parent_pointers(node->addr,children);
		}break; 

		case 32:{
			astChildren* children = initialize_ast_children(); 

			add_to_children(children, makeLeaf(node->children->head->lexicalTOKEN));
			add_to_children(children, node->children->head->next->next->addr);

			node->addr = makeNode(FUNCTIONCALL_NODE,0,NULL,children,NULL,NULL); 

			add_parent_pointers(node->addr,children);
		}break; 

		case 40:{
			astChildren* children = initialize_ast_children();

			add_to_children(children, makeLeaf(node->children->head->next->next->lexicalTOKEN));

			node->addr = makeNode(READ_NODE,0,NULL,children,NULL,NULL); 

			add_parent_pointers(node->addr,children);
		}break;

		case 41:{
			astChildren* children = initialize_ast_children();
			
			add_to_children(children, makeLeaf(node->children->head->next->next->lexicalTOKEN));
			
			node->addr = makeNode(PRINT_NODE,0,NULL,children,NULL,NULL); 

			add_parent_pointers(node->addr,children);
			
		}break;


		case 54:{
			astChildren* children = initialize_ast_children(); 

			add_to_children(children, node->children->head->next->addr);
			add_to_children(children, node->children->head->next->next->next->addr);
			add_to_children(children, node->children->head->next->next->next->next->next->addr);

			node->addr = makeNode(BOOLEANEXPRESSION1_NODE,0,NULL,children,NULL,NULL); 

			add_parent_pointers(node->addr,children);
		}break; 

		case 55:{
			astChildren* children = initialize_ast_children(); 

			add_to_children(children, node->children->head->next->next->addr);

			node->addr = makeNode(BOOLEANEXPRESSION2_NODE,0,NULL,children,NULL,NULL); 

			add_parent_pointers(node->addr,children);
		}break; 

		case 56:{
			astChildren* children = initialize_ast_children(); 

			add_to_children(children, node->children->head->addr);
			add_to_children(children, node->children->head->next->addr);
			add_to_children(children, node->children->head->next->next->addr);

			node->addr = makeNode(BOOLEANEXPRESSION3_NODE,0,NULL,children,NULL,NULL); 

			add_parent_pointers(node->addr,children);
		}break; 

		case 57:{
			astChildren* children = initialize_ast_children(); 

			add_to_children(children, makeLeaf(node->children->head->lexicalTOKEN));

			node->addr = makeNode(CONSTRAINEDVARS_NODE,0,NULL,children,NULL,NULL);

			node->addr->children->head->concat = node->children->head->next->addr;  

			add_parent_pointers(node->addr,children);
		}break; 


		case 60:{
			astChildren* children = initialize_ast_children(); 

			add_to_children(children, makeLeaf(node->children->head->lexicalTOKEN));
			
			node->addr = makeNode(VAR_NODE,0,NULL,children,NULL,NULL);

			node->addr->children->head->concat = node->children->head->next->addr; 

			add_parent_pointers(node->addr,children);
		}break;

		case 61:
		case 62:
		case 63:{
			astChildren* children = initialize_ast_children(); 

			add_to_children(children, makeLeaf(node->children->head->lexicalTOKEN));
			
			node->addr = makeNode(VAR_NODE,0,NULL,children,NULL,NULL); 

			add_parent_pointers(node->addr,children);
		}break; 

		case 58:
		case 59:{
			astChildren* children = initialize_ast_children(); 

			add_to_children(children, makeLeaf(node->children->head->lexicalTOKEN));
			
			node->addr = makeNode(CONSTRAINEDVARS_NODE,0,NULL,children,NULL,NULL); 

			add_parent_pointers(node->addr,children);
		}break; 

		case 64:
		case 65:{
			astChildren* children = initialize_ast_children(); 

			add_to_children(children, node->children->head->addr);
			
			node->addr = makeNode(VAR_NODE,0,NULL,children,NULL,NULL); 

			add_parent_pointers(node->addr,children);
		}break;

		case 66:{
			astChildren* children = initialize_ast_children(); 

			add_to_children(children, makeLeaf(node->children->head->next->lexicalTOKEN));
	
			node->addr = makeNode(SIZEEXP_NODE,0,NULL,children,NULL,NULL); 

			add_parent_pointers(node->addr,children);
		}break; 

		case 67:{
			
			ASTNode* leaf1 = makeLeaf(node->children->head->next->lexicalTOKEN);
			ASTNode* leaf2 = makeLeaf(node->children->head->next->next->next->lexicalTOKEN);
			
			leaf1->concat = leaf2;
			node->addr = leaf1; 
			
		}break; 

		/******* 65 rules done *****/




		case 2:
		case 33:
		case 38:		
		{

			node->children->head->addr->concat = node->children->head->next->addr;
			node->addr = node->children->head->addr;

		}break;

		/******* 68 rules done *****/

		case 14:{

			astChildren* children = initialize_ast_children(); 

			add_to_children(children, node->children->head->addr);
			add_to_children(children, makeLeaf(node->children->head->next->lexicalTOKEN));
			
			node->addr = makeNode(PARAMETER_NODE,0,NULL,children,NULL,NULL); 

			add_parent_pointers(node->addr,children);

			node->addr->concat = node->children->head->next->next->addr;

		}break;


		case 35:{

			astChildren* children = initialize_ast_children(); 

			add_to_children(children, node->children->head->next->next->addr);
			add_to_children(children, node->children->head->next->next->next->next->addr);
			add_to_children(children, node->children->head->next->next->next->next->next->next->addr);

			node->addr = makeNode(IF_NODE,0,NULL,children,NULL,NULL); 

			TreeNode* x = node->children->head->next->next->next->next;

			//Concatenation
			x->addr->concat = x->next->addr;
			
			

			add_parent_pointers(node->addr,children);
		}break;

		case 36:{

			astChildren* children = initialize_ast_children(); 

			add_to_children(children, node->children->head->next->addr);

			node->addr = makeNode(ELSE_NODE,0,NULL,children,NULL,NULL);
			
			TreeNode* x = node->children->head->next;

			//Concatenation
			x->addr->concat = x->next->addr;		
			 

			add_parent_pointers(node->addr,children);
		}break;
		
		case 81:{

			astChildren* children = initialize_ast_children(); 

			add_to_children(children, node->children->head->addr);
			
			node->addr = makeNode(ROW,0,NULL,children,NULL,NULL); 

			add_parent_pointers(node->addr,children);

			node->addr->concat = node->children->head->next->addr;

		}break;





		/******** 72 rules done *********/

		case 19:
		case 27:
		case 48:
		case 78:
		case 80:
		case 82:
		case 85:
		case 87:{

			node->addr = node->children->head->next->addr;

		}break;

		/********** 80 rules done *******/

		case 77:
		case 84:{

			node->addr = makeLeaf(node->children->head->lexicalTOKEN);

			TreeNode* x = node;
			x->addr->concat = x->children->head->next->addr;

		}break;
 
		/********* 82 rules done *******/





		/***************** Now 6 (42-47) rules for arithmetic expressions *******************/

		case 43:
		case 44:
		case 46:
		case 47:{
			node->addr = NULL;
		}break;

		//Arithmetic Expression Node encountered
		case 42:
		case 45:{
			l_attributes(node,NULL);
			
		}break;


	}
}


void l_attributes(TreeNode* node, TreeNode* prev_sibling){

	if(node==NULL){
		return;
	}

	TreeNode* temp, *prev_temp;
	Children* children;

	int rule_no = node->rule_no;

	switch(rule_no){

		case 42:
		case 45:{

			/** Go to chilren ***/

			children = node->children;
			temp = children->head;
			prev_temp = NULL;

			while(temp!=NULL){

				l_attributes(temp,prev_temp);
				
				prev_temp = temp;
				temp = temp->next;
			}

			/***** At the end *****/


			node->addr = node->children->head->next->addr;

		}break;

		case 43:
		case 46:{

			/*** Process Itself ***/

			if(node->parent->info->type.nonterminal==arithmeticExpression || node->parent->info->type.nonterminal==arithmeticExpression1){
				node->inh_addr = prev_sibling->addr;
			}

			else{

				astChildren* ASTCHILDREN = initialize_ast_children();

				
				add_to_children(ASTCHILDREN, node->parent->children->head->addr);
				add_to_children(ASTCHILDREN, node->parent->inh_addr);
				add_to_children(ASTCHILDREN, prev_sibling->addr);

				//printf("%s\n",NonTerminalMap[node->parent->info->type.nonterminal]);

				node->inh_addr = makeNode(OPERATOR_NODE,0,NULL,ASTCHILDREN,NULL,NULL);

				/**

				if(node->parent->info->type.nonterminal==(NonTerminal)x){

					node->inh_addr = makeNode(OPERATOR1_NODE,0,NULL,ASTCHILDREN,NULL,NULL);
				}
				else{
					node->inh_addr = makeNode(OPERATOR2_NODE,0,NULL,ASTCHILDREN,NULL,NULL);
				}

				**/

				add_parent_pointers(node->inh_addr,ASTCHILDREN);

			}
			

			/** Go to chilren ***/

			children = node->children;
			temp = children->head;
			prev_temp = NULL;

			while(temp!=NULL){

				l_attributes(temp,prev_temp);
				
				prev_temp = temp;
				temp = temp->next;
			}


			/***** At the end *****/
			node->addr = node->children->head->next->next->addr;

		}break;


		case 44:
		case 47:{

			/*** Process it's inh_addr**/

			if(node->parent->info->type.nonterminal== arithmeticExpression || node->parent->info->type.nonterminal==arithmeticExpression1){
				node->inh_addr = prev_sibling->addr;
			}

			else{

				astChildren* ASTCHILDREN = initialize_ast_children();

				
				add_to_children(ASTCHILDREN, node->parent->children->head->addr);
				add_to_children(ASTCHILDREN, node->parent->inh_addr);
				add_to_children(ASTCHILDREN, prev_sibling->addr);

				//printf("%s\n",NonTerminalMap[node->parent->info->type.nonterminal]);

				node->inh_addr = makeNode(OPERATOR_NODE,0,NULL,ASTCHILDREN,NULL,NULL);

				/**

				if(node->parent->info->type.nonterminal == (NonTerminal)x){
					node->inh_addr = makeNode(OPERATOR1_NODE,0,NULL,ASTCHILDREN,NULL,NULL);
				}
				else{
					node->inh_addr = makeNode(OPERATOR2_NODE,0,NULL,ASTCHILDREN,NULL,NULL);
				}

				**/

				add_parent_pointers(node->inh_addr,ASTCHILDREN);

			}

			/***Then make it's inh_addr as its addr **/

			node-> addr = node->inh_addr;

		}break;

		default:{
			
		}break;

	}

}


/**Make a Leaf Node**/
ASTNode* makeLeaf(Token* lexicalTOKEN){
	return makeNode(-1,1,(ASTNode*)NULL,(astChildren*)NULL,(ASTNode*)NULL,lexicalTOKEN);
}


/**Make a new AST Node**/
ASTNode* makeNode(Labeltype label, int leaf, ASTNode* parent, astChildren* children, ASTNode* next, Token* lexicalTOKEN){
	
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));

	node->label = label;
	node->leaf = leaf;
	node->parent = parent;
	node->children = children;
	node->next = next;
	node->lexicalTOKEN = lexicalTOKEN;

	node->concat = NULL;

	node->current_scope = NULL;

	node->nodeType = -1;

	node->return_parameters = NULL;

	if(node->leaf==1){
		node->line_no = node->lexicalTOKEN->line_no;
	}

	node->parent_function = NULL;

	node->str_length = 0;
	node->dimension1 = 0;
	node->dimension2 = 0;

	node->name = NULL;
	node->tuple = NULL;

	node->val = NULL;
	node->str = NULL;

	return node;
}


astChildren* initialize_ast_children(){
	astChildren* children = (astChildren*)malloc(sizeof(astChildren));
	children->head = NULL;
	children->no_siblings = 0;
	children->last = NULL;

	return children;
}

void add_to_children(astChildren* children, ASTNode* node){
	if(children->head==NULL){
		children->head = node;
		children->last = node;
	}
	else{
		children->last->next = node;
		children->last = children->last->next;
	}

	children->no_siblings++;
}

void add_parent_pointers(ASTNode* parent, astChildren* children){
	ASTNode* temp = children->head;
	ASTNode* temp2;

	while(temp!=NULL){
		temp->parent = parent;

		temp2 = temp->concat;
		while(temp2!=NULL){
			temp2->parent = parent;
			temp2 = temp2->concat;
		}

		temp = temp->next;
	}

	parent->line_no = children->head->line_no;
}


/************************* Printing Utilities ************************/

void printAST(AST* ast){
	printf("%-25s %-25s %-10s %s\n\n\n", "NODE","PARENT","DEPTH","LEAF");
	printAST_util(ast->root,1);
}

void printAST_util(ASTNode* node, int tabs){

	if(node==NULL)
		return;

	printASTNode(node,tabs);

	astChildren* children = node->children;

	ASTNode* temp;

	if(children!=NULL){

		temp = children->head;

		while(temp!=NULL){
			printAST_util(temp,tabs+1);
			temp = temp->next;
		}
	}

	
	//If its a compresses concatenated node
	temp = node->concat;

	printAST_util(temp,tabs);

}

void printASTNode(ASTNode* node, int tabs){
	char* parent;
	if(node->parent==NULL){
		parent = "NULL";
	}
	else{
		parent = LabelMap[node->parent->label];
	}
	if(node->leaf==1){
		printf("%-25s %-25s %-10d %s\n",TerminalMap[node->lexicalTOKEN->type],parent,tabs,"yes");		
	}
	else{
		printf("%-25s %-25s %-10d %s\n",LabelMap[node->label],parent,tabs,"no");		
	}
		
}

/*********************************************************************************/


TypeList* initialize_type_list(){
	TypeList* list = (TypeList*)malloc(sizeof(TypeList));
	list->head = NULL;
	list->no_siblings = 0;
	
	return list;
}

void add_to_return_parameters(TypeList* list, Tokentype type){

	TypeNode* node = (TypeNode*)malloc(sizeof(TypeNode));
	node->type = type;
	node->next = NULL;

	if(list->head==NULL){
		list->head = node;
		list->last = node;
	}
	else{
		list->last->next = node;
		list->last = list->last->next;
	}

	list->no_siblings++;
}

void print_compression_ratio(ParseTree* tree1, AST* tree2){
	
	int parsetree_nodes = 0;
	int parsetree_size = 0;
	countNodesParseTree_util(tree1->root, &parsetree_nodes, &parsetree_size);

	int AST_nodes = 0;
	int AST_size = 0;
	countNodesAST_util(tree2->root, &AST_nodes, &AST_size);

	printf("\n\n-----------------------------------------------------------------------");
	printf("\n\nParse tree Number of nodes = %d\nAllocated Memory for Parse Tree = %d bytes\n\n",parsetree_nodes,parsetree_size);
	printf("AST Number of nodes = %d\nAllocated Memory for AST = %d bytes\n\n", AST_nodes, AST_size);

	float compressesion_ratio = ((parsetree_size - AST_size)/(float)parsetree_size)*(float)100;

	printf("Compression ratio: %f", compressesion_ratio);

	printf("\n\n-----------------------------------------------------------------------\n\n\n");

}



void countNodesAST_util(ASTNode* node, int* num, int* size){

	if(node==NULL)
		return;

	//Count the node
	*num = *num+1;
	*size = *size + sizeof(ASTNode) - (2*sizeof(char*) + 2*sizeof(int) + sizeof(int*)); //Excluding fields used for Code Generation
	
	astChildren* children = node->children;
	ASTNode* temp;
	if(children!=NULL){
		temp = children->head;
		while(temp!=NULL){
			countNodesAST_util(temp,num,size);
			temp = temp->next;
		}
	}

	
	//If its a compresses concatenated node
	temp = node->concat;
	countNodesAST_util(temp,num,size);

}


void countNodesParseTree_util(TreeNode* node, int* num, int* size){

	if(node==NULL){
		return;
	}

	//Count the node
	*num = *num+1;
	*size = *size + sizeof(TreeNode);

	//Then go to the children
	Children* children = node->children;

	//If not a leaf node
	if(children != NULL){
		TreeNode* temp = children->head;		
		while(temp!=NULL){
			countNodesParseTree_util(temp, num,size);
			temp = temp->next;
		}
		
	}	
}