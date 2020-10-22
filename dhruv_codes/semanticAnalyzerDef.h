/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#ifndef _SEMANTICANALYZERDEF_
#define _SEMANTICANALYZERDEF_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbolTableDef.h"

#define ERROR_STRING_SIZE 100

struct error{
	int line_no;
	char* str;
	struct error*  next;

};

typedef struct error Error;


struct errorList{
	Error* head;
	int total_errors;
};

typedef struct errorList ErrorList;

#endif