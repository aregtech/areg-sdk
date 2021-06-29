#ifndef AREG_TRACE_PRIVATE_TRAPROPERTYVALUE_HPP
#define AREG_TRACE_PRIVATE_TRAPROPERTYVALUE_HPP
/************************************************************************
 * \file        areg/trace/private/TracePropertyValue.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Trace Property value class. Used when reading or 
 *              saving configuration file
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/NEMemory.hpp"
#include "areg/trace/NETrace.hpp"
#include "areg/base/String.hpp"

//////////////////////////////////////////////////////////////////////////
// TracePropertyValue class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Trace property value is used when reading or saving data from 
 *          configuration file. Every value in configuration file is
 *          considered as set of property key and its value.
 *          In Key-Value pair, the property values are set as string.
 *          The property value object gets value as string (in most cases)
 *          and converts to appropriate value type.
 **/
class TracePropertyValue
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates empty property value
     **/
    TracePropertyValue( void );

    /**
     * \brief   Initializes property value from given string
     * \param   initValue   The property value as a string to initialize
     **/
    TracePropertyValue( const char * initValue );

    /**
     * \brief   Initializes property value from given string
     * \param   initValue   The property value as a string to initialize
     **/
    TracePropertyValue( const String & initValue );

    /**
     * \brief   Copies property value data from given source.
     * \param   source  The source of data to copy.
     **/
    TracePropertyValue( const TracePropertyValue & source );

    /**
     * \brief   Destructor
     **/
    ~TracePropertyValue( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copies property value data from given source.
     * \param   source  The source of data to copy
     **/
    const TracePropertyValue & operator = ( const TracePropertyValue & source );

    /**
     * \brief   Set property value as a string
     * \param   newValue    The string as a value to set
     **/
    void operator = ( const String & newValue );

    /**
     * \brief   Set property value as a string
     * \param   newValue    The string as a value to set
     **/
    void operator = ( const char * newValue );

    /**
     * \brief   Set property value as an integer
     * \param   newValue    The integer as a value to set
     **/
    void operator = ( unsigned int newValue );

    /**
     * \brief   Set property value as a logging priority
     * \param   newValue    The logging priority as a value to set
     **/
    void operator = ( NETrace::eLogPriority newValue );

    /**
     * \brief   Set property value as a boolean
     * \param   newValue    The boolean priority as a value to set
     **/
    void operator = ( bool newValue );

    /**
     * \brief   Adds additional priority to the existing value
     **/
    void operator += ( NETrace::eLogPriority newValue );

    /**
     * \brief   Returns property value as a string
     **/
    operator const char * ( void ) const;

    /**
     * \brief   Returns property value as a string object
     **/
    operator const String & ( void ) const;

    /**
     * \brief   Returns property value as an integer
     **/
    operator unsigned int ( void ) const;

    /**
     * \brief   Returns property value as a logging priority (NETrace::eLogPriority)
     **/
    operator NETrace::eLogPriority ( void ) const;

    /**
     * \brief   Returns property value as a boolean.
     **/
    operator bool ( void ) const;

    /**
     * \brief   Returns property value as a one byte value.
     **/
    operator unsigned char ( void ) const;

    /**
     * \brief   Returns property value as a one byte value.
     **/
    operator float ( void ) const;

    /**
     * \brief   Checks equality of two property values and returns true,
     *          if property values are equal.
     * \param   other   The property value to compare
     **/
    bool operator == ( const TracePropertyValue & other ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if the property value is valid.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Returns priority value of the the property as integer.
     **/
    inline unsigned int getPriority( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The property value container as string object
     **/
    String          mValue;
    /**
     * \brief   The priority level of the property value.
     **/
    unsigned int    mPriority;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Parses given string and sets the priority value.
     * \param   newValue    The value as a string to parse and set.
     **/
    void _setValue( const char * newValue);
};

//////////////////////////////////////////////////////////////////////////
// TracePropertyValue class inline methods implementation
//////////////////////////////////////////////////////////////////////////

inline bool TracePropertyValue::isValid( void ) const
{
    return mValue.isEmpty() == false;
}

inline unsigned int TracePropertyValue::getPriority( void ) const
{
    return mPriority;
}

#endif  // AREG_TRACE_PRIVATE_TRAPROPERTYVALUE_HPP
