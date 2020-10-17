/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#include <stdio.h>
#include <stdlib.h>

#include "lexerDef.h"

//Function Declarations

Token* getNextToken(FILE** file_ptr);
FILE* getStream(FILE* file_ptr);
void printTokenList(char *input_code_file);
void addToken(Token* token, Tokentype type, char* lexeme, int valueType, Value* value);
void removeComments(char *testcaseFile);
FILE* initialize_lexer(char *input_code_file);