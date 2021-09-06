#ifndef _TOKEN_H
#define _TOKEN_H

#define TOKBUFSZ (512)


typedef enum token_types
{
	LABELID=0,ID,
	DECLARE,
	ADDR,
	INTLITERAL,
	TAKER,
	RANGE,EXISTS,ROUTE,RANDG,RANDL,
	GETROUTE,GETOBJECT,GETLEADER,WAIT,MSG,
	CMP,TEST,EQ,
	FLAG,
	RETURN,HALT,
	BRF,BRT,BRA,BSR,
	COMMENT,
	SUBROUTINE,INTEGER,
	AND,OR,ADD,SUB,STORE,
	NOP,USER,
	SCANEOF

} token ;

typedef struct token_table
{
	char  *tokstring ;
	token tok ;

} TOKEN_TABLE ;
token next_token(void) ;
void match(token tok) ;
token check_reserved(void) ;
char *debug_token(token tok) ;
char *gettoken_buffer() ;
void SetFile(FILE *f) ;


token scanner(FILE *f) ;
extern char token_buffer[TOKBUFSZ] ;
extern int token_cnt ;

#endif
