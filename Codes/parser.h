#ifndef _PARSERDEF_
#define _PARSERDEF_

#include <stdio.h>
#include <stdlib.h>

//Includes all functionalities of Lexer
#include "lexerDef.h"
#include "NaryTreeDef.h"
//Total number of nonterminals
#define TOTAL_NON_TERMINALS 46

//Actual total terminals + $ will be the number of columns in the parse table -- so 40(actual terminals) + 1
#define TOTAL_TERMINALS 41
#define MAX_SYMBOL_SIZE 50
#define BIT_VECTOR_SIZE (sizeof(int)-1+ TOTAL_TERMINALS)/sizeof(int)

//Terminal symbols are nothing but the token types
typedef Tokentype Terminal;


typedef enum{
	 
	<start>,
    <gen-dec_block> ,
    <assign_block>,
    <gen-dec>,
    <type>,
    <var_names> ,
    <Jagarr_type>,
     <Rectarr_type>,
     <dims_J>,
     <brackets> ,
     <populate>,
     <vals>,
     <nex>,
     <val_ext>,
      <dims_R>,
      <var_Ind>,
      <assign_stmt>,
      <arithmetic_expr>,
      <bool_expr>,
      <gen_var_name>,
      <expr1>,
       <term1>,
       <var>,  
  
	leftHandSide1,
	leftHandSide2,
	rightHandSide1,
	rightHandSide2
	 
	 
	 
}NonTerminal;




//Either terminal or non-terminal
typedef union{
	Terminal terminal;
	NonTerminal nonterminal;
}Symbol_node_type;


struct symbol_node{
	Symbol_node_type type; //Type of the Node -- whether terminal or non terminal
	int term_or_nonterm; //0 for Terminal and 1 for Non Terminal

	struct symbol_node* next; //Pointer to the next node in the list
};

typedef struct symbol_node Symbol_node;


//A particular rhs of a rule is a list of symbols i.e terminals and non-terminals
struct symbol_list{
	Symbol_node* head;
	int length; 
};

typedef struct symbol_list Symbol_list;


//RHS of a non terminal can have multiple Rules , hence a linked list of rules.
struct rule{
	Symbol_list* symbols; //One rule (i.e RHS of a rule) can have terminals and nonterminals both
	struct rule* next; //Pointer to next Rule for that non-terminal
	int rule_no;
};
typedef struct rule Rule;


//One non terminal can have multiple RHS (rules) like A ==> aB | c | dEf
struct rules{
	Rule* head; //A linked list of rules for a non-terminal
	int length; //Number of rules for a particular non-terminal
};

typedef struct rules Rules;


//Grammar as an array of rules
struct grammar{
	int no_of_rules;
	Rules** rules; 	
};

typedef struct grammar Grammar;



//BIT Vectors for first and follow sets
struct firstAndFollow{
	int** first;
	int** follow;
}; 

typedef struct firstAndFollow FirstAndFollow;



//Each cell of a parsing table either points to NULL or to a rule of the grammar
struct cell{	
	
	//LHS of the rule
	int non_term_index;

	//Pointer to the RHS of the rule
	Rule* rule;
};

typedef struct cell Cell;

struct parsingTable{
	Cell*  cells[TOTAL_NON_TERMINALS][TOTAL_TERMINALS];
};

typedef struct parsingTable ParsingTable;



//Function Declarations


/****************************************************************************************************************************/

/**Loading Grammar**/


Grammar* load_grammar_from_text_file(char* grammar_text_file);



Grammar* initialize_grammar();
Symbol_list* initialize_symbols_list();
Rule* initialize_rule(int* next_rule_no);
Symbol_node* create_symbol(int enum_int, int term_or_nonterm);
Symbol_node* add_symbol_to_symbol_list(Symbol_list* list, Symbol_node* sym_node, Symbol_node* curr_node);
void add_rule_to_grammar(Grammar* grm, NonTerminal non_term, Rule* new_rule);
int find(char* str, int term_or_nonterm);
void print_grammar(Grammar* grm);


/****************************************************************************************************************************/

 



/****************************************************************************************************************************/

/**Parsing Table**/


void createParseTable(Grammar* grm, FirstAndFollow* sets, ParsingTable* table);



Cell* get_cell(Rule* rule, int non_term_index);
ParsingTable* initialize_Table();
void print_parsing_table(ParsingTable* table);


/****************************************************************************************************************************/


/**Parsing**/


ParseTree*  parseInputSourceCode(char *testcaseFile, ParsingTable* table, int* syntax_errors);



void printParseTree(ParseTree* tree);
void printParseTree_util(TreeNode* node, FILE** fp);
void printNode(FILE** fp, TreeNode* node, int isLeaf, char* empty, char* no, char* yes, char* root);
void printChildren(Children* children);
void print_top_of_stack(TreeNode* tree_node);





#endif