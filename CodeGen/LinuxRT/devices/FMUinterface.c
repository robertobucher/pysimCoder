/*
COPYRIGHT (C) 2016  Roberto Bucher (roberto.bucher@supsi.ch)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#include <pyblock.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "fmi2.h"
#include <fmu_fun.h>

double get_run_time();

struct fmustruct{
  FMU* fmu;
  fmi2Component c;
};

static void init(python_block *block)
{
  ModelDescription* md;
  char * guid;
  const char *instanceName; 
  char *fmuResourceLocation = getTempResourcesLocation(block->str);
  fmi2CallbackFunctions callbacks = {fmuLogger, calloc, free, NULL, NULL};
  Element *defaultExp;
  fmi2Real tolerance = 0; 
  fmi2Boolean toleranceDefined = fmi2False;
  ValueStatus vs;
  fmi2Status fmi2Flag;
    
  struct fmustruct *strFMU = (struct fmustruct *) malloc(sizeof(struct fmustruct));  
  FMU *fmu = (FMU*) malloc(sizeof(FMU));

  loadFMU(fmu, block->str);
  md = fmu->modelDescription;
  guid = getAttributeValue((Element *)md, att_guid);
    
  instanceName = getAttributeValue((Element *)getCoSimulation(md), att_modelIdentifier);
  fmi2Component c = fmu->instantiate(instanceName, fmi2CoSimulation, guid, fmuResourceLocation,
			&callbacks, fmi2False, fmi2False);
  free(fmuResourceLocation);
  if (!(c)) {
    printf("could not instantiate model");
    exit(1);
  }

  defaultExp = getDefaultExperiment(md);
  if (defaultExp) tolerance = getAttributeDouble(defaultExp, att_tolerance, &vs);
  if (vs == valueDefined) {
    toleranceDefined = fmi2True;
  }

    fmi2Flag = fmu->setupExperiment(c, toleranceDefined, tolerance, 0.0, fmi2False, 0.0);
    if (fmi2Flag > fmi2Warning) {
        printf("could not initialize model; failed FMI setup experiment");
	exit(1);
    }
    
    fmi2Flag = fmu->enterInitializationMode(c);
    if (fmi2Flag > fmi2Warning) {
        printf("could not initialize model; failed FMI enter initialization mode");
	exit(1);
    }
    
    fmi2Flag = fmu->exitInitializationMode(c);
    if (fmi2Flag > fmi2Warning) {
        printf("could not initialize model; failed FMI exit initialization mode");
	exit(1);
    }
  
  strFMU->fmu = fmu;
  strFMU->c = c;
  block->ptrPar = (void *) strFMU;
}

static void inout(python_block *block)
{
  fmi2Status ret;
  fmi2Component myC = (fmi2Component) block->ptrPar;
  fmi2Real inp[block->nin];
  fmi2Real out[block->nout];
  int i;
  int * intPar = block->intPar;
  double * realPar    = block->realPar;
  int len_in = block->nin;
  int len_out = block->nout;
  
  double ts = realPar[0];;
  double dt = realPar[1];
  double *u;
  double *y;
  
  struct fmustruct *strFMU =  (struct fmustruct *) block->ptrPar;
  fmi2Component  c = strFMU->c;

  double t = get_run_time();
  double tend = t + ts;
 
  /* Update the outputs */
  ret = strFMU->fmu->getReal(c, &intPar[2+len_in], len_out, out);
  if(ret) exit(1);

  for(i=0;i<len_out;i++){
    y = (double *) block->y[i];
    y[0] = (double) out[i];
  }

  /* Set the inputs */
  if(len_in != 0){
    for(i=0;i<len_in;i++){
      u = (double *) block->u[i];
      inp[i] = (fmi2Real) u[0];
    }
    ret = strFMU->fmu->setReal(c, &intPar[2], len_in, inp);
    if(ret) exit(1);
  }
  
  /* Perform the integration step */
  do{
    t +=dt;
    do{
      ret = strFMU->fmu->doStep(c, t, dt, fmi2True);
    }while(ret!=fmi2OK);
    t += dt;
  }while(t<tend);
}

static void end(python_block *block)
{
  struct fmustruct *strFMU =  (struct fmustruct *) block->ptrPar;
  dlclose(strFMU->fmu->dllHandle);
  freeModelDescription(strFMU->fmu->modelDescription);
  deleteUnzippedFiles(block->str);
}

void FMUinterface(int flag, python_block *block)
{
  if (flag==CG_OUT){          /* get input */
    inout(block);
  }
  else if (flag==CG_END){     /* termination */ 
    end(block);
  }
  else if (flag ==CG_INIT){    /* initialisation */
    init(block);
  }
}


