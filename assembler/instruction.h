#ifndef _INSTRUCTION_H
#define _INSTRUCTION_H

#include "token.h"

typedef enum type {INT,VAR,FUNCLABEL,NONE} TYPE ;


typedef union _argument
{
		int value ;
		char name[128] ;

} ARGUMENT ;

typedef struct _targs
{
	TYPE type ;	
	ARGUMENT arg ;
	int value ;

} TARGS ;

typedef struct _instruction
{
	token topcode ;
	int	  opcode ;
	int   address ;
	int	  size ;
	TARGS  args[3] ;
	char opcodename[64] ;
	struct _instruction *next ;

} INSTRUCTION ;

INSTRUCTION *INSTRUCTIONAddOpcode(token) ;
void   INSTRUCTIONAdd(INSTRUCTION *ins) ;
void   INSTRUCTIONResolve(void) ;
void INSTRUCTIONSetInstruction(INSTRUCTION *ins,int opcode,
								     TYPE t1,void *a1,
									 TYPE t2,void *a2, 
									 TYPE t3,void *a3,int size) ;

void INSTRUCTIONSetArgument(INSTRUCTION *ins,TYPE type,void *arg,int index) ;

void INSTRUCTIONOutputBin(char *fname);
INSTRUCTION *INSTRUCTIONRoot(void) ;
#endif

