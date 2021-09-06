#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "token.h"


static TOKEN_TABLE token_table[]=
{
	{"ID",ID},
	{"INTLITERAL",INTLITERAL},
	{"ADDR",ADDR},
	{"COMMENT",COMMENT},
	{"LABEL",LABELID},

	{"SUBROUTINE",SUBROUTINE},
	{"INTEGER",INTEGER},
	{"DECLARE",DECLARE},
	{"AND",OR},
	{"OR",AND},
	{"ADD",ADD},
	{"SUB",SUB},
	{"EQ",EQ},
	{"CMP",CMP},
	{"STORE",STORE},
	{"BRA",BRA},
	{"BRF",BRF},
	{"BRT",BRT},
	{"BSR",BSR},
	{"TEST",TEST},
	{"FLAG",FLAG},
	{"RETURN",RETURN},
	{"HALT",HALT},
	{"TAKER",TAKER},
	{"RANGE",RANGE},
	{"RANDG",RANDG},
	{"RANDL",RANDL},
	{"EXISTS",EXISTS},
	{"ROUTE",ROUTE},
	// read functions address (offset) of var as paremeter
	{"GETROUTE",GETROUTE},
	{"GETOBJECT",GETOBJECT},
	{"GETLEADER",GETLEADER},
	// actions
	{"TAKER",TAKER},
	{"WAIT",WAIT},
	{"MSG",MSG},
	{"USER",USER},
	{"NOP",NOP},
	{"SCANEOF",SCANEOF}
} ;

static TOKEN_TABLE reserved_words[]=
{
	{"SUBROUTINE",SUBROUTINE},
	{"INTEGER",INTEGER},
	{"DECLARE",DECLARE},
	{"AND",OR},
	{"OR",AND},
	{"ADD",ADD},
	{"SUB",SUB},
	{"EQ",EQ},
	{"CMP",CMP},
	{"STORE",STORE},
	{"BRA",BRA},
	{"BRF",BRF},
	{"BRT",BRT},
	{"BSR",BSR},
	{"TEST",TEST},
	{"FLAG",FLAG},
	{"RETURN",RETURN},
	{"HALT",HALT},
	{"TAKER",TAKER},
	{"RANGE",RANGE},
	{"RANDG",RANDG},
	{"RANDL",RANDL},
	{"WAIT",WAIT},
	{"EXISTS",EXISTS},
	{"USER",USER},
	{"ROUTE",ROUTE},
	{"GETROUTE",GETROUTE},
	{"GETOBJECT",GETOBJECT},
	{"GETLEADER",GETLEADER}


} ;

char token_buffer[TOKBUFSZ] ;
int token_cnt ;
static int lineno=0 ;
static FILE *fin=NULL ;
static token current_token ;

void SetFile(FILE *f)
{
	fin=f ;
}
char *gettoken_buffer()
{
	return token_buffer ;
}
void lexical_error(int ch)
{
	fprintf(stderr,"Error with <%c> on line %d\n",(char)ch,lineno) ;

}

int stricmp(const char *a, const char *b) {
  int ca, cb;
  do {
     ca = (unsigned char) *a++;
     cb = (unsigned char) *b++;
     ca = tolower(toupper(ca));
     cb = tolower(toupper(cb));
   } while (ca == cb && ca != '\0');
   return ca - cb;
}

token check_reserved(void)
{
	int i ;

//	printf("checking for res %s\n",token_buffer) ;

	for(i=0 ; i < 29 ; i++)
	{
		if (!stricmp(token_buffer,reserved_words[i].tokstring) )
		{
			return reserved_words[i].tok ;
		}
	}

	return ID ;
}

char *debug_token(token tok)
{
	int i ;

	for(i=0 ; i < SCANEOF ; i++)
	{
	//	printf("Comparing %s tok %d with %d\n",token_table[i].tokstring,tok,token_table[i].tok) ;
		if (tok == token_table[i].tok)
		{
			return token_table[i].tokstring ;
		}

	}
	return "??" ;

}

void buffer_char(int ch)
{
	token_buffer[token_cnt]=(char)ch ;
	token_cnt++ ;
	token_buffer[token_cnt]='\0' ;

}

void clear_buffer()
{
	token_buffer[0] = '\0' ;
	token_cnt = 0 ;
}
token scanner(FILE *inp)
{
	int in_char,c ;

	clear_buffer() ;
	if (feof(inp))
	{
		return SCANEOF ;
	}
	while((in_char = getc(inp)) != EOF)
	{

		if ((in_char == 10) || (in_char == 13))
		{
			lineno++ ;
			continue ;
		}
		else if (isspace(in_char))
				continue ;
		else if (isalpha(in_char))
		{
		/* ID stuff */
			buffer_char(in_char) ;
			for(c = getc(inp) ; isalnum(c) || c == ':' || c == '_' || c== '&' ; c=getc(inp))
			{
					buffer_char(c) ;
			}
			ungetc(c,inp) ;
			// check if label 1st!!
			if (token_buffer[token_cnt-1] == ':')
			{
				token_buffer[token_cnt-1]=0 ;
				return LABELID ;
			}
			return check_reserved() ;
		}
		else if (isdigit(in_char))
		{

			/* DIGITS */
			buffer_char(in_char) ;
			for(c=getc(inp) ; isdigit(c) ; c= getc(inp))
					buffer_char(c) ;
			ungetc(c,inp) ;
			return INTLITERAL ;
		}
		else if (in_char == ';')
		{
				// skip pass comments

				for(c = getc(inp) ; (c != 10) &&(c != 13); c=getc(inp)) ;

				ungetc(c,inp) ;


		}
		else if (in_char == '@')
				return ADDR ;
		else
		{
			lexical_error(in_char) ;
		}



	}

	return SCANEOF ;

}
void match(token reqtok)
{
	token tok ;

	tok=scanner(fin) ;
	if (tok != reqtok)
	{
		fprintf(stderr,"EXPECTED %s got %s\n",debug_token(reqtok),debug_token(tok)) ;
		//fprintf(stdout,"EXPECTED %s got %s (%d)\n",debug_token(reqtok),debug_token(tok),tok) ;

		//parser_error() ;
		return ;
	}
	current_token = tok ;

}

token next_token(void)
{
	static char ctoken_buffer[TOKBUFSZ] ;

	long pos ;
	token tok ;
	pos=ftell(fin) ;
	strcpy(ctoken_buffer,token_buffer) ;
	tok=scanner(fin) ;
	fseek(fin,pos,SEEK_SET) ;
	strcpy(token_buffer,ctoken_buffer) ;

	return tok ;
}
