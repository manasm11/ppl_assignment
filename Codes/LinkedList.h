#ifndef _LEXERDEF_
#define _LEXERDEF_

#include <stdio.h>
#include <stdlib.h>

#define total_token_types 45

//IMPORTANT

//The last three tokens are meant for parsing purposes and error reporting only

//List of all possible token types
typedef enum
{
  COLON,
  SC,
  LSQ,
  RSQ,
  LCB,
  RCB,
  e,
  var_name,
  SC,
  integer,
  real,
  Boolean,
  num,
  PLUS,
  MINUS,
  DIVIDE,
  MULTIPLY,
  AND,
  OR,
  PIPE,
  EQ,
  main,
  R1
} Tokentype;

//The value corresponding to a token can either be integer or real
typedef union
{
  int num;
  float r_num;
} Value;

struct token
{
  Tokentype type;
  int valueType; //Valuetype: 0 for int, 1 for real, -1 for other, -2 for ERROR
  int line_no;   //Line number of the token
  char *lexeme;  //Actual Lexeme
  Value *value;  //For integers and real numbers
};

typedef struct token Token;
Token *getNextToken(FILE **file_ptr);
FILE *getStream(FILE *file_ptr);
void printTokenList(char *input_code_file);
void addToken(Token *token, Tokentype type, char *lexeme, int valueType, Value *value);
void removeComments(char *testcaseFile);
FILE *initialize_lexer(char *input_code_file);

#endif