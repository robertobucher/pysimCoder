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
    comedi_t *it      ; //** Comedi interface handler */
    
    int subdev ; //** Comedi subdevice
    int chan   ; //** channel selection
  
    double threshold  ;
  
  } ComediDigOut ;


void comedi_digital_output(int flag, python_block *block) 
{
  ComediDigOut *DOdata ; //** define the local datastructure 
  
  double * u ; //** Python floating point input variable 
  unsigned int data   ; /*  */
     
  switch(flag) 
  {  
      
      case INIT: //** Card and port init
        block->ptrPar = malloc(sizeof(ComediDigOut));
         if (block->ptrPar  == NULL ) 
           { //** in case of error exit 
	     exit(-1);
             return; //** --> Exit point  
           }
	 DOdata = (ComediDigOut *) block->ptrPar; //** map the local strucutre in the python_block user data  
              
         DOdata->it = comedi_open(block->str); /* */
         // code to check  
         if ( DOdata->it <= 0 ) {
            fprintf(stderr, "Comedi Digital Output not found. \n");
            exit(1);
          } else
            /* printf("Comedi Digital Output found!\n");  */
       
         DOdata->subdev = 2 ; // digital 
         DOdata->chan    = block->intPar[0] ; 
         DOdata->threshold = block->realPar[0] ; //
       
         comedi_dio_config(DOdata->it, DOdata->subdev , DOdata->chan, COMEDI_OUTPUT);
         data = 0 ;
         comedi_dio_write(DOdata->it, DOdata->subdev, DOdata->chan, data)  ; /* clear the output */
      break; 
      
      case OUT:
         DOdata = (ComediDigOut *) block->ptrPar ;
         
         u = block->u[0] ; //** get the input from Scilab/Python
                                      
	 if ( u[0] >  DOdata->threshold)
	   data = 1 ; // :)
	 else
	   data = 0 ; // :(         
               
	 comedi_dio_write(DOdata->it, DOdata->subdev, DOdata->chan, data)  ;

      break;
      
      case END:
        DOdata = (ComediDigOut *) block->ptrPar ;
	data = 0 ; //** low output 
	comedi_dio_config(DOdata->it, DOdata->subdev, DOdata->chan, COMEDI_OUTPUT);
        comedi_dio_write(DOdata->it, DOdata->subdev, DOdata->chan, data)  ;
        
	// comedi_close(it);
      
      break;	  
        
  } // close the switch
   
} // close the function 

