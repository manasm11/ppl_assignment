/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "optimizer.h"
#include "intermediateCode.h"

extern char* TerminalMap[];
extern char* NonTerminalMap[];

int character_no,register_no,label_no;


Tuple* make_tuple(Tokentype op,	char* arg1,	char* arg2,	char* result, ASTNode* node1, ASTNode* node2, ASTNode* node3, int process){
	Tuple* tuple = (Tuple*)malloc(sizeof(Tuple));

	tuple->op = op;
	tuple->arg1 = arg1;
	tuple->arg2 = arg2;
	tuple->result = result;

	tuple->node1= node1;
	tuple->node2= node2;
	tuple->node3= node3;

	tuple->process = process;

	return tuple;
}

void add_tuple(TupleList* list, Tuple* tuple){
	
	if(list->head==NULL){
		list->head = tuple;
		list->last = tuple;
	}
	else{
		list->last->next = tuple;
		list->last = tuple;
	}

	list->total_tuples++;
}

TupleList* initialize_tuples(){
	TupleList* list = (TupleList*)malloc(sizeof(TupleList));
	list->head = NULL;
	list->last = NULL;
	list->total_tuples = 0;

	return list;
}


TupleList* generateIR(AST* tree){

	//First do the possible optimizations
	optimizer(tree);

	character_no = 0;
	register_no = 0;
	label_no = 0;

	TupleList* list = initialize_tuples();
	generateIR_util(tree->root,list);

	print_ir_code(list);

	return list;
}

void generateIR_util(ASTNode* node, TupleList* list){
	if(node==NULL)
		return;

	astChildren* children = node->children;
	ASTNode* temp;


	if(children!=NULL){
		temp = children->head;
		while(temp!=NULL){
			generateIR_util(temp,list);
			temp = temp->next;
		}
	}

	//Process the node
	checkNode(node,list);

	
	//If its a compresses concatenated node
	temp = node->concat;
	generateIR_util(temp,list); 
}

void checkNode(ASTNode* node, TupleList* list){

	if(node->label==VAR_NODE){
		process_var_node(node,list);
	}

	else if(node->label==CONSTRAINEDVARS_NODE){
		process_var_node(node,list);
	}

	else if(node->label==OPERATOR_NODE){
		process_arithmetic_expression(node,list);
	}

	//<booleanExpression> ===> OP <booleanExpression> CL <logicalOp1> OP <booleanExpression> CL
	else if(node->label==BOOLEANEXPRESSION1_NODE){
		process_boolean_expression(node,list,1);
	}
	//<booleanExpression> ===> NOT OP <booleanExpression> CL
	else if(node->label==BOOLEANEXPRESSION2_NODE){
		process_boolean_expression(node,list,2);
	}
	//<booleanExpression> ===> <constrainedVars> <relationalOp> <constrainedVars>
	else if(node->label==BOOLEANEXPRESSION3_NODE){
		process_boolean_expression(node,list,3);
	}

	//<stmt> ===> <ioStmt> 
	else if(node->label==READ_NODE || node->label==PRINT_NODE){
		process_io_statement(node,list);
	}

	//<stmt> ===> <assignmentStmt> 
	else if(node->label == ASSIGNMENTSTMT1_NODE || node->label==ASSIGNMENTSTMT2_NODE){
		process_assignment_statement(node, list);
	}

	//<stmt> ===> <conditionalStmt> 
	else if(node->label==IF_NODE){
		process_conditional_statement(node,list);
	}
	//<stmt> ===> <declarationStmt>  
	else if(node->label==DECLARATIONSTMT_NODE){
		node->tuple = NULL;
		//Do nothing
	}

}


void process_conditional_statement(ASTNode* node, TupleList* list){
	
	ASTNode* boolean_exp = node->children->head;
	char* boolean_name = boolean_exp->name;
	Tuple* boolean_tuple = boolean_exp->tuple;


	//Creating 6 tuples and 2 labels required	
	char* label1 = generate_label();
	char* label2 = generate_label();	
	Tuple* new_tuple1 = make_tuple(IF, boolean_name, NULL, NULL,boolean_exp,NULL,NULL,1);
	Tuple* new_tuple2 = make_tuple(GOTO, label1, NULL, NULL,NULL,NULL,NULL,0);
	Tuple* new_tuple3 = make_tuple(GOTO, label2, NULL, NULL,NULL,NULL,NULL,1);
	Tuple* new_tuple4 = make_tuple(LABEL, label1, NULL, NULL,NULL,NULL,NULL,1);
	Tuple* new_tuple5 = make_tuple(LABEL, label2, NULL, NULL,NULL,NULL,NULL,1);

	
	Tuple* temp = boolean_tuple->next;
	boolean_tuple->next = new_tuple1;
	new_tuple1->next = new_tuple2;
	new_tuple2->next = temp;

	//Else label has to be inserted after statements in the if block
	ASTNode* statement = boolean_exp->next;	

	//Assuming no statement in if block has a tuple
	temp = new_tuple2;

	while(statement!=NULL){
		if(statement->tuple!=NULL){
			temp = statement->tuple;
		}
		statement = statement->concat;
	}

	
	Tuple* temp2 = temp->next;
	temp->next = new_tuple3;
	new_tuple3->next = new_tuple4;
	new_tuple4->next = temp2;

	//After this, else part starts
	temp = new_tuple4;

	//If no else part
	if(boolean_exp->next->next==NULL){
		//Do nothing
	}
	
	//node->tuple has to point to the last tuple of the entire if block
	else{

		statement = boolean_exp->next->next->children->head;
		while(statement!=NULL){
			if(statement->tuple!=NULL){
				temp = statement->tuple;
			}
			statement = statement->concat;
		}

			
	}

	temp2 = temp->next;
	temp->next = new_tuple5;
	new_tuple5->next = temp2;

	node->tuple = new_tuple5;
	list->last = new_tuple5;
}



void process_var_node(ASTNode* node, TupleList* list){
	astChildren* children = node->children;
	ASTNode* temp = children->head;

	node->name = generate_name();
	Tuple* new_tuple = NULL;

	//Leaf Node
	if(temp->leaf==1){
		
		//<var> ===> NUM 
		if(temp->lexicalTOKEN->type == NUM){
			new_tuple = make_tuple(ASSIGNOP, temp->lexicalTOKEN->lexeme, NULL, node->name, temp, NULL, node,0);
		}

		if(temp->lexicalTOKEN->type == RNUM){
			new_tuple = make_tuple(ASSIGNOP, temp->lexicalTOKEN->lexeme, NULL, node->name, temp, NULL, node,0);
		}
		//<var> ===> STR
		else if(temp->lexicalTOKEN->type == STR){
			new_tuple = make_tuple(ASSIGNOP, temp->lexicalTOKEN->lexeme, NULL, node->name, temp, NULL, node,0);
		}

		//<var> ===> ID <matrix_may_be>  
		else if(temp->lexicalTOKEN->type == ID){
			if(temp->concat==NULL)
				new_tuple = make_tuple(ASSIGNOP, temp->lexicalTOKEN->lexeme, NULL, node->name, temp, NULL, node,0);
			else{

				char* str = (char*)malloc(sizeof(char)*10);
				sprintf(str,"%d",temp->val[0]);
				new_tuple = make_tuple(ASSIGNOP, str, NULL, node->name, temp, NULL, node,0);	
			}
		}
	}

	//Non Leaf Node
	else{

		//<var> ===> <matrix_actual>
		if(temp->label==ROW){
			char* matrix_str = generate_matrix_str(temp);
			new_tuple = make_tuple(ASSIGNOP, matrix_str, NULL, node->name, temp, NULL, node,0);
		}		

	}

	add_tuple(list, new_tuple);
	node->tuple = new_tuple;
}



void process_arithmetic_expression(ASTNode* node, TupleList* list){

	node->name = generate_name();

	Tuple* new_tuple = make_tuple(node->children->head->lexicalTOKEN->type, node->children->head->next->name, node->children->head->next->next->name, node->name,node->children->head->next,node->children->head->next->next, node,0);
	add_tuple(list,new_tuple);
	node->tuple = new_tuple;

}

void process_boolean_expression(ASTNode* node, TupleList* list, int num){
	
	node->name = generate_name();
	Tuple* new_tuple = NULL;

	switch(num){
		case 1:{
			new_tuple = make_tuple(node->children->head->next->lexicalTOKEN->type, node->children->head->name, node->children->head->next->next->name, node->name, node->children->head, node->children->head->next->next, node,0);

		}break;

		case 2:{
			new_tuple = make_tuple(NOT, node->children->head->name, NULL, node->name, node->children->head, NULL, node,0);

		}break;

		case 3:{
			new_tuple = make_tuple(node->children->head->next->lexicalTOKEN->type, node->children->head->name, node->children->head->next->next->name, node->name,node->children->head, node->children->head->next->next, node,0);
		}break;
	}	

	add_tuple(list, new_tuple);
	node->tuple = new_tuple;
}

void process_io_statement(ASTNode* node, TupleList* list){

	Tuple* new_tuple;

	if(node->label==READ_NODE){
		new_tuple = make_tuple(READ, node->children->head->lexicalTOKEN->lexeme ,NULL, NULL,node->children->head,NULL,NULL,0);
	}
	else if(node->label==PRINT_NODE){
		new_tuple = make_tuple(PRINT, node->children->head->lexicalTOKEN->lexeme ,NULL, NULL,node->children->head,NULL,NULL,1);
	}

	add_tuple(list, new_tuple);
	node->tuple = new_tuple;
}

void process_assignment_statement(ASTNode* node, TupleList* list){
	
	ASTNode* rhs = node->children->head->next;


	if(node->label==ASSIGNMENTSTMT1_NODE){		

		if(rhs->label!=SIZEEXP_NODE){

			node->name = node->children->head->lexicalTOKEN->lexeme;

			Tuple* new_tuple = make_tuple(ASSIGNOP, node->children->head->next->name ,NULL, node->name, rhs, NULL, node->children->head,1);
			add_tuple(list, new_tuple);
			node->tuple = new_tuple;
		}

		else if(rhs->label==SIZEEXP_NODE){
			
			printf("Hello\n");
			node->name = node->children->head->lexicalTOKEN->lexeme;

			char* str = (char*)malloc(sizeof(char)*10);
			sprintf(str,"%d",rhs->val[0]);

			Tuple* new_tuple = make_tuple(ASSIGNOP, str ,NULL, node->name, rhs, NULL, node->children->head,1);
			
			add_tuple(list, new_tuple);
			node->tuple = new_tuple;
		}
		
	}

	else if(node->label==ASSIGNMENTSTMT2_NODE){

		ASTNode* lhs = node->children->head;

		char* str1 = (char*)malloc(sizeof(char)*10);
		sprintf(str1,"%d",rhs->val[0]);
		Tuple* new_tuple = make_tuple(ASSIGNOP, str1 ,NULL, lhs->lexicalTOKEN->lexeme, rhs, NULL, lhs,1);
		add_tuple(list, new_tuple);
		node->tuple = new_tuple;

		lhs = lhs->concat;
		char* str2 = (char*)malloc(sizeof(char)*10);
		sprintf(str2,"%d",rhs->val[0]);
		new_tuple = make_tuple(ASSIGNOP, str1 ,NULL, lhs->lexicalTOKEN->lexeme, rhs, NULL, lhs,1);
		
		add_tuple(list, new_tuple);
		node->tuple = new_tuple;
	}
}

char* generate_name(){
	char* str = (char*)malloc(sizeof(char)*10);
	sprintf(str, "t%d",register_no);

	register_no++;

	return str;
}

char* generate_label(){
	char* str = (char*)malloc(sizeof(char)*10);
	sprintf(str, "L%d",label_no);
	label_no++;

	return str;
}

char* generate_matrix_str(ASTNode* temp){
	char* str = (char*)malloc(sizeof(char)*100);
	sprintf(str, "***");
	
	return str;
}

void print_ir_code(TupleList* list){

	printf("\n\n---------------------------Printing Intermediate Code Quadruples----------------------------\n\n");
	char* arg1 = "arg1";
	char* arg2 = "arg2";
	char* result = "result";
	char* op = "Op";
	printf("%-15s%-15s%-15s%-15s\n\n",op,arg1,arg2,result);

	Tuple* temp = list->head;
	char* null = "NULL";
	while(temp!=NULL){

		print_tuple(temp);
		temp = temp->next;
	}

	printf("\n\n-------------------------------Intermediate Code Printed-------------------------------\n\n");
}


void print_tuple(Tuple* temp){
	char* null = "NULL";
	printf("%-15s", TerminalMap[temp->op]);
		
	if(temp->arg1!=NULL)
		printf("%-15s",temp->arg1);
	else
		printf("%-15s",null);

	if(temp->arg2!=NULL)
		printf("%-15s",temp->arg2);
	else
		printf("%-15s",null);

	if(temp->result!=NULL)
		printf("%-15s",temp->result);
	else
		printf("%-15s",null);

	printf("\n");
}
