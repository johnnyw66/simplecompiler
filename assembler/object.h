#ifndef _OBJECT_H
#define _OBJECT_H

typedef int SWID  ;

typedef struct _swvector 
{
	long x,y,z ;

} SWVECTOR ;

typedef struct _object
{
       SWID  	id ;  		/* Unique ID for this object */
       SWVECTOR       posistion ;             /* objects posistion vector */
       SWVECTOR       direction ;             /* objects direction vector */
       void * (*Update)(struct _object *objptr) ;
       void * (*Collision)(struct _object *objptr,struct _object *byobjectptr) ;
       void * (*Render)(struct _object *objptr) ;         /* Render routine*/
	   CPU * router ;
} OBJECT ;


#endif
