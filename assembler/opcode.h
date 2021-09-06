#ifndef _OPCODE_H
#define _OPCODE_H

#include "token.h"

typedef enum opcode_types
{

	TAKEROP=0,
	RANGEOP,EXISTSOP,ROUTEOP,RANDGOP,RANDLOP,
	GETROUTEOP,GETOBJECTOP,GETLEADEROP,WAITOP,MSGOP,
	CMPOP,
	TESTOP,EQOP,
	FLAGOP,
	RETURNOP,HALTOP,
	BRFOP,BRTOP,BRAOP,BSROP,
	ANDOP,OROP,ADDOP,SUBOP,USEROP,NOPOP,
	STOREOP
} OPCODE ;

typedef struct opcode_table
{
	OPCODE opcode ;
	token token ;
	int	basemask ;

} OPCODE_TABLE ;

//extern OPCODE_TABLE opcode_table[];
OPCODE_TABLE *token2op(token tok) ;
token opcode2token(OPCODE) ;

#endif
