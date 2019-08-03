/*
 * Copyright QTronic GmbH. All rights reserved.
 */

/* ---------------------------------------------------------------------------*
 * XmlElement.cpp
 * Elements implementation that describe content of model description of a
 * FMI 2.0 model. All elements have Element as parent class. Elements have
 * attributes and other specific content.
 *
 * Author: Adrian Tirea
 * ---------------------------------------------------------------------------*/

#include "fmu20/XmlElement.h"
#include <assert.h>
#include <map>
#include <string>
#include <vector>
#include <string.h> // strcmp
#include "fmu20/XmlParserException.h"

#ifdef STANDALONE_XML_PARSER
#define logThis(n, ...) printf(__VA_ARGS__); printf("\n")
#else
#include "GlobalIncludes.h"
#include "logging.h"  // logThis
#endif  // STANDALONE_XML_PARSER

Element::~Element() {
    for (std::map<XmlParser::Att, char *>::const_iterator it = attributes.begin(); it != attributes.end(); ++it) {
        free(it->second);
    }
    attributes.clear();
}
template <typename T> void Element::deleteListOfElements(const std::vector<T *> &list) {
    typename std::vector<T*>::const_iterator it;
    for (it = list.begin(); it != list.end(); ++it) {
        delete *it;
    }
}
void Element::handleElement(XmlParser *parser, const char *childName, int isEmptyElement) {
    throw XmlParserException("Elements are not expected inside '%s'. Found instead '%s'",
        XmlParser::elmNames[type],
        childName);
}
void Element::printElement(int indent) {
    std::string indentS(indent, ' ');
    logThis(ERROR_INFO, "%s%s", indentS.c_str(), XmlParser::elmNames[type]);
    for (std::map<XmlParser::Att, char *>::const_iterator it = attributes.begin(); it != attributes.end(); ++it) {
        logThis(ERROR_INFO, "%s%s=%s", indentS.c_str(), XmlParser::attNames[it->first], it->second);
    }
}
template <typename T> void Element::printListOfElements(int indent, const std::vector<T *> &list) {
    typename std::vector<T*>::const_iterator it;
    for (it = list.begin(); it != list.end(); ++it) {
        (*it)->printElement(indent);
    }
}

const char *Element::getAttributeValue(XmlParser::Att att) {
    std::map<XmlParser::Att, char *>::const_iterator it = attributes.find(att);
    if (it != attributes.end()) {
        return it->second;
    }
    return NULL;
}
int Element::getAttributeInt(XmlParser::Att att, XmlParser::ValueStatus *vs) {
    int n = 0;
    const char *value = getAttributeValue(att);
    if (!value) { *vs = XmlParser::valueMissing; return n; }
    *vs = (1 == sscanf(value, "%d", &n)) ? XmlParser::valueDefined : XmlParser::valueIllegal;
    return n;
}
unsigned int Element::getAttributeUInt(XmlParser::Att att, XmlParser::ValueStatus *vs) {
    unsigned int u = -1;
    const char* value = getAttributeValue(att);
    if (!value) { *vs = XmlParser::valueMissing; return u; }
    *vs = (1 == sscanf(value, "%u", &u)) ? XmlParser::valueDefined : XmlParser::valueIllegal;
    return u;
}
double Element::getAttributeDouble(XmlParser::Att att, XmlParser::ValueStatus *vs) {
    double d = 0;
    const char* value = getAttributeValue(att);
    if (!value) { *vs = XmlParser::valueMissing; return d; }
    *vs = (1 == sscanf(value, "%lf", &d)) ? XmlParser::valueDefined : XmlParser::valueIllegal;
    return d;
}
bool Element::getAttributeBool(XmlParser::Att att, XmlParser::ValueStatus *vs) {
    const char* value = getAttributeValue(att);
    if (!value) {*vs = XmlParser::valueMissing; return false;}
    *vs = XmlParser::valueDefined;
    if (!strcmp(value, "true")) return true;
    if (!strcmp(value, "false")) return false;
    *vs = XmlParser::valueIllegal;
    return false;
}

ListElement::~ListElement() {
    deleteListOfElements(list);
}
void ListElement::handleElement(XmlParser *parser, const char *childName, int isEmptyElement) {
    XmlParser::Elm childType = parser->checkElement(childName);
    if (childType == XmlParser::elm_Item) {
        Element *item = new Element;
        item->type = childType;
        parser->parseElementAttributes(item);
        if (!isEmptyElement) {
            parser->parseEndElement();
        }
        list.push_back(item);
    } else {
        throw XmlParserException("Expected '%s' element inside '%s'. Found instead: '%s'.",
                XmlParser::elmNames[XmlParser::elm_Item],
                XmlParser::elmNames[XmlParser::elm_Enumeration],
                childName);
    }
}
void ListElement::printElement(int indent) {
    Element::printElement(indent);
    int childIndent = indent + 1;
    printListOfElements(childIndent, list);
}


Unit::Unit() {
    baseUnit = NULL;
}
Unit::~Unit() {
    if (baseUnit) delete baseUnit;
    deleteListOfElements(displayUnits);
}
void Unit::handleElement(XmlParser *parser, const char *childName, int isEmptyElement) {
    XmlParser::Elm childType = parser->checkElement(childName);
    if (childType == XmlParser::elm_BaseUnit) {
        baseUnit = new Element;
        baseUnit->type = childType;
        parser->parseElementAttributes(baseUnit);
        if (!isEmptyElement) {
            parser->parseEndElement();
        }
    } else if (childType == XmlParser::elm_DisplayUnit) {
        Element *displayUnit = new Element;
        displayUnit->type = childType;
        parser->parseElementAttributes(displayUnit);
        displayUnits.push_back(displayUnit);
        if (!isEmptyElement) {
            parser->parseEndElement();
        }
    } else {
        throw XmlParserException("Expected '%s' | '%s' element inside '%s'. Found instead: '%s'.",
            XmlParser::elmNames[XmlParser::elm_BaseUnit],
            XmlParser::elmNames[XmlParser::elm_DisplayUnit],
            XmlParser::elmNames[XmlParser::elm_SourceFiles],
            childName);
    }
}
void Unit::printElement(int indent) {
    Element::printElement(indent);
    int childIndent = indent + 1;
    if (baseUnit) baseUnit->printElement(childIndent);
    printListOfElements(childIndent, displayUnits);
}
Element *Unit::getDisplayUnit(const char *name) {
    if (!name) return NULL;
    for (std::vector<Element *>::const_iterator it = displayUnits.begin(); it != displayUnits.end(); ++it) {
        const char *unitName = (*it)->getAttributeValue(XmlParser::att_name);
        if (unitName && 0 == strcmp(name, unitName)) {
            return (*it);
        }
    }
    return NULL;
}


SimpleType::SimpleType() {
    typeSpec = NULL;
}
SimpleType::~SimpleType() {
    if (typeSpec) delete typeSpec;
}
void SimpleType::handleElement(XmlParser *parser, const char *childName, int isEmptyElement) {
    XmlParser::Elm childType = parser->checkElement(childName);
    switch (childType) {
        case XmlParser::elm_Real:
        case XmlParser::elm_Integer:
        case XmlParser::elm_Boolean:
        case XmlParser::elm_String: {
            typeSpec = new Element;
            typeSpec->type = childType;
            parser->parseElementAttributes(typeSpec);
            if (!isEmptyElement) {
                parser->parseEndElement();
            }
            break;
        }
        case XmlParser::elm_Enumeration: {
            typeSpec = new ListElement;
            typeSpec->type = childType;
            parser->parseElementAttributes(typeSpec);
            if (!isEmptyElement) {
                parser->parseChildElements(typeSpec);
            }
            break;
        }
        default: {
            throw XmlParserException(
                "Expected '%s' | '%s' | '%s' | '%s' | '%s' element inside '%s'. Found instead: '%s'.",
                XmlParser::elmNames[XmlParser::elm_Real],
                XmlParser::elmNames[XmlParser::elm_Integer],
                XmlParser::elmNames[XmlParser::elm_Boolean],
                XmlParser::elmNames[XmlParser::elm_String],
                XmlParser::elmNames[XmlParser::elm_Enumeration],
                XmlParser::elmNames[type],
                childName);
        }
    }
}
void SimpleType::printElement(int indent) {
    Element::printElement(indent);
    int childIndent = indent + 1;
    if (typeSpec) typeSpec->printElement(childIndent);
}


Component::~Component() {
    deleteListOfElements(files);
}
void Component::handleElement(XmlParser *parser, const char *childName, int isEmptyElement) {
    XmlParser::Elm childType = parser->checkElement(childName);
    if (childType == XmlParser::elm_SourceFiles) {
        if (!isEmptyElement) {
            parser->parseChildElements(this);
        }
    } else if (childType == XmlParser::elm_File) {
        Element *sourceFile = new Element;
        sourceFile->type = childType;
        parser->parseElementAttributes(sourceFile);
        if (!isEmptyElement) {
            parser->parseEndElement();
        }
        files.push_back(sourceFile);
    } else {
        throw XmlParserException("Expected '%s' | '%s' element inside '%s'. Found instead: '%s'.",
            XmlParser::elmNames[XmlParser::elm_File],
            XmlParser::elmNames[XmlParser::elm_SourceFiles],
            XmlParser::elmNames[type],
            childName);
    }
}
void Component::printElement(int indent) {
    Element::printElement(indent);
    int childIndent = indent + 1;
    printListOfElements(childIndent, files);
}


ScalarVariable::ScalarVariable() {
    typeSpec = NULL;
}
ScalarVariable::~ScalarVariable() {
    delete typeSpec;
    deleteListOfElements(annotations);
}
void ScalarVariable::handleElement(XmlParser *parser, const char *childName, int isEmptyElement) {
    XmlParser::Elm childType = parser->checkElement(childName);
    switch (childType) {
        case XmlParser::elm_Real:
        case XmlParser::elm_Integer:
        case XmlParser::elm_Boolean:
        case XmlParser::elm_String:
        case XmlParser::elm_Enumeration: {
            typeSpec = new Element;
            typeSpec->type = childType;
            parser->parseElementAttributes(typeSpec);
            if (!isEmptyElement) {
                parser->parseEndElement();
            }
            break;
        }
        case XmlParser::elm_Annotations: {
            // no attributes expected; this class handles also the Tool
            if (!isEmptyElement) parser->parseChildElements(this);
            break;
        }
        case XmlParser::elm_Tool: {
            Element *tool = new Element;
            tool->type = childType;
            parser->parseElementAttributes(tool, false);
            if (!isEmptyElement) {
                parser->parseSkipChildElement();
            }
            annotations.push_back(tool);
            break;
        }
        default: {
            throw XmlParserException(
                "Expected '%s' | '%s' | '%s' | '%s' | '%s' | '%s' | '%s' element inside '%s'. Found instead: '%s'.",
                XmlParser::elmNames[XmlParser::elm_Real],
                XmlParser::elmNames[XmlParser::elm_Integer],
                XmlParser::elmNames[XmlParser::elm_Boolean],
                XmlParser::elmNames[XmlParser::elm_String],
                XmlParser::elmNames[XmlParser::elm_Enumeration],
                XmlParser::elmNames[XmlParser::elm_Annotations],
                XmlParser::elmNames[XmlParser::elm_Tool],
                XmlParser::elmNames[type],
                childName);
        }
    }
}
fmi2ValueReference ScalarVariable::getValueReference() {
    XmlParser::ValueStatus vs;
    fmi2ValueReference ref = getAttributeUInt(XmlParser::att_valueReference, &vs);
    assert(vs == XmlParser::valueDefined);  // this is a required attribute
    return ref;
}
XmlParser::Enu ScalarVariable::getVariability() {
    const char *value = getAttributeValue(XmlParser::att_variability);
    if (!value) {
        return XmlParser::enu_continuous;  // default
    }
    try {
        return XmlParser::checkEnumValue(value);
    } catch (XmlParserException& ) {
        return XmlParser::enu_BAD_DEFINED;
    }
}
XmlParser::Enu ScalarVariable::getCausality() {
    const char *value = getAttributeValue(XmlParser::att_causality);
    if (!value) {
        return XmlParser::enu_local;  // default
    }
    try {
        return XmlParser::checkEnumValue(value);
    } catch (XmlParserException& ) {
        return XmlParser::enu_BAD_DEFINED;
    }
}
void ScalarVariable::printElement(int indent) {
    Element::printElement(indent);
    int childIndent = indent + 1;
    typeSpec->printElement(childIndent);
    printListOfElements(childIndent, annotations);
}

ModelStructure::ModelStructure() {
    unknownParentType = XmlParser::elm_BAD_DEFINED;
}

ModelStructure::~ModelStructure() {
    deleteListOfElements(outputs);
    deleteListOfElements(derivatives);
    deleteListOfElements(discreteStates);
    deleteListOfElements(initialUnknowns);
}
void ModelStructure::handleElement(XmlParser *parser, const char *childName, int isEmptyElement) {
    XmlParser::Elm childType = parser->checkElement(childName);
    switch (childType) {
    case XmlParser::elm_Outputs:
    case XmlParser::elm_Derivatives:
    case XmlParser::elm_DiscreteStates:
    case XmlParser::elm_InitialUnknowns:
        {
            // no attributes expected; this class handles also the Unknown
            if (!isEmptyElement) {
                unknownParentType = childType;
                parser->parseChildElements(this);
            }
            break;
        }
    case XmlParser::elm_Unknown:
        {
            Element *unknown = new Element;
            unknown->type = childType;
            parser->parseElementAttributes(unknown);
            if (!isEmptyElement) {
                parser->parseEndElement();
            }
            switch (unknownParentType) {
                case XmlParser::elm_Outputs:
                    outputs.push_back(unknown);
                    break;
                case XmlParser::elm_Derivatives:
                    derivatives.push_back(unknown);
                    break;
                case XmlParser::elm_DiscreteStates:
                    discreteStates.push_back(unknown);
                    break;
                case XmlParser::elm_InitialUnknowns:
                    initialUnknowns.push_back(unknown);
                    break;
                default: {
                    throw XmlParserException(
                    "Element '%s' must be inside of '%s' | '%s' | '%s' | '%s'.",
                    XmlParser::elmNames[XmlParser::elm_Unknown],
                    XmlParser::elmNames[XmlParser::elm_Outputs],
                    XmlParser::elmNames[XmlParser::elm_Derivatives],
                    XmlParser::elmNames[XmlParser::elm_DiscreteStates],
                    XmlParser::elmNames[XmlParser::elm_InitialUnknowns]);
}
            }
            break;
        }
    default:
        {
            throw XmlParserException(
                "Expected '%s' | '%s' | '%s' | '%s' | '%s' element inside '%s'. Found instead: '%s'.",
                XmlParser::elmNames[XmlParser::elm_Outputs],
                XmlParser::elmNames[XmlParser::elm_Derivatives],
                XmlParser::elmNames[XmlParser::elm_DiscreteStates],
                XmlParser::elmNames[XmlParser::elm_InitialUnknowns],
                XmlParser::elmNames[XmlParser::elm_Unknown],
                XmlParser::elmNames[type],
                childName);
        }
    }
}
void ModelStructure::printElement(int indent) {
    Element::printElement(indent);
    int childIndent = indent + 1;
    printListOfElements(childIndent, outputs);
    printListOfElements(childIndent, derivatives);
    printListOfElements(childIndent, discreteStates);
    printListOfElements(childIndent, initialUnknowns);
}


ModelDescription::ModelDescription() {
    modelExchange = NULL;
    coSimulation = NULL;
    defaultExperiment = NULL;
    modelStructure = NULL;
}
ModelDescription::~ModelDescription() {
    deleteListOfElements(unitDefinitions);
    deleteListOfElements(typeDefinitions);
    if (coSimulation) delete coSimulation;
    if (modelExchange) delete modelExchange;
    deleteListOfElements(logCategories);
    if (defaultExperiment) delete defaultExperiment;
    deleteListOfElements(vendorAnnotations);
    deleteListOfElements(modelVariables);
    if (modelStructure) delete modelStructure;
}
void ModelDescription::handleElement(XmlParser *parser, const char *childName, int isEmptyElement) {
    XmlParser::Elm childType = parser->checkElement(childName);
    switch (childType) {
    case XmlParser::elm_CoSimulation:
        {
            coSimulation = new Component;
            coSimulation->type = childType;
            parser->parseElementAttributes(coSimulation);
            if (!isEmptyElement) {
                parser->parseChildElements(coSimulation);
            }
            break;
        }
    case XmlParser::elm_ModelExchange:
        {
            modelExchange = new Component;
            modelExchange->type = childType;
            parser->parseElementAttributes(modelExchange);
            if (!isEmptyElement) {
                parser->parseChildElements(modelExchange);
            }
            break;
        }
    case XmlParser::elm_UnitDefinitions:
        {
            // no attributes expected; this class handles the Category
            if (!isEmptyElement) parser->parseChildElements(this);
            break;
        }
    case XmlParser::elm_Unit:
        {
            Unit *unit = new Unit;
            unit->type = childType;
            parser->parseElementAttributes(unit);
            if (!isEmptyElement) {
                parser->parseChildElements(unit);
            }
            unitDefinitions.push_back(unit);
            break;
        }
    case XmlParser::elm_TypeDefinitions:
        {
            // no attributes expected; this class handles also the SimpleType
            if (!isEmptyElement) parser->parseChildElements(this);
            break;
        }
    case XmlParser::elm_SimpleType:
        {
            SimpleType *type = new SimpleType;
            type->type = childType;
            parser->parseElementAttributes(type);
            if (!isEmptyElement) {
                parser->parseChildElements(type);
            }
            typeDefinitions.push_back(type);
            break;
        }
    case XmlParser::elm_DefaultExperiment:
        {
            defaultExperiment = new Element;
            defaultExperiment->type = childType;
            parser->parseElementAttributes(defaultExperiment);
            if (!isEmptyElement) {
                parser->parseEndElement();
            }
            break;
        }
    case XmlParser::elm_LogCategories:
        {
            // no attributes expected; this class handles also the Category
            if (!isEmptyElement) parser->parseChildElements(this);
            break;
        }
    case XmlParser::elm_Category:
        {
            Element *category = new Element;
            category->type = childType;
            parser->parseElementAttributes(category);
            if (!isEmptyElement) {
                parser->parseEndElement();
            }
            logCategories.push_back(category);
            break;
        }
    case XmlParser::elm_VendorAnnotations:
        {
            // no attributes expected; this class handles also the Tool
            if (!isEmptyElement) parser->parseChildElements(this);
            break;
        }
    case XmlParser::elm_Tool:
        {
            Element *tool = new Element;
            tool->type = childType;
            parser->parseElementAttributes(tool, false);
            if (!isEmptyElement) {
                parser->parseSkipChildElement();
            }
            vendorAnnotations.push_back(tool);
            break;
        }
    case XmlParser::elm_ModelVariables:
        {
            // no attributes expected; this class handles also the ScalarVariable
            if (!isEmptyElement) parser->parseChildElements(this);
            break;
        }
    case XmlParser::elm_ScalarVariable:
        {
            ScalarVariable *variable = new ScalarVariable;
            variable->type = childType;
            parser->parseElementAttributes(variable);
            if (!isEmptyElement) {
                parser->parseChildElements(variable);
            }
            modelVariables.push_back(variable);
            break;
        }
    case XmlParser::elm_ModelStructure:
        {
            modelStructure = new ModelStructure;
            modelStructure->type = childType;
            parser->parseElementAttributes(modelStructure);
            if (!isEmptyElement) {
                parser->parseChildElements(modelStructure);
            }
            break;
        }
    default:
        throw XmlParserException("Element '%s' is not expected inside of '%s' | '%s' | '%s' | '%s'.",
            childName,
            XmlParser::elmNames[type]);
    }
}

void ModelDescription::printElement(int indent) {
    Element::printElement(indent);
    int childIndent = indent + 1;

    if (coSimulation) coSimulation->printElement(childIndent);
    if (modelExchange) modelExchange->printElement(childIndent);
    for (std::vector<Unit *>::const_iterator it = unitDefinitions.begin(); it != unitDefinitions.end(); ++it) {
        (*it)->printElement(childIndent);
    }
    for (std::vector<SimpleType *>::const_iterator it = typeDefinitions.begin(); it != typeDefinitions.end(); ++it) {
        (*it)->printElement(childIndent);
    }
    for (std::vector<Element *>::const_iterator it = logCategories.begin(); it != logCategories.end(); ++it) {
        (*it)->printElement(childIndent);
    }
    if (defaultExperiment) defaultExperiment->printElement(childIndent);
    for (std::vector<Element *>::const_iterator it = vendorAnnotations.begin(); it != vendorAnnotations.end(); ++it) {
        (*it)->printElement(childIndent);
    }
    for (std::vector<ScalarVariable *>::const_iterator it = modelVariables.begin(); it != modelVariables.end(); ++it) {
        (*it)->printElement(childIndent);
    }
    if (modelStructure) modelStructure->printElement(childIndent);
}

SimpleType *ModelDescription::getSimpleType(const char *name) {
    for (std::vector<SimpleType *>::const_iterator it = typeDefinitions.begin(); it != typeDefinitions.end(); ++it) {
        const char *typeName = (*it)->getAttributeValue(XmlParser::att_name);
        if (typeName && 0 == strcmp(typeName, name)) {
            return (*it);
        }
    }
    return NULL;
}

ScalarVariable *ModelDescription::getVariable(const char *name) {
    if (!name) return NULL;
    for (std::vector<ScalarVariable *>::const_iterator it = modelVariables.begin(); it != modelVariables.end(); ++it) {
        const char *varName = (*it)->getAttributeValue(XmlParser::att_name);
        if (varName && 0 == strcmp(name, varName)) {
            return (*it);
        }
    }
    return NULL;
}

// Enumeration and Integer have the same base type while
// Real, String, Boolean define own base types.
static int sameBaseType(XmlParser::Elm  t1, XmlParser::Elm  t2){
    return t1 == t2 ||
           t1 == XmlParser::elm_Enumeration && t2 == XmlParser::elm_Integer ||
           t2 == XmlParser::elm_Enumeration && t1 == XmlParser::elm_Integer;
}

ScalarVariable *ModelDescription::getVariable(fmi2ValueReference vr, XmlParser::Elm type) {
    for (std::vector<ScalarVariable *>::const_iterator it = modelVariables.begin(); it != modelVariables.end(); ++it) {
        if (vr == (*it)->getValueReference() && sameBaseType(type, (*it)->typeSpec->type)) {
            return (*it);
        }
    }
    return NULL;
}

const char *ModelDescription::getDescriptionForVariable(ScalarVariable *sv) {
    const char *desc = sv->getAttributeValue(XmlParser::att_description);
    // found description
    if (desc) return desc;
    // look for description of declared type:
    const char *typeName = sv->typeSpec->getAttributeValue(XmlParser::att_declaredType);
    if (typeName) {
        SimpleType *type = getSimpleType(typeName);
        if (type) return type->getAttributeValue(XmlParser::att_description);
    }
    return NULL;
}

const char *ModelDescription::getAttributeFromTypeOrDeclaredType(ScalarVariable *sv, XmlParser::Att a) {
    const char *stringValue = sv->typeSpec->getAttributeValue(a);
    if (stringValue) return stringValue;
    // look for stringValue inside declared type:
    const char *typeName = sv->typeSpec->getAttributeValue(XmlParser::att_declaredType);
    if (typeName) {
        SimpleType *type = getSimpleType(typeName);
        if (type) return type->typeSpec->getAttributeValue(a);
    }
    return NULL;
}

Unit *ModelDescription::getUnit(const char *name) {
    if (!name) return NULL;
    for (std::vector<Unit *>::const_iterator it = unitDefinitions.begin(); it != unitDefinitions.end(); ++it) {
        const char *unitName = (*it)->getAttributeValue(XmlParser::att_name);
        if (unitName && 0 == strcmp(name, unitName)) {
            return (*it);
        }
    }
    return NULL;
}
