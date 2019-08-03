/* ---------------------------------------------------------------------------*
 * XmlParserCApi.cpp
 * Expose functionality of XmlParser.cpp as a c API.
 * Call functions of this file to parse and get information from xml
 * model description file of a FMI 2.0 model.
 *
 * Author: Adrian Tirea
 * Copyright QTronic GmbH. All rights reserved.
 * ---------------------------------------------------------------------------*/

#ifndef XML_PARSER_C_API_H
#define XML_PARSER_C_API_H
#ifdef __cplusplus
extern "C" {
#endif

#ifndef fmi2TypesPlatform_h
// same as in FMI 2.0 specification
typedef unsigned int fmi2ValueReference;
#endif

// elements from ModelDescription
typedef struct ModelDescription ModelDescription;
typedef struct ModelStructure ModelStructure;
typedef struct ScalarVariable ScalarVariable;
typedef struct Component Component;
typedef struct SimpleType SimpleType;
typedef struct Unit Unit;
typedef struct ListElement ListElement;
typedef struct Element Element;

// Elements names used in ModelDescription.xml
typedef enum {
    elm_BAD_DEFINED = -1,
    elm_fmiModelDescription, elm_ModelExchange, elm_CoSimulation, elm_SourceFiles, elm_File,
    elm_UnitDefinitions, elm_Unit, elm_BaseUnit, elm_DisplayUnit, elm_TypeDefinitions,
    elm_SimpleType, elm_Real, elm_Integer, elm_Boolean, elm_String,
    elm_Enumeration, elm_Item, elm_LogCategories, elm_Category, elm_DefaultExperiment,
    elm_VendorAnnotations, elm_Tool, elm_ModelVariables, elm_ScalarVariable, elm_Annotations,
    elm_ModelStructure, elm_Outputs, elm_Derivatives, elm_DiscreteStates, elm_InitialUnknowns,
    elm_Unknown
} Elm;

// Attributes names used in ModelDescription.xml
typedef enum {
    att_BAD_DEFINED = -1,
    att_fmiVersion, att_modelName, att_guid, att_description, att_author,
    att_version, att_copyright, att_license, att_generationTool, att_generationDateAndTime,
    att_variableNamingConvention, att_numberOfEventIndicators, att_name, att_kg, att_m,
    att_s, att_A, att_K, att_mol, att_cd,
    att_rad, att_factor, att_offset, att_quantity, att_unit,
    att_displayUnit, att_relativeQuantity, att_min, att_max, att_nominal,
    att_unbounded, att_value, att_startTime, att_stopTime, att_tolerance,
    att_stepSize, att_valueReference, att_causality, att_variability, att_initial,
    att_previous, att_canHandleMultipleSetPerTimeInstant, att_declaredType, att_start, att_derivative,
    att_reinit, att_index, att_dependencies, att_dependenciesKind, att_modelIdentifier,
    att_needsExecutionTool, att_completedIntegratorStepNotNeeded, att_canBeInstantiatedOnlyOncePerProcess, att_canNotUseMemoryManagementFunctions, att_canGetAndSetFMUstate,
    att_canSerializeFMUstate, att_providesDirectionalDerivative, att_canHandleVariableCommunicationStepSize, att_canInterpolateInputs, att_maxOutputDerivativeOrder,
    att_canRunAsynchronuously
} Att;

// Enumerated values names used in ModelDescription.xml
typedef enum {
    enu_BAD_DEFINED = -1,
    enu_flat, enu_structured, enu_dependent, enu_constant, enu_fixed,
    enu_tunable, enu_discrete, enu_parameter, enu_calculatedParameter, enu_input,
    enu_output, enu_local, enu_independent, enu_continuous, enu_exact,
    enu_approx, enu_calculated
} Enu;

typedef enum {
    valueMissing,
    valueDefined,
    valueIllegal
} ValueStatus;

// Returns NULL to indicate failure
// Otherwise, return the root node md of the AST. From the result of this
// function user can access all other elements from ModelDescription.xml.
// The receiver must call freeModelDescription(md) to release AST memory.
ModelDescription* parse(char* xmlPath);
void freeModelDescription(ModelDescription *md);


/* ModelDescription functions */
// get number of unit definitions
int getUnitDefinitionsSize(ModelDescription *md);
// get unit definition at index
Unit *getUnitDefinition(ModelDescription *md, int index);
// get number of type definitions
int getTypeDefinitionsSize(ModelDescription *md);
// get type definition at index
SimpleType *getTypeDefinition(ModelDescription *md, int index);
// Null or component for ModelExchange
Component *getModelExchange(ModelDescription *md);
// Null or component for CoSimulation
Component *getCoSimulation(ModelDescription *md);
// get number of log categories
int getLogCategoriesSize(ModelDescription *md);
// get log category at index
Element *getLogCategory(ModelDescription *md, int index);
// Null or DefaultExperiment
Element *getDefaultExperiment(ModelDescription *md);
// get number of vendor annotations
int getVendorAnnotationsSize(ModelDescription *md);
// get vendor annotation at index
Element *getVendorAnnotation(ModelDescription *md, int index);
// get number of scalar variables
int getScalarVariableSize(ModelDescription *md);
// get scalar variable at index
ScalarVariable *getScalarVariable(ModelDescription *md, int index);
// Null or ModelStructure
ModelStructure  *getModelStructure (ModelDescription *md);

// get the SimpleType definition by name, if any. NULL if not found.
SimpleType *getSimpleType(ModelDescription *md, const char *name);
// get the ScalarVariable by name, if any. NULL if not found.
ScalarVariable *getVariable(ModelDescription *md, const char *name);
// get description from variable, if not present look for type definition description.
const char *getDescriptionForVariable(ModelDescription *md, ScalarVariable *sv);

/* ModelStructure functions */
// get number of outputs
int getOutputsSize(ModelStructure *ms);
// get output at index
Element *getOutput(ModelStructure *ms, int index);
// get number of derivatives
int getDerivativesSize(ModelStructure *ms);
// get derivative at index
Element *getDerivative(ModelStructure *ms, int index);
// get number of discrete states
int getDiscreteStatesSize(ModelStructure *ms);
// get discrete state at index
Element *getDiscreteState(ModelStructure *ms, int index);
// get number of initial unknowns
int getInitialUnknownsSize(ModelStructure *ms);
// get initial unknown at index
Element *getInitialUnknown(ModelStructure *ms, int index);

/* ScalarVariable functions */
// one of Real, Integer, etc.
Element *getTypeSpec(ScalarVariable *sv);
// get number of annotations
int getAnnotationsSize(ScalarVariable *sv);
// get annotation at index
Element *getAnnotation(ScalarVariable *sv, int index);

// get the valueReference of current variable. This attribute is mandatory for a variable.
fmi2ValueReference getValueReference(ScalarVariable *sv);
// returns one of constant, fixed, tunable, discrete, continuous.
// If value is missing, the default continuous is returned.
// If unknown value, return enu_BAD_DEFINED.
Enu getVariability(ScalarVariable *sv);
// returns one of parameter, calculatedParameter, input, output, local, independent.
// If value is missing, the default local is returned.
// If unknown value, return enu_BAD_DEFINED.
Enu getCausality(ScalarVariable *sv);

/* Component functions */
// get number of files
int getFilesSize(Component *c);
// get file at index
Element *getFile(Component *c, int index);

/* SimpleType functions */
// one of RealType, IntegerType etc.
Element *getTypeSpecDef(SimpleType *st);

/* Unit functions */
// Null or base unit.
Element *getBaseUnit(Unit *u);
// get number of display units
int getDisplayUnitsSize(Unit *u);
// get display unit at index
Element *getDisplayUnit(Unit *u, int index);

/* ListElement functions */
// get list size
int getListSize(ListElement *le);
// get element at index
Element *getElementFromList(ListElement *le, int index);

/* Element functions. All upper types are also an Element,
 * therefore first parameter can be those types. */
Elm getElementType(Element *el);
// get name of the element type
const char *getElementTypeName(Element *el);
// get array with all attributes. Caller must free the array, but not the content.
// The result array is formed by key1, value1, key2, value2, ...
// n is output with size of array, even number.
const char **getAttributesAsArray(Element *el, int *n);
// value or NULL if attribute not present
const char *getAttributeValue(Element *el, Att att);
int getAttributeInt(Element *el, Att att, ValueStatus *vs);
unsigned int getAttributeUInt(Element *el, Att att, ValueStatus *vs);
double getAttributeDouble(Element *el, Att att, ValueStatus *vs);
// 0 for false, otherwise 1
int getAttributeBool(Element *el, Att att, ValueStatus *vs);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif
#endif // XML_PARSER_C_API_H
