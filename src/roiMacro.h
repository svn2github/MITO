/**
 * \file roiMacro.h
 * \brief File per la definizione di macro per la ROI
 * \author ICAR-CNR Napoli
 */

#ifndef _roiMacro_h_
#define _roiMacro_h_

#include <assert.h>
#include <iostream>
#include <itkMacro.h>

/*
 * \brief Macro per il set
 */
#define roiSetMacro(name,type) \
    virtual void set##name (type _arg) {\
    this->##name = _arg; \
} 

/*
 * \brief Macro per il get
 */
#define roiGetMacro(name,type) \
    virtual type get##name () const { \
    return this->##name; \
} 

/*
 * \brief Macro per il set di una stringa
 */
#define roiSetStringMacro(name) \
    virtual void set##name (const char *_arg) \
{ \
    this->##name = _arg; \
} 

/*
 * \brief Macro per il get di una stringa
 */
#define roiGetStringMacro(name) \
    virtual const char *get##name () const { \
    return this->##name.c_str(); \
} 

/*
 * \brief Macro per l'is
 */
#define roiIsMacro(name) \
    virtual bool is##name () const { \
    return this->##name; \
} 

#endif _roiMacro_h_