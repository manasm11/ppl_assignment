/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


/*************************************************************************************************************************

1. The entire file consists of 4 parts:

	a. Loading Grammar from the text file
	b. Computing First and Follow sets
	c. Comuting Parse Table
	d. Parsing the source code and computing the parse tree.

2. In every part, utility functions are provided to print the results like:

	a. Printing the loaded grammar from the data structure.
	b. Printing the first and follow sets
	c. Printing the Parse Table --- where 'O' represents a filled cell
	d. Printing the top of the stack during parsing
	e. Printing the parse tree
	

**************************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "parser.h"
#include "Stack.h"
#include "NaryTree.h"
#include "lookUpTable.h"
#include "lexer.h"




//Mappings of ENUM to String
char* TerminalMap[] = {
	"ASSIGNOP","COMMENT","FUNID","ID","NUM","RNUM","STR","END","INT","REAL","STRING","MATRIX","MAIN","SQO","SQC","OP","CL","SEMICOLON","COMMA","IF","ELSE","ENDIF","READ","PRINT","FUNCTION","PLUS","MINUS","MUL","DIV","SIZE","AND","OR","NOT","LT","LE","EQ","GT","GE","NE","e","$","ERROR", "BOOLEAN", "LABEL", "GOTO"
};

char* NonTerminalMap[] = {
	"mainFunction","stmtsAndFunctionDefs","more_stmtsAndFunctionDefs","stmtOrFunctionDef","stmt","funCallStmt","functionDef","parameter_list","type","remainingList","declarationStmt","assignmentStmt","assignmentStmt1","assignmentStmt2","leftHandSide1","leftHandSide2","rightHandSide1","rightHandSide2","functionCall","inputParameterList","conditionalStmt","add_else","otherStmts","ioStmt","arithmeticExpression","x","arithmeticExpression1","y","arithmeticExpression2","operator1","operator2","booleanExpression","constrainedVars","var","size_exp","matrix_may_be","logicalOp1","relationalOp","var_list","more_ids","matrix_actual","rows","more_rows","row","more_nums","listVar"
};




//PART 1

/*************************************************************************************************************************/

//Functions to load grammar from text file into the data structure




//Loads grammar from text file into the corresponding data structure -- assuming the grammar is writtern in the mentioned format
Grammar* load_grammar_from_text_file(char* grammar_text_file){

	int next_rule_no = 1;

	FILE* input = fopen(grammar_text_file, "r");

	if(input==NULL){
		fprintf(stderr, "Error Opening Grammar File\n");
		return NULL;
	}

	int end_of_file = 0;

	Grammar* grm = initialize_grammar();

	char ch;

	while(1){

		if(end_of_file==1)
			return grm;

		//Waiting for LHS of a rule
		while((ch = fgetc(input))!='<'){
			//If no new rule is available
			if(ch==EOF){
				return grm;
			}
		}
		
		//Load a new rule

		char* non_term_str = (char*)malloc(sizeof(char)*MAX_SYMBOL_SIZE);
		int i = 0;
		while((ch = fgetc(input))!='>'){
			non_term_str[i++] = ch;
		}
		non_term_str[i] = '\0';

		//Load LHS of the rule -- the corresponding enum
		NonTerminal non_term  =  find(non_term_str,1);

		//Now look for RHS of the rule
		Rule* new_rule = initialize_rule(&next_rule_no);

		Symbol_list* list = initialize_symbols_list();

		Symbol_node* curr_node = list->head;


		while(1){
			if(end_of_file==1)
				break;

			ch = fgetc(input);

			while(ch==' '|| ch == '\t' || ch== '=' || ch== '>'){
				ch = fgetc(input);
			}

			//For new line, new rule begins
			if(ch == '\n')
				break;

			//If EOF, notify not to read further for new rules
			else if(ch == EOF){
				end_of_file = 1;
				break;
			}

			else if(ch>='A'&&ch<='Z' || ch=='e'){
				
				char* term_str = (char*)malloc(sizeof(char)*MAX_SYMBOL_SIZE);
				int i = 0;
				term_str[i++] = ch;
				ch = fgetc(input);
				while(ch>='A'&&ch<='Z'){
					term_str[i++] = ch;
					ch = fgetc(input);
				}

				term_str[i] = '\0';

								
				Symbol_node* sym_node = create_symbol(find(term_str,0), 0);

				//Adding Symbol to the list
				curr_node = add_symbol_to_symbol_list(list, sym_node, curr_node);
				

				if(ch == EOF || ch == '\n'){
					if(ch==EOF)
						end_of_file = 1;
					break;
				}
			}

			else if(ch=='<'){
				
				char* non_term_str = (char*)malloc(sizeof(char)*MAX_SYMBOL_SIZE);
				int i = 0;
				while((ch = fgetc(input))!='>'){
					non_term_str[i++] = ch;
				}

				non_term_str[i] = '\0';
				
				Symbol_node* sym_node = create_symbol(find(non_term_str,1), 1);

				//Adding Symbol to the list
				curr_node = add_symbol_to_symbol_list(list, sym_node, curr_node);
				
			}
		}
		
		//Assigning the symbol list to the new_rule
		new_rule->symbols = list;
		
		//Add new rule to the grammar
		add_rule_to_grammar(grm,non_term,new_rule);		
	}
} 


Grammar* initialize_grammar(){
	Grammar* grm = (Grammar*)malloc(sizeof(struct grammar));
	grm -> no_of_rules = 0;

	//Total non-terminals will be equal to total number of rules
	grm -> rules = (Rules**)malloc(sizeof(Rules*)*TOTAL_NON_TERMINALS);

	for(int i = 0; i<TOTAL_NON_TERMINALS;i++){
		grm->rules[i] = (Rules*)malloc(sizeof(struct rules));
		grm->rules[i]->head = NULL;
		grm->rules[i]->length = 0;		
	}

	return grm;
}


Symbol_list* initialize_symbols_list(){
	Symbol_list* list = (Symbol_list*)malloc(sizeof(Symbol_list));
	list->head = NULL;
	list->length = 0;

	return list;
}

Rule* initialize_rule(int* next_rule_no){
	Rule* new_rule = (Rule*)malloc(sizeof(Rule));
	new_rule->next = NULL;
	new_rule->symbols = NULL;

	new_rule->rule_no = *next_rule_no;
	*next_rule_no = *next_rule_no+1;

	return new_rule;
}

Symbol_node* create_symbol(int enum_int, int term_or_nonterm){
	Symbol_node* sym_node = (Symbol_node*)malloc(sizeof(Symbol_node));
	sym_node->next=NULL;
	Symbol_node_type type;
	if(term_or_nonterm==0){
		type.terminal = (Tokentype)enum_int;
	}
	else{
		type.nonterminal = (NonTerminal)enum_int;
	}
	sym_node->type = type;
	sym_node->term_or_nonterm = term_or_nonterm;

	return sym_node;


}

int find(char* str, int term_or_nonterm){
	
	//Terminal
	if(term_or_nonterm==0){
		for(int i=0;i<total_token_types;i++){
			if(strcmp(str,TerminalMap[i])==0)
				return i;
		}
	}

	//Non terminal
	else{
		for(int i=0;i<TOTAL_NON_TERMINALS;i++){
			if(strcmp(str,NonTerminalMap[i])==0)
				return i;
		}
	}
}


Symbol_node* add_symbol_to_symbol_list(Symbol_list* list, Symbol_node* sym_node, Symbol_node* curr_node){
	
	//If initially the list is empty
	if(curr_node==NULL){
		sym_node->next = list->head;
		list->head = sym_node;
		curr_node = list->head;
	}
	else{
		curr_node->next = sym_node;
		curr_node = curr_node->next;
	}

	list->length++;

	return curr_node;
}

void add_rule_to_grammar(Grammar* grm, NonTerminal non_term, Rule* new_rule){

	new_rule->next = grm->rules[non_term]->head;
	grm->rules[non_term]->head = new_rule;

	//One more Rule added for the non terminal
	grm->rules[non_term]->length++;

	grm->no_of_rules++;
}

void print_grammar(Grammar* grm){
	printf("\n------------------------ Printing Grammar from Data Structure---------------------\n\n");

	for(int i=0;i<TOTAL_NON_TERMINALS;i++){
		printf("%d.  <%s> ===> ",(i+1), NonTerminalMap[i]);

		Rules* rules = grm->rules[i];

		Rule* temp = rules->head;

		for(int j = 0;j< rules->length;j++){
			if(j!=0)
				printf("| ");

			Symbol_list* symbols = temp->symbols;
			Symbol_node* temp2 = symbols->head;

			for(int k=0;k<symbols->length;k++){
				if(temp2->term_or_nonterm==0)
					printf("%s", TerminalMap[temp2->type.terminal]);
				else
					printf("<%s>", NonTerminalMap[temp2->type.nonterminal]);

				printf(" ");
				temp2 = temp2->next;
			}

			
			temp = temp->next;
		}
		printf("\n");
	}

	printf("\n------------------------ Grammar Over---------------------\n");
}

//Functions to load grammar from the text file and printing it are over

/*************************************************************************************************************************/
















//PART 2


/*************************************************************************************************************************/

//Functions to Compute the first and follow sets

FirstAndFollow* ComputeFirstAndFollowSets(Grammar* grm){

	FirstAndFollow* sets = (FirstAndFollow*)malloc(sizeof(FirstAndFollow));

	//Initialize sets to empty
	intitialize_sets(sets);	

	//Computer First Sets
	for(int i=0;i<TOTAL_NON_TERMINALS;i++){
		compute_First(grm,(NonTerminal)i, sets->first);		
	}

	//Compute Follow Sets
	compute_Follow(grm,sets);

	return sets;
}

void print_first_and_follow_sets(FirstAndFollow* sets){

	printf("\n\n----------------------------Printing First Set--------------------------------\n\n");
	for(int i=0;i<TOTAL_NON_TERMINALS;i++){
		printf("%d. %s => ",(i+1),NonTerminalMap[i]);
		print_set(sets->first[i]);
	}	

	printf("\n---------------------------------First Set Over-----------------------------------\n");

	printf("\n\n----------------------------Printing Follow Set--------------------------------\n\n");
	for(int i=0;i<TOTAL_NON_TERMINALS;i++){
		printf("%d. %s => ",(i+1),NonTerminalMap[i]);
		print_set(sets->follow[i]);
	}	

	printf("\n---------------------------------Follow Set Over-----------------------------------\n");

}

void print_set(int* set){
	printf("{ ");
	for(int i=0;i<TOTAL_TERMINALS;i++){
		if(checkPresence(set,i)){
			printf("%s ",TerminalMap[i]);
		}
	}
	printf("}\n");
}


void intitialize_sets(FirstAndFollow* sets){

	sets->first = (int**)malloc(TOTAL_NON_TERMINALS*sizeof(int*));
	sets->follow = (int**)malloc(TOTAL_NON_TERMINALS*sizeof(int*));

	
	for(int i=0;i<TOTAL_NON_TERMINALS;i++){

		sets->first[i] = (int*)malloc(BIT_VECTOR_SIZE*sizeof(int));
		sets->follow[i] = (int*)malloc(BIT_VECTOR_SIZE*sizeof(int));

		//Initializing bit vector to 0
		memset(sets->first[i],0,sizeof(sets->first[i]));
		memset(sets->follow[i],0,sizeof(sets->follow[i]));		
	}
}


void compute_First(Grammar* grm, NonTerminal non_term, int** first){

	Rules* rules = grm->rules[non_term];

	Rule* temp = rules->head;

	for(int j = 0;j< rules->length;j++){

		Symbol_list* symbols = temp->symbols;
		Symbol_node* temp2 = symbols->head;

		int k;
		for(k=0;k<symbols->length;k++){

			//If a terminal has occurred
			if(temp2->term_or_nonterm==0){

				addToSet(first[non_term],temp2->type.terminal);				
				break;
			}

			else{
				compute_First(grm, temp2->type.nonterminal,first);

				//if e not in first of the nonterminal
				int whetherChanged = union_sets(first[non_term], first[temp2->type.nonterminal]);

				if(!checkPresence(first[temp2->type.nonterminal], EPSILON)){
					break;
				}
			}			

			temp2 = temp2->next;
		}

		//All traversed
		if(k==symbols->length){			
			addToSet(first[non_term],EPSILON);
		}
		
		temp = temp->next;
	}
}

// returns set1 = set1 union set2

//It does not add epsilon to set1 if it exists in set2
int union_sets(int* set1, int* set2){
	int whetherChanged = 0;
	for(int i=0;i<TOTAL_TERMINALS;i++){
		if(i== (int)EPSILON){
			continue;
		}

		if(checkPresence(set2,i)){
			
			if(!checkPresence(set1,i))
				whetherChanged = 1;
			addToSet(set1,i);			
		}
	}
	return whetherChanged;
}

void compute_Follow(Grammar* grm, FirstAndFollow* sets){
	
	addToSet(sets->follow[mainFunction],DOLLAR);
	int whetherChanged = 1;
	while(whetherChanged==1){
		whetherChanged = 0;
		whetherChanged = compute_Follow_Util(grm,sets->first,sets->follow, whetherChanged);
	}
}

int compute_Follow_Util(Grammar* grm, int** first, int** follow, int whetherChanged){
	for(int i=0;i<TOTAL_NON_TERMINALS;i++){
		
		Rules* rules = grm->rules[i];

		Rule* temp = rules->head;

		for(int j = 0;j< rules->length;j++){
			
			Symbol_list* symbols = temp->symbols;
			Symbol_node* temp2 = symbols->head;

			for(int k=0;k<symbols->length;k++){

				//See if it's a non_terminal 
				if(temp2->term_or_nonterm==1){

					//Start from the next node
					Symbol_node* temp3 = temp2->next;

					while(temp3!=NULL){

						//If terminal
						if(temp3->term_or_nonterm==0){
							
							if(!checkPresence(follow[temp2->type.nonterminal], temp3->type.terminal))
								whetherChanged = 1;

							
							addToSet(follow[temp2->type.nonterminal], temp3->type.terminal);
							break;
						}
						//If non-terminal
						else{
							whetherChanged = MAX(whetherChanged,union_sets(follow[temp2->type.nonterminal],first[temp3->type.nonterminal]));

							//If epsilon not a part
							if(!checkPresence(first[temp3->type.nonterminal], EPSILON)){
								break;
							}
						}

						temp3 = temp3->next;						
					}

					//If entire rule is traversed
					if(temp3 == NULL){
						whetherChanged = MAX(whetherChanged,union_sets(follow[temp2->type.nonterminal],follow[i]));
					}
				}								

				temp2 = temp2->next;
			}

			temp = temp->next;
		}		
	}
	return whetherChanged;
}

int MAX(int a, int b){
	return (a>b)? a:b;
}


void addToSet(int* bitVector, int i){
	bitVector[i/sizeof(int)] = (bitVector[i/sizeof(int)]) | (1 << (i%sizeof(int)));
}

void removeFromSet(int* bitVector, int i){
	bitVector[i/sizeof(int)] = (bitVector[i/sizeof(int)]) & (~(1 << (i%sizeof(int))));
}

int checkPresence(int* bitVector,  int i){
	return ((bitVector[i/sizeof(int)] & (1 << (i%sizeof(int)) )) != 0 ); 
}




//Functions for computing First and Follow sets are over

/*************************************************************************************************************************/










//PART 3

/*************************************************************************************************************************/

//Functions to create and print Parse Table


void createParseTable(Grammar* grm, FirstAndFollow* sets, ParsingTable* table){
	for(int i=0;i<TOTAL_NON_TERMINALS;i++){
		Rules* rules = grm->rules[i];

		Rule* temp = rules->head;

		for(int j = 0;j< rules->length;j++){

			Symbol_list* symbols = temp->symbols;
			Symbol_node* temp2 = symbols->head;

			int to_be_continued = 0;

			for(int k=0;k<symbols->length;k++){

				to_be_continued = 0;

				//Terminal
				if(temp2->term_or_nonterm==0){

					//If it is epsilon
					if(temp2->type.terminal==EPSILON){
						for(int x=0;x<TOTAL_TERMINALS;x++){
							
							if(checkPresence(sets->follow[i], x)){							

								if(table->cells[i][x]!=NULL){
									printf("ERROR: Multiple rules clashing in a table-cell <%s> -> %s\n",NonTerminalMap[i],TerminalMap[x]);
								}

								table->cells[i][x] = get_cell(temp,i);
							}
						}

						continue;
					}

					//any other terminal
					else{
						if(table->cells[i][temp2->type.terminal]!=NULL){
							printf("ERROR: Multiple rules clashing in a table-cell <%s> -> %s\n",NonTerminalMap[i],TerminalMap[temp2->type.terminal]);
						}
						table->cells[i][temp2->type.terminal] = get_cell(temp,i);
						break;
					}
				}

				//Non-terminal
				else{
					for(int x=0;x<TOTAL_TERMINALS;x++){

						if(checkPresence(sets->first[temp2->type.nonterminal], x)){

							if(x!=EPSILON){

								if(table->cells[i][x]!=NULL){
									printf("ERROR: Multiple rules clashing in a table-cell <%s> -> %s\n",NonTerminalMap[i],TerminalMap[x]);
								}
								table->cells[i][x] = get_cell(temp,i);
							}

							//If epsilon exists, then continue
							else{
								to_be_continued = 1;
							}
						}
					}
				}

				if(to_be_continued==1)
					continue;
				else
					break;

				temp2 = temp2->next;
			}


			temp = temp->next;
		}

		
	}
}

Cell* get_cell(Rule* rule, int non_term_index){
	Cell* new_cell = (Cell*)malloc(sizeof(Cell));
	new_cell->rule = rule;
	new_cell->non_term_index = non_term_index;

	return new_cell;
}

ParsingTable* initialize_Table(){
	ParsingTable* table = (ParsingTable*)malloc(sizeof(ParsingTable));
	for(int i=0;i<TOTAL_NON_TERMINALS;i++){
		for(int j=0;j<TOTAL_TERMINALS;j++){
			table->cells[i][j] = NULL;
		}
	}

	return table;
}

void print_parsing_table(ParsingTable* table){
	printf("\n\n----------------------------Printing Parsing Table--------------------------------\n\n");	

	printf("Note: 'x' corresonds to an empty cell pointing to no rule, whereas 'O' represents a cell pointing to a rule\n\n");

	for(int i=0;i<TOTAL_NON_TERMINALS;i++){
		printf("%2d. %27s : ",(i+1),NonTerminalMap[i]);
		for(int z=0;z<TOTAL_TERMINALS;z++){
			if(table->cells[i][z]!=NULL){
				printf("O ");
				//printf("%d ",table->cells[i][z]->rule->rule_no);
			}
			else
				printf("x ");
			printf(" ");
		}

		printf("\n");
	}	

	printf("\n---------------------------------Parsing Table Over-----------------------------------\n");
}

/************************************************************************************************************************/











//PART 4


/*************************************************************************************************************************/

//Functions to create and print parse tree

ParseTree*  parseInputSourceCode(char *testcaseFile, ParsingTable* table, int* syntax_errors){

	/**********************************************************************/

							//INITIALIZATION PHASE

	//Initialize_lexer
	FILE* fp = initialize_lexer(testcaseFile);

	if(fp == NULL){
		printf("\nFile not opened for parsing\n");
		return NULL;
	}

	printf("\n\n------------------------------LEXER INITIALIZED--------------------------\n\n");

	//Initializing ParseTree
	ParseTree* tree = initialize_tree();

	//Making Start symbol as the root of parse tree
	add_root(tree);
	printf("\n\n------------------------------TREE INITIALIZED---------------------------\n\n");


	//Initializing Stack
	Stack* stack = initialize_stack();

	//Pushing $ and starting nonterminal on the stack
	TreeNode* dollar_node =  create_node(0,	DOLLAR, NULL);

	push(stack,dollar_node);
	push(stack,tree->root);

	printf("\n\n------------------------------STACK INITIALIZED--------------------------\n\n");

	printf("\n\nStart obtaining tokens from lexer and parse them\n\n");

	/****************************************************************************/
								//Parsing Phase Begins

	Tokentype lookahead;

	//Initially get a token from the file
	Token* new_token = getNextToken(&fp);


	//If first token received is NULL
	if(new_token==NULL){
		printf("\n\nInput File Empty\n\n");
		return NULL;			
	}

	
	int error_detect = 0;
	


	while(1){

		//If input is consumed
		if(new_token==NULL){

			//If input consumed
			if(isTopDOLLAR(stack) && error_detect!=1){
				printf("\n\n---------------------STACK EMPTY AND INPUT CONSUMED------------------------\n\n");
				printf("\n\n------Input source code is syntactically CORRECT - PARSING SUCCESSFUL-----------------\n\n");
			}
			else{
				*syntax_errors = 1;
				printf("\n\n------Input source code is syntactically WRONG - PARSING NOT SUCCESSFUL---------------\n\n");
			}

			break;
		}

		
		//Otherwise get the lookahead symbol
		lookahead = new_token->type;
				

		if(lookahead==ERROR){
			//Skip Lexical Error
			*syntax_errors=1;
			new_token= getNextToken(&fp);
			continue;
		}



		//If top of the stack is $ and input is still left
		if(isTopDOLLAR(stack)){
			*syntax_errors = 1;
			printf("\n\nUnexpected tokens at the end - Stack empty but program still left\n\n");
			printf("\n\n------Input source code is syntactically WRONG - PARSING NOT SUCCESSFUL---------------\n\n");
			break;
		}


		//POP the symbol on the top of the stack
		StackNode* stk_node = pop(stack);
		TreeNode* tree_node = stk_node->treeNodePointer;

		
		//Terminal
		if(tree_node->info->term_or_nonterm==0){

			//Symbol on the stack matches with the lookahead symbol
			if(lookahead == tree_node->info->type.terminal){

				//Will help in printing the parse tree
				tree_node->lexicalTOKEN = new_token;

				//Get next Lookahead Symbol
				new_token = getNextToken(&fp);
				continue;			
							
			}

			//If lookahead and top of stack don't match

			/******************************************************** Panic Recovery Mode *************************************************/

			else{

				
				//Report Error
				error_detect = 1;
				*syntax_errors = 1;
				printf("Line No: %d, Syntax error: The token  for lexeme %s does not match at line %d\n",new_token->line_no,new_token->lexeme,new_token->line_no);

				//error recovery	

				StackNode* stk_node2 = top(stack);
				TreeNode* tree_node2 = stk_node2->treeNodePointer;


				//Skip lookahead symbols until something matches
				while(new_token!=NULL){

					lookahead = new_token->type;

					//Check for the current one 
					if(lookahead == tree_node->info->type.terminal){
						push(stack, tree_node);
						break;
					}

					
					//Check for the following one

					//If terminal
					if(tree_node2->info->term_or_nonterm==0){
						if(lookahead == tree_node2->info->type.terminal){
							break;
						}
					}
					else{
						Cell* to_be_applied_rule2 = table->cells[tree_node2->info->type.nonterminal][lookahead];
						if(to_be_applied_rule2!=NULL){
							break;
						}
					}
					
					
					if(new_token->type==SEMICOLON){
						new_token = getNextToken(&fp);
						break;
					}
					

					new_token = getNextToken(&fp);
					
				}
		
				continue;			

			}

			/******************************************************** Panic Mode Over ***************************************************/			
		}			
		

		//if top of the stack is a non terminal
		else{

			//Get the rule to be applied from the parsing table
			Cell* to_be_applied_rule = table->cells[tree_node->info->type.nonterminal][lookahead];


			//If no rule matches

			/******************************************************** Panic Recovery Mode *************************************************/

			if(to_be_applied_rule == NULL){

				//Report Error
				error_detect = 1;
				*syntax_errors = 1;
				printf("Line No: %d, Syntax error: The token  for lexeme %s does not match at line %d\n",new_token->line_no,new_token->lexeme,new_token->line_no);



				//error recovery	

				StackNode* stk_node2 = top(stack);
				TreeNode* tree_node2 = stk_node2->treeNodePointer;

				//Skip lookahead symbols until something matches
				while(new_token!=NULL){

					lookahead = new_token->type;

					//Check for the current one 
					Cell* to_be_applied_rule1 = table->cells[tree_node->info->type.nonterminal][lookahead];


					if(to_be_applied_rule1!=NULL){
						//Push it on the stack and start the iteration all over
						push(stack, tree_node);
						break;
					}

					//Check for the following one

					//If terminal
					if(tree_node2->info->term_or_nonterm==0){
						if(lookahead == tree_node2->info->type.terminal){
							break;
						}
					
						else{
							Cell* to_be_applied_rule2 = table->cells[tree_node2->info->type.nonterminal][lookahead];
							if(to_be_applied_rule2!=NULL){
								break;
							}
						}
					}

					
					if(new_token->type==SEMICOLON){
						new_token = getNextToken(&fp);
						break;
					}
					


					new_token = getNextToken(&fp);
					
				}
				
			}

			/******************************************************** Panic Mode Over ***************************************************/


			//Normal Case
			else{				
				//Add Children to the parse tree for the popped non terminal from the stack
				add_children(tree_node, to_be_applied_rule->rule);

				//PUSH RHS of the rule on the top of the stack
				push_children_on_the_stack(stack,tree_node);
			}		

		}

		//Go to begin of the while loop
	}

	if(fp!=NULL)
		fclose(fp);

	return tree;

}


void printParseTree(ParseTree* tree){
	FILE* fp1 = NULL;

	//fprintf(fp1,"\n------------------------------------------------------Printing Parse Tree-----------------------------------------------------\n\n");
	printf("\n------------------------------------------------------Printing Parse Tree On the Console-----------------------------------------------\n\n");
	
	//fprintf(fp1, "%-25s %-10s %-15s %-15s %-30s %-5s %s\n\n\n", "LEXEME","LINE","TOKEN","VALUE","PARENT","LEAF","NODE");
	printf("%-25s %-10s %-15s %-15s %-30s %-5s %s\n\n\n", "LEXEME","LINE","TOKEN","VALUE","PARENT","LEAF","NODE");

	printParseTree_util(tree->root,&fp1);

	printf("\n------------------------------------------------------Parse Tree Printed On the Console-----------------------------------------------\n\n");
}


void printParseTree_util(TreeNode* node, FILE** fp1){

	if(node==NULL){
		return;
	}

	char* empty = "****";
	char* no = "no";
	char* yes = "yes";
	char* root = "ROOT";


	Children* children = node->children;


	//If not a leaf node
	if(children != NULL){

		TreeNode* temp = children->head;
		
		
		//Printing the left most child
		printParseTree_util(temp,fp1);

		//Then print the node
		printNode(fp1,node, 0, empty,no,yes,root);


		//Then proceed to rest of the children
		temp = temp->next;
		
		while(temp!=NULL){
			printParseTree_util(temp, fp1);
			temp = temp->next;
		}
		
	}

	//If Leaf Node, print it
	else{
		printNode(fp1,node, 1, empty,no,yes,root);		
	}
	
}

//This function prints the stored info at one node of a tree
void printNode(FILE** fp, TreeNode* node, int isLeaf, char* empty, char* no, char* yes, char* root){

	char* error = "(ERROR)" ;

	//Leaf Node
	if(isLeaf){	
		
		//If leaf node is a non-terminal (in-case of incomplete trees) -- ERROR CASE
		if(node->info->term_or_nonterm==1){
			
			//Root Node
			if(node->parent == NULL){
				//fprintf(*fp, "%-25s %-10s %-15s %-15s %-30s %-5s <%s>\n", empty,empty,empty,empty,root,yes,NonTerminalMap[node->info->type.nonterminal]);
				printf("%-25s %-10s %-15s %-15s %-30s %-5s <%s>\n", empty,empty,empty,empty,root,yes,NonTerminalMap[node->info->type.nonterminal]);
			}
			else{
				//fprintf(*fp, "%-25s %-10s %-15s %-15s %-30s %-5s <%s>\n", empty,empty,empty,empty,NonTerminalMap[node->parent->info->type.nonterminal],yes,NonTerminalMap[node->info->type.nonterminal]);
				printf("%-25s %-10s %-15s %-15s %-30s %-5s <%s>\n", empty,empty,empty,empty,NonTerminalMap[node->parent->info->type.nonterminal],yes,NonTerminalMap[node->info->type.nonterminal]);

			}
		}	

		//No lexeme for epsilon
		else if(node->info->type.terminal==EPSILON){
			
			//fprintf(*fp, "%-25s %-10s %-15s %-15s %-30s %-5s %s\n", empty,empty,empty,empty,NonTerminalMap[node->parent->info->type.nonterminal],yes,TerminalMap[node->info->type.terminal]);
			printf("%-25s %-10s %-15s %-15s %-30s %-5s %s\n", empty,empty,empty,empty,NonTerminalMap[node->parent->info->type.nonterminal],yes,TerminalMap[node->info->type.terminal]);

		}

		//Some terminal nodes may not be assigned lexical tokens since they are not matched -- ERROR CASE
		else if(node->lexicalTOKEN==NULL){
			//fprintf(*fp, "%-25s %-10s %-15s %-15s %-30s %-5s %s\n", error, error, error, empty, NonTerminalMap[node->parent->info->type.nonterminal], yes, TerminalMap[node->info->type.terminal]);
			printf("%-25s %-10s %-15s %-15s %-30s %-5s %s\n", error, error, error, empty, NonTerminalMap[node->parent->info->type.nonterminal], yes, TerminalMap[node->info->type.terminal]);
		}
		
		//Not an integer or Real Number
		else if(node->lexicalTOKEN->valueType== -1){
			//fprintf(*fp, "%-25s %-10d %-15s %-15s %-30s %-5s %s\n", node->lexicalTOKEN->lexeme, node->lexicalTOKEN->line_no, TerminalMap[node->lexicalTOKEN->type], empty, NonTerminalMap[node->parent->info->type.nonterminal], yes, TerminalMap[node->info->type.terminal]);
			printf("%-25s %-10d %-15s %-15s %-30s %-5s %s\n", node->lexicalTOKEN->lexeme, node->lexicalTOKEN->line_no, TerminalMap[node->lexicalTOKEN->type], empty, NonTerminalMap[node->parent->info->type.nonterminal], yes, TerminalMap[node->info->type.terminal]);
		}

		//Integer
		else if(node->lexicalTOKEN->valueType== 0){
			//fprintf(*fp, "%-25s %-10d %-15s %-15d %-30s %-5s %s\n", node->lexicalTOKEN->lexeme, node->lexicalTOKEN->line_no, TerminalMap[node->lexicalTOKEN->type], node->lexicalTOKEN->value->num, NonTerminalMap[node->parent->info->type.nonterminal], yes, TerminalMap[node->info->type.terminal]);
			printf("%-25s %-10d %-15s %-15d %-30s %-5s %s\n", node->lexicalTOKEN->lexeme, node->lexicalTOKEN->line_no, TerminalMap[node->lexicalTOKEN->type], node->lexicalTOKEN->value->num, NonTerminalMap[node->parent->info->type.nonterminal], yes, TerminalMap[node->info->type.terminal]);
		}

		//Real
		else{
			//fprintf(*fp, "%-25s %-10d %-15s %-15f %-30s %-5s %s\n", node->lexicalTOKEN->lexeme, node->lexicalTOKEN->line_no, TerminalMap[node->lexicalTOKEN->type], node->lexicalTOKEN->value->r_num, NonTerminalMap[node->parent->info->type.nonterminal], yes, TerminalMap[node->info->type.terminal]);
			printf("%-25s %-10d %-15s %-15f %-30s %-5s %s\n", node->lexicalTOKEN->lexeme, node->lexicalTOKEN->line_no, TerminalMap[node->lexicalTOKEN->type], node->lexicalTOKEN->value->r_num, NonTerminalMap[node->parent->info->type.nonterminal], yes, TerminalMap[node->info->type.terminal]);

		}

	}

	//Non Leaf Node
	else{

		//Root Node
		if(node->parent == NULL){
			//fprintf(*fp, "%-25s %-10s %-15s %-15s %-30s %-5s <%s>\n", empty,empty,empty,empty,root,no,NonTerminalMap[node->info->type.nonterminal]);
			printf("%-25s %-10s %-15s %-15s %-30s %-5s <%s>\n", empty,empty,empty,empty,root,no,NonTerminalMap[node->info->type.nonterminal]);
		}
		else{
			//fprintf(*fp, "%-25s %-10s %-15s %-15s %-30s %-5s <%s>\n", empty,empty,empty,empty,NonTerminalMap[node->parent->info->type.nonterminal],no,NonTerminalMap[node->info->type.nonterminal]);
			printf("%-25s %-10s %-15s %-15s %-30s %-5s <%s>\n", empty,empty,empty,empty,NonTerminalMap[node->parent->info->type.nonterminal],no,NonTerminalMap[node->info->type.nonterminal]);

		}
	}
}



/************************************************* EXTRA FUNCTIONS (Used for Debugging during parsing) ****************************************************/

void print_top_of_stack(TreeNode* tree_node){
	printf("TOP OF STACK: ");
	if(tree_node->info->term_or_nonterm==0){
		printf("%s\n", TerminalMap[tree_node->info->type.terminal]);
		
	}
	else{
		printf("<%s>\n", NonTerminalMap[tree_node->info->type.nonterminal]);
	}

}


void printChildren(Children* children){
	TreeNode* temp = children->head;
	while(temp!=NULL){
		if(temp->info->term_or_nonterm==0)
			printf("%s ",TerminalMap[temp->info->type.terminal]);
		else{
			printf("<%s> ",NonTerminalMap[temp->info->type.nonterminal]);
		}
		temp = temp->next;
	}
	
}


//Functions for parsing are over

/*************************************************************************************************************************/





