#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opcode.h"
#include "instruction.h"
#include "targs.h"

static INSTRUCTION *root = NULL ;
static INSTRUCTION *base = NULL ;

INSTRUCTION *INSTRUCTIONRoot()
{
	return root ;
}

void INSTRUCTIONResolve(void)
{

	INSTRUCTION *ins ;
	ins = root ;
	while(ins != NULL)
	{

		TARGSResolve(&ins->args[0]) ;
		TARGSResolve(&ins->args[1]) ;
		TARGSResolve(&ins->args[2]) ;

		ins=ins->next ;
	}

}

void INSTRUCTIONAdd(INSTRUCTION *ins)
{

	if (base == NULL)
	{
			base=root=ins ;
			ins->next = NULL ;
	} else
	{
			base->next = ins ;
			base=ins ;
			ins->next=NULL ;
	}


}

INSTRUCTION * INSTRUCTIONAddOpcode(token tok)
{
	INSTRUCTION *ins ;
	ins= (INSTRUCTION *)malloc(sizeof(INSTRUCTION)) ;
	ins->topcode = tok ;
	ins->opcode = 0 ;		// undefined opcode
	ins->size=0 ;
	ins->address=0 ;

	ins->args[0].arg.value=0 ;
	ins->args[1].arg.value=0 ;
	ins->args[2].arg.value=0 ;


	INSTRUCTIONAdd(ins) ;
	return ins ;

}


void INSTRUCTIONSetArgument(INSTRUCTION *ins,TYPE type,void *arg,int index)
{

	ins->args[index].type=type ;
	switch(type)
	{
		case INT:
				ins->args[index].arg.value=(int)arg ;
				break ;
		case FUNCLABEL:
		case VAR:
				strcpy(ins->args[index].arg.name,(char *)arg) ;
				break ;
	}

}

void INSTRUCTIONSetInstruction(INSTRUCTION *ins,int opcode,
								     TYPE t1,void *a1,
									 TYPE t2,void *a2,
									 TYPE t3,void *a3,int size)
{

	ins->opcode=opcode ;

	INSTRUCTIONSetArgument(ins,t1,a1,0) ;
	INSTRUCTIONSetArgument(ins,t2,a2,1) ;
	INSTRUCTIONSetArgument(ins,t3,a3,2) ;


}



int INSTRUCTIONFormBase(INSTRUCTION *ins)
{
	// defines lower three bits of instructions
	// opcode depending on type of arguments
	// bit 0 defines type of arg[0]
	// bit 1 defines type of arg[1]
	// bit 2 defines type of arg[2]

	// This does not mean that there are only 32 opcodes
	// (i.e IT is not true that 5 bits define the base
	// of the opcode .

	// Some instructions make PART OR NO use of base bits.
	// eg. RETURN instruction uses none!
	// TAKER uses 1 bit (bit 0)
	// This only means that there can only be 32 instructions
	// with 3 parameter types. (e.g `EQ a b temp` type instruction-
	// infact EG is a bad example cos - third param must always be
	// a ID)


	TARGS *t ;
	int base ;

	base=0 ;
	t=&ins->args[2] ;
	base+=TARGSGetBaseType(t) ;
	base<<=1 ;
	t=&ins->args[1] ;
	base+=TARGSGetBaseType(t) ;
	base<<=1 ;
	t=&ins->args[0] ;
	base+=TARGSGetBaseType(t) ;

	return base ;

}

unsigned long INSTRUCTIONFormArgs(INSTRUCTION *ins)
{
	unsigned long args ;
	unsigned long base,binop ;

	OPCODE_TABLE *opent ;
	OPCODE opcode ;

	opent=token2op(ins->topcode) ;
	binop=opcode=opent->opcode ;
	base=INSTRUCTIONFormBase(ins) ;
	binop<<=27;

	switch(opcode)
	{
		case BRAOP:
		case BRFOP:
		case BRTOP:
		case BSROP:
			args=((-ins->address+ins->args[0].value)>>2)-1 ;
			return binop | (args<<3) | base ;

		default:
			args = (ins->args[0].value & 255) |
				((ins->args[1].value & 255)<<8) |
				((ins->args[2].value & 255)<<16) ;
			return binop | (args<<3) | base ;

	}

}

void INSTRUCTIONOutputInstruction(INSTRUCTION *ins,FILE *fo)
{
	unsigned long binop ;
	binop=INSTRUCTIONFormArgs(ins) ;
	fwrite(&binop,sizeof(binop),1,fo) ;


}



void INSTRUCTIONOutputBin(char *fname)
{
	FILE *fout ;
	INSTRUCTION *ins ;

	fout=fopen(fname,"wb") ;

	if (fout == NULL)
	{
			fprintf(stderr,"Failed to open binary file %s for output\n") ;
			return ;
	}

	ins=INSTRUCTIONRoot() ;
	while(ins != NULL)
	{

			INSTRUCTIONOutputInstruction(ins,fout) ;
			ins=ins->next ;

	}

	fclose(fout) ;

}
