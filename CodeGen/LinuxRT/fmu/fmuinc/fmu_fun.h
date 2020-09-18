#define UNZIP_CMD "unzip -o -d "
#define RESOURCES_DIR "resources"
#define DLL_DIR   "binaries/linux64/"
#define XML_FILE  "modelDescription.xml"
#define DLL_SUFFIX ".so"

char *getTempResourcesLocation(const char* fmuFileName);
void *getAdr(int *success, HMODULE dllHandle, const char *functionName);
void loadFMU(FMU * fmu, const char* fmuFileName);
void deleteUnzippedFiles(const char* fmuFileName);
void fmuLogger(void *componentEnvironment, fmi2String instanceName, fmi2Status status, fmi2String category, fmi2String message, ...);
