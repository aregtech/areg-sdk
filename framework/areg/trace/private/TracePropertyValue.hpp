#ifndef AREG_TRACE_PRIVATE_TRCEAPROPERTYVALUE_HPP
#define AREG_TRACE_PRIVATE_TRCEAPROPERTYVALUE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/TracePropertyValue.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
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
    explicit TracePropertyValue( const char * initValue );

    /**
     * \brief   Initializes property value from given string
     * \param   initValue   The property value as a string to initialize
     **/
    explicit TracePropertyValue( const String & initValue );

    /**
     * \brief   Copies property value data from given source.
     * \param   source  The source of data to copy.
     **/
    TracePropertyValue( const TracePropertyValue & source );

    /**
     * \brief   Moves property value data from given source.
     * \param   source  The source of data to move.
     **/
    TracePropertyValue( TracePropertyValue && source ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~TracePropertyValue( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copies property value data from given source.
     * \param   source  The source of data to copy
     **/
    inline TracePropertyValue & operator = ( const TracePropertyValue & source );

    /**
     * \brief   Moves property value data from given source.
     * \param   source  The source of data to move.
     **/
    inline TracePropertyValue & operator = ( TracePropertyValue && source ) noexcept;

    /**
     * \brief   Set property value as a string
     * \param   newValue    The string as a value to set
     **/
    inline TracePropertyValue & operator = ( const String & newValue );

    /**
     * \brief   Set property value as a string
     * \param   newValue    The string as a value to set
     **/
    inline TracePropertyValue & operator = ( const char * newValue );

    /**
     * \brief   Set property value as an integer
     * \param   newValue    The integer as a value to set
     **/
    inline TracePropertyValue & operator = ( unsigned int newValue );

    /**
     * \brief   Set property value as a logging priority
     * \param   newValue    The logging priority as a value to set
     **/
    inline TracePropertyValue & operator = ( NETrace::eLogPriority newValue );

    /**
     * \brief   Set property value as a boolean
     * \param   newValue    The boolean priority as a value to set
     **/
    TracePropertyValue & operator = ( bool newValue );

    /**
     * \brief   Returns property value as a string
     **/
    inline explicit operator const char * ( void ) const;

    /**
     * \brief   Returns property value as a string object
     **/
    inline explicit operator const String & ( void ) const;

    /**
     * \brief   Returns property value as a logging priority (NETrace::eLogPriority)
     **/
    inline explicit operator NETrace::eLogPriority ( void ) const;

    /**
     * \brief   Returns property value as an 32-bit unsigned integer
     **/
    explicit operator unsigned int ( void ) const;

    /**
     * \brief   Returns property value as an 16-bit unsigned integer
     **/
    explicit operator unsigned short( void ) const;

    /**
     * \brief   Returns property value as a boolean.
     **/
    explicit operator bool ( void ) const;

    /**
     * \brief   Returns property value as a one byte value.
     **/
    explicit operator unsigned char ( void ) const;

    /**
     * \brief   Returns property value as a one byte value.
     **/
    explicit operator float ( void ) const;

    /**
     * \brief   Checks equality of two property values and returns true,
     *          if property values are equal.
     * \param   other   The property value to compare
     **/
    inline bool operator == ( const TracePropertyValue & other ) const;

    /**
     * \brief   Adds additional priority to the existing value
     **/
    TracePropertyValue & operator += ( NETrace::eLogPriority newValue );

    /**
     * \brief   Changes the priority value.
     *          This does not remove the scope logging priority bits.
     * \param   newPriority     New priority to set.
     * \return  Returns the actual priority value.
     **/
    unsigned int setPriority( NETrace::eLogPriority newPriority );

    /**
     * \brief   Sets the data of the property value.
     * \param   newValue    New data to set.
     **/
    void setData( String newValue );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if the property key value is empty.
     **/
    inline bool isEmtpy( void ) const;

    /**
     * \brief   Returns true if the property value is valid.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Returns priority value of the property as integer.
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
};

//////////////////////////////////////////////////////////////////////////
// Hasher of TracePropertyValue class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the TracePropertyValue.
 */
namespace std
{
    template<>
    struct hash<TracePropertyValue>
    {
        //! A function to convert TracePropertyValue object to unsigned int.
        inline unsigned int operator()(const TracePropertyValue& key) const
        {
            return static_cast<unsigned int>(key);
        }
    };
}

//////////////////////////////////////////////////////////////////////////
// TracePropertyValue class inline methods implementation
//////////////////////////////////////////////////////////////////////////

inline bool TracePropertyValue::isEmtpy( void ) const
{
    return mValue.isEmpty();
}

inline bool TracePropertyValue::isValid( void ) const
{
    return mValue.isEmpty() == false;
}

inline unsigned int TracePropertyValue::getPriority( void ) const
{
    return mPriority;
}

inline TracePropertyValue & TracePropertyValue::operator = ( const TracePropertyValue & source )
{
    mValue  = source.mValue;
    return (*this);
}

inline TracePropertyValue & TracePropertyValue::operator = ( TracePropertyValue && source ) noexcept
{
    mValue  = std::move( source.mValue );
    return (*this);
}

inline TracePropertyValue & TracePropertyValue::operator = ( const String & newValue )
{
    setData( newValue );
    return (*this);
}

inline TracePropertyValue & TracePropertyValue::operator = ( const char * newValue )
{
    setData( newValue );
    return (*this);
}

inline TracePropertyValue & TracePropertyValue::operator = ( unsigned int newValue )
{
    mValue = String::toString( newValue, NEString::eRadix::RadixDecimal );
    return (*this);
}

inline TracePropertyValue & TracePropertyValue::operator = ( NETrace::eLogPriority newValue )
{
    setPriority( newValue );
    return (*this);
}

inline TracePropertyValue::operator const char * (void) const
{
    return static_cast<const char *>(mValue);
}

inline TracePropertyValue::operator const String & (void) const
{
    return mValue;
}

inline TracePropertyValue::operator NETrace::eLogPriority ( void ) const
{
    return static_cast<NETrace::eLogPriority>(mPriority);
}

inline bool TracePropertyValue::operator == ( const TracePropertyValue & other ) const
{
    return (this != &other ? mValue == other.mValue : true);
}

#endif  // AREG_TRACE_PRIVATE_TRCEAPROPERTYVALUE_HPP
