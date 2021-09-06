#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


#include "token.h"
#include "opcode.h"
#include "instruction.h"



typedef struct _label
{
	char id[128] ;
	unsigned int address ;
	INSTRUCTION *instruction ;
	struct _label *next ;

} LABEL ;






static LABEL *stacklabel=NULL ;
static LABEL *rootlabel = NULL ;

LABEL *LABELPushLabel(char *name) ;

LABEL *LABELPushLabel(char *name)
{
	// Push Label onto Stack ...
	// use this when finding a label

	LABEL *lab ;
	lab = (LABEL *) malloc(sizeof(LABEL)) ;
	lab->instruction = NULL ;
	lab->address = 0 ;
	strcpy(lab->id,name) ;
	lab->next = stacklabel ;
	stacklabel=lab ;
	return lab ;

}

void LABELPopLabels(INSTRUCTION *ins)
{

	LABEL *lab=stacklabel ;

	while(stacklabel != NULL)
	{
		LABEL *oldnext ;

		stacklabel->instruction = ins ;
		// transfer from stack to root label
		oldnext=stacklabel->next ;
		stacklabel->next= rootlabel ;
		rootlabel = stacklabel ;
		stacklabel = oldnext ;

	}

}
void LABELResolveLabels()
{

	LABEL *label = rootlabel ;
	while(label != NULL)
	{
		if (label->instruction == NULL)
		{
				printf("Fucking Hell- We have a label without a instruction!!! (%s)\n",label->id) ;
		} else
		{
			label->address=label->instruction->address ;
		//	printf("Resolved %s address %d\n",label->id,label->address) ;
		}
		label=label->next ;
	}

}

int LABELGetAddress(char *name)
{

	LABEL *label = rootlabel ;

	while(label != NULL)
	{
		if (!strcmp(name,label->id))
		{
			// found label ID now see if its resolved
			if (label->instruction == NULL)
			{
				printf("This label (%s)has no instruction!!!\n",name) ;
				return -1 ;
			}
			return label->address ;
		}

		label=label->next ;
	}
	printf("LABEL %s NOT FOUND!!! \n",name) ;

	return -1 ;

}


typedef struct _variable
{
	char id[128] ;
	unsigned int address ;
	struct _variable *next ;

} VARIABLE ;

static VARIABLE *rootvar=NULL ;
static int varaddress=0 ;

void VARIABLEAdd(char *name) ;


void VARIABLEAdd(char *name)
{


		VARIABLE *var = (VARIABLE *)malloc(sizeof(VARIABLE)) ;
		var->address = varaddress ;
		strcpy(var->id,name) ;
		var->next=rootvar ;
		rootvar=var ;

		varaddress++ ;

}

int	VARIABLEGetAddress(char *name)
{
	VARIABLE *var = rootvar ;

	while(var != NULL)
	{
		if (!strcmp(name,var->id))
		{
			// found label ID now see if its resolved
			return var->address ;
		}

		var=var->next ;
	}
	printf("VAR %s NOT FOUND!!! \n",name) ;

	return -1 ;

}


void TARGSResolve(TARGS *t)
{
	switch(t->type)
	{
		case FUNCLABEL:
					t->value=LABELGetAddress(t->arg.name) ;
//					printf("FUNC:Resolve %s as %d\n",t->arg.name,t->value) ;
					break ;
		case VAR:
					t->value = VARIABLEGetAddress(t->arg.name) ;
//					printf("VAR:Resolve %s as %d\n",t->arg.name,t->value) ;
					break ;
		default:
					t->value=t->arg.value ;
					break ;
	}

}

int TARGSGetBaseType(TARGS *t)
{


	switch(t->type)
	{

		case NONE:
		case INT:
				return 0 ;
		default:
				return 1 ;


	}


}
TYPE Gettype(token tok)
{
	switch(tok)
	{
		case INTLITERAL:
						return INT ;
		case ID:
						return VAR ;

	}

}

void label()
{

	match(LABELID) ;
	LABELPushLabel(gettoken_buffer()) ;

}


void declare_id()
{

		match(DECLARE) ;
		match(ID) ;

		if (next_token() == INTEGER)
		{
//			printf("\rDeclare int %s\n",gettoken_buffer()) ;
			VARIABLEAdd(gettoken_buffer()) ;
			match(INTEGER) ;

		} else
		{
//			printf("\rDeclare function %s\n",gettoken_buffer) ;
			match(SUBROUTINE) ;

		}
}

int opcode()
{
	static char id1[512] ;
	static char id2[512] ;
	static char id3[512] ;
	static int address=0 ;
	INSTRUCTION *ins ;
	token tok  ;
	tok = next_token() ;
	ins=INSTRUCTIONAddOpcode(tok) ;
	LABELPopLabels(ins) ;

	strcpy(ins->opcodename,debug_token(tok) ) ;

	switch(tok)
	{


		case ADD:
		case SUB:
		case AND:
		case OR:
				{
				token ntok ;
				void *p1,*p2,*p3 ;
				TYPE t1,t2,t3 ;

				match(tok) ;

				ntok=next_token() ;
				t1=Gettype(ntok) ;
				match(ntok) ;
				strcpy(id1,gettoken_buffer()) ;
				if (t1 == INT)
				{
					p1 = (void *)atoi(id1) ;
				} else
				{
					p1 = id1 ;
				}



				ntok=next_token() ;
				t2=Gettype(ntok) ;
				match(ntok) ;
				strcpy(id2,gettoken_buffer()) ;
				if (t2 == INT)
				{
					p2 = (void *)atoi(id2) ;
				} else
				{
					p2 = id2 ;
				}

				ntok=next_token() ;
				t3=Gettype(ntok) ;
				match(ntok) ;
				strcpy(id3,gettoken_buffer()) ;
				if (t3 == INT)
				{
					p3 = (void *)atoi(id3) ;
				} else
				{
					p3 = id3 ;
				}

				INSTRUCTIONSetInstruction(ins,(int)tok,
									 t1,p1,
									 t2,p2,
									 t3,p3,4) ;

				printf("\r%04d:%s",address,debug_token(tok)) ;
				ins->address=address ;
				address+=4 ;

				}

				break ;


		case USER:

				match(USER) ;
				match(INTLITERAL) ;
				strcpy(id1,gettoken_buffer()) ;
				match(ID) ;
				strcpy(id2,gettoken_buffer()) ;

				INSTRUCTIONSetInstruction(ins,(int)tok,
									 INT,(void *)atoi(id1),
									 VAR,id2,
									 NONE,NULL,4) ;

				printf("\r%04d:USER",address) ;
				ins->address=address ;
				address+=4 ;
				break ;

		case FLAG:
				match(FLAG) ;
				match(ID) ;
				INSTRUCTIONSetInstruction(ins,(int)tok,
									 VAR,gettoken_buffer(),
									 NONE,NULL,
									 NONE,NULL,4) ;

				printf("\r%04d:FLG ",address) ;
				ins->address=address ;
				address+=4 ;
				break ;

		case CMP:
				match(CMP) ;
				match(ID) ;
				strcpy(id1,gettoken_buffer()) ;
				match(INTLITERAL) ;
				INSTRUCTIONSetInstruction(ins,(int)tok,
									 VAR,id1,
									 INT,(void *)atoi(gettoken_buffer()),
									 NONE,NULL,4) ;

				printf("\r%04d:CMP ",address) ;
				ins->address=address ;
				address+=4 ;
				break ;
		case WAIT:
		case RANDG:
		case RANDL:
		case TAKER:
		case ROUTE:
				match(tok) ;
				match(INTLITERAL) ;
				INSTRUCTIONSetInstruction(ins,(int)tok,
									 INT,(void *)atoi(gettoken_buffer()),
									 NONE,NULL,
									 NONE,NULL,4) ;

				printf("\r%04d:EXTF",address) ;
				ins->address=address ;
				address+=4 ;

				break ;

	case STORE:
				match(STORE) ;

				if (next_token() == ID)
				{
					match(ID) ;
					strcpy(id1,gettoken_buffer()) ;
					match(ID) ;
					strcpy(id2,gettoken_buffer()) ;


					INSTRUCTIONSetInstruction(
							ins,(int)tok,
							VAR,id1,
							VAR,id2,
							NONE,NULL,
							4) ;

				} else
				{
					match(INTLITERAL) ;
					strcpy(id1,gettoken_buffer()) ;
					match(ID) ;
					strcpy(id2,gettoken_buffer()) ;

					INSTRUCTIONSetInstruction(
							ins,(int)tok,
							INT,(void *)atoi(id1),
							VAR,id2,
							NONE,NULL,
							4) ;


				}
				printf("\r%04d:STOR",address) ;
				ins->address=address ;
				address+=4 ;
				break ;



		case EQ:
				match(EQ) ;

				match(ID) ;
				strcpy(id1,gettoken_buffer()) ;

				match(ID) ;
				strcpy(id2,gettoken_buffer()) ;

				match(ID) ;
				strcpy(id3,gettoken_buffer()) ;

				INSTRUCTIONSetInstruction(
							ins,(int)tok,
							VAR,id1,
							VAR,id2,
							VAR,id3,
							4) ;

				printf("\r%04d:EQU ",address) ;
				ins->address=address ;
				address+=4 ;

				break ;

		case GETROUTE:
				match(tok) ;
				match(ADDR) ;
				match(ID) ;
				INSTRUCTIONSetInstruction(
							ins,(int)tok,
							VAR,gettoken_buffer(),
							NONE,NULL,
							NONE,NULL,
							4) ;
				printf("\r%04d:GETR",address) ;
				ins->address=address ;
				address+=4 ;

				break ;

		case TEST:
				match(TEST) ;
				match(ID) ;

				INSTRUCTIONSetInstruction(
							ins,(int)tok,
							VAR,gettoken_buffer(),
							NONE,NULL,
							NONE,NULL,
							4) ;
				printf("\r%04d:TEST",address) ;
				ins->address=address ;
				address+=4 ;
				break ;

		case BRF:
		case BRT:
		case BSR:
		case BRA:
				match(tok) ;
				match(ID) ;
				INSTRUCTIONSetInstruction(
							ins,(int)tok,
							FUNCLABEL,gettoken_buffer(),
							NONE,NULL,
							NONE,NULL,
							4) ;
				printf("\r%04d:BRA ",address) ;
				ins->address=address ;
				address+=4 ;

				break ;
		case RETURN:
				match(RETURN) ;
				INSTRUCTIONSetInstruction(
							ins,(int)tok,
							NONE,NULL,
							NONE,NULL,
							NONE,NULL,
							4) ;

				printf("\r%04d:RET ",address) ;
				ins->address=address ;
				address+=4 ;

				break ;
		case HALT:
				match(HALT) ;
				INSTRUCTIONSetInstruction(
							ins,(int)tok,
							NONE,NULL,
							NONE,NULL,
							NONE,NULL,
							4) ;

				printf("\r%04d:HALT",address) ;
				ins->address=address ;
				address+=4 ;
				break ;

		case SCANEOF:
				break ;

		default:
				printf("Ekkkk! %s\n",debug_token(tok));
				match(tok) ;
				break ;
	}
	return address+ins->size;

}

int system_goal(FILE *fin)
{
	int size ;

	token tok ;

	SetFile(fin) ;

	while(1)
	{

		tok = next_token() ;
		switch(tok)
		{
			case LABELID:
						label() ;
						break ;
			case DECLARE:
						declare_id() ;
						break ;
			case SCANEOF:
						return size;
			default:
						size=opcode() ;
						break ;

		}

	}

}

int Assemble(char *infilename,char *outfilename)
{
FILE *fin ;
int size ;

	fin=fopen(infilename,"rb") ;

	if (!fin)
	{
		fprintf(stderr,"Failed to open file %s\n",infilename) ;
		return 0 ;
	}
	size=system_goal(fin) ;		// parse file
	fclose(fin) ;

	// now go through  resolving labels and vars
	LABELResolveLabels() ;
	INSTRUCTIONResolve() ;
	INSTRUCTIONOutputBin(outfilename) ;
	printf("\n\n;Assembled %s size %d bytes\n",outfilename,size) ;
	return 1 ;

}
int main(int argc,char **argv)
{

static char infilename[512] ;
static char outfilename[512] ;


	if (argc < 3)
	{
		strcpy(infilename,"c:\\eg.asm") ;
		strcpy(outfilename,"c:\\eg.bin") ;

		fprintf(stderr,"\nFormally known as Swiv2 Assembler V1.0\n") ;
		fprintf(stderr,"***ASSEMBLER ERROR!!!***\n") ;
		fprintf(stderr,"\nUsage %s sourcefile outfile\n",argv[0]) ;
		return 0 ;

	} else
	{
		strcpy(infilename,argv[1]) ;
		strcpy(outfilename,argv[2]) ;

	}

	if (Assemble(infilename,outfilename))
	{
	//	Simulate(outfilename) ;
		return 1 ;
	}
	// failed to assemble file

	return 0 ;
}
