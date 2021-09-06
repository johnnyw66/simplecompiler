#ifndef _MACHINE_H
#define _MACHINE_H

typedef enum _CPUSTATE {RUNNING,HALTED,WAITING,CRASHED} CPUSTATE;

#define MAX_STACK_SIZE (32) 

typedef struct _stack
{
	unsigned long stackptr ;
	unsigned long stack[MAX_STACK_SIZE] ;

} STACK ;

int  STACKpush(STACK *stack,unsigned long val) ;
unsigned long STACKpop(STACK *stack) ;

#define		GETBRANCH(_A)		((_A>>3) & 0x00000ff) 
#define		GETPAR(_A)			((_A>>3) & 0x00ffffff) 
#define		GETOBJECTID(_A)		((_A>>3) & 0x00000fff)
#define		GETOBJECTTYPE(_A)	((_A>>3) & 0x00000fff)
#define		GETRANGE(_A)		((_A>>3) & 0x00fff000)
#define		GETROUTE(_A)		((_A>>3) & 0x00ffffff)
#define		GETTIME(_A)			((_A>>3) & 0x00ffffff)
#define		GETVAR(_A)			((_A>>3) & 0x00ffffff)
			
#define		GETARG1(_A)			((_A>>3) & 0x000000ff)			
#define		GETARG2(_A)			(((_A>>3) & 0x0000ff00)>>8)			
#define		GETARG3(_A)			(((_A>>3) & 0x00ff0000)>>16)			

#define		GETVAR1(_A)			((_A>>3) & 0x000000ff)			
#define		GETVAR2(_A)			(((_A>>3) & 0x0000ff00)>>8)			
#define		GETVAR3(_A)			(((_A>>3) & 0x00ff0000)>>16)			


#define		VARMEMORY (32) 


typedef struct _cpu 
{
	long	pc ;
	long	flag ;
	long	trace ;
	long	*memory ;
	OPCODE	opcode ;
	OBJECT *object ;
	OBJECTID objectid ;		/* Object executing this code */
	CPUSTATE	state ;
	unsigned long	ticks ;
	unsigned long	waiting_for ;
	unsigned long var[VARMEMORY] ;
	STACK	stack ;

} CPU ;


CPU *CPUCreate(char *memory,OBJECT *object) ;
void CPUReset(CPU *cpu) ;
CPUSTATE CPUExecute(CPU *cpu) ;
void CPUSetTrue(CPU *cpu) ;
void CPUSetFalse(CPU *cpu) ;
long CPUTestTrue(CPU *cpu) ;
long CPUTestFalse(CPU *cpu) ;
void CPUTraceOn(CPU *cpu) ;
void CPUTraceOff(CPU *cpu) ;
void CPUTrace(CPU *cpu) ;

#endif


