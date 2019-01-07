// 31 Jan 2006 : Coding by Simone Mannori
//               basic examples from Comedi PDF and comedilib/demo/<sourcecode> 
//
// Computational function for Comedi data acquisition cards 
// 

//** -------------------------- COMEDI ANALOG OUTPUT -------------------------------

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
    int range  ; //** range selection 
    int aref   ; //** zero volt reference 
    lsampl_t maxdata  ;        /* maximum sample value (raw data) */
    comedi_range *range_ds   ; /* range */
  
  } ComediAnOut ;

void comedi_analog_output(int flag, python_block *block) 
{
  ComediAnOut *AOdata ; //** define the local datastructure
 
  lsampl_t data      ; //** local row dta input 
  double   volts     ; //* 
  double * u         ; //** Roberto

      
  switch(flag)
  {  
      case INIT: //** Card and port init   
        block->ptrPar = malloc(sizeof(ComediAnOut));
        if (block->ptrPar  == NULL ) 
          { //** in case of error exit 
	    exit(-1); 
            return; //** --> Exit point  
          }
	AOdata = (ComediAnOut *) block->ptrPar; //** map the local strucutre in the python_block user data  
	   
	AOdata->it = comedi_open(block->str); /* */
        
        // code to check  
        if ( AOdata->it <= 0 )
          {
            fprintf(stderr, "Comedi Analog Output not found. \n");
            exit(1);
          }
        else
            /* printf("Comedi Analog Output found!\n");  */
       
        AOdata->subdev = 1 ; // analog channel output
        AOdata->chan   = block->intPar[0] ; // output channel selection
        AOdata->range  = block->intPar[1] ; // range selection
        AOdata->aref   = AREF_GROUND    ; // zero volt reference
        AOdata->maxdata  = comedi_get_maxdata(AOdata->it, AOdata->subdev, AOdata->chan);
        AOdata->range_ds = comedi_get_range  (AOdata->it, AOdata->subdev, AOdata->chan, AOdata->range);
        
        volts = 0.000 ; // zero on the output 
        data = comedi_from_phys(volts, AOdata->range_ds, AOdata->maxdata) ; // convert to integer data
	/* printf("Data-> 0x%0x\n",data); */
        //** force the output to zero pysical volts 
        comedi_data_write(AOdata->it, AOdata->subdev, AOdata->chan, AOdata->range, AOdata->aref, data)  ;
      break; 
      
      case OUT:
         AOdata = (ComediAnOut *) block->ptrPar ;
         u = block->u[0] ; //** get the input from Scilab/Python 
         volts = u[0] ;       
   	 data = comedi_from_phys(volts, AOdata->range_ds, AOdata->maxdata) ;  
	 comedi_data_write(AOdata->it, AOdata->subdev, AOdata->chan, AOdata->range, AOdata->aref, data)  ;
      break;
      
      case END:
        AOdata = (ComediAnOut *) block->ptrPar ;
        volts  = 0.000 ; // zero on the output 
        data = comedi_from_phys(volts, AOdata->range_ds, AOdata->maxdata) ;
        comedi_data_write(AOdata->it, AOdata->subdev, AOdata->chan, AOdata->range, AOdata->aref, data)  ;
        
	// comedi_close(it);
      
      break;	  
        
  } // close the switch
   
} // close the function 

