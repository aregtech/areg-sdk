#ifndef AREG_TRACE_PRIVATE_TRACEPROPERTY_HPP
#define AREG_TRACE_PRIVATE_TRACEPROPERTY_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/TraceProperty.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Trace Property value class. Used when reading or 
 *              saving configuration file
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"
#include "areg/base/TEProperty.hpp"
#include "areg/trace/private/TracePropertyValue.hpp"
#include "areg/trace/private/TracePropertyKey.hpp"
#include "areg/trace/NETrace.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class File;

//////////////////////////////////////////////////////////////////////////
// TraceProperty class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The logging property object, which is a configuration
 *          of property key and property value pairs. The properties
 *          are used to read or save data in logging configuration file.
 **/
class TraceProperty
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The definition of logging property.
     * \tparam  Key     The logging property key object
     * \tparam  Value   The logging property value object
     **/
    using Property  = TEProperty<TracePropertyKey, TracePropertyValue>;

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates empty property.
     **/
    TraceProperty( void ) = default;
    /**
     * \brief   Creates property with specified key and value passed as a string.
     *          The parameters are parsed to initialize property data.
     * \param   Key     The logging property key as a string to parse.
     * \param   Value   The logging property value as a string to set.
     **/
    TraceProperty( const char * Key, const char * Value );
    /**
     * \brief   Creates property with specified key passed as a string and value passed as an integer.
     *          The parameters are parsed to initialize property data.
     * \param   Key     The logging property key as a string to parse.
     * \param   Value   The logging property value as a digit to set.
     **/
    TraceProperty( const char * Key, unsigned int Value );
    /**
     * \brief   Creates property with specified key passed as a string and value passed as a logging priority.
     *          The parameters are parsed to initialize property data.
     * \param   Key     The logging property key as a string to parse.
     * \param   Value   The logging property value as a logging priority to set.
     **/
    TraceProperty( const char * Key, NETrace::eLogPriority Value);
    /**
     * \brief   Creates property with specified key passed as a string and value passed as a boolean.
     *          The parameters are parsed to initialize property data.
     * \param   Key     The logging property key as a string to parse.
     * \param   Value   The logging property value as a boolean to set.
     **/
    TraceProperty( const char * Key, bool Value );
    /**
     * \brief   Initializes the property parameters by receiving data as a string.
     *          It parses the string and extracts comments, property key and value pair.
     * \param source    The source as a string to parse.
     */
    TraceProperty( String & source );
    /**
     * \brief   Copies trance property data from given source.
     * \param   source  The source to copy data
     **/
    TraceProperty( const TraceProperty & source );
    /**
     * \brief   Moves trance property data from given source.
     * \param   source  The source to move data
     **/
    TraceProperty( TraceProperty && source ) noexcept;
    /**
     * \brief   Destructor
     **/
    ~TraceProperty( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Checks equality of two logging property objects and returns true
     *          if objects are equal.
     * \param   other   The logging property to compare.
     **/
    bool operator == ( const TraceProperty & other ) const;

    /**
     * \brief   Copies logging property from given source.
     * \param   source  The source of property to copy data.
     **/
    TraceProperty & operator = ( const TraceProperty & source );

    /**
     * \brief   Moves logging property from given source.
     * \param   source  The source of property to move data.
     **/
    TraceProperty & operator = ( TraceProperty && source ) noexcept;

    /**
     * \brief   Sets given string as a property value
     * \param   newValue    The property value as a string to set.
     **/
    inline TraceProperty & operator = ( const char * newValue );

    /**
     * \brief   Sets given integer as a property value
     * \param   newValue    The property value as an integer to set.
     **/
    inline TraceProperty & operator = ( unsigned int newValue );

    /**
     * \brief   Sets given logging priority as a property value
     * \param   newValue    The property value as a logging priority to set.
     **/
    inline TraceProperty & operator = ( NETrace::eLogPriority newValue );

    /**
     * \brief   Sets given boolean as a property value
     * \param   newValue    The property value as a boolean to set.
     **/
    inline TraceProperty & operator = ( bool newValue );

    /**
     * \brief   Converts property to string, by converting Value field
     **/
    inline explicit operator const char * ( void ) const;

    /**
     * \brief   Converts property to string object, by converting Value field
     **/
    inline explicit operator const String & ( void ) const;

    /**
     * \brief   Converts property to integer, by converting Value field
     **/
    inline explicit operator unsigned int ( void ) const;

    /**
     * \brief   Converts property to NETrace::eLogPriority, by converting Value field
     **/
    inline explicit operator NETrace::eLogPriority ( void ) const;

    /**
     * \brief   Converts property to boolean, by converting Value field
     **/
    inline explicit operator bool ( void ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Set property key and value as a data.
     * \param   Key     The property key as a string to parse and set.
     * \param   Value   The property value as a string to set.
     **/
    void setData( const char * Key, const char * Value);

    /**
     * \brief   Set property key and value as a data.
     * \param   Key     The property key as a string to parse and set.
     * \param   Value   The property value as a string to set.
     * \param   Comment The property comment to set.
     **/
    void setData( const char * Key, const char * Value, const char * Comment);

    /**
     * \brief   Returns logging property key object.
     **/
    inline const TracePropertyKey & getKey( void ) const;

    /**
     * \brief   Returns logging property value objects
     **/
    inline const TracePropertyValue & getValue( void ) const;

    /**
     * \brief   Returns logging property value object, which is possible to modify
     **/
    inline TracePropertyValue & getValue( void );

    /**
     * \brief   Returns property comment string.
     **/
    inline const String & getComment( void ) const;

    /**
     * \brief   Returns true if property is valid.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Call to read a single property entry from given file.
     *          The file should be opened for the reading.
     * \param   fileConfig  The instance of configuration file, 
     *                      which is opened for reading.
     * \return  Returns true if succeeded to read a property and initialize object.
     *          Returns false if failed to read property data or the file is not opened for reading.
     **/
    bool readProperty( const File & fileConfig );

    /**
     * \brief   Parses single line of string to extract property data.
     * \param   line    The single line of string to parse to initialize property data.
     * \return  Returns true if succeeded to initialize key and value or logging property.
     *          Otherwise, returns false.
     **/
    bool parseProperty( String & line );
    bool parseProperty( const char * logSetting );

    /**
     * \brief   Clears the property data and invalidates object.
     *          Clears comment if passed parameter is 'true'.
     * \param   clearComment    If 'true', the comment is cleared as well.
     **/
    void clearProperty( bool clearComment = true );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Parses the property as a string and extracts appropriate comment, key and value pairs.
     * \param   source  The property as a string to pars and extract data.
     **/
    void _parseProperty(String& source);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The property object of logging
     **/
    TraceProperty::Property   mProperty;
    /**
     * \brief   The string as a comment of property.
     **/
    String                    mComment;
};

//////////////////////////////////////////////////////////////////////////
// Hasher of TraceProperty class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the TraceProperty.
 */
namespace std
{
    template<>
    struct hash<TraceProperty>
    {
        //! A function to convert TraceProperty object to unsigned int.
        inline unsigned int operator()(const TraceProperty& key) const
        {
            return static_cast<unsigned int>(key);
        }
    };
}

//////////////////////////////////////////////////////////////////////////
// TraceProperty class inline methods implementation
//////////////////////////////////////////////////////////////////////////

inline TraceProperty & TraceProperty::operator = ( const char * newValue )
{
    mProperty.mValue.second = newValue;
    return (*this);
}

inline TraceProperty & TraceProperty::operator = ( unsigned int newValue )
{
    mProperty.mValue.second = newValue;
    return (*this);
}

inline TraceProperty & TraceProperty::operator = ( NETrace::eLogPriority newValue )
{
    mProperty.mValue.second = newValue;
    return (*this);
}

inline TraceProperty & TraceProperty::operator = ( bool newValue )
{
    mProperty.mValue.second = newValue;
    return (*this);
}

inline TraceProperty::operator const char * ( void ) const
{
    return static_cast<const char *>(mProperty.mValue.second);
}

inline TraceProperty::operator const String & ( void ) const
{
    return static_cast<const String &>(mProperty.mValue.second);
}

inline TraceProperty::operator unsigned int ( void ) const
{
    return static_cast<unsigned int>(mProperty.mValue.second);
}

inline TraceProperty::operator NETrace::eLogPriority ( void ) const
{
    return static_cast<NETrace::eLogPriority>(mProperty.mValue.second);
}

inline TraceProperty::operator bool( void ) const
{
    return static_cast<bool>(mProperty.mValue.second);
}

inline const TracePropertyKey & TraceProperty::getKey( void ) const
{
    return mProperty.mValue.first;
}

inline const TracePropertyValue & TraceProperty::getValue( void ) const
{
    return mProperty.mValue.second;
}

inline TracePropertyValue & TraceProperty::getValue( void )
{
    return mProperty.mValue.second;
}

inline const String & TraceProperty::getComment( void ) const
{
    return mComment;
}

inline bool TraceProperty::isValid(void) const
{
    return (mProperty.mValue.first.isValidKey() && mProperty.mValue.second.isValid());
}

#endif  // AREG_TRACE_PRIVATE_TRACEPROPERTY_HPP
