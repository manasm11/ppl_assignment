/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "optimizer.h"
#include "semanticAnalyzer.h"
#include "typeExtractor.h"
#include "symbolTable.h"
#include "ast.h"




void optimize_stmt(ASTNode* node){	
	//<stmt> ===> <assignmentStmt>  
	if(node->label == ASSIGNMENTSTMT1_NODE || node->label==ASSIGNMENTSTMT2_NODE){
		optimize_assignment_statement(node);
	}

	// <stmt> ===> <conditionalStmt>
	if(node->label==IF_NODE){

		optimize_conditional_statement(node);

	}
}


// <stmt> ===> <conditionalStmt>
void optimize_conditional_statement(ASTNode* node){

	optimize_boolean_exp(node->children->head);

	//Go to other statements inside if
	ASTNode* temp = node->children->head->next;

	while(temp!=NULL){

		optimize_stmt(temp);		
		temp = temp->concat;
	} 

	//Check for the else part
	temp = node->children->head->next->next;

	//If no else part
	if(temp==NULL){
		return;
	}

	temp = temp->children->head;

	//Traverse through the statements in else part
	while(temp!=NULL){

		optimize_stmt(temp);
		temp = temp->concat;
	}
}



//<stmt> ===> <assignmentStmt>  
void optimize_assignment_statement(ASTNode* node){

	ASTNode* lhs = node->children->head;
	ASTNode* rhs = node->children->head->next;

	int lhs_datatype;
	int rhs_datatype;

	SymbolNode* info;

	//<assignmentStmt> ===> <assignmentStmt1>
	if(node->label==ASSIGNMENTSTMT1_NODE){		

		//Get rhs datatype

		//<rightHandSide1> ===> <size_exp>
		
		if(rhs->label==SIZEEXP_NODE){
			optimize_sizeexp(rhs);			
		}

		//<rightHandSide1> ===> <arithmeticExpression>
		else{
			optimize_arithmetic_exp(rhs);			
		}
		
		rhs_datatype = rhs->nodeType;
		lhs_datatype = lhs->nodeType;

		/************************************************************************************/

		info = findInfo(lhs);
		

		//Check the string size
		if(lhs_datatype==STRING && rhs_datatype==STRING){
			//For Code Generation
			info->symbol->str = rhs->str;
			/*********************************************/
		
		}

		//Check the string size
		if(lhs_datatype==INT && rhs_datatype==INT){
			//For Code Generation
			info->symbol->val = rhs->val;
			/*********************************************/
		}

		//Check the matrix size
		if(lhs_datatype==MATRIX && rhs_datatype==MATRIX){

			//For Code Generation
			info->symbol->val = rhs->val;
			/*********************************************/
						
		}


	}

	/****************************************************************************************/

	//<assignmentStmt> ===> <assignmentStmt2>
	if(node->label == ASSIGNMENTSTMT2_NODE){

		ASTNode* temp = lhs;		

		//<rightHandSide2> ===> <size_exp>
		if(rhs->label!=FUNCTIONCALL_NODE){
			optimize_sizeexp(rhs);		
		}

		
		//For Code Generation
		if(rhs->label!=FUNCTIONCALL_NODE){
			temp = lhs;
			temp->val = (int*)malloc(sizeof(int));
			temp->val[0] = rhs->val[0];
			temp = temp->concat;
			temp->val = (int*)malloc(sizeof(int));
			temp->val[0] = rhs->val[1];
		}
		/*********************************************/

	}

}

void optimizer_util(ASTNode* node){
	if(node==NULL)
		return;
	
	//Go to all the statements
	astChildren* children = node->children;

	//Go to first statement of the function
	ASTNode* temp = children->head;

	while(temp!=NULL){

		optimize_stmt(temp);
		temp = temp->concat;
	}
}

void optimizer(AST* ast){

	//Running Semantic Analyzer
	optimizer_util(ast->root);	
}


void optimize_arithmetic_exp(ASTNode* node){


	astChildren* children;
	ASTNode* temp1, *temp2;
	Tokentype operator;

	if(node->label==OPERATOR_NODE){

		//Process Children First
		children = node->children;
		temp1 = children->head->next;
		temp2 = temp1->next;
		operator = children->head->lexicalTOKEN->type;

		optimize_arithmetic_exp(temp1);
		optimize_arithmetic_exp(temp2);

		
		if(temp1->nodeType==INT && temp2->nodeType==INT && (operator==PLUS || operator==MINUS || operator==MUL)){
			
			//For Code Generation
			node->val = (int*)malloc(sizeof(int));
			if(operator==PLUS)
				node->val[0] = temp1->val[0]+temp2->val[0];
			if(operator==MINUS)
				node->val[0] = temp1->val[0]-temp2->val[0];
			if(operator==MUL)
				node->val[0] = temp1->val[0]*temp2->val[0];			
			/*********************************************/

			return;
		}

		

		if(temp1->nodeType==STRING && temp2->nodeType==STRING && (operator==PLUS)){
			

			//For Code Generation
			node->str = (char*)malloc(sizeof(char)*(node->str_length+1));
			char* str1 = temp1->str;
			char* str2 = temp2->str;

			int q = 0;
			int z = 0;
			while(str1[q]!='\0'){
				node->str[z++] = str1[q];
				q++;
			}
			q=0;
			while(str2[q]!='\0'){
				node->str[z++] = str2[q];
				q++;
			}
			node->str[z] = '\0';
			/*********************************************/

			return;
		}

		if(temp1->nodeType==MATRIX && temp2->nodeType==MATRIX && (operator==PLUS || operator==MINUS)){
			
				node->dimension1 = temp1->dimension1;
				node->dimension2 = temp1->dimension2;

				//For Code Generation
				node->val = (int*)malloc(sizeof(int)*(node->dimension1)*(node->dimension2));
				
				int* matrix1 = temp1->val;
				int* matrix2 = temp2->val;
				
				int dimension1 = node->dimension1;
				int dimension2 = node->dimension2;

				
				for(int y=0;y<dimension1;y++){
					for(int z=0;z<dimension2;z++){
						
						if(operator==PLUS)
							node->val[y*dimension2+z] = matrix1[y*dimension2+z] + matrix2[y*dimension2+z];
						else
							node->val[y*dimension2+z] = matrix1[y*dimension2+z] - matrix2[y*dimension2+z];
					}
				}			

			return;
		}		

		return;
		
	}

	//Otherwise it's a variable node
	else{
		optimize_var_node(node);
	}
}


void optimize_boolean_exp(ASTNode* node){

	//<booleanExpression> ===> OP <booleanExpression> CL <logicalOp1> OP <booleanExpression> CL  
	if(node->label==BOOLEANEXPRESSION1_NODE){
		astChildren* children = node->children;
		ASTNode* temp1 = children->head;
		ASTNode* temp2 = children->head->next->next;

		optimize_boolean_exp(temp1);
		optimize_boolean_exp(temp2);

		if(temp1->nodeType==BOOLEAN && temp2->nodeType==BOOLEAN){
						
			//For Code Generation
			node->val = (int*)malloc(sizeof(int));
			int val1 = temp1->val[0];
			int val2 = temp2->val[0];
			int result=0;
			Tokentype operator = temp1->next->lexicalTOKEN->type;
			switch(operator){
				case AND:{
					if(val1==1 && val2==1)
						result = 1;
				}break;
				case OR:{
					if(val1==1 || val2==1)
						result = 1;
				}break;				
			}
			node->val[0] = result;
			/*********************************************/

		}
		
		return;
	}

	//<booleanExpression> ===> NOT OP <booleanExpression> CL
	if(node->label==BOOLEANEXPRESSION2_NODE){

		ASTNode* temp = node->children->head;

		optimize_boolean_exp(temp);

		if(temp->nodeType==BOOLEAN){
			
			//For Code Generation
			node->val = (int*)malloc(sizeof(int));
			if(temp->val[0]==1)
				node->val[0] = 0;
			else
				node->val[0] = 1;
			/*********************************************/
		}
		
		return;
	}

	//<booleanExpression> ===> <constrainedVars> <relationalOp> <constrainedVars>
	if(node->label==BOOLEANEXPRESSION3_NODE){

		astChildren* children = node->children;

		//2 constrained variables nodes
		ASTNode* var_node1 = children->head;
		ASTNode* var_node2 = children->head->next->next;

		//Get their types
		optimize_var_node(var_node1);
		optimize_var_node(var_node2);

		//Let it automatically evaluate to true because NASM can't handle real numbers
		if(var_node1->nodeType==REAL && var_node2->nodeType==REAL){
			node->val = (int*)malloc(sizeof(int));
			node->val[0] = 1;
		}
		
		//<constrainedVars> ===> NUM
		if(var_node1->nodeType==INT && var_node2->nodeType==INT){
			
			//For Code Generation
			node->val = (int*)malloc(sizeof(int));
			int val1 = var_node1->val[0];
			int val2 = var_node2->val[0];
			int result=0;
			Tokentype operator = var_node1->next->lexicalTOKEN->type;
			switch(operator){
				case LT:{
					if(val1<val2)
						result = 1;
				}break;
				case LE:{
					if(val1<=val2)
						result = 1;
				}break;
				case GT:{
					if(val1>val2)
						result = 1;
				}break;
				case GE:{
					if(val1>=val2)
						result = 1;
				}break;
				case EQ:{
					if(val1==val2)
						result = 1;
				}break;
				case NE:{
					if(val1!=val2)
						result = 1;
				}break;
			}
			node->val[0] = result;
			/*********************************************/

		}
		
	}
}

void optimize_var_node(ASTNode* node){

	astChildren* children = node->children;
	ASTNode* temp = children->head;

	//Leaf Node
	if(temp->leaf==1){
		
		//<var> ===> NUM 
		if(temp->lexicalTOKEN->type == NUM){
			
			//For Code Generation
			temp->val = (int*)malloc(sizeof(int));
			temp->val[0] = temp->lexicalTOKEN->value->num;
			node->val = temp->val;
			/*********************************************/
		}
		
		//<var> ===> STR
		else if(temp->lexicalTOKEN->type == STR){
			
			//For Code Generation
			temp->str = (char*)malloc(sizeof(char)*(node->str_length+1));
			strcpy(temp->str, temp->lexicalTOKEN->lexeme+1);
			temp->str[node->str_length] = '\0';
			node->str = temp->str;
			/*********************************************/
		}

		//<var> ===> ID <matrix_may_be>  
		else if(temp->lexicalTOKEN->type == ID){
				
				//Matrix Element
				if(temp->concat!=NULL){					

					//For Code Generation
					SymbolNode* info = findInfo(temp);
					int* matrix = info->symbol->val;
					int col_size = info->symbol->dimension2;
					int num1 = temp->concat->lexicalTOKEN->value->num-1;
					int num2 = temp->concat->concat->lexicalTOKEN->value->num-1;
					temp->val = (int*)malloc(sizeof(int));
					temp->val[0] = matrix[num1*col_size+num2];
					node->val = temp->val;
					/*********************************************/
				}

				//Only Id
				else{						
					
					SymbolNode* info = findInfo(temp);
					
					if(temp->nodeType==MATRIX){
						
						//For Code Generation
						temp->val = info->symbol->val;
						node->val = temp->val;
						/*********************************************/
					}

					if(temp->nodeType==STRING){
						
						//For Code Generation
						temp->str = info->symbol->str;
						node->str = temp->str;
						/*********************************************/
					}

					//For Code Generation
					if(temp->nodeType==INT){
						temp->val = info->symbol->val;	
						node->val = temp->val;					
					}
					/*********************************************/
				}				
		}
	}

	//Non Leaf Node
	else{

		//<var> ===> <matrix_actual>
		if(temp->label==ROW){
				//For Code Generation			
				temp->val = (int*)malloc((sizeof(int))*(node->dimension1)*(node->dimension2));				
				int z = 0;			

				ASTNode* temp2 = temp;
				while(temp2!=NULL){
					ASTNode* col = temp2->children->head;

					while(col!=NULL){

						temp->val[z++] = col->lexicalTOKEN->value->num;
						col = col->concat;
					}
					temp2 = temp2->concat;
				}

				node->val = temp->val;
			
			/*********************************************/
		}	

	}
			
}

void optimize_sizeexp(ASTNode* node){

	ASTNode* id = node->children->head;		

		SymbolNode* info = findInfo(id);
		
		//The type is INT
		if(id->nodeType==STRING){
			//For Code Generation
			node->val = (int*)malloc(sizeof(int));
			node->val[0] = info->symbol->width;
			/*********************************************/

		}

		//Else the type is INT x INT
		else if(id->nodeType==MATRIX){
			
			//For Code Generation
			node->val = (int*)malloc(sizeof(int)*2);
			node->val[0] = info->symbol->dimension1;
			node->val[1] = info->symbol->dimension2;
			/*********************************************/
		}	
}



