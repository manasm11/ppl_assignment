/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "semanticAnalyzer.h"
#include "typeExtractor.h"
#include "symbolTable.h"
#include "ast.h"


/*** Extracts the type of an arithmetic expression ****/

void get_type_arithmetic_exp(ASTNode* node, int* undeclared_variable, ErrorList* errors){

	if(node==NULL){
		return;
	}

	astChildren* children;
	ASTNode* temp1, *temp2;
	Tokentype operator;

	if(node->label==OPERATOR_NODE){

		//Process Children First
		children = node->children;
		temp1 = children->head->next;
		temp2 = temp1->next;
		operator = children->head->lexicalTOKEN->type;

		get_type_arithmetic_exp(temp1, undeclared_variable, errors);
		get_type_arithmetic_exp(temp2, undeclared_variable, errors);

		if(temp1->nodeType==ERROR || temp2->nodeType==ERROR){
			node->nodeType = ERROR;
			return;
		}

		if(temp1->nodeType==INT && temp2->nodeType==INT && (operator==PLUS || operator==MINUS || operator==MUL)){
			node->nodeType = INT;

			return;
		}

		if(temp1->nodeType==REAL && temp2->nodeType==REAL && (operator==PLUS || operator==MINUS || operator==MUL)){
			node->nodeType = REAL;
			return;
		}

		if(temp1->nodeType==INT && temp2->nodeType==INT && (operator==DIV)){
			node->nodeType = REAL;
			return;
		}

		if(temp1->nodeType==REAL && temp2->nodeType==REAL && (operator==DIV)){
			node->nodeType = REAL;
			return;
		}

		if(temp1->nodeType==STRING && temp2->nodeType==STRING && (operator==PLUS)){
			node->str_length = temp1->str_length + temp2->str_length;
			node->nodeType = STRING;

			return;
		}

		if(temp1->nodeType==MATRIX && temp2->nodeType==MATRIX && (operator==PLUS || operator==MINUS)){
			node->nodeType = MATRIX;

			//Beginning of rows
			int check_size=1;
			
			//Check Compatibility of matrices
			if(temp1-> dimension1==0 || temp2->dimension1==0 || temp1->dimension2 ==0 || temp2->dimension2==0 || temp1->dimension1 != temp2->dimension1 || temp1->dimension2 != temp2->dimension2){
				check_size = 0;
			}

			if(check_size!=1){
				node->nodeType = ERROR;

				char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
				sprintf(str,"Error: Adding Matrices of incompatibe sizes");
				add_error(errors,str,node->line_no);				
				//REPORT ERROR
			}
			else{
				node->dimension1 = temp1->dimension1;
				node->dimension2 = temp1->dimension2;				
			}

			return;
		}


		node->nodeType = ERROR;
	
		char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
		sprintf(str,"Error: Invalid operand types for operator '%s'",node->children->head->lexicalTOKEN->lexeme);
		add_error(errors,str,node->line_no);
		//REPORT ERROR

		return;
		
	}

	//Otherwise it's a variable node
	else{
		get_type_var_node(node, undeclared_variable, errors);
	}
}


void get_type_boolean_exp(ASTNode* node, int* undeclared_variable, ErrorList* errors){


	//<booleanExpression> ===> OP <booleanExpression> CL <logicalOp1> OP <booleanExpression> CL  
	if(node->label==BOOLEANEXPRESSION1_NODE){
		astChildren* children = node->children;
		ASTNode* temp1 = children->head;
		ASTNode* temp2 = children->head->next->next;

		get_type_boolean_exp(temp1,undeclared_variable, errors);
		get_type_boolean_exp(temp2, undeclared_variable, errors);

		if(temp1->nodeType==ERROR || temp2->nodeType==ERROR){
			node->nodeType = ERROR;
		}

		
		else if(temp1->nodeType==BOOLEAN && temp2->nodeType==BOOLEAN){
			node->nodeType = BOOLEAN;
		}
		
		else{

			char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
			sprintf(str,"Error: Ivalid boolean expression as operand for '%s'",node->children->head->next->lexicalTOKEN->lexeme);
			add_error(errors,str,node->line_no);
			//REPORT ERROR
			
			node->nodeType = ERROR;
		}
		

		return;
	}

	//<booleanExpression> ===> NOT OP <booleanExpression> CL
	if(node->label==BOOLEANEXPRESSION2_NODE){

		ASTNode* temp = node->children->head;

		get_type_boolean_exp(temp,undeclared_variable, errors);

		
		if(temp->nodeType==BOOLEAN){
			node->nodeType = BOOLEAN;
		}
		else{
			node->nodeType = ERROR;
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
		get_type_var_node(var_node1,undeclared_variable, errors);
		get_type_var_node(var_node2,undeclared_variable, errors);


		if(var_node1->nodeType==ERROR || var_node2->nodeType==ERROR){
			node->nodeType = ERROR;
		}

		//<constrainedVars> ===> NUM
		else if(var_node1->nodeType==INT && var_node2->nodeType==INT){
			node->nodeType = BOOLEAN;		

		}

		//<constrainedVars> ===> RNUM
		else if(var_node1->nodeType==REAL && var_node2->nodeType==REAL){
			node->nodeType = BOOLEAN;
		}

		else{
			node->nodeType = ERROR;

			char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
			sprintf(str,"Error: Invalid operand types for relational operator '%s'",node->children->head->next->lexicalTOKEN->lexeme);
			add_error(errors,str,node->line_no);
			//REPORT ERROR
		}

		return;
	}
}


//Gives a type to a variable Node
void get_type_var_node(ASTNode* node, int* undeclared_variable, ErrorList* errors){

	astChildren* children = node->children;
	ASTNode* temp = children->head;

	//Leaf Node
	if(temp->leaf==1){
		
		//<var> ===> NUM 
		if(temp->lexicalTOKEN->type == NUM){
			node->nodeType = INT;
		}
		//<var> ===> RNUM
		else if(temp->lexicalTOKEN->type == RNUM){
			node->nodeType = REAL;
		}
		//<var> ===> STR
		else if(temp->lexicalTOKEN->type == STR){
			node->nodeType = STRING;
			node->str_length = strlen(temp->lexicalTOKEN->lexeme) - 2;			
		}

		//<var> ===> ID <matrix_may_be>  
		else if(temp->lexicalTOKEN->type == ID){

			//If undeclared variable
			if(temp->nodeType==ERROR){

				*undeclared_variable =1;
				node->nodeType = ERROR;
			}
			else{

				//Check whether the variable is initialized or not
				check_assigned(temp, errors);	

				//Matrix Element
				if(temp->concat!=NULL){
					node->nodeType = INT;			

				}

				//Only Id
				else{						
					node->nodeType = temp->nodeType;
					SymbolNode* info = findInfo(temp);
					
					if(temp->nodeType==MATRIX){
						node->dimension1 = info->symbol->dimension1;
						node->dimension2 = info->symbol->dimension2;

					}

					if(temp->nodeType==STRING){
						node->str_length = info->symbol->width;

					}			
				}
			}	
		}
	}

	//Non Leaf Node
	else{

		//<var> ===> <matrix_actual>
		if(temp->label==ROW){
			node->nodeType = MATRIX;

			get_dimensions_matrix(node, errors);

			if(node->dimension1==0 || node->dimension2==0){

				node->nodeType = ERROR;

				char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
				sprintf(str,"Error: Invalid Matrix");
				add_error(errors,str,node->line_no);
				//REPORT ERROR
			}
		}

		//<var> ===> <functionCall>
		else if(temp->label==FUNCTIONCALL_NODE){

			get_type_function_call(temp,undeclared_variable,errors);

			if(temp->nodeType==ERROR){
				//Undefined Function
				node->nodeType = ERROR;
			}
			else{
				//Only one parameter is returned
				if(temp->return_parameters!=NULL && temp->return_parameters->head->next==NULL){
					node->nodeType = temp->return_parameters->head->type;
				}
				else{

					char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
					sprintf(str,"Error: Inappropriate no. of return parameters");
					add_error(errors,str,node->line_no);
					//REPORT ERROR
					
					node->nodeType = ERROR; //This var node is error node
				}
				
			}		

		}

	}
			
}


//Set the return parameters of the function call
void get_type_function_call(ASTNode* node, int* undeclared_variable, ErrorList* errors){

	
	
	node->return_parameters = initialize_type_list();

	//The FUNID Node
	ASTNode* temp = node->children->head;

	//If the corresponding definition doesn't exist or recursive call
	if(temp->nodeType==ERROR){
		node->nodeType = ERROR;

		*undeclared_variable = 1;
		return;
	}

	//Find the corresponding definition
	SymbolNode* info = getSymbolInfo(temp->lexicalTOKEN->lexeme, node->current_scope->table);

	//The funid corresponding to definition
	ASTNode* def_node = info->symbol->astNode;

	//Go to the first output parameter
	temp = def_node->parent->children->head;

	while(temp!=NULL){

		Tokentype datatype = temp->children->head->lexicalTOKEN->type;

		add_to_return_parameters(node->return_parameters,datatype);

		temp = temp->concat;
	}

	check_input_parameters(node,errors);

	return;
}


void get_type_sizeexp(ASTNode* node, int* undeclared_variable, ErrorList* errors){

	ASTNode* id = node->children->head;

	//If the variable is undeclared
	if(id->nodeType==ERROR){
		node->nodeType = ERROR;
		*undeclared_variable = 1;
	}
	else{	

		//Check whether the variable is initialized or not
		check_assigned(id, errors);	

		SymbolNode* info = findInfo(id);
		
		//The type is INT
		if(id->nodeType==STRING){
			node->nodeType = INT;
		}

		//Else the type is INT x INT
		else if(id->nodeType==MATRIX){
			node->nodeType = -1;
			node->return_parameters = initialize_type_list();
			add_to_return_parameters(node->return_parameters,INT);
			add_to_return_parameters(node->return_parameters,INT);

		}

		else{

			char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
			sprintf(str,"Error: Size exp can only have string or matrix as input");
			add_error(errors,str,node->line_no);
			//REPORT ERROR
			
			node->nodeType = ERROR;
		}		

	}
}

void get_dimensions_matrix(ASTNode* node, ErrorList* errors){
	int i = 0;
	int j = 0;

	ASTNode* row = node->children->head;
	ASTNode* col;

	while(row!=NULL){
		i++;
		col = row->children->head;

		int q = 0;
		while(col!=NULL){
			q++;
			col = col->concat;
		}

		if(j==0)
			j = q;
		else{
			//Invalid Matrix
			if(j!=q){
				node->nodeType = ERROR;
				return;
			}
		}
		row = row->concat;
	}

	node->dimension1 = i;
	node->dimension2 = j;
}

