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
    char ComediDevName[20]; //** 
    comedi_t *it      ; //** Comedi interface handler*/
    
    int subdev ; //** Comedi subdevice
    int chan   ; //** channel selection
    int range  ; //** range selection 
    int aref   ; //** zero volt reference 
    lsampl_t maxdata  ;        /* maximum sample value (raw data) */
    comedi_range *range_ds   ; /* range */
  
  } ComediAnIn ;

//** Main Python Block 
void comedi_analog_input(int flag, python_block *block) 
{
  ComediAnIn *AIdata ; //** define the local datastructure 
  char sName[15]     ;
  int NameLen ;
  
  lsampl_t data      ; //** local row data input 
  double   volts     ; //* 
  double * y         ; //** Roberto

  switch (flag)
  {  
      case INIT: //** Card and port init   
        block->ptrPar = malloc(sizeof(ComediAnIn));
        if (block->ptrPar  == NULL ) 
          { //** in case of error exit
            fprintf(stderr, " HELP ! \n"); 
	   exit(-1); //** magic code 
           return; //** --> Exit point  
          }
        else
          {
            /* fprintf(stderr, " Allocated ! \n");  */
          }

        AIdata = (ComediAnIn *) block->ptrPar; //** map the local structure in the "python_block" user data  
             
	sprintf(AIdata->ComediDevName,block->str); //** create the string 
	
	AIdata->it = comedi_open(AIdata->ComediDevName); /* */
        if (!(AIdata->it))
          {
            fprintf(stderr, "Comedi Analog Input at %s not found. \n", AIdata->ComediDevName);
            exit(-1); //** magic code 
            return; //** --> Exit point  
          }
        else
            /* printf("Comedi AnalogInput found! \n");  */
       
        AIdata->subdev = 0 ; // analog channels input
        AIdata->chan   = block->intPar[0] ; // channel selection  
        AIdata->range  = block->intPar[1] ; // range selection 
        AIdata->aref = AREF_GROUND      ; // zero volt reference 
        AIdata->maxdata =   comedi_get_maxdata(AIdata->it, AIdata->subdev, AIdata->chan);
        AIdata->range_ds =  comedi_get_range  (AIdata->it, AIdata->subdev, AIdata->chan, AIdata->range);
 
       break; 
      
      case OUT:
        AIdata = (ComediAnIn *) block->ptrPar;

	comedi_data_read(AIdata->it, AIdata->subdev, AIdata->chan, AIdata->range, AIdata->aref, &data);

        volts = comedi_to_phys(data, AIdata->range_ds, AIdata->maxdata);

        //** printf(" datain = %f \n", volts) ; //** DEBUG only 

        y = block->y[0] ; 

        y[0] = volts ; 

     
      break;
      
      case END:
      
      // comedi_close(it);
      
      break;	  
        
  } // close the switch
   
} // close the function 

