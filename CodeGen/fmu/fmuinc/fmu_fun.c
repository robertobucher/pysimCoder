#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fmi2.h>
#include <fmu_fun.h>

static int unzip(const char *zipPath, const char *outPath) {
  int code;
  int n;
  char* cmd;

  n = strlen(UNZIP_CMD) + strlen(outPath) + 1 +  strlen(zipPath) + 16;
  cmd = (char*)calloc(sizeof(char), n);
  sprintf(cmd, "%s%s \"%s\" > /dev/null", UNZIP_CMD, outPath, zipPath);
  code = system(cmd);

  return 1;
}

static char* getFmuPath(const char* fmuFileName){
  /* Not sure why this is useful.  Just returning the filename. */
  return strdup(fmuFileName);
}

static char * getTmpPath(const char* fmuFileName) {
  int len = strlen(fmuFileName);
  char * name = calloc(sizeof(char), len-3);
  strncpy(name, fmuFileName, len-4);
  name[len-4] = '\0';
  char * results = calloc(sizeof(char), len+2);
  sprintf(results,"/tmp/%s", name);
  return(results);
}

char *getTempResourcesLocation(const char* fmuFileName) {
  char *tempPath = getTmpPath(fmuFileName);
  int len = 10 + strlen(RESOURCES_DIR) + strlen(tempPath);
  char *resourcesLocation = (char *) calloc(sizeof(char), len);
	 
  strcpy(resourcesLocation, "file:///");
  strcat(resourcesLocation, tempPath);
  strcat(resourcesLocation, "/resources/");
  free(tempPath);
  return resourcesLocation;
}

void *getAdr(int *success, HMODULE dllHandle, const char *functionName) {
  void* fp;
  fp = dlsym(dllHandle, functionName);
  if (!fp) {
    printf ("Error was: %s\n", dlerror());
   printf("warning: Function %s not found in dll\n", functionName);
    *success = 0;
  }
  return fp;
}

static int loadDll(const char* dllPath, FMU *fmu) {
  int s = 1;

  HMODULE h = dlopen(dllPath, RTLD_LAZY);

  if (!h) {
    printf("The error was: %s\n", dlerror());
    printf("error: Could not load %s\n", dllPath);
    return 0; // failure
  }
  fmu->dllHandle = h;
  fmu->getTypesPlatform          = (fmi2GetTypesPlatformTYPE *)      getAdr(&s, h, "fmi2GetTypesPlatform");
  fmu->getVersion                = (fmi2GetVersionTYPE *)            getAdr(&s, h, "fmi2GetVersion");
  fmu->setDebugLogging           = (fmi2SetDebugLoggingTYPE *)       getAdr(&s, h, "fmi2SetDebugLogging");
  fmu->instantiate               = (fmi2InstantiateTYPE *)           getAdr(&s, h, "fmi2Instantiate");
  fmu->freeInstance              = (fmi2FreeInstanceTYPE *)          getAdr(&s, h, "fmi2FreeInstance");
  fmu->setupExperiment           = (fmi2SetupExperimentTYPE *)       getAdr(&s, h, "fmi2SetupExperiment");
  fmu->enterInitializationMode   = (fmi2EnterInitializationModeTYPE *) getAdr(&s, h, "fmi2EnterInitializationMode");
  fmu->exitInitializationMode    = (fmi2ExitInitializationModeTYPE *) getAdr(&s, h, "fmi2ExitInitializationMode");
  fmu->terminate                 = (fmi2TerminateTYPE *)             getAdr(&s, h, "fmi2Terminate");
  fmu->reset                     = (fmi2ResetTYPE *)                 getAdr(&s, h, "fmi2Reset");
  fmu->getReal                   = (fmi2GetRealTYPE *)               getAdr(&s, h, "fmi2GetReal");
  fmu->getInteger                = (fmi2GetIntegerTYPE *)            getAdr(&s, h, "fmi2GetInteger");
  fmu->getBoolean                = (fmi2GetBooleanTYPE *)            getAdr(&s, h, "fmi2GetBoolean");
  fmu->getString                 = (fmi2GetStringTYPE *)             getAdr(&s, h, "fmi2GetString");
  fmu->setReal                   = (fmi2SetRealTYPE *)               getAdr(&s, h, "fmi2SetReal");
  fmu->setInteger                = (fmi2SetIntegerTYPE *)            getAdr(&s, h, "fmi2SetInteger");
  fmu->setBoolean                = (fmi2SetBooleanTYPE *)            getAdr(&s, h, "fmi2SetBoolean");
  fmu->setString                 = (fmi2SetStringTYPE *)             getAdr(&s, h, "fmi2SetString");
  fmu->getFMUstate               = (fmi2GetFMUstateTYPE *)           getAdr(&s, h, "fmi2GetFMUstate");
  fmu->setFMUstate               = (fmi2SetFMUstateTYPE *)           getAdr(&s, h, "fmi2SetFMUstate");
  fmu->freeFMUstate              = (fmi2FreeFMUstateTYPE *)          getAdr(&s, h, "fmi2FreeFMUstate");
  fmu->serializedFMUstateSize    = (fmi2SerializedFMUstateSizeTYPE *) getAdr(&s, h, "fmi2SerializedFMUstateSize");
  fmu->serializeFMUstate         = (fmi2SerializeFMUstateTYPE *)     getAdr(&s, h, "fmi2SerializeFMUstate");
  fmu->deSerializeFMUstate       = (fmi2DeSerializeFMUstateTYPE *)   getAdr(&s, h, "fmi2DeSerializeFMUstate");
  fmu->getDirectionalDerivative  = (fmi2GetDirectionalDerivativeTYPE *) getAdr(&s, h, "fmi2GetDirectionalDerivative");
  fmu->setRealInputDerivatives   = (fmi2SetRealInputDerivativesTYPE *) getAdr(&s, h, "fmi2SetRealInputDerivatives");
  fmu->getRealOutputDerivatives  = (fmi2GetRealOutputDerivativesTYPE *) getAdr(&s, h, "fmi2GetRealOutputDerivatives");
  fmu->doStep                    = (fmi2DoStepTYPE *)                getAdr(&s, h, "fmi2DoStep");
  fmu->cancelStep                = (fmi2CancelStepTYPE *)            getAdr(&s, h, "fmi2CancelStep");
  fmu->getStatus                 = (fmi2GetStatusTYPE *)             getAdr(&s, h, "fmi2GetStatus");
  fmu->getRealStatus             = (fmi2GetRealStatusTYPE *)         getAdr(&s, h, "fmi2GetRealStatus");
  fmu->getIntegerStatus          = (fmi2GetIntegerStatusTYPE *)      getAdr(&s, h, "fmi2GetIntegerStatus");
  fmu->getBooleanStatus          = (fmi2GetBooleanStatusTYPE *)      getAdr(&s, h, "fmi2GetBooleanStatus");
  fmu->getStringStatus           = (fmi2GetStringStatusTYPE *)       getAdr(&s, h, "fmi2GetStringStatus");

  if (fmu->getVersion == NULL && fmu->instantiate == NULL) {
    printf("warning: Functions from FMI 2.0 could not be found in %s\n", dllPath);
    printf("warning: Simulator will look for FMI 2.0 RC1 functions names...\n");
    s = 1;
    fmu->getTypesPlatform          = (fmi2GetTypesPlatformTYPE *)      getAdr(&s, h, "fmiGetTypesPlatform");
    fmu->getVersion                = (fmi2GetVersionTYPE *)            getAdr(&s, h, "fmiGetVersion");
    fmu->setDebugLogging           = (fmi2SetDebugLoggingTYPE *)       getAdr(&s, h, "fmiSetDebugLogging");
    fmu->instantiate               = (fmi2InstantiateTYPE *)           getAdr(&s, h, "fmiInstantiate");
    fmu->freeInstance              = (fmi2FreeInstanceTYPE *)          getAdr(&s, h, "fmiFreeInstance");
    fmu->setupExperiment           = (fmi2SetupExperimentTYPE *)       getAdr(&s, h, "fmiSetupExperiment");
    fmu->enterInitializationMode   = (fmi2EnterInitializationModeTYPE *) getAdr(&s, h, "fmiEnterInitializationMode");
    fmu->exitInitializationMode    = (fmi2ExitInitializationModeTYPE *) getAdr(&s, h, "fmiExitInitializationMode");
    fmu->terminate                 = (fmi2TerminateTYPE *)             getAdr(&s, h, "fmiTerminate");
    fmu->reset                     = (fmi2ResetTYPE *)                 getAdr(&s, h, "fmiReset");
    fmu->getReal                   = (fmi2GetRealTYPE *)               getAdr(&s, h, "fmiGetReal");
    fmu->getInteger                = (fmi2GetIntegerTYPE *)            getAdr(&s, h, "fmiGetInteger");
    fmu->getBoolean                = (fmi2GetBooleanTYPE *)            getAdr(&s, h, "fmiGetBoolean");
    fmu->getString                 = (fmi2GetStringTYPE *)             getAdr(&s, h, "fmiGetString");
    fmu->setReal                   = (fmi2SetRealTYPE *)               getAdr(&s, h, "fmiSetReal");
    fmu->setInteger                = (fmi2SetIntegerTYPE *)            getAdr(&s, h, "fmiSetInteger");
    fmu->setBoolean                = (fmi2SetBooleanTYPE *)            getAdr(&s, h, "fmiSetBoolean");
    fmu->setString                 = (fmi2SetStringTYPE *)             getAdr(&s, h, "fmiSetString");
    fmu->getFMUstate               = (fmi2GetFMUstateTYPE *)           getAdr(&s, h, "fmiGetFMUstate");
    fmu->setFMUstate               = (fmi2SetFMUstateTYPE *)           getAdr(&s, h, "fmiSetFMUstate");
    fmu->freeFMUstate              = (fmi2FreeFMUstateTYPE *)          getAdr(&s, h, "fmiFreeFMUstate");
    fmu->serializedFMUstateSize    = (fmi2SerializedFMUstateSizeTYPE *) getAdr(&s, h, "fmiSerializedFMUstateSize");
    fmu->serializeFMUstate         = (fmi2SerializeFMUstateTYPE *)     getAdr(&s, h, "fmiSerializeFMUstate");
    fmu->deSerializeFMUstate       = (fmi2DeSerializeFMUstateTYPE *)   getAdr(&s, h, "fmiDeSerializeFMUstate");
    fmu->getDirectionalDerivative  = (fmi2GetDirectionalDerivativeTYPE *) getAdr(&s, h, "fmiGetDirectionalDerivative");
    fmu->setRealInputDerivatives   = (fmi2SetRealInputDerivativesTYPE *) getAdr(&s, h, "fmiSetRealInputDerivatives");
    fmu->getRealOutputDerivatives  = (fmi2GetRealOutputDerivativesTYPE *) getAdr(&s, h, "fmiGetRealOutputDerivatives");
    fmu->doStep                    = (fmi2DoStepTYPE *)                getAdr(&s, h, "fmiDoStep");
    fmu->cancelStep                = (fmi2CancelStepTYPE *)            getAdr(&s, h, "fmiCancelStep");
    fmu->getStatus                 = (fmi2GetStatusTYPE *)             getAdr(&s, h, "fmiGetStatus");
    fmu->getRealStatus             = (fmi2GetRealStatusTYPE *)         getAdr(&s, h, "fmiGetRealStatus");
    fmu->getIntegerStatus          = (fmi2GetIntegerStatusTYPE *)      getAdr(&s, h, "fmiGetIntegerStatus");
    fmu->getBooleanStatus          = (fmi2GetBooleanStatusTYPE *)      getAdr(&s, h, "fmiGetBooleanStatus");
    fmu->getStringStatus           = (fmi2GetStringStatusTYPE *)       getAdr(&s, h, "fmiGetStringStatus");
  }
  return s;
}

void loadFMU(FMU * fmu, const char* fmuFileName) {
  char* fmuPath;
  char* tmpPath;
  char* xmlPath;
  char* dllPath;
  const char *modelId;

  // get absolute path to FMU, NULL if not found
  fmuPath = getFmuPath(fmuFileName);
  if (!fmuPath) exit(EXIT_FAILURE);

  // unzip the FMU to the tmpPath directory
  tmpPath = getTmpPath(fmuFileName);
  if (!unzip(fmuPath, tmpPath)) exit(EXIT_FAILURE);

  // parse tmpPath\modelDescription.xml
  xmlPath = calloc(sizeof(char), strlen(tmpPath) + strlen(XML_FILE) + 2);
  sprintf(xmlPath, "%s/%s", tmpPath, XML_FILE);
  
  fmu->modelDescription = parse(xmlPath);
  free(xmlPath);
  if (!fmu->modelDescription) exit(EXIT_FAILURE);
  
  modelId = getAttributeValue((Element *)getCoSimulation(fmu->modelDescription), att_modelIdentifier);

  // load the FMU dll
  dllPath = calloc(sizeof(char), strlen(tmpPath) + strlen(DLL_DIR)
		   + strlen(modelId) +  strlen(DLL_SUFFIX) + 3);
  sprintf(dllPath, "%s/%s%s%s", tmpPath, DLL_DIR, modelId, DLL_SUFFIX);
    
  if (!loadDll(dllPath, fmu)) {
    free(dllPath);
    free(fmuPath);
    free(tmpPath);
    exit(EXIT_FAILURE);
  }
  free(dllPath);
  free(fmuPath);
  free(tmpPath);
}

void deleteUnzippedFiles(const char* fmuFileName) {
  char *fmuTempPath = getTmpPath(fmuFileName);
  char *cmd = (char *)calloc(15 + strlen(fmuTempPath), sizeof(char));
  sprintf(cmd, "rm -rf %s", fmuTempPath);
  system(cmd);
  free(cmd);
  free(fmuTempPath);
}

void fmuLogger(void *componentEnvironment, fmi2String instanceName, fmi2Status status,
               fmi2String category, fmi2String message, ...)
{
}

