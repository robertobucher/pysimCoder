/*
 * Copyright QTronic GmbH. All rights reserved.
 */

/* ---------------------------------------------------------------------------*
 * XmlParserException.h
 * Exception used in parsing model description of a FMI 2.0 model.
 *
 * Author: Adrian Tirea
 * ---------------------------------------------------------------------------*/

#ifndef FMU20_XML_PARSER_EXCEPTION_H
#define FMU20_XML_PARSER_EXCEPTION_H

#include <exception>
#include <stdarg.h>

static char* strallocprintf(const char *format, va_list argp);

// message passed in constructor is freed in destructor.
class XmlParserException : public std::exception {
 public:
    char *message;

 public:
    XmlParserException(const char *format, ...) {
    va_list argp;
    va_start(argp, format);
    message = strallocprintf(format, argp);
    va_end(argp);
    }
    ~XmlParserException() throw() {
        if (message) delete[]message;
    }

    virtual const char *what() const throw() {
        return message;
    }
};

static char *strallocprintf(const char *format, va_list argp) {
    int n;
    char *result;
#ifdef _MSC_VER
    n = _vscprintf(format, argp);
#else
    va_list argcopy;
    va_copy(argcopy, argp);
    n = vsnprintf(NULL, 0, format, argcopy);
    va_end(argcopy);
#endif

    result = new char[n + 1];
    vsprintf(result, format, argp);
    return result;
}

#endif // FMU20_XML_PARSER_EXCEPTION_H
