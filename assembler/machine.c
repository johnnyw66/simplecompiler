#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "opcode.h"
#define OBJECT void
#define OBJECTID int 
#include "machine.h"

// Machine Simulator
#define ASSERT(_A) 
#define TRUE  (-1) 
#define FALSE (!TRUE)

unsigned long FindType(unsigned long v)
{
	// TODO
	return 0 ;
}

unsigned long FindObjectInRange(void *o,unsigned long v,unsigned long v2)
{
	//TODO

	return 0 ;
}


unsigned long OBJECTGetRoute(void *o)
{

	return 0 ;
}

unsigned long OBJECTSetRoute(void *o,unsigned long v)
{

	return 0 ;
}


CPU *CPUCreate(char *memory,OBJECT *object)
{
	int i ;
	CPU *cpu ;
	cpu=(CPU *)malloc(sizeof(CPU)) ;
	ASSERT(cpu) ;
	cpu->stack.stackptr=0 ;
	cpu->pc=0 ;
	cpu->memory=(OPCODE *)memory ;
	cpu->ticks=0 ;
	cpu->state = RUNNING ;
	cpu->object =object ;
	cpu->trace=TRUE ;
	cpu->flag=0 ;

	for(i=0 ; i < VARMEMORY ; i++)
	{
		cpu->var[i]=0 ;			
	}

//	cpu->objectid = object->objectid ;
	return cpu ;
}
void CPUTraceOn(CPU *cpu)
{
	cpu->trace=TRUE ;
}

unsigned long CPUGetVar(CPU *cpu,unsigned long offset)
{
	
	if (offset > VARMEMORY) 
	{
			printf("GPE READ Error Memory Request out of bounds %x\n",offset) ;
			return 0 ;
	}
	return cpu->var[offset] ;
}

unsigned long CPUSetVar(CPU *cpu,unsigned long offset,unsigned long value)
{
	
	if (offset > VARMEMORY) 
	{
			printf("GPE WRITE Error Memory Request out of bounds %x\n",offset) ;
			return 0 ;
	}
	cpu->var[offset]=value ;
	return cpu->var[offset] ;


}


void CPUTraceOff(CPU *cpu)
{
	cpu->trace=FALSE ;
}

void CPUReset(CPU *cpu)
{
	ASSERT(cpu) ;
	cpu->stack.stackptr=0 ;
	cpu->pc=0 ;
	cpu->ticks=0 ;
	cpu->state = RUNNING ;
}

void CPUSetTrue(CPU *cpu)
{
	ASSERT(cpu) ;
	cpu->flag = TRUE ;

}

void CPUSetFalse(CPU *cpu)
{
	ASSERT(cpu) ;
	cpu->flag = FALSE ;
}


long CPUTestTrue(CPU *cpu)
{
	ASSERT(cpu) ;
	return ((cpu->flag == TRUE) ? TRUE : FALSE ) ;

}

long CPUTestFalse(CPU *cpu)
{
	ASSERT(cpu) ;
	return ((cpu->flag == FALSE) ? TRUE : FALSE ) ;
}


CPUSTATE CPUExecute(CPU *cpu)
{
	OPCODE opcode ;
	unsigned long arg[3] ;
	unsigned long base ;

	cpu->ticks++ ;


	if (cpu->state == WAITING)
	{
		printf("Waiting for ticks to go down ticks = %d waiting for %d\n",cpu->ticks,cpu->waiting_for) ;

		if (cpu->ticks < cpu->waiting_for)
		{
			CPUTrace(cpu) ;
			return WAITING ;	
		} 
		cpu->state = RUNNING ;
	}

	while(cpu->state == RUNNING) 
	{
		unsigned long opc = (unsigned long)cpu->memory[cpu->pc++] ;
		cpu->opcode=opcode=(OPCODE)((opc &  0xff000000)>>27);

		base=opc & 7 ;
	
		if (((base & 1) != 0))
		{
			arg[0] = CPUGetVar(cpu,GETVAR1(opc)) ;
		} else
		{
			arg[0] = GETVAR1(opc) ; 
		}
		if (((base & 2) != 0))
		{
			arg[1] = CPUGetVar(cpu,GETVAR2(opc)) ;
		} else
		{
			arg[1] = GETVAR2(opc) ; 
		}
		if (((base & 4) != 0))
		{
			arg[2] = CPUGetVar(cpu,GETVAR3(opc)) ;
		} else
		{
			arg[2] = GETVAR3(opc) ; 
		}
				

		CPUTrace(cpu) ; 
		printf("opc %04x (base %x):",opc>>3,base) ; getchar() ;

		switch(opcode)
		{	


			case USEROP:
					printf("USER\n") ;
					break ;

			case CMPOP:
					printf("Comparing %d with %d\n",arg[0],arg[1]) ;
					if (arg[0] == arg[1])		
					{
							printf("Compare True\n") ;
							CPUSetTrue(cpu) ;
										
					} else
					{
							printf("Compare False\n") ;
							CPUSetFalse(cpu) ;
					}
				
					break ;
	
			case FLAGOP:
					CPUSetVar(cpu,arg[0],cpu->flag) ;
					break ;

			case GETROUTEOP:
					break ;

			case TESTOP:
					printf("testing %d\n",arg[0]) ;
					if (arg[0] == TRUE )
					{
							printf("Set True for testop\n") ;	
							CPUSetTrue(cpu) ;
										
					} else
					{
							printf("Set False for testop\n") ;
							CPUSetFalse(cpu) ;
					}
					break ;

			case EQOP:
					if (arg[0] == arg[1])
					{
						printf("Equate == Setting TRUE in var %d\n",GETARG3(opc)) ;
						CPUSetVar(cpu,GETARG3(opc),TRUE) ;
													
					} else
					{
						printf("Equate != Setting FALSE in var %d\n",GETARG3(opc)) ;
						CPUSetVar(cpu,GETARG3(opc),FALSE) ;
					}
					break ;
			
			case TAKEROP:
					printf("TAKE-ROUTE %d\n",arg[0]) ;
					OBJECTSetRoute(cpu->object,arg[0]) ;
					break ;	

			case BRAOP:
				
					cpu->pc+=GETBRANCH(opc) ;
					break ;

			case BRTOP:
					
					if (CPUTestTrue(cpu))
					{
						cpu->pc+=GETBRANCH(opc) ;
					}
					break ;

			case BRFOP:
				
					if (CPUTestFalse(cpu))
					{
						cpu->pc+=GETBRANCH(opc) ;
					}
					break ;
			case BSROP:
				
					if (STACKpush(&cpu->stack,cpu->pc))
					{	
						cpu->pc+=GETBRANCH(opc) ;
					} else
					{
							printf("Stack OverFlow\n") ;
							return CRASHED ;
					}	
					break ;
					
			case RETURNOP:
						if (0==(cpu->pc=STACKpop(&cpu->stack)))
						{
							printf("Stack Underflow\n") ;
							return CRASHED ;	
						}
					
						break ;
		
			case HALTOP:
						cpu->state=HALTED ;
						return HALTED ;
			
			case	ROUTEOP:
					if (GETROUTE(opcode)  == OBJECTGetRoute(cpu->object))
					{
						CPUSetTrue(cpu) ;
					} else
					{
						CPUSetFalse(cpu) ;
					}
					break ;
					

			case	RANDLOP:
					if ((rand() & 255) < GETPAR(opc))
					{
						CPUSetTrue(cpu) ;
					} else
					{
						CPUSetFalse(cpu) ;
					}
					break ;
		
			case	RANDGOP:
					if ((rand() & 255) > GETPAR(opc))
					{
						CPUSetTrue(cpu) ;
					} else
					{
						CPUSetFalse(cpu) ;
					}
					break ;

		
			case EXISTSOP:
					if (FindType(GETOBJECTTYPE(opcode)) > 0)
					{
						CPUSetTrue(cpu) ;
					} else
					{
						CPUSetFalse(cpu) ;
					}
					break ;

			case RANGEOP:
					if (FindObjectInRange(
						cpu->object,
						GETOBJECTID(opc),
						GETRANGE(opc)
						) > 0 )
					{
						CPUSetTrue(cpu) ;

					} else
					{	
						CPUSetFalse(cpu) ;
					}

					break ;


			case	WAITOP:
					cpu->waiting_for=cpu->ticks+arg[0] ;
					cpu->state=WAITING ;
					return WAITING ;

			default:
					printf("UNRECOGNISED OPCODE %x\n",opcode) ; getchar() ;
					break ;
					cpu->state=HALTED ;
					return HALTED;


		}
		
	
	}

	return cpu->state ;

}


void CPUTrace(CPU *cpu)
{
	if (cpu->trace)
	{
		printf("\n%04x OPCODE %x flag %x\n",cpu->pc-1,cpu->opcode,cpu->flag) ;
		printf("=%s\n",debug_token(opcode2token(cpu->opcode))) ;
	}

}

int  STACKpush(STACK *stack,unsigned long val)
{

	if (stack->stackptr == MAX_STACK_SIZE) 
	{
			
		return 0 ;
	}
	stack->stack[stack->stackptr] = val ;
	stack->stackptr++ ;
	return -1 ;
}

unsigned long STACKpop(STACK *stack)
{
	unsigned long v ;
	if (stack->stackptr == 0)
	{
		printf("Underflow Stack Error\n") ;
		return 0 ;
	}
	v=stack->stack[--stack->stackptr] ;
	return v ;
}
char *LoadBin(char *file)
{

	FILE *fin ; 
	int size ;
	char *mem ;

	fin=fopen(file,"rb") ;
	if (fin == NULL)
	{
		fprintf(stderr,"error opening bin file %s\n",file) ;
		return NULL ;
	}
	fseek(fin,0,SEEK_END) ;
	size=ftell(fin) ;
	fseek(fin,0,SEEK_SET) ;
	mem=(char *)malloc(size) ;
	if (mem == NULL)
	{
		fclose(fin) ;
		fprintf(stderr,"error alloc memory for bin file\n") ;
		return NULL;
				
	}
	fread(mem,1,size,fin) ;
	fclose(fin) ;
	return mem ;

}

void Simulate(char *file)
{
	CPUSTATE state ;
	CPU *cpu ;
	char *mem ;
	mem=LoadBin(file) ;

	cpu=CPUCreate(mem,NULL) ;
	do 
	{
		state=CPUExecute(cpu) ;

	} while(state == WAITING) ;

	printf("Finished with state %d\n",(int)state) ;

}