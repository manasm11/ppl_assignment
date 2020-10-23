/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#ifndef _LEXERDEF_
#define _LEXERDEF_

#include <stdio.h>
#include <stdlib.h>


#define total_token_types 58

//IMPORTANT

//The last three tokens are meant for parsing purposes and error reporting only


//List of all possible token types
typedef enum{
	ASSIGNOP,
	COMMENT,
	FUNID,
	NUM,
	RNUM,
	STR,
	END,
	INT,
	STRING,
	MATRIX,
	MAIN,
	OP,
	CL,
	COMMA,
	IF,
	ELSE,
	ENDIF,
	READ,
	PRINT,
	FUNCTION,
	NOT,
	LT,
	LE,
	EQ,
	GT,
	GE,
	NE,
	DOLLAR,
	ERROR,
	LABEL,
	GOTO,DECLARE,ID, COLON,LIST,OF, VARIABLES,INTEGER,SEMICOLON,
REAL,BOOLEAN,JAGGED, ARRAY,SQO,DOT,SQC,R1,SIZE,VALUES,LCB,RCB,PLUS,MINUS,MUL,DIV,AND,OR,EPSILON
}Tokentype;



//The value corresponding to a token can either be integer or real
typedef union{
	int num;
	float r_num;
}Value;


struct token{
	Tokentype type; 
	int valueType; //Valuetype: 0 for int, 1 for real, -1 for other, -2 for ERROR
	int line_no; //Line number of the token
	char* lexeme; //Actual Lexeme
	Value* value; //For integers and real numbers
};

typedef struct token Token;


#endif
