#include <stdio.h>
#include <stdlib.h>

#include "program.h"

#define _ERROR(_A) printf(_A)

#define FAILED (-1)
#define OK	   (0)

int PROGRAMMNGRHDRReadPrograms(PROGRAMMNGRHDR *hdr,char *filenm)
{
	FILE *fin ;
	char *memory ;

	fin=fopen(filenm,"rb") ;
	if (!fin)
	{
		_ERROR("Failed to Read Program File\n") ;
		return FAILED ;
	}

	if (fread(hdr,sizeof(PROGRAMMNGRHDR),1,fin) == -1)
	{
		_ERROR("Failed to Read Header Program File\n") ;
		fclose(fin) ;
		return FAILED ;
	}

	memory=(char *)malloc(hdr->size) ;
	if (memory == NULL)
	{
		_ERROR("Failed to Allocate Memory for programs\n") ;
		fclose(fin) ;
		return FAILED ;
	}

	hdr->memory = memory ;

	if (fread(memory,1,hdr->size,fin) == -1)
	{
		_ERROR("Failed to Read MAIN Program\n") ;
		fclose(fin) ;
		return FAILED ;
	}


	fclose(fin) ;
	return OK ;
}
void PROGRAMMNGRHDRRelocatePrograms(PROGRAMMNGRHDR *hdr)
{

	int  i ;
	for(i=0 ; i < hdr->noprograms ; i++)
	{
			hdr->programs[i] = hdr->memory+(int)hdr->programs[i] ;

	}


}


PROGRAMMNGRHDR* PROGRAMMNGRHDRCreate(char *filenm)
{
	static PROGRAMMNGRHDR *programs ;
	programs = (PROGRAMMNGRHDR *)malloc(sizeof(PROGRAMMNGRHDR)) ;

	if (PROGRAMMNGRHDRReadPrograms(programs,filenm))
	{
		_ERROR("Failed in Reading Programs\n") ;
		return NULL ;
	}

	PROGRAMMNGRHDRRelocatePrograms(programs) ;
	return programs ;
}
