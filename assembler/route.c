#include <stdio.h>
#include "scitypes.h"
#include "routes.h"



#define _ERROR(_A) printf(_A) 

static ROUTEFILE routefile ;


static LVECTOR  *ROUTEFILEGetRoute(ROUTEFILE *rf,long index)
{
	if (index > rf->noroutes)
	{
		_ERROR("GetRoute: Index Error\n") ;
		return NULL ;
	}

	return &rf->routes[index].v ;
}


static void ROUTEFILEReadRoutes(ROUTEFILE *rf,char *filenm) 
{
	FILE *fin ;
	fin=fopen(filenm,"rb") ;
	if (fin == NULL)
	{
		_ERROR("Failed to open route file\n") ;
		return ;
	}

	fread(rf,sizeof(ROUTEFILE),1,fin) ;
	fclose(fin) ;
	return ;
}


void ReadRoutes(char *filenm)
{
	ROUTEFILEReadRoutes(&routefile,filenm) ;
}

LVECTOR  *GetRoute(long index)
{
	return ROUTEFILEGetRoute(&routefile,index) ;
}

