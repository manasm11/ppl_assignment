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




void check_stmt(ASTNode* node, ErrorList* errors){

	//<stmt> ===> <ioStmt>
	if(node->label==PRINT_NODE|| node->label==READ_NODE){
		
		check_io_stmt(node,errors);		
	}

	//<stmt> ===> <funCallStmt>
	if(node->label==FUNCTIONCALL_NODE){
		
		check_function_call_statement(node,errors);
	}

	//<stmt> ===> <assignmentStmt>  
	if(node->label == ASSIGNMENTSTMT1_NODE || node->label==ASSIGNMENTSTMT2_NODE){

		check_assignment_statement(node,errors);
	}

	// <stmt> ===> <conditionalStmt>
	if(node->label==IF_NODE){

		check_conditional_statement(node,errors);

	}

}



//<stmt> ===> <assignmentStmt>  
void check_assignment_statement(ASTNode* node, ErrorList* errors){

	ASTNode* lhs = node->children->head;
	ASTNode* rhs = node->children->head->next;

	int lhs_datatype;
	int rhs_datatype;

	SymbolNode* info;

	//<assignmentStmt> ===> <assignmentStmt1>
	if(node->label==ASSIGNMENTSTMT1_NODE){

		//Find the datatype of lhs - ID
		if(lhs->nodeType==ERROR){
			lhs_datatype = ERROR;
		}
		else{
			//Mark the variable as assigned
			info = findInfo(lhs);			
			info->symbol->assigned = 1;
			//printf("%s\n",lhs->lexicalTOKEN->lexeme);
			lhs_datatype = lhs->nodeType;
		}

		/************************************************************************************/

		//Get rhs datatype
		
		//<rightHandSide1> ===> <size_exp>
		int undeclared_variable = 0;
		if(rhs->label==SIZEEXP_NODE){
			get_type_sizeexp(rhs,&undeclared_variable,errors);			
		}

		//<rightHandSide1> ===> <arithmeticExpression>
		else{
			get_type_arithmetic_exp(rhs,&undeclared_variable,errors);			
		}
		
		rhs_datatype = rhs->nodeType;

		/************************************************************************************/

		//If either of them is ERROR, then no need to match
		if(lhs_datatype==ERROR || rhs_datatype==ERROR){			

			if(lhs_datatype==ERROR){
				char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
				sprintf(str,"Error: Invalid LHS for Assignment Statement");
				add_error(errors,str,node->line_no);
				//REPORT ERROR - invalid lhs
			}
			if(rhs_datatype==ERROR){
				char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
				sprintf(str,"Error: Invalid RHS for Assignment Statement");
				add_error(errors,str,node->line_no);
				//REPORT ERROR - invalid rhs
			}
			
			return;			
		}

		//If both lhs and rhs are valid types, then check for match
		if(lhs_datatype!=rhs_datatype){

			char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
			sprintf(str,"Error: Type mismatch for assignment statement");
			add_error(errors,str,node->line_no);
			//REPORT ERROR - LHS mismatch RHS
		}

		//Check the string size
		if(lhs_datatype==STRING && rhs_datatype==STRING){
			if(rhs->str_length!=0)
				info->symbol->width = rhs->str_length;		
		}

		//Check the matrix size
		if(lhs_datatype==MATRIX && rhs_datatype==MATRIX){

			if(rhs->dimension1!=0 && rhs->dimension2!=0){
				info->symbol->width = rhs->dimension1*rhs->dimension2*2;
				info->symbol->dimension1 = rhs->dimension1;
				info->symbol->dimension2 = rhs->dimension2;
				
			}			
		}


	}

	/****************************************************************************************/

	//<assignmentStmt> ===> <assignmentStmt2>
	if(node->label == ASSIGNMENTSTMT2_NODE){

		ASTNode* temp = lhs;
		int undeclared_variable = 0;

		//<rightHandSide2> ===> <functionCall>
		if(rhs->label==FUNCTIONCALL_NODE){
			get_type_function_call(rhs, &undeclared_variable,errors);
		}

		//<rightHandSide2> ===> <size_exp>
		else{
			get_type_sizeexp(rhs,&undeclared_variable,errors);		
		}

		if(rhs->nodeType==ERROR){

				//REPORT ERROR - Undefined RHS call
				char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
				sprintf(str,"Error: Invalid RHS for the assignment statement");
				add_error(errors,str,node->line_no);

				return;
		}

		//Now match the type and number of parameters
		TypeNode* return_parameter = rhs->return_parameters->head;
		
		int x = 1;
		while(temp!=NULL && return_parameter!=NULL){

			//Mark the variable as assigned
			if(temp->nodeType!=ERROR){
				SymbolNode* info = findInfo(temp);
				info->symbol->assigned = 1;
			}

			lhs_datatype = temp->nodeType;
			rhs_datatype = return_parameter->type;

			if(lhs_datatype!=rhs_datatype){
				char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
				sprintf(str,"Error: Type Mismatch for Output parameter number %d",x);
				add_error(errors,str,node->line_no);
				//REPORT MISMATCH
			}


			temp = temp->concat;
			return_parameter = return_parameter->next;
			x++;
		}

		if(return_parameter!=NULL || temp!=NULL){
			char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
			sprintf(str,"Error: Number of receiving parameters for the function call is not equal to number of returned parameters");
			add_error(errors,str,node->line_no);
			//REPORT ERROR

			return;
		}
		

	}

}

// <stmt> ===> <conditionalStmt>
void check_conditional_statement(ASTNode* node, ErrorList* errors){

	int undeclared_variable = 0;

	get_type_boolean_exp(node->children->head, &undeclared_variable,errors);

	//Go to other statements inside if
	ASTNode* temp = node->children->head->next;

	while(temp!=NULL){

		if(temp->label==FUNCTIONDEF_NODE){
			semantic_analyzer_util(temp,errors);
		}
		else{
			check_stmt(temp,errors);
		}
		
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

		if(temp->label==FUNCTIONDEF_NODE){
			semantic_analyzer_util(temp,errors);
		}
		else{
			check_stmt(temp,errors);
		}
		
		temp = temp->concat;
	}
}


void check_io_stmt(ASTNode* node, ErrorList* errors){

	int undeclared_variable=0;

	ASTNode* id = node->children->head;


	//If the variable is undeclared
	if(id->nodeType==ERROR){
		node->nodeType = ERROR;
		undeclared_variable = 1;
	}

	//Otherwise get its datatype
	else{

		int datatype = id->nodeType;

		node->nodeType = datatype;

		//Print Statement
		if(node->label==PRINT_NODE){
			//Check whether the variable is initialized or not
			check_assigned(id,errors);		
		}

		//Read Statement
		else{

			if(datatype==STRING || datatype==MATRIX){
				node->nodeType = ERROR;

				char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
				sprintf(str,"Error: Read statement can only have int or real datatype as input");
				add_error(errors,str,node->line_no);
				//REPORT ERROR
			}
			else{
				//Check whether the variable is initialized or not
				SymbolNode* info = findInfo(id);			
				info->symbol->assigned = 1;

				//For Code generation
				if(datatype==INT){
					info->symbol->val = (int*)malloc(sizeof(int));
					info->symbol->val[0] = 0;
				}
				/*************************************/
				
			}
		}

	}	
}



void check_function_call_statement(ASTNode* node, ErrorList* errors){

	int undeclared_variable = 0;
	get_type_function_call(node,&undeclared_variable,errors);

	if(undeclared_variable==1){		
		return;
	}	

	//Check it has no output parameters	
	if(node->return_parameters!=NULL){

		char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
		sprintf(str,"Error: The function call has returned parameters but nothing to receive them");
		add_error(errors,str,node->line_no);
		//REPORT ERROR
	}
	
}



void check_input_parameters(ASTNode* node, ErrorList* errors){
	
	//Out parameters are already checked
	if(node->nodeType==ERROR){
		return;
	}

	//The FUNID Node
	ASTNode* temp = node->children->head;

	//Find the corresponding definition
	SymbolNode* info = getSymbolInfo(temp->lexicalTOKEN->lexeme, node->current_scope->table);

	//The funid corresponding to definition
	ASTNode* def_node = info->symbol->astNode;

	//Go to the first input parameter
	ASTNode* param = def_node->next;

	//Go to the first called parameter
	ASTNode* call_param = node->children->head->next;

	int undeclared_variable = 0;

	int num = 1;

	while(param!=NULL && call_param!=NULL){

		Tokentype datatype_def = param->children->head->lexicalTOKEN->type;

		get_type_var_node(call_param, &undeclared_variable,errors);

		//This can be ERROR also
		Tokentype datatype_call = call_param->nodeType;

		
		if(datatype_def!=datatype_call){

			char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
			sprintf(str,"Error: Type Mismatch for Input parameter number %d",num);
			add_error(errors,str,node->line_no);
			//REPORT MISMATCH
			
		} 

		//Go to next parameters
		call_param = call_param->concat;
		param = param->concat;
		num++;
	}

	if(call_param==NULL && param==NULL)
		return;

	if(param!=NULL || call_param!=NULL){

		char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
		sprintf(str,"Error: Number of formal parameters is not equal to number of actual input parameters for the function");
		add_error(errors,str,node->line_no);
		//REPORT ERROR		
	}


}


void semantic_analyzer_util(ASTNode* node, ErrorList* errors){
	if(node==NULL)
		return;

	
	//Go to all the statements
	astChildren* children = node->children;

	//Go to first statement of the function
	ASTNode* temp = children->head;

	if(node->label==FUNCTIONDEF_NODE){

		//First set the input parameters assigned to 1
		ASTNode* input_param = node->children->head->next->next;
		while(input_param!=NULL){
			SymbolNode* info = findInfo(input_param->children->head->next);
			info->symbol->assigned = 1;

			input_param = input_param->concat;			
		}

		//Now go to the statements
		temp = temp->next->next->next;
	}

	while(temp!=NULL){

		//If function definition inside function - recursive call
		if(temp->label==FUNCTIONDEF_NODE){
			semantic_analyzer_util(temp,errors);
		}
		//Otherwise check the correctness of the statement
		else{
			check_stmt(temp,errors);
		}

		temp = temp->concat;
	}
}

void semantic_analyzer(AST* ast, ErrorList* errors, SymTableTree* tree){

	//Running Semantic Analyzer
	semantic_analyzer_util(ast->root,errors);

	//Check for other errors
	check_other_errors(ast->root,errors);

	//Set the offsets in the symbol table
	set_offsets(tree->root);

	//Sorting errors by line number
	sort_errors(errors);
}

/*******************************************************************************************/



ErrorList* initialize_errors(){
	ErrorList* list = (ErrorList*)malloc(sizeof(ErrorList));
	list->total_errors = 0;
	list->head = NULL;

	return list;
}

void add_error(ErrorList* list, char* str, int line_no){
	
	Error* error = (Error*)malloc(sizeof(Error));
	error->line_no = line_no;
	error->str = str;

	error->next = list->head;
	list->head = error;

	list->total_errors++;
}

void sort_errors(ErrorList* list){
	if(list->head==NULL){
		return;
	}
	sort_errors_util(list->head,list->total_errors);
}


void sort_errors_util(Error* temp, int count){
	if(temp->next==NULL){
		return;
	}	
	sort_errors_util(temp->next,count--);	
	
	// insert_in_sorted_order
	int line_no = temp->line_no;
	char* str = temp->str;

	Error* pretemp = temp;
	temp = temp->next;
	while(temp!=NULL){

		if(temp->line_no > line_no){
			break;
		}		

		pretemp->line_no = temp->line_no;
		pretemp->str = temp->str;
		
		temp = temp->next;
		pretemp = pretemp->next;
	
	}

	pretemp->line_no = line_no;
	pretemp->str = str;
}

void print_errors(ErrorList* errors){
	Error* temp = errors->head;
	printf("\n\n----------------------------------Printing Semantic Errors-------------------------------\n\n");
	while(temp!=NULL){
		printf("Line No: %d, %s\n",temp->line_no,temp->str);
		temp=temp->next;
	}

	//printf("\n\n-------------------------------------All errors printed----------------------------------\n\n");
}


/*******************************************************************************************/

void check_other_errors(ASTNode* node, ErrorList* errors){
	if(node==NULL){
		return;
	}

	astChildren* children = node->children;
	
	if(children!=NULL){
		ASTNode* temp = children->head;

		while(temp!=NULL){
			check_other_errors(temp,errors);
			temp = temp->next;
		}
	}

	//Process the node
	if(node->label==FUNCTIONDEF_NODE){
		check_for_assigned_parameters(node,errors);
	}

	

	node = node->concat;
	check_other_errors(node,errors);

}


void check_for_assigned_parameters(ASTNode* node, ErrorList* errors){
	ASTNode* output_parameters = node->children->head;

	while(output_parameters!=NULL){
		SymbolNode* info = findInfo(output_parameters->children->head->next);

		if(info->symbol->assigned!=1){
			char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
			sprintf(str,"Error: Output parameter '%s' not assigned any value",output_parameters->children->head->next->lexicalTOKEN->lexeme);
			add_error(errors,str,output_parameters->line_no);
		}		

		/***********************************************************************************************************
		Tokentype datatype_def = info->symbol->datatype;

		//String or Matrix can't be input or output parameters for a function
		if(datatype_def==STRING || datatype_def==MATRIX){
			char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
			sprintf(str,"Error: String or Matrix can't be Input/Output Parameters of a Function for Code Generation");
			add_error(errors,str,node->line_no);
		}
		***********************************************************************************************************/
		output_parameters = output_parameters->concat;
	}

	/***********************************************************************************************************

	//Check for input parameters
	ASTNode* input_parameters = node->children->head->next->next;
	while(input_parameters!=NULL){
		SymbolNode* info = findInfo(input_parameters->children->head->next);
		Tokentype datatype_def = info->symbol->datatype;

		

		//String or Matrix can't be input or output parameters for a function
		if(datatype_def==STRING || datatype_def==MATRIX){
			char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
			sprintf(str,"Error: String or Matrix can't be Input/Output Parameters of a Function for Code Generation");
			add_error(errors,str,node->line_no);
		}
		
		input_parameters = input_parameters->concat;
	}
	***********************************************************************************************************/

}

void check_assigned(ASTNode* node, ErrorList* errors){
	SymbolNode* info = findInfo(node);

	//Info can't be NULL as only declared variables are sent here
	int assigned = info->symbol->assigned;

	if(assigned!=1){
		char* str = (char*)malloc(sizeof(str)*ERROR_STRING_SIZE);
		sprintf(str,"Error: Uninitialized Variable '%s' used on RHS of Assignment Statement",node->lexicalTOKEN->lexeme);
		add_error(errors,str,node->line_no);
	}
}





void set_offsets(SymTableTreeNode* node){

	/****** Printing the current table ******/

	set_offset(node);

	/****** Now go to the children ********/

	nestedScopes* children = node->children;

	SymTableTreeNode* temp;

	if(children!=NULL){
		temp = children->head;

		while(temp!=NULL){
			set_offsets(temp);
			temp = temp->next;			
		}
		
	}
}

void set_offset(SymTableTreeNode* node){
	char* scope = node->scope;
	char* static_parent;

	if(node->parent!=NULL){
		static_parent = node->parent->scope;
	}
	else{
		static_parent=NULL;
	}

	SymbolTable* table = node->table;

	slotsList* symbolList = (slotsList*)malloc(sizeof(slotsList));
	symbolList->head = NULL;
	SymbolNode* temp;

	//Collecting symbols
	for(int i=0;i<SYMBOL_TABLE_SLOTS;i++){
		SymbolNode* n = table->slots[i]->head;

		while(n!=NULL){
			temp = (SymbolNode*)malloc(sizeof(SymbolNode));
			temp->symbol = n->symbol;

			temp->next = symbolList->head;
			symbolList->head = temp;

			n = n->next;
		}
	}

	sort_symbols(symbolList);

	//Now set offsets
	temp = symbolList->head;
	int offset = 0;
	while(temp!=NULL){

		if(temp->symbol->lexicalTOKEN->type==FUNID){
			temp = temp->next;
			continue;
		}


		temp->symbol->offset = offset;
		if(temp->symbol->datatype!=MATRIX){
			offset+=temp->symbol->width;
		}
		else{
			offset+= (temp->symbol->dimension1*temp->symbol->dimension2)*2;
		}
		temp = temp->next;
	}
}

