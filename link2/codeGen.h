/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "intermediateCodeDef.h"

void generate_code(TupleList* list, char* outfile);
void process_tuple(Tuple* tuple, FILE** fp);
void process_ASSIGNOP(Tuple* tuple, FILE** fp);
void process_PRINT(Tuple* tuple, FILE** fp);
void process_IF(Tuple* tuple, FILE** fp);
void process_LABEL(Tuple* tuple, FILE** fp);
void process_GOTO(Tuple* tuple, FILE** fp);