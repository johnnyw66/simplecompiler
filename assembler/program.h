#ifndef _PROGRAM_H
#define _PROGRAM_H

#define MAXPROGRAMS (64) 

typedef struct _programmngrhdr
{
	long size ;
	long noprograms ;
	char *programs[MAXPROGRAMS] ;
	char *memory ;
} PROGRAMMNGRHDR ;

int PROGRAMMNGRHDRReadPrograms(PROGRAMMNGRHDR *hdr,char *filenm) ;
void PROGRAMMNGRHDRRelocatePrograms(PROGRAMMNGRHDR *hdr) ;
PROGRAMMNGRHDR* PROGRAMMNGRHDRCreate(char *filenm) ;

#endif

