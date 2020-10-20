 


/***********************************************************************************************************************

1. The file is always read in blocks - i.e into the buffer, whether we are reading file for obtaining next token or removing comments.
2. Buffering improves the performance as we don't have to go to the file again and again for obtaining next character.
3. The entire DFA is coded in the function getNextToken.
4. The DFA is coded in the form of switch cases. Hence, there are around 50 cases corresponding to the states in the DFA.
5. The token map is included only for printing the tokens i.e a particular token string will be indexed by the corresponding enum integer.

***********************************************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "lookUpTable.h"
#include "lexer.h"


//Mapping of ENUM to String
char* tokenMap[] = {"ASSIGNOP","COMMENT","FUNID","ID","NUM","RNUM","STR","END","INT","REAL","STRING","MATRIX","MAIN","SQO","SQC","OP","CL","SEMICOLON","COMMA","IF","ELSE","ENDIF","READ","PRINT","FUNCTION","PLUS","MINUS","MUL","DIV","SIZE","AND","OR","NOT","LT","LE","EQ","GT","GE","NE","e","$","ERROR", "BOOLEAN", "LABEL", "GOTO"
};

//Maximum Size of the Lexer Buffer
#define BUFFER_SIZE 4096 //4KB BUFFER -- Standard size

//Maximum Slots in the Lookup table
int NO_SLOTS = 8;

//Global Lexer Buffers

//TWIN BUFFERING SCHEME
char* lexer_buffer_current;
char* lexer_buffer_previous;

//Current Line Number initialized as 1
int current_line = 1;

//Initializing the forward_pointer to beginning of the buffer
int forward_pointer = 0;

//Denote whether retraction twice has occurred in one particular case or not
int retract_twice = 0;

//LookUpTable for lookup
LookUpTable* lookUpTable;

//Whether file should be read further or not
int file_over = 0;




//Function to load buffer from the input file
FILE* getStream(FILE* file_ptr){

	//Making the current buffer as previous buffer
	char* temp = lexer_buffer_previous;
	lexer_buffer_previous = lexer_buffer_current;
	lexer_buffer_current = temp;


	/**************************Now loading new buffer************************/

	//Initializing buffer to 0
	memset(lexer_buffer_current, 0 , sizeof(lexer_buffer_current));
	
	//If end of file is reached
	if(feof(file_ptr)){
		//printf("\nFile Over....................\n\n");
		fclose(file_ptr);
 		return NULL;
 	}
	
	//Reading from the input file 
	int x;
	if((x = fread(lexer_buffer_current, sizeof(char), BUFFER_SIZE, file_ptr))>0){
		lexer_buffer_current[BUFFER_SIZE]='\0';
 		//Even if the number of characters read are less than BUFFER_SIZE, the rest of the buffer is already initialized to '\0' whose ascii value is 0.
 		return file_ptr;
 	}

 	//If the number of bytes read are 0
 	else{
 		//fprintf(stderr,"\nFile Over....................\n\n");
 		fclose(file_ptr);
 		return NULL;
 	}
}


//This function returns the next token from the input source code
Token* getNextToken(FILE** file_ptr){

	if(file_over==1){

		if(*file_ptr!=NULL)
			fclose(*file_ptr);

		return NULL;
	}

	Token* token = (Token*)malloc(sizeof(Token));

	//Loading Buffer
	if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
		forward_pointer = 0;
		*file_ptr = getStream(*file_ptr);
		if(*file_ptr==NULL){
			file_over = 1;
			return NULL;
		} 
	}

	//Initially DFA is at start state
	int state = 0;
	int final_state_reached = 0;
	char* lexeme = (char*)malloc(100*sizeof(char));
	int lexeme_ptr = 0;
	while(1){
		switch(state){

			//case for the start state

			case 0:{

				//Refill the buffer
				if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						file_over = 1;
						return NULL;						
					}

					state = 0; 
				}

				//If one particular case of double retraction has occurred
				else if(retract_twice==1){
					lexeme[lexeme_ptr++] = '.';
					state = 31;
					retract_twice = 0; 
				}

				//Ignore whitespaces and tabs and carriage return also(in some cases)
				else if(lexer_buffer_current[forward_pointer]==' '|| lexer_buffer_current[forward_pointer]=='\t'|| lexer_buffer_current[forward_pointer] == '\r' || lexer_buffer_current[forward_pointer]==(char)13){
					state = 0;
					forward_pointer++;
				}

				//Increment Line Number
				else if(lexer_buffer_current[forward_pointer]=='\n'){
					state = 1;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='['){
					state = 2;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]==']'){
					state = 3;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='('){
					state = 4;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]==')'){
					state = 5;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]==';'){
					state = 6;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]==','){
					state = 7;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='+'){
					state = 8;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='-'){
					state = 9;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='*'){
					state = 10;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='/'){
					state = 11;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='@'){
					state = 12;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='<'){
					state = 13;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='>'){
					state = 16;
					forward_pointer++;
				}
				else if(lexer_buffer_current[forward_pointer]=='='){
					state = 19;
					forward_pointer++;
				}

				//Variable Identifier
				else if(isalpha(lexer_buffer_current[forward_pointer])){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 24;
					forward_pointer++;
				}

				//Function Identifier
				else if(lexer_buffer_current[forward_pointer]=='_'){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 27;
					forward_pointer++;
				}

				//Comments
				else if(lexer_buffer_current[forward_pointer]=='#'){
					state = 30;
					forward_pointer++;
				}

				//Logical Operator
				else if(lexer_buffer_current[forward_pointer]=='.'){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 31;
					forward_pointer++;
				}

				//String
				else if(lexer_buffer_current[forward_pointer]=='\"'){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 43;
					forward_pointer++;
				}

				//NUM or RNUM
				else if(isdigit(lexer_buffer_current[forward_pointer])){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 46;
					forward_pointer++;
				}				

				//If an unknown symbol is encountered
				else{
					fprintf(stderr, "Line No: %d, Lexical Error, Unknown Symbol %c at line %d\n",current_line, lexer_buffer_current[forward_pointer], current_line);
					lexeme[lexeme_ptr++] = lexer_buffer_current[forward_pointer];
					//Skip the symbol
					forward_pointer++;
					lexeme_ptr++;
					state = 52;
				}

			}break;


			//cases for rest of the states

			//Refer to DFA for the State Numbers

			case 1:{
				//printf("\n--------------------------------Line Number Increased--------------------------------------\n\n");
				current_line++;
				state = 0;

			}break;

			case 2:{		
				strcpy(lexeme,"[");
				addToken(token, SQO, lexeme, -1, NULL);	
				final_state_reached = 1;
			}break;

			case 3:{
				strcpy(lexeme,"]");
				addToken(token, SQC, lexeme, -1, NULL);	
				final_state_reached = 1;
			}break;

			case 4:{
				strcpy(lexeme,"(");
				addToken(token, OP, lexeme, -1, NULL);	
				final_state_reached = 1;
			}break;

			case 5:{
				strcpy(lexeme,")");
				addToken(token, CL, lexeme, -1, NULL);	
				final_state_reached = 1;
			}break;

			case 6:{
				strcpy(lexeme,";");
				addToken(token, SEMICOLON, lexeme, -1, NULL);	
				final_state_reached = 1;
			}break;

			case 7:{
				strcpy(lexeme,",");
				addToken(token, COMMA, lexeme, -1, NULL);	
				final_state_reached = 1;
			}break;

			case 8:{
				strcpy(lexeme,"+");
				addToken(token, PLUS, lexeme, -1, NULL);	
				final_state_reached = 1;
			}break;

			case 9:{
				strcpy(lexeme,"-");
				addToken(token, MINUS, lexeme, -1, NULL);	
				final_state_reached = 1;
			}break;

			case 10:{
				strcpy(lexeme,"*");
				addToken(token, MUL, lexeme, -1, NULL);	
				final_state_reached = 1;
			}break;

			case 11:{
				strcpy(lexeme,"/");
				addToken(token, DIV, lexeme, -1, NULL);	
				final_state_reached = 1;
			}break;

			case 12:{
				strcpy(lexeme,"@");
				addToken(token, SIZE, lexeme, -1, NULL);	
				final_state_reached = 1;
			}break;

			case 13:{
				if(lexer_buffer_current[forward_pointer]=='='){
					state = 15;	
					forward_pointer++;				
				}

				//If the forward_pointer has reached the end of Buffer -- either the file is finished or more is left
				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended return the token
						state = 14;
						file_over = 1;	
						forward_pointer++;										
					}
					else{
						state = 13;
					}					
				}
				//Other Character
				else{
					state = 14;	
					forward_pointer++;				
				}				

			}break;

			case 14:{
				strcpy(lexeme,"<");
				addToken(token, LT, lexeme, -1, NULL);	
				
				//retract
				forward_pointer--;
				final_state_reached = 1;
			}break;

			case 15:{
				strcpy(lexeme,"<=");
				addToken(token, LE, lexeme, -1, NULL);	
				final_state_reached = 1;
			}break;

			case 16:{
				
				if(lexer_buffer_current[forward_pointer]=='='){
					state = 18;	
					forward_pointer++;				
				}

				//If the forward_pointer has reached the end of Buffer -- either the file is finished or more is left
				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended return the token
						state = 17;	
						forward_pointer++;
						file_over = 1;										
					}
					//Otherwise revisit the state
					else{
						state = 16;
					}					
				}
				//Other Character
				else{
					state = 17;	
					forward_pointer++;				
				}				

			}break;

			case 17:{
				strcpy(lexeme,">");
				addToken(token, GT, lexeme, -1, NULL);

				//retract
				forward_pointer--;
				final_state_reached = 1;
			}break;

			case 18:{
				strcpy(lexeme,">=");
				addToken(token, GE, lexeme, -1, NULL);	
				final_state_reached = 1;
			}break;

			case 19:{
				
				if(lexer_buffer_current[forward_pointer]=='='){
					state = 21;	//For '=='
					forward_pointer++;				
				}

				else if(lexer_buffer_current[forward_pointer]=='/'){
					state = 22;	//For '=/='
					forward_pointer++;				
				}

				//If the forward_pointer has reached the end of Buffer -- either the file is finished or more is left
				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended return the token
						state = 20;	
						forward_pointer++;
						file_over = 1;										
					}
					//Otherwise revisit the state
					else{
						state = 19;
					}					
				}

				//Other Character
				else{
					state = 20;	
					forward_pointer++;				
				}				

			}break;

			case 20:{
				strcpy(lexeme,"=");
				addToken(token, ASSIGNOP, lexeme, -1, NULL);

				//retract
				forward_pointer--;
				final_state_reached = 1;

			}break;

			case 21:{
				strcpy(lexeme,"==");
				addToken(token, EQ, lexeme, -1, NULL);	
				final_state_reached = 1;
			}break;

			case 22:{
				if(lexer_buffer_current[forward_pointer]=='='){
					state = 23;
					forward_pointer++;
				}
				//Refill the buffer if it has reached the end of buffer
				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended, display error
						strcpy(lexeme, "=/");
						lexeme_ptr = strlen(lexeme);
						fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line, lexeme ,current_line);
						file_over = 1;
						state = 52;											
					}
					//Otherwise revisit the state
					else{
						state = 22;
					}
				}

				//If some other character is encountered, display error
				else{
					strcpy(lexeme, "=/");
					lexeme_ptr = strlen(lexeme);
					fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line, lexeme ,current_line);
					state = 52;											
				}

			}break;

			case 23:{
				strcpy(lexeme,"=/=");
				addToken(token, NE, lexeme, -1, NULL);	
				final_state_reached = 1;
			}break;


			case 24:{
				if(isdigit(lexer_buffer_current[forward_pointer])){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 25;
					forward_pointer++;
				}
				else if(isalpha(lexer_buffer_current[forward_pointer])){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 24;
					forward_pointer++;
				}
				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended, return ID token
						file_over = 1;
						state = 26;
						forward_pointer++;					
					}
					//Otherwise revisit the state
					else{
						state = 24;
					}
				}
				//Other Character, so return ID token
				else{
					state = 26;
					forward_pointer++;
				}
			}break;

			case 25:{

				lexeme[lexeme_ptr] = '\0';

				if(strlen(lexeme)>20){
					fprintf(stderr, "Line No: %d, Lexical Error, Identifier %s at line %d is longer than the prescribed length of 20 characters\n",current_line, lexeme,current_line);
					state = 52;
				}
				else{
					//Perform Lookup
					Node* lookup = getTokenType(lexeme, lookUpTable);

					if(lookup==NULL)
						token->type = ID;
					else
						token->type = lookup->type;

					addToken(token, token->type, lexeme, -1, NULL);	
					final_state_reached = 1;
				}

				
			}break;

			case 26:{

				lexeme[lexeme_ptr] = '\0';

				if(strlen(lexeme)>20){
					fprintf(stderr, "Line No: %d, Lexical Error, Identifier %s at line %d is longer than the prescribed length of 20 characters\n" ,current_line, lexeme, current_line);
					state = 52;
				}

				else{
					//Perform Lookup
					Node* lookup = getTokenType(lexeme, lookUpTable);

					if(lookup==NULL)
						token->type = ID;
					else
						token->type = lookup->type;

					addToken(token, token->type, lexeme, -1, NULL);	

					//retract
					forward_pointer--;
					final_state_reached = 1;
				}
				

			}break;

			case 27:{
				if(isalpha(lexer_buffer_current[forward_pointer])){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 28;
					forward_pointer++;
				}
				//Refill the buffer if it has reached the end of buffer
				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						
						//If the file has ended, display error
						lexeme[lexeme_ptr]='\0';
						fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n", current_line,lexeme ,current_line);
						file_over = 1;
						state = 52;	

					}
					//Otherwise revisit the state
					else{
						state = 27;
					}
				}

				//If some other character is encountered, display error
				else{
					lexeme[lexeme_ptr]='\0';
					fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line, lexeme ,current_line);
					state = 52;	
				}

			}break;

			case 28:{
				if(isdigit(lexer_buffer_current[forward_pointer]) || isalpha(lexer_buffer_current[forward_pointer])){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 28;
					forward_pointer++;
				}
				
				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended, return FUNID token
						state = 29;
						forward_pointer++;	
						file_over = 1;				
					}
					//Otherwise revisit the state
					else{
						state = 28;
					}
				}
				//Other Character, so return FUNID token
				else{
					state = 29;
					forward_pointer++;
				}
			}break;

			case 29:{

				lexeme[lexeme_ptr] = '\0';

				if(strlen(lexeme)>20){
					fprintf(stderr, "Line No: %d, Lexical Error, Function Identifier %s at line %d is longer than the prescribed length of 20 characters\n",current_line, lexeme,current_line);
					state = 52;
				}
				else{
					//Perform Lookup
					Node* lookup = getTokenType(lexeme, lookUpTable);

					if(lookup==NULL)
						token->type = FUNID;
					else
						token->type = lookup->type;

					addToken(token, token->type, lexeme, -1, NULL);	

					//retract
					forward_pointer--;
					final_state_reached = 1;
				}				

			}break;


			//Case for comments -- No token returned
			case 30:{
				if(lexer_buffer_current[forward_pointer]=='\n'){
					//printf("\nComment Ignored\n");
					state = 1;
					forward_pointer++;
				}

				//Refill the buffer
				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended, do nothing						
						return NULL;					
					}
					//Otherwise revisit the state
					else{
						state = 30;
					}
				}
				//Keep on ignoring the comment
				else{
					state = 30;
					forward_pointer++;
				}
			}break;

			//Logical Operators
			case 31:{
				if(lexer_buffer_current[forward_pointer]=='a'){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 32;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='o'){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 33;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='n'){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 34;
					forward_pointer++;
				}
				
				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended, display error
						lexeme[lexeme_ptr]='\0';
						fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line,  lexeme ,current_line);
						file_over = 1;
						state = 52;					
					}
					//Otherwise revisit the state
					else{
						state = 31;
					}
				}
				//If some other character is encountered, display error
				else{
					lexeme[lexeme_ptr]='\0';
					fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line,  lexeme ,current_line);
					state = 52;
				}

			}break;


			case 32:{
				if(lexer_buffer_current[forward_pointer]=='n'){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 35;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended, display error
						lexeme[lexeme_ptr]='\0';
						fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line,  lexeme ,current_line);
						file_over = 1;
						state = 52;					
					}
					//Otherwise revisit the state
					else{
						state = 32;
					}
				}
				//If some other character is encountered, display error
				else{
					lexeme[lexeme_ptr]='\0';
					fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line,  lexeme ,current_line);
					state = 52;
				}
			}break;

			case 33:{
				if(lexer_buffer_current[forward_pointer]=='r'){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 36;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended, display error
						lexeme[lexeme_ptr]='\0';
						fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line,  lexeme ,current_line);
						file_over = 1;
						state = 52;					
					}
					//Otherwise revisit the state
					else{
						state = 33;
					}
				}
				//If some other character is encountered, display error
				else{
					lexeme[lexeme_ptr]='\0';
					fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line,  lexeme ,current_line);
					state = 52;
				}
			}break;

			case 34:{
				if(lexer_buffer_current[forward_pointer]=='o'){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 37;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended, display error
						lexeme[lexeme_ptr]='\0';
						fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line,  lexeme ,current_line);
						file_over = 1;
						state = 52;					
					}
					//Otherwise revisit the state
					else{
						state = 34;
					}
				}
				//If some other character is encountered, display error
				else{
					lexeme[lexeme_ptr]='\0';
					fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line,  lexeme ,current_line);
					state = 52;
				}
			}break;

			case 35:{
				if(lexer_buffer_current[forward_pointer]=='d'){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 38;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended, display error
						lexeme[lexeme_ptr]='\0';
						fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line,  lexeme ,current_line);
						file_over = 1;
						state = 52;				
					}
					//Otherwise revisit the state
					else{
						state = 35;
					}
				}
				//If some other character is encountered, display error
				else{
					lexeme[lexeme_ptr]='\0';
					fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line,  lexeme ,current_line);
					
					state = 52;
				}
			}break;

			case 36:{
				if(lexer_buffer_current[forward_pointer]=='.'){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 39;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended, display error
						lexeme[lexeme_ptr]='\0';
						fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line, lexeme ,current_line);
						file_over = 1;
						state = 52;					
					}
					//Otherwise revisit the state
					else{
						state = 36;
					}
				}
				//If some other character is encountered, display error
				else{
					lexeme[lexeme_ptr]='\0';
					fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line, lexeme ,current_line);
					
					state = 52;
				}
			}break;

			case 37:{
				if(lexer_buffer_current[forward_pointer]=='t'){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 40;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended, display error
						lexeme[lexeme_ptr]='\0';
						fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line, lexeme ,current_line);
						file_over = 1;
						state = 52;					
					}
					//Otherwise revisit the state
					else{
						state = 37;
					}
				}
				//If some other character is encountered, display error
				else{
					lexeme[lexeme_ptr]='\0';
					fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line, lexeme ,current_line);
						
					state = 52;
				}
			}break;

			case 38:{
				if(lexer_buffer_current[forward_pointer]=='.'){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 41;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended, display error
						lexeme[lexeme_ptr]='\0';
						fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line, lexeme ,current_line);
						file_over = 1;
						state = 52;					
					}
					//Otherwise revisit the state
					else{
						state = 38;
					}
				}
				//If some other character is encountered, display error
				else{
					lexeme[lexeme_ptr]='\0';
					fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line, lexeme ,current_line);
					
					state = 52;
				}
			}break;

			case 39:{
				lexeme[lexeme_ptr] = '\0';
				addToken(token, OR, lexeme, -1, NULL);	
				final_state_reached = 1;
				
			}break;

			case 40:{
				if(lexer_buffer_current[forward_pointer]=='.'){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 42;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended, display error
						lexeme[lexeme_ptr]='\0';
						fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line, lexeme ,current_line);
						file_over = 1;
						state = 52;					
					}
					//Otherwise revisit the state
					else{
						state = 40;
					}
				}
				//If some other character is encountered, display error
				else{
					lexeme[lexeme_ptr]='\0';
					fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line, lexeme ,current_line);
					
					state = 52;
				}
			}break;

			case 41:{
				lexeme[lexeme_ptr] = '\0';
				addToken(token, AND, lexeme, -1, NULL);	
				final_state_reached = 1;				
			}break;

			case 42:{
				lexeme[lexeme_ptr] = '\0';
				addToken(token, NOT, lexeme, -1, NULL);	
				final_state_reached = 1;
			}break;

			//String
			case 43:{
				
				if((lexer_buffer_current[forward_pointer]>='a' && lexer_buffer_current[forward_pointer]<='z') || lexer_buffer_current[forward_pointer]==' '){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 44;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended, display error
						lexeme[lexeme_ptr]='\0';
						fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line, lexeme ,current_line);
						file_over = 1;
						state = 52;					
					}
					//Otherwise revisit the state
					else{
						state = 43;
					}
				}
				//If some other character is encountered, display error
				else{
					//If the file has ended, display error
					lexeme[lexeme_ptr]='\0';
					fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line, lexeme ,current_line);
						
					state = 52;
				}
			}break;

			case 44:{
				if(lexer_buffer_current[forward_pointer]=='\"'){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 45;
					forward_pointer++;
				}

				else if((lexer_buffer_current[forward_pointer]>='a' && lexer_buffer_current[forward_pointer]<='z') || lexer_buffer_current[forward_pointer]==' '){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 44;
					forward_pointer++;
				}

				//Refill the buffer
				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended, display error
						lexeme[lexeme_ptr]='\0';
						fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line, lexeme ,current_line);
						file_over = 1;
						state = 52;					
					}
					//Otherwise revisit the state
					else{
						state = 44;
					}
				}
				//If some other character is encountered, display error
				else{
					//If the file has ended, display error
					lexeme[lexeme_ptr]='\0';
					fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line, lexeme ,current_line);
					
					state = 52;
				}
			}break;

			case 45:{
				lexeme[lexeme_ptr] = '\0';

				//Subtract 2 for two ""
				if(strlen(lexeme)-2>20){
					fprintf(stderr, "Line No: %d, Lexical Error, String Length Exceeded -- %s\n",current_line, lexeme);
					state = 52;					
				}
				else{
					addToken(token, STR, lexeme, -1, NULL);	
					final_state_reached = 1;
				}

				
			}break;

			//Integer or Real
			case 46:{

				if(isdigit(lexer_buffer_current[forward_pointer])){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;

					//Revisit the state for more digits
					state = 46;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='.'){
					//Check whether this leads to a logical operator or RNUM further
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;
					state = 48;
					forward_pointer++;
				}

				//Refill the buffer
				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended, return NUM token
						state = 47;
						forward_pointer++;
						file_over = 1;					
					}
					//Otherwise revisit the state
					else{
						state = 46;
					}
				}

				//Other Character - return NUM token
				else{
					state = 47;
					forward_pointer++;
				}

			}break;

			//For NUM
			case 47:{
				lexeme[lexeme_ptr] = '\0';

				//Obtain Value from string
				int value = atoi(lexeme);
				token->value = (Value*)malloc(sizeof(Value));
				token->value->num = value;

				addToken(token, NUM, lexeme, 0, token->value);

				//retract
				forward_pointer--;	
				final_state_reached = 1;
			}break;

			case 48:{

				if(isdigit(lexer_buffer_current[forward_pointer])){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;

					//GO for RNUM
					state = 49;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='a'||lexer_buffer_current[forward_pointer]=='o'||lexer_buffer_current[forward_pointer]=='n'){
					//Check whether this leads to a logical operator or not

					//If yes, we need to return the NUM token and retract twice
					state = 51;
					forward_pointer++;
				}

				//Refill the buffer
				else if(lexer_buffer_current[forward_pointer]=='\0'|| lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended, atleast return the NUM
						file_over = 1;
						state = 51;	
					}
					//Otherwise revisit the state
					else{
						state = 48;
					}
				}

				//Other Character - Atleast return NUM and then see
				else{
					state = 51;	
					forward_pointer++;	
				}

			}break;

			case 49:{
				if(isdigit(lexer_buffer_current[forward_pointer])){
					lexeme[lexeme_ptr] = lexer_buffer_current[forward_pointer];
					lexeme_ptr++;

					//GO for RNUM
					state = 50;
					forward_pointer++;
				}

				else if(lexer_buffer_current[forward_pointer]=='\0' || lexer_buffer_current[forward_pointer]==-1){
					forward_pointer = 0;
					*file_ptr = getStream(*file_ptr);
					if(*file_ptr==NULL){
						//If the file has ended, display error
						lexeme[lexeme_ptr]='\0';
						fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line, lexeme ,current_line);
						file_over = 1;
						state = 52;	
					}
					//Otherwise revisit the state
					else{
						state = 49;
					}
				}

				//Other Character - Display Error
				else{
					
					lexeme[lexeme_ptr]='\0';
					fprintf(stderr, "Line No: %d, Lexical Error, Unknown pattern %s at line %d\n",current_line, lexeme ,current_line);
					
					state = 52;	
				}

			}break;

			//RNUM
			case 50:{
				lexeme[lexeme_ptr] = '\0';

				//Obtain Value from string
				float value = atof(lexeme);
				token->value = (Value*)malloc(sizeof(Value));
				token->value->r_num = value;

				addToken(token, RNUM, lexeme, 1, token->value);
				final_state_reached = 1;
			}break;

			//Retract Twice and return NUM
			case 51:{

				//-1 because . is also added in the string
				lexeme[lexeme_ptr-1] = '\0';
				
				//Obtain Value from string
				int value = atoi(lexeme);
				token->value = (Value*)malloc(sizeof(Value));
				token->value->num = value;

				addToken(token, NUM, lexeme, 0, token->value);

				//retract twice
				forward_pointer--;
				retract_twice = 1;
				
				final_state_reached = 1;
				
			}break;

			
			//ERROR TOKEN RETURN
			case 52:{

				//Pass the wrong lexeme as error token
				lexeme[lexeme_ptr] = '\0';
				
				addToken(token, ERROR, lexeme, -2, NULL);
				//-2 for error

				final_state_reached = 1;
				
			}break;

		}

		if(final_state_reached==1)
			break;
	}

	return token;
}



//Function for cleaning the file -- removing comments
 

	 

		 

	//Input file will always be closed by getStream function
	 


/***********************************************************************************************************************/

//Additional Functions added for modularity

//Function for adding the token fields
void addToken(Token* token, Tokentype type, char* lexeme, int valueType, Value* value){
	token->line_no = current_line;
	token->type = type;
	token-> valueType = valueType;
	token-> lexeme = lexeme;
	token-> value = value;
}


//Lexer has to be initialized by the parser before it calls for tokens
FILE* initialize_lexer(char *input_code_file){

	//Allocating Memory for buffers
	lexer_buffer_current = (char*)malloc(sizeof(char)*(BUFFER_SIZE+1));
	lexer_buffer_previous = (char*)malloc(sizeof(char)*(BUFFER_SIZE+1));
		
	//Initializing buffers to 0
	memset(lexer_buffer_current, 0, sizeof(lexer_buffer_current));
	memset(lexer_buffer_previous, 0, sizeof(lexer_buffer_previous));

	//Reinitialize the variables
    current_line=1; file_over = 0;  forward_pointer = 0;  retract_twice=0;
    

	//Initializing the LookUpTable
	lookUpTable = newTable(NO_SLOTS);

	FILE *file_ptr = fopen(input_code_file, "r");
	
	if(file_ptr == NULL){
		fprintf(stderr,"Error Opening File\n");
		return NULL;
	}

	else{
		printf("\n\n--------------------File Opened Successfully For Parsing------------------\n\n");
	}
	
    return file_ptr;

}


//Function for printing tokens of a particular Code file
void printTokenList(char *input_code_file){

	//Allocating Memory for buffers
	lexer_buffer_current = (char*)malloc(sizeof(char)*(BUFFER_SIZE+1));
	lexer_buffer_previous = (char*)malloc(sizeof(char)*(BUFFER_SIZE+1));
	
	
	//Initializing buffers to 0
	memset(lexer_buffer_current, 0, sizeof(lexer_buffer_current));
	memset(lexer_buffer_previous, 0, sizeof(lexer_buffer_previous));

	//Initializing the LookUpTable
	lookUpTable = newTable(NO_SLOTS);

	FILE *file_ptr = fopen(input_code_file, "r");

	if(file_ptr==NULL){
		fprintf(stderr,"Error Opening File\n");
		return;
	}
	else{
		printf("\n\nFile Opened Successfully For Printing Tokens\n\n");
	}

	//Reinitialize the variables
    current_line=1; file_over = 0;  forward_pointer = 0;  retract_twice=0;

	Token *token;

	printf("Printing tokens.............\n\n");
	
	token = getNextToken(&file_ptr);
	int i = 1;
	while(token!=NULL){
		if(token->valueType==-1)//
			printf("Line Number: %-5d     Lexeme: %22s               Token Type: %15s\n",token->line_no, token->lexeme, tokenMap[token->type]);
		if(token->valueType==0)
			printf("Line Number: %-5d     Lexeme: %22s               Token Type: %15s\n",token->line_no, token->lexeme, tokenMap[token->type]);
		if(token->valueType==1)
			printf("Line Number: %-5d     Lexeme: %22s               Token Type: %15s\n",token->line_no, token->lexeme, tokenMap[token->type]);
		if(token->valueType==-2)
			printf("Line Number: %-5d     Lexeme: %22s               Token Type: %15s\n",token->line_no, token->lexeme, tokenMap[token->type]);
		
		token = getNextToken(&file_ptr);
		i++;
	}

	printf("\n\nFile Closed\n\n");
	
	
	if(file_ptr!=NULL)
		fclose(file_ptr);	
	}

