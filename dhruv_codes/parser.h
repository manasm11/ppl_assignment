/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#include <stdio.h>
#include <stdlib.h>

#include "parserDef.h"
#include "NaryTreeDef.h"

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

/**First and Follow**/


FirstAndFollow* ComputeFirstAndFollowSets(Grammar* grm);



void print_first_and_follow_sets(FirstAndFollow* sets);
void print_set(int* set);
void intitialize_sets(FirstAndFollow* sets);
void compute_First(Grammar* grm, NonTerminal non_term, int** first,int i);
int union_sets(int* set1, int* set2);
void compute_Follow(Grammar* grm, FirstAndFollow* sets);
int compute_Follow_Util(Grammar* grm, int** first, int** follow, int hasChanged);
int MAX(int a, int b);



/**Bit Vector for Sets**/

void addToSet(int* bitVector, int k);
void removeFromSet(int* bitVector, int k);
int checkPresence(int* bitVector,  int k);


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



