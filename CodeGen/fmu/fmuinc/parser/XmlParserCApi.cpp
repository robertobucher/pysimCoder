/* ---------------------------------------------------------------------------*
 * XmlParserCApi.cpp
 * Expose functionality of XmlParser.cpp as a c API.
 * Call functions of this file to parse and get information from xml
 * model description file of a FMI 2.0 model.
 *
 * Author: Adrian Tirea
 * Copyright QTronic GmbH. All rights reserved.
 * ---------------------------------------------------------------------------*/

#include "XmlParserCApi.h"
#include "fmu20/XmlParser.h"
#include "fmu20/XmlElement.h"

#ifdef STANDALONE_XML_PARSER
#define logThis(n, ...) printf(__VA_ARGS__); printf("\n")
#define checkStrdup(str) strdup(str)
#else
#include "logging.h"  // logThis
#endif  // STANDALONE_XML_PARSER

ModelDescription* parse(char* xmlPath) {
    XmlParser parser(xmlPath);
    return parser.parse();
}
void freeModelDescription(ModelDescription *md) {
    if (md) delete md;
}

/* ModelDescription fields access*/
int getUnitDefinitionsSize(ModelDescription *md) {
    return md->unitDefinitions.size();
}

Unit *getUnitDefinition(ModelDescription *md, int index) {
    return md->unitDefinitions.at(index);
}

int getTypeDefinitionsSize(ModelDescription *md) {
    return md->typeDefinitions.size();
}

SimpleType *getTypeDefinition(ModelDescription *md, int index) {
    return md->typeDefinitions.at(index);
}

Component *getModelExchange(ModelDescription *md) {
    return md->modelExchange;
}

Component *getCoSimulation(ModelDescription *md) {
    return md->coSimulation;
}

int getLogCategoriesSize(ModelDescription *md) {
    return md->logCategories.size();
}

Element *getLogCategory(ModelDescription *md, int index) {
    return md->logCategories.at(index);
}

Element *getDefaultExperiment(ModelDescription *md) {
    return md->defaultExperiment;
}

int getVendorAnnotationsSize(ModelDescription *md) {
    return md->vendorAnnotations.size();
}

Element *getVendorAnnotation(ModelDescription *md, int index) {
    return md->vendorAnnotations.at(index);
}

int getScalarVariableSize(ModelDescription *md) {
    return md->modelVariables.size();
}

ScalarVariable *getScalarVariable(ModelDescription *md, int index) {
    return md->modelVariables.at(index);
}

ModelStructure  *getModelStructure (ModelDescription *md) {
    return md->modelStructure;
}

SimpleType *getSimpleType(ModelDescription *md, const char *name) {
    return md->getSimpleType(name);
}

ScalarVariable *getVariable(ModelDescription *md, const char *name) {
    return md->getVariable(name);
}

const char *getDescriptionForVariable(ModelDescription *md, ScalarVariable *sv) {
    return md->getDescriptionForVariable(sv);
}

/* ModelStructure fields access */
int getOutputs(ModelStructure *ms) {
    return ms->outputs.size();
}

Element *getOutput(ModelStructure *ms, int index) {
    return ms->outputs.at(index);
}

int getDerivativesSize(ModelStructure *ms) {
    return ms->derivatives.size();
}

Element *getDerivative(ModelStructure *ms, int index) {
    return ms->derivatives.at(index);
}

int getDiscreteStates(ModelStructure *ms) {
    return ms->discreteStates.size();
}

Element *getDiscreteState(ModelStructure *ms, int index) {
    return ms->discreteStates.at(index);
}

int getInitialUnknowns(ModelStructure *ms) {
    return ms->initialUnknowns.size();
}

Element *getInitialUnknown(ModelStructure *ms, int index) {
    return ms->initialUnknowns.at(index);
}

/* ScalarVariable field access */
Element *getTypeSpec(ScalarVariable *sv) {
    return sv->typeSpec;
}

int getAnnotationsSize(ScalarVariable *sv) {
    return sv->annotations.size();
}

Element *getAnnotation(ScalarVariable *sv, int index) {
    return sv->annotations.at(index);
}

fmi2ValueReference getValueReference(ScalarVariable *sv) {
    return sv->getValueReference();
}
// returns one of constant, fixed, tunable, discrete, continuous.
// If value is missing, the default continuous is returned.
// If unknown value, return enu_BAD_DEFINED.
Enu getVariability(ScalarVariable *sv) {
    return (Enu)sv->getVariability();
}
// returns one of parameter, calculatedParameter, input, output, local, independent.
// If value is missing, the default local is returned.
// If unknown value, return enu_BAD_DEFINED.
Enu getCausality(ScalarVariable *sv) {
    return (Enu)sv->getCausality();
}

/* Component field access */
int getFilesSize(Component *c) {
    return c->files.size();
}

Element *getFile(Component *c, int index) {
    return c->files.at(index);
}

/* SimpleType field access */
Element *getTypeSpecDef(SimpleType *st) {
    return st->typeSpec;
}

/* Unit field access */
Element *getBaseUnit(Unit *u) {
    return u->baseUnit;
}

int getDisplayUnitsSize(Unit *u) {
    return u->displayUnits.size();
}

Element *getDisplayUnit(Unit *u, int index) {
    return u->displayUnits.at(index);
}

/* ListElement field access */
int getListSize(ListElement *le) {
    return le->list.size();
}

Element *getElementFromList(ListElement *le, int index) {
    return le->list.at(index);
}

/* Element functions */
Elm getElementType(Element *el) {
    return (Elm)el->type;
}

const char *getElementTypeName(Element *el) {
    return XmlParser::elmNames[el->type];
}

const char **getAttributesAsArray(Element *el, int *n) {
    *n = el->attributes.size();
    const char **result = (const char **)calloc(2 * (*n), sizeof(char *));
    if (!result) {
        logThis(ERROR_FATAL, "Out of memory");
        n = 0;
        return NULL;
    }
    int i = 0;
    for (std::map<XmlParser::Att, char *>::iterator it = el->attributes.begin(); it != el->attributes.end(); ++it ) {
        result[i] = (const char*)XmlParser::attNames[it->first];
        result[i + 1] = it->second;
        i = i + 2;
    }
    return result;
}

const char *getAttributeValue(Element *el, Att att) {
    return el->getAttributeValue((XmlParser::Att)att);
}

int getAttributeInt(Element *el, Att att, ValueStatus *vs) {
    return el->getAttributeInt((XmlParser::Att)att, (XmlParser::ValueStatus *)vs);
}

unsigned int getAttributeUInt(Element *el, Att att, ValueStatus *vs) {
    return el->getAttributeUInt((XmlParser::Att)att, (XmlParser::ValueStatus *)vs);
}

double getAttributeDouble(Element *el, Att att, ValueStatus *vs) {
    return el->getAttributeDouble((XmlParser::Att)att, (XmlParser::ValueStatus *)vs);
}

int getAttributeBool(Element *el, Att att, ValueStatus *vs) {
    if (el->getAttributeBool((XmlParser::Att)att, (XmlParser::ValueStatus *)vs)) return 1;
    return 0;
}
