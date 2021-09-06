#include <stdio.h>
#include "opcode.h"

static OPCODE_TABLE opcode_table[]=
{

	{TAKEROP,TAKER,1},
	{RANGEOP,RANGE,1},
	{EXISTSOP,EXISTS,1},
	{ROUTEOP,ROUTE,1},
	{RANDGOP,RANDG,1},
	{RANDLOP,RANDL,1},
	{GETROUTEOP,GETROUTE,1},
	{GETOBJECTOP,GETOBJECT,1},
	{GETLEADEROP,GETLEADER,1},
	{WAITOP,WAIT,1},
	{MSGOP,MSG,1},
	{CMPOP,CMP,3},
	{USEROP,USER,3},
	{TESTOP,TEST,1},
	{EQOP,EQ,7},
	{FLAGOP,FLAG,1},
	{RETURNOP,RETURN,0},
	{HALTOP,HALT,0},
	{BRFOP,BRF,1},
	{BRTOP,BRT,1},
	{BRAOP,BRA,1},
	{BSROP,BSR,1},
	{ANDOP,AND,7},
	{OROP,OR,7},
	{ADDOP,ADD,7},
	{SUBOP,SUB,7},
	{STOREOP,STORE,3}
	
} ;


OPCODE_TABLE *token2op(token tok) 
{
	int i ;

	for(i=0 ; i < STOREOP ; i++)
	{
		if (tok == opcode_table[i].token)
			return &opcode_table[i] ;
	}
	printf("token2op::TOKEN NOT FOUND! for %s\n",debug_token(tok)) ;
	return  &opcode_table[0] ;
}

token opcode2token(OPCODE opcode) 
{
	int i ;

	for(i=0 ; i < STOREOP ; i++)
	{
		if (opcode == opcode_table[i].opcode)
			return opcode_table[i].token ;
	}
	return  NOP ;
}

