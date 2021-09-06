#ifndef _ROUTES_H
#define _ROUTES_H


#define MAXROUTES (512)
#define ROUTENAME 


typedef struct _route
{
	LVECTOR v ;
#ifdef ROUTENAME
	char routename[12] ;
#endif
} ROUTE ;

typedef struct routes
{
	long noroutes ;
	ROUTE routes[MAXROUTES] ;	

} ROUTEFILE  ;



void ReadRoutes(char *filenm) ;
LVECTOR  *GetRoute(long index) ;


#endif
