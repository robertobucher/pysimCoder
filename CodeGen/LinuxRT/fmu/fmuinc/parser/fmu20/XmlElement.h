/*
 * Copyright QTronic GmbH. All rights reserved.
 */

/* ---------------------------------------------------------------------------*
 * XmlElement.h
 * Contains elements classes that describe content of model description of a
 * FMI 2.0 model. All elements have Element as parent class. Elements have
 * attributes and other specific content.
 *
 * Author: Adrian Tirea
 * ---------------------------------------------------------------------------*/

#ifndef FMU20_XML_ELEMENT_H
#define FMU20_XML_ELEMENT_H

#include <map>
#include <vector>
#include "fmu20/XmlParser.h"

class Element {
 public:
    XmlParser::Elm type;  // element type
    std::map<XmlParser::Att, char*> attributes;  // map with key one of XmlParser::Att

 public:
    virtual ~Element();
    virtual void handleElement(XmlParser *parser, const char *childName, int isEmptyElement);
    virtual void printElement(int indent);
    const char *getAttributeValue(XmlParser::Att att);  // value or NULL if not present
    int getAttributeInt(XmlParser::Att att, XmlParser::ValueStatus *vs);
    unsigned int getAttributeUInt(XmlParser::Att att, XmlParser::ValueStatus *vs);
    double getAttributeDouble(XmlParser::Att att, XmlParser::ValueStatus *vs);
    bool getAttributeBool(XmlParser::Att att, XmlParser::ValueStatus *vs);

    template <typename T> void printListOfElements(int indent, const std::vector<T *> &list);
    template <typename T> void deleteListOfElements(const std::vector<T *> &list);
};


class ListElement : public Element {
 public:
    std::vector<Element*> list;  // list of Element

 public:
    ~ListElement();
    void handleElement(XmlParser *parser, const char *childName, int isEmptyElement);
    void printElement(int indent);
};


class Unit : public Element {
 public:
    std::vector<Element *> displayUnits;  // list of DisplayUnit
    Element *baseUnit;                    // null or BaseUnit

 public:
    Unit();
    ~Unit();
    void handleElement(XmlParser *parser, const char *childName, int isEmptyElement);
    void printElement(int indent);
    Element *getDisplayUnit(const char *name);
};


class SimpleType : public Element {
 public:
    Element *typeSpec;  // one of RealType, IntegerType etc.

 public:
    SimpleType();
    ~SimpleType();
    void handleElement(XmlParser *parser, const char *childName, int isEmptyElement);
    void printElement(int indent);
};


class Component : public Element {
 public:
    std::vector<Element *> files;  // list of File. Only meaningful for source code FMUs (not .dll).

 public:
    ~Component();
    void handleElement(XmlParser *parser, const char *childName, int isEmptyElement);
    void printElement(int indent);
};


class ScalarVariable : public Element {
 public :
    Element *typeSpec;                   // one of Real, Integer, etc
    std::vector<Element *> annotations;  // list of Annotations
    // int modelIdx;                     // only used in fmu10

 public:
    ScalarVariable();
    ~ScalarVariable();
    void handleElement(XmlParser *parser, const char *childName, int isEmptyElement);
    void printElement(int indent);
    // get the valueReference of current variable. This attribute is mandatory for a variable.
    fmi2ValueReference getValueReference();
    // returns one of constant, fixed, tunable, discrete, continuous.
    // If value is missing, the default continuous is returned.
    // If unknown value, return enu_BAD_DEFINED.
    XmlParser::Enu getVariability();
    // returns one of parameter, calculatedParameter, input, output, local, independent.
    // If value is missing, the default local is returned.
    // If unknown value, return enu_BAD_DEFINED.
    XmlParser::Enu getCausality();
};


class ModelStructure : public Element {
 private:
    XmlParser::Elm unknownParentType;  // used in handleElement to know in which list next Unknown belongs.

 public:
    std::vector<Element *> outputs;            // list of Unknown
    std::vector<Element *> derivatives;        // list of Unknown
    std::vector<Element *> discreteStates;     // list of Unknown
    std::vector<Element *> initialUnknowns;    // list of Unknown

 public:
    ModelStructure();
    ~ModelStructure();
    void handleElement(XmlParser *parser, const char *childName, int isEmptyElement);
    void printElement(int indent);
};

class ModelDescription : public Element {
 public:
    std::vector<Unit *> unitDefinitions;        // list of Units
    std::vector<SimpleType *> typeDefinitions;  // list of Types
    Component *modelExchange;                   // NULL or ModelExchange
    Component *coSimulation;                    // NULL or CoSimulation
                                                // At least one of CoSimulation, ModelExchange must be present.
    std::vector<Element *> logCategories;       // list of Category
    Element *defaultExperiment;                 // NULL or DefaultExperiment
    std::vector<Element *> vendorAnnotations;   // list of Tools
    std::vector<ScalarVariable *> modelVariables;  // list of ScalarVariable
    ModelStructure *modelStructure;             // not NULL ModelStructure

 public:
    ModelDescription();
    ~ModelDescription();
    void handleElement(XmlParser *parser, const char *childName, int isEmptyElement);
    void printElement(int indent);
    // get the SimpleType definition by name, if any. NULL if not found.
    SimpleType *getSimpleType(const char *name);
    // get the ScalarVariable by name, if any. NULL if not found.
    ScalarVariable *getVariable(const char *name);
    // get the ScalarVariable by vr and type. NULL if not found.
    ScalarVariable *getVariable(fmi2ValueReference vr, XmlParser::Elm type);
    // get description from variable, if not present look for type definition description.
    const char *getDescriptionForVariable(ScalarVariable *sv);
    // get attribute from type, if not present look for it inside declared type.
    // Attributes example: 'min', 'max', 'quantity'.
    const char *getAttributeFromTypeOrDeclaredType(ScalarVariable *sv, XmlParser::Att a);
    // get the Unit as defined in UnitDefinitions. NULL if not found.
    Unit *getUnit(const char *name);
};

#endif // FMU20_XML_ELEMENT_H
