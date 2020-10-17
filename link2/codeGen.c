/****

Name: Shivankit Gaind

ID: 2015A7PS0076P

****/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "codeGen.h"
#include "optimizer.h"
#include "semanticAnalyzer.h"
#include "typeExtractor.h"
#include "symbolTable.h"
#include "ast.h"


void generate_code(TupleList* list, char* outfile){

	FILE* fp = fopen(outfile,"w");

	fprintf(fp,"%cinclude 'printInteger.asm'\nsection .data\nnewLine db 0ah\nsection .bss\nsection .text\nglobal _start\n_start:\nmov ebp,esp\n",'%');

	Tuple* tuple = list->head;

	while(tuple!=NULL){
		if(tuple->process==1)
			process_tuple(tuple,&fp);
		tuple = tuple->next;
	}

	fprintf(fp,"\n\nmov ebx,0\nmov eax,1\nint 80h");

	fclose(fp);
}

void process_tuple(Tuple* tuple, FILE** fp){
	if(tuple->op==ASSIGNOP){
		process_ASSIGNOP(tuple,fp);
	}
	else if(tuple->op==PRINT){
		process_PRINT(tuple,fp);
	}
	else if(tuple->op==IF){
		process_IF(tuple,fp);
	}
	else if(tuple->op==LABEL){
		process_LABEL(tuple,fp);
	}
	else if(tuple->op==GOTO){
		process_GOTO(tuple,fp);
	}

}

void process_ASSIGNOP(Tuple* tuple, FILE** fp){
	ASTNode* rhs = tuple->node1;
	ASTNode* lhs = tuple->node3;

	SymbolNode* info = findInfo(lhs);

	int offset = info->symbol->offset;
	int datatype = info->symbol->datatype;

	if(datatype==INT){
		fprintf(*fp, "mov ax, %d\n",rhs->val[0]);
		fprintf(*fp, "mov [ebp+%d], ax\n",offset);
	}
	else if(datatype==STRING){

		//printf("%s %d\n",info->symbol->str, info->symbol->width);

		char* str = info->symbol->str;
		int q = 0;
		while(str[q]!='\0'){
			fprintf(*fp, "mov ah, '%c'\n",str[q]);
			fprintf(*fp, "mov [ebp+%d], ah\n",offset+q);
			q++;
		}

	}
	else if(datatype==MATRIX){
		int* val = info->symbol->val;

		int q = 0;
		for(int i=0;i<info->symbol->dimension1;i++){
			for(int j=0;j<info->symbol->dimension2;j++){
				fprintf(*fp, "mov ax, %d\n",val[ i*info->symbol->dimension2 + j]);
				fprintf(*fp, "mov [ebp+%d], ax\n",offset+ q);

				//Width of int is 2
				q+=2;
			}
		}

	}

}

void process_PRINT(Tuple* tuple, FILE** fp){
	
	ASTNode* id = tuple->node1;

	SymbolNode* info = findInfo(id);

	int offset = info->symbol->offset;
	int datatype = info->symbol->datatype;

	if(datatype==INT){
		fprintf(*fp, "mov ax, [ebp+%d]\n",offset);
		fprintf(*fp, "call iprintLF\n");		
	}
	else if(datatype==STRING){
		fprintf(*fp, "add ebp, %d\nmov edx, %d\nmov  ecx, ebp\nmov ebx, 1\nmov eax, 4\nint 80h\n", offset, info->symbol->width);
		fprintf(*fp, "mov edx, 1\nmov  ecx, newLine\nmov ebx, 1\nmov eax, 4\nint 80h\n");

	}
	else if(datatype==MATRIX){
		int* val = info->symbol->val;

		int q = 0;
		for(int i=0;i<info->symbol->dimension1;i++){
			for(int j=0;j<info->symbol->dimension2;j++){

				fprintf(*fp, "mov ax, [ebp+%d]\n",offset+q);
				fprintf(*fp, "call iprintLF\n");
				//Width of int is 2
				q+=2;
			}
		}
	}
}

void process_IF(Tuple* tuple, FILE** fp){
	ASTNode* boolean_exp = tuple->node1;

	//Don't Jump
	if(boolean_exp->val[0]==1){
		
	}
	//Jump
	else{
		fprintf(*fp, "jmp %s\n", tuple->next->arg1);
	}
}

void process_LABEL(Tuple* tuple, FILE** fp){
	fprintf(*fp, "%s:\n", tuple->arg1);
}

void process_GOTO(Tuple* tuple, FILE** fp){
	fprintf(*fp, "jmp %s\n", tuple->arg1);
}
