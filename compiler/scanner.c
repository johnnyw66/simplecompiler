#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef enum token_types
{
	BEGIN,END,READ,WRITE,ID,INTLITERAL,LPAREN,RPAREN,SEMICOLON,
	COMMA,ASSIGNOP,PLUSOP,MINUSOP,OROP,ANDOP,EQUOP,IFS,ELSE,RETURN,
	FUNCTION,COLON,
	RANGE,RANDG,RANDL,ROUTE,EXISTS,
	TAKER,WAIT,MSG,CASE,SWITCH,BREAK,ADDR,
	GETROUTE,GETLEADER,GETOBJECT,CALL,INT,FOR,
	COMMENT,SCANEOF

} token ;


typedef struct token_table
{
	char  *tokstring ;
	token tok ;

} TOKEN_TABLE ;

static TOKEN_TABLE token_table[]=
{
	{"BEGIN",BEGIN},
	{"END",END},
	{"READ",READ},
	{"WRITE",WRITE},
	{"ID",ID},
	{"INTLITERAL",INTLITERAL},
	{"LPAREN",LPAREN},
	{"RPAREN",RPAREN},
	{"SEMICOLON",SEMICOLON},
	{"COMMA",COMMA},
	{"ASSIGNOP",ASSIGNOP},
	{"PLUSOP",PLUSOP},
	{"MINUSOP",MINUSOP},
	{"OROP",OROP},
	{"EQUOP",EQUOP},
	{"ANDOP",ANDOP},
	{"IF",IFS},
	{"ELSE",ELSE},
	{"RETURN",RETURN},
	{"CASE",CASE},
	{"SWITCH",SWITCH},
	{"FUNCTION",FUNCTION},
	// test functions
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
	{":",COLON},
	{"INT",INT},
	{"FOR",FOR},
	{"@",ADDR},
	{"COMMENT",COMMENT},
	{"SCANEOF",SCANEOF}

} ;

static TOKEN_TABLE reserved_words[]=
{
	{"BEGIN",BEGIN},
	{"END",END},
	{"READ",READ},
	{"FUNCTION",FUNCTION},
	{"IF",IFS},
	{"ELSE",ELSE},
	{"SWITCH",SWITCH},
	{"CASE",CASE},
	{"BREAK",BREAK},
	{"RETURN",RETURN},

// test functions
	{"RANGE",RANGE},
	{"RANDG",RANDG},
	{"RANDL",RANDL},
	{"EXISTS",EXISTS},
	{"ROUTE",ROUTE},
	// actions
	{"TAKER",TAKER},
	{"WAIT",WAIT},
	{"MSG",MSG},
	{"INT",INT},
	// get functions
	{"GETROUTE",GETROUTE},
	{"GETOBJECT",GETOBJECT},
	{"GETLEADER",GETLEADER},

	{"FOR",FOR},
	{"WRITE",WRITE}

} ;

#define TOKBUFSZ (512)

extern token scanner(FILE *f) ;
static char token_buffer[TOKBUFSZ] ;
static int token_cnt ;
static int lineno=0 ;
token current_token ;
FILE *fin ;
FILE *outfile ;

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

	for(i=0 ; i < 24 ; i++)
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
		if (tok == token_table[i].tok)
			return token_table[i].tokstring ;

	}
	return "??" ;

}
void lexical_error(int ch)
{
	fprintf(stderr,"Error with <%c> on line %d\n",(char)ch,lineno) ;

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
			for(c = getc(inp) ; isalnum(c) || c == '_' ; c=getc(inp))
			{
					buffer_char(c) ;
			}
			ungetc(c,inp) ;
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
		else if (in_char == '(')
				return LPAREN ;
		else if (in_char == ')')
				return RPAREN ;
		else if (in_char == ';')
				return SEMICOLON ;
		else if (in_char == ',')
				return COMMA ;
		else if (in_char == '#')
		{
				// skip pass comments

				for(c = getc(inp) ; c != 10 ; c=getc(inp)) ;

				ungetc(c,inp) ;
//				return COMMENT ;
		}
		else if (in_char == '@')
				return ADDR ;
		else if (in_char == '/')
		{
			c=getc(inp) ;
			if (c == '/')
			{
				// Comment
				for(c = getc(inp) ; c != 10 ; c=getc(inp)) ;

				ungetc(c,inp) ;
//				return COMMENT ;

			}
			else {
					ungetc(c,inp) ;
					return PLUSOP ;
			}

		}
		else if (in_char =='+')
				return PLUSOP ;
		else if (in_char =='-')
				return MINUSOP ;
		else if (in_char == '|')
				return OROP ;
		else if (in_char == '&')
				return ANDOP ;
		else if (in_char == '{')
				return BEGIN ;
		else if (in_char == '}')
				return END ;
		else if (in_char == ':')
				return COLON ;

		else if (in_char == '=')
		{
			c=getc(inp) ;
			if (c == '=')
				return EQUOP ;
			else {
					ungetc(c,inp) ;
					return ASSIGNOP ;
				//	lexical_error(in_char) ;
			}
		}
		else if (in_char == '_')
		{

			buffer_char(in_char) ;
			for(c=getc(inp) ; isalpha(c) ; c= getc(inp))
					buffer_char(c) ;
			ungetc(c,inp) ;
			return FUNCTION ;

		} else
		{
			lexical_error(in_char) ;
		}



	}

	return SCANEOF ;


}




/* semantic record type */

#define MAXIDLEN (32)
typedef char string[MAXIDLEN] ;

typedef struct _op_rec
{
	enum op { PLUS,MINUS,ANDO,ORO,EQUO,UNKNOWN } operater ;

} op_rec ;

enum expr { IDEXPR,LITERALEXPR,TEMPEXPR,FUNCEXPR,ADDREXPR,TUPLEIDEXPR} ;

typedef struct _expr_rec
{
	enum expr kind ;
	string fname ;
	union {
		string name ;
		int val ;
	} ;

} expr_rec ;

/* Parser stuff */
void expression(expr_rec *) ;
void id_list(expr_rec *) ;
void expr_list(expr_rec *) ;
void primary(expr_rec *) ;
void match(token tok) ;
token next_token(void) ;
void ident(expr_rec *) ;
void identtype(expr_rec *,enum expr) ;
expr_rec process_literal(void) ;
expr_rec process_id(enum expr) ;
void debug_expr(expr_rec expr) ;
void statement_list(expr_rec *expr) ;
void statementif(expr_rec *expr) ;
void statementfor(expr_rec *expr) ;
void switchstatement(expr_rec *expr) ;



extern int lookup(string s) ;
extern void enter(string s) ;
#define MAX_SYMBOLS (1024)
static string symbol_table[MAX_SYMBOLS] ;
static int symbolcnt=0 ;
static int max_temp = 0 ;

#define MAX_STACK (256)

static int labelcnt=0 ;
static int labelindex = 0 ; /* used by label stack */
static int labelstack[MAX_STACK] ;

int getlabel()
{
	int lid ;
	lid = labelcnt ;
	labelcnt++ ;
	return lid ;
}

void pushlabel(int lid)
{
	labelstack[labelindex]=lid ;
	labelindex++ ;
}

int poplabel()
{
	labelindex-- ;
	return  labelstack[labelindex] ;
}


void generate(char *p1,char *p2,char *p3,char *p4)
{
	fprintf(outfile,"\t%s %s %s %s\n",p1,p2,p3,p4) ;
}

void enter(string s)
{
	strcpy(symbol_table[symbolcnt],s) ;
	symbolcnt++ ;
}

int lookup(char *s)
{
	int i ;

	for(i=0 ; i < symbolcnt ; i++)
	{
		if (!strcmp(s,(const char *)&symbol_table[i]))
		{
			return 1;
		}
	}
	return 0 ;

}
char *exprtype(enum expr type)
{
	switch(type)
	{
		case FUNCEXPR:
				return "subroutine" ;
		case TUPLEIDEXPR:
				return "tuple" ;

		default:
			return "Integer" ;

	}

}

/* CODE GENERATION */
void check_id(char *s,enum expr type)
{

	if (!lookup(s))
	{
			enter(s) ;
			generate("declare",s,exprtype(type),"") ;

	}

}


void reset_temp(void)
{
	max_temp=0 ;
}

char *get_temp(void)
{

	static char tempname[MAXIDLEN] ;
	max_temp++ ;
	sprintf(tempname,"Temp&%d",max_temp) ;
	check_id(tempname,IDEXPR) ;
	return tempname ;

}



void start(void)
{
//	printf("start\n") ;

}

void finish(void)
{
	generate("HALT","","","") ;

}

char *toktostring(token tok)
{
	int i ;

	for(i=0 ; i < SCANEOF ; i++)
	{
		if (tok == token_table[i].tok)
			return token_table[i].tokstring ;

	}
	return "???" ;

}
void debug_expr(expr_rec expr)
{
	printf("expr:name %s type (%d) %d\n",expr.name,expr.kind) ;
	printf("expr:value %d\n",expr.val) ;

}




char *copybuff(char *buff)
{
	char *cbuff = (char *)malloc(strlen(buff)+1) ;
	strcpy(cbuff,buff) ;
	return cbuff ;
}

char *extract(expr_rec expr)
{
	/* TODO */
	static char buff[64] ;

	switch(expr.kind)
	{
		case TUPLEIDEXPR:
					printf("***TUPLE!!!!!!!\n") ;
					sprintf(buff,"%s",expr.name) ;
					return copybuff(buff) ;

		case IDEXPR:
					sprintf(buff,"%s",expr.name) ;
					return copybuff(buff) ;
		case ADDREXPR:
					sprintf(buff,"@%s",expr.name) ;
					return copybuff(buff) ;

		case FUNCEXPR:
					sprintf(buff,"%d",expr.val) ;
					return  copybuff(buff);
			case TEMPEXPR:
					sprintf(buff,"%s",expr.name) ;
					return copybuff(buff) ;
		case LITERALEXPR:
					sprintf(buff,"%d",expr.val) ;
					return  copybuff(buff);
		default:
					sprintf(buff,"???%s (%d)",expr.name,expr.kind) ;
					return  copybuff(buff);

	}

}

void assignflag(expr_rec target)
{
	generate("FLAG ","",target.name,"") ;
}

void assign(expr_rec target,expr_rec source)
{
	generate("STORE ",extract(source),target.name,"") ;
}


op_rec process_op(void)
{

	op_rec o ;
	switch(current_token)
	{
		case PLUSOP:
				o.operater=PLUS ;
				break ;

		case MINUSOP:
				o.operater=MINUS ;
				break ;
		case ANDOP:
				o.operater=ANDO ;
				break ;

		case OROP:
				o.operater=ORO ;
				break ;
		case EQUOP:
				o.operater=EQUO ;
				break ;

		default:
				o.operater=UNKNOWN ;
				break ;

	}
	return o ;

}

char *extractop(op_rec op)
{
	switch(op.operater)
	{

		case PLUS:

			return "ADD ";
		case MINUS:
			return "SUB " ;

		case ORO:
			return "OR " ;

		case ANDO:
			return "AND " ;

		case EQUO:
			 return "EQ " ;
		default:
			return "???? " ;
	}

}
expr_rec gen_infix(expr_rec e1, op_rec op,expr_rec e2)
{


	expr_rec erec ;
//	printf("gen_infix\n") ;
	erec.kind=TEMPEXPR ;
	strcpy(erec.name,get_temp()) ;
	generate(extractop(op),extract(e1),extract(e2),erec.name) ;
	return erec ;

}


expr_rec process_id_tuple(enum expr type,int cnt)
{
	expr_rec t;
//	printf("process id\n") ;
	check_id(token_buffer,type) ;
	t.kind = type ;
	strcpy(t.name,token_buffer) ;
	return t ;

}
expr_rec process_id(enum expr type)
{
	expr_rec t;
//	printf("process id\n") ;
	check_id(token_buffer,type) ;
	t.kind = type ;
	strcpy(t.name,token_buffer) ;
	return t ;
}

expr_rec process_literal(void)
{
	expr_rec t ;
//	printf("proccess literal\n") ;
	t.kind = LITERALEXPR ;
	(void)sscanf(token_buffer,"%d",&t.val) ;
	return t ;

}
expr_rec process_function(token tok)
{
	token ntok ;
	expr_rec t ;
	ntok = next_token() ;

	if (ntok == INTLITERAL)
	{
		match(INTLITERAL) ;
		t.kind = FUNCEXPR ;
		t.val=atoi(token_buffer) ;
	} else if (ntok == ID)
	{
		ident(&t) ;
	} else if (ntok == ADDR)
	{
		match(ADDR) ;
		ident(&t) ;
		t.kind = ADDREXPR ;
	}

	return t ;
}

void function_expr(token tok,expr_rec out_expr)
{
	generate(toktostring(tok),extract(out_expr),"","") ;
}

void write_expr(expr_rec out_expr)
{
	generate("WRITE ",extract(out_expr),"Integer","") ;

}

/* PARSER */


void id_list(expr_rec *expr)
{
	ident(expr) ;
	while(next_token() == COMMA)
	{
			match(COMMA) ;
			ident(expr) ;
	}

}

void getaddress(expr_rec *expr)
{
	match(ADDR) ;
	ident(expr) ;
	expr->kind = ADDREXPR ;			// change to addr expression!

}

void function_expr_list(token tok,expr_rec *expr)
{
	expr_rec src ;


	if (next_token() == ADDR)
	{
		getaddress(expr) ;

	} else
	{
		expression(expr) ;

	}

	src=*expr ;
	function_expr(tok,src) ;

	while(next_token() == COMMA)
	{
		match(COMMA) ;
		if (next_token() == ADDR)
		{
			getaddress(expr) ;
		} else
		{
			expression(expr) ;
		}
		src=*expr ;
		function_expr(tok,src) ;
	}

}

void expr_list(expr_rec *expr)
{
	expr_rec src ;

	expression(expr) ;
	src=*expr ;
	write_expr(src) ;

	while(next_token() == COMMA)
	{
		match(COMMA) ;
		expression(expr) ;
		src=*expr ;
		write_expr(src) ;
	}

}

void syntax_error(token tok)
{
		fprintf(stderr,"syntax Error on line %d\n",lineno) ;
		fprintf(stderr,"TOKEN %s\n",debug_token(tok)) ;
}

void parser_error()


{
		fprintf(stderr,"Parser Error on line %d\n",lineno) ;
}

void match(token reqtok)
{
	token tok ;

	tok=scanner(fin) ;
	if (tok != reqtok)
	{
		fprintf(stderr,"EXPECTED %s got %s\n",debug_token(reqtok),debug_token(tok)) ;
		parser_error() ;
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

void ident_tuple(expr_rec *expr,int cnt)
{
	match(ID) ;
	*expr=process_id_tuple(TUPLEIDEXPR,cnt) ;
}

void ident(expr_rec *expr)
{
//	printf("ident\n") ;
	match(ID) ;
	*expr=process_id(IDEXPR) ;

}

void identtype(expr_rec *expr,enum expr type)
{
//	printf("ident\n") ;
	*expr=process_id(type) ;
}



void primary(expr_rec *expr)
{
	expr_rec assign_expr,source_expr ;

	token tok = next_token() ;

	switch (tok)
	{
		case LPAREN:
				match(LPAREN) ;
				expression(expr) ;
				match(RPAREN) ;
				break ;

		case FUNCTION:
		case RANDG:
		case RANDL:
		case ROUTE:
		case EXISTS:
		case RANGE:

		case TAKER:
		case GETROUTE:
		case GETOBJECT:
		case GETLEADER:
		case WAIT:
		case MSG:
				match(tok) ;
				match(LPAREN) ;
				source_expr=process_function(tok);
				assign_expr.kind=TEMPEXPR ;
				strcpy(assign_expr.name,get_temp()) ;
				function_expr(tok,source_expr) ;
				assignflag(assign_expr) ;
				*expr=assign_expr ;
				match(RPAREN) ;
				break ;
		case ID:
				ident(expr) ;
				break ;
		case INTLITERAL:
				match(INTLITERAL) ;
				*expr=process_literal() ;
				break ;
		default:
				syntax_error(tok) ;
				break ;

	}


}
void add_op(op_rec *op)
{
	token tok = next_token() ;
//	printf("add_op\n") ;

	if (tok == PLUSOP || tok == MINUSOP
		|| tok == OROP || tok == ANDOP || tok == EQUOP
		)
	{
			match(tok) ;
			*op=process_op() ;
	}
		else
			syntax_error(tok) ;
}

void expression(expr_rec *result)
{
		expr_rec left_operand,right_operand ;
		op_rec op ;
		token t ;
//		printf("expression\n") ;
		primary(&left_operand) ;
		for(t= next_token() ;
			t == PLUSOP || t == MINUSOP
				|| t==ANDOP || t == OROP || t== EQUOP ;
		t = next_token() )
		{
			add_op(& op) ;
			primary(&right_operand) ;
			left_operand=gen_infix(left_operand,op,right_operand) ;

		}
		*result = left_operand ;

}
void return_code()
{
	match(RETURN) ;
	generate("RETURN","","","") ;
	match(SEMICOLON) ;

}
void externallcall(expr_rec *expr)
{
	fprintf(outfile,"\tBSR %s\n",expr->name) ;
}

void statement(expr_rec *expr)
{
int tuplecnt ;

expr_rec assign_expr,source_expr ;

	token tok=next_token() ;
//	printf("statement\n") ;

	switch(tok)
	{
		case SEMICOLON:
				match(SEMICOLON) ;
				break ;
		case BREAK:
				break ;
		case LPAREN:
				match(LPAREN) ;
				match(INT) ;
				tuplecnt=1 ;
				while
				(
					(next_token() == INT)
					|| (next_token() == COMMA)
				)
				{
					token tok ;
					tok = next_token() ;
					match(tok) ;
					if (tok == INT)
					{
						tuplecnt++ ;
					}
				}
				match(RPAREN) ;
				ident_tuple(expr,tuplecnt) ;
				match(SEMICOLON) ;
				break ;

		case INT:
				match(INT) ;
				ident(expr) ;		// just declare var
				match(SEMICOLON) ;
				break ;

		case RETURN:
				return_code();
				break ;

		case FOR:
				statementfor(expr) ;
				break ;

		case IFS:
			statementif(expr) ;
			break ;
		case SWITCH:
				switchstatement(expr) ;
				break ;

		case ID:
					match(ID) ;
					if (next_token() == ASSIGNOP)
					{
						identtype(&assign_expr,IDEXPR) ;
					} else
					{
						identtype(&assign_expr,FUNCEXPR) ;

					}

					if (next_token() == ASSIGNOP)
					{
						match(ASSIGNOP) ;
						expression(expr) ;
						match(SEMICOLON) ;
						source_expr=*expr ;
						assign(assign_expr,source_expr) ;
					} else
					{

						match(LPAREN) ;
						externallcall(&assign_expr) ;
						match(RPAREN) ;
						match(SEMICOLON) ;

					}
					reset_temp() ;
				break ;

		case READ:
				match(READ) ;
				match(LPAREN) ;
				id_list(expr) ;
				match(RPAREN) ;
				match(SEMICOLON) ;
				break ;

		case FUNCTION:
		case RANDG:
		case RANDL:
		case ROUTE:
		case EXISTS:
		case RANGE:

		case TAKER:
		case WAIT:

		case GETROUTE:
		case GETOBJECT:
		case GETLEADER:

		case MSG:

				match(tok) ;
				match(LPAREN) ;
				function_expr_list(tok,expr) ;
				match(RPAREN) ;
				match(SEMICOLON) ;
				break ;

		case WRITE:
				match(WRITE) ;
				match(LPAREN) ;
				expr_list(expr) ;
				match(RPAREN) ;
				match(SEMICOLON) ;
				break ;


		default:
				syntax_error(tok) ;
				break ;

	}


}


void casestatement(expr_rec *cf,expr_rec *expr,int endlabel)
{
	expr_rec value ;
	int labelid,mlabelid ;

	mlabelid = 0 ;

	labelid=getlabel() ;
	pushlabel(labelid);

	do
	{
		match(CASE) ;
		match(INTLITERAL) ;
		value=process_literal() ;
		match(COLON) ;
		fprintf(outfile,"\tCMP %s %d\n",cf->name,value.val) ;

		if ((next_token() == CASE) && (mlabelid == 0))
		{
			mlabelid = getlabel() ;

		}
		if ((mlabelid != 0) && (next_token() == CASE))
		{
			fprintf(outfile,"\tBRT L%d\n",mlabelid) ;

		} else
		{
			fprintf(outfile,"\tBRF L%d\n",labelid) ;
		}
	}
	while ( next_token() == CASE) ;

	if (mlabelid != 0)
	{
		fprintf(outfile,"L%d:\n",mlabelid) ;
	}
	statement_list(expr) ;


	if (next_token() == BREAK)
	{
		match(BREAK) ;
		match(SEMICOLON) ;
		fprintf(outfile,"\tBRA L%d\n",endlabel) ;
	}

	fprintf(outfile,"L%d:\n",poplabel()) ;
}


void casestatements(expr_rec *cf,expr_rec *expr,int endlabel)
{

	token tok ;

	casestatement(cf,expr,endlabel) ;
	while(1)
	{
		tok= next_token() ;
		if (tok == CASE)
		{
				casestatement(cf,expr,endlabel) ;
		} else
		{

			return ;
		}
	}

}
void switchstatement(expr_rec *expr)
{
	token tok ;
	expr_rec compare_expr ;
	int elabelid ;

	match(SWITCH) ;
	match(LPAREN) ;
	ident(&compare_expr) ;
	match(RPAREN) ;
	match(BEGIN) ;
	elabelid = getlabel() ;
	pushlabel(elabelid) ;
	casestatements(&compare_expr,expr,elabelid) ;
	match(END) ;
	elabelid = poplabel() ;
	fprintf(outfile,"L%d:\n",elabelid) ;

}



void statementfor(expr_rec *expr)
{
	expr_rec test_expr ;
	token tok ;
	int labelid,conlabelid ;



	match(FOR) ;
	match(LPAREN) ;
	statement(expr) ;		// start expression

	conlabelid = getlabel() ;
	fprintf(outfile,"L%d:\n",conlabelid) ;

	expression(&test_expr) ;		// test condition
	match(SEMICOLON) ;


	labelid=getlabel() ;
	pushlabel(labelid) ;

	fprintf(outfile,"\tTEST %s\n",test_expr.name) ;
	fprintf(outfile,"\tBRF L%d\n",labelid) ;

	statement(expr) ;				// statement
	match(RPAREN) ;
	match(BEGIN) ;
	reset_temp() ;
	statement_list(expr) ;
	match(END) ;

	fprintf(outfile,"\tBRA L%d\n",conlabelid) ;
	fprintf(outfile,"L%d:\n",poplabel()) ;


}



void statementif(expr_rec *expr)
{
	token tok ;
	int labelid ;

	match(IFS) ;
	match(LPAREN) ;
	expression(expr) ;
	match(RPAREN) ;
	match(BEGIN) ;
	labelid=getlabel() ;
	pushlabel(labelid) ;

	fprintf(outfile,"\tTEST %s\n",expr->name) ;
	fprintf(outfile,"\tBRF L%d\n",labelid) ;

	reset_temp() ;
	statement_list(expr) ;
	match(END) ;
	tok=next_token() ;
	labelid=getlabel() ;

	if (tok == ELSE)
	{
		fprintf(outfile,"\tBRA L%d\n",labelid) ;
	}

	fprintf(outfile,"L%d:\n",poplabel()) ;
	pushlabel(labelid);

	if (tok == ELSE)
	{
		match(ELSE) ;
		match(BEGIN) ;
		statement_list(expr) ;
		match(END) ;
		fprintf(outfile,"L%d:\n",poplabel()) ;
	} else
	{
			poplabel() ;
	}

}


void statement_list(expr_rec *expr)
{
//	printf("statement_list\n") ;

	statement(expr) ;
	while(1)
	{
		switch(next_token())
		{
			case RETURN:
					return_code() ;
					break ;
			case SEMICOLON:
			case INT:
			case ID:
			case READ:
			case WRITE:
			case FUNCTION:
			case GETROUTE:
			case RANDG:
			case RANDL:
			case ROUTE:
			case EXISTS:
			case RANGE:
			case LPAREN:
			case TAKER:
			case WAIT:
			case MSG:

					statement(expr) ;
					break ;
			case FOR:
					statementfor(expr) ;
					break ;
			case IFS:
					statementif(expr) ;
					break ;
			case SWITCH:
					switchstatement(expr) ;
					break ;
			default:
					return ;
		}

	}

}

void function_start()
{
	int lid ;

	fprintf(outfile,"; function %s\n",token_buffer) ;
	lid=getlabel() ;
	fprintf(outfile,"L%d:\n",lid) ;
	fprintf(outfile,"%s:\n",token_buffer) ;
}

void program(expr_rec *expr)
{
	token tok ;
//	printf("program\n") ;
	match(ID) ;
	function_start() ;
	match(LPAREN) ;
	tok=next_token() ;
	if (tok != RPAREN)
	{
			id_list(expr) ;
	}
	match(RPAREN) ;
	match(BEGIN) ;
	statement_list(expr) ;
	match(END) ;
	generate("RETURN","","","") ;
}


void program_list(expr_rec *expr)
{

	token tok ;
//	start() ;
	program(expr) ;
	while(1)
	{
		tok = next_token() ;
		if (tok == ID)
		{
			program(expr) ;
		} else
		{
			return ;
		}
	}



}



void system_goal(void)
{
	expr_rec expr ;
	program_list(&expr) ;
	match(SCANEOF) ;
	finish() ;
}

int main(int argc,char **argv)
{
static char infilename[512] ;
static char outfilename[512] ;


	if (argc < 3)
	{
		fprintf(stderr,"\nFormally Known as Swiv2 Compiler V1.1\n") ;
		fprintf(stderr,"*** COMPILE ERROR!!! ***\n") ;
		fprintf(stderr,"\nUsage %s sourcefile outputfile\n",argv[0]) ;
	//	strcpy(infilename,"c:\\eg.sw") ;
	//	strcpy(outfilename,"c:\\eg.asm") ;
		return 0 ;

	} else
	{
		strcpy(infilename,argv[1]) ;
		strcpy(outfilename,argv[2]) ;
	}

	fin=fopen(infilename,"rb") ;

	if (!fin)
	{
		fprintf(stderr,"Failed to open file %s\n",infilename) ;
		return 0 ;
	}

	outfile=fopen(outfilename,"wb") ;
	if (!outfile)
	{
		fprintf(stderr,"Failed to open file %s for writing\n",outfilename) ;
		return 0 ;
	}

	system_goal() ;
	fclose(fin) ;
	printf(";Compiled %d lines\n",lineno) ;
	return 1 ;

}
