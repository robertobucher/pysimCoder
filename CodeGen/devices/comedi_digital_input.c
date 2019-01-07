// 31 Jan 2006 : Coding by Simone Mannori
//               basic examples from Comedi PDF and comedilib/demo/<sourcecode> 
//
// Computational function for Comedi data acquisition cards 
// 

//** -------------------------- COMEDI ANALOG INPUT --------------------------------------

//**  General includes 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <asm/types.h>
#include <math.h>

/* Specific for python block developement */
#include <pyblock.h>

/* Specific for Comedilib */
#include <comedilib.h>

//** Comedi static structure inside block->work  
  typedef struct
  {
    int comedi_device ; //** 
    comedi_t *it      ; //** Comedi interface handler*/
    
    int subdev ; //** Comedi subdevice
    int chan   ; //** channel selection
  
  } ComediDigIn ;

void comedi_digital_input(int flag, python_block *block) 
{
  ComediDigIn *DIdata ; //** define the local datastructure 
  
  lsampl_t data      ; //** local row data input 
  double   d_data    ; //* 
  double * y         ; //** Roberto
      
  switch(flag)
  {  
      
      case INIT: //** Card and port init   
        
        block->ptrPar = malloc(sizeof(ComediDigIn));
        if (block->ptrPar  == NULL ) 
          { //** in case of error exit 
	    exit(-1);
            return; //** --> Exit point  
          }
	DIdata = (ComediDigIn *) block->ptrPar; //** map the local strucutre in the python_block user data  
       
        DIdata->it = comedi_open(block->str); /* */
        // code to check  
        if ( DIdata->it <= 0 )
          {
            fprintf(stderr, "Comedi Digital Input not found. \n");
            exit(1);
          } else
         /* printf("Comedi Digital Input found!\n");  */
       
        DIdata->subdev = 2 ; // digital channel I/O
        DIdata->chan   = block->intPar[0] ; 
        comedi_dio_config(DIdata->it, DIdata->subdev, DIdata->chan, COMEDI_INPUT); //** configure as input 
       break; 
      
      case OUT:
        DIdata = (ComediDigIn *) block->ptrPar ;
        comedi_dio_read( DIdata->it, DIdata->subdev, DIdata->chan, &data);
        d_data = (double) data;
	y = block->y[0] ;
        y[0] = d_data ;  
      break;
      
      case END:
          // comedi_close(it);
      break;	  
        
  } // close the switch
   
} // close the function 

