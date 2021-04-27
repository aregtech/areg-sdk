#ifndef AREG_PERSIST_CEPROPERTY_HPP
#define AREG_PERSIST_CEPROPERTY_HPP
/************************************************************************
 * \file        areg/persist/CEProperty.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Property object to persist data
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/persist/CEPropertyKey.hpp"
#include "areg/persist/CEPropertyValue.hpp"
#include "areg/base/TEProperty.hpp"
#include "areg/base/CEString.hpp"

//////////////////////////////////////////////////////////////////////////
// CEProperty class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Property object of persistence, which consists of key and value pairs.
 **/
class AREG_API CEProperty
{
//////////////////////////////////////////////////////////////////////////
// Types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The definition of type Property, which is a pair of Key and Value
     **/
    typedef TEProperty<CEPropertyKey, CEPropertyValue> Property;

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    CEProperty( void );

    /**
     * \brief   Parses and initializes Key-Value and the comment data from given parameters.
     * \param   keySet      The Key as a string to parse.
     * \param   valueSet    The Value as a string to parse.
     * \param   comment     The optional comment for the property.
     **/
    CEProperty( const char * keySet, const char * valueSet, const char * comment = NULL );
    /**
     * \brief   Initializes Key, value and the comment.
     * \param   newProperty The property as a key and value pair to set.
     * \param   comment     The optional comment for the property.
     **/
    CEProperty( const CEProperty::Property & newProperty, const char * comment = NULL );
    /**
     * \brief   Copies data from given source
     * \param   source  The source to copy data
     **/
    CEProperty( const CEProperty & source );
    /**
     * \brief   Destructor
     **/
    virtual ~CEProperty( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies property data from given source
     * \param   source  The source to copy data.
     **/
    const CEProperty & operator = ( const CEProperty & source );

    /**
     * \brief   Checks equality of two property objects.
     * \param   other   The property object to check.
     * \return  Returns true if property objects are equal.
     **/
    bool operator == ( const CEProperty & other ) const;

    /**
     * \brief   Checks inequality of two property objects.
     * \param   other   The property object to check.
     * \return  Returns true if property objects are not equal.
     **/
    bool operator != ( const CEProperty & other ) const;

    /**
     * \brief   Converts and returns 32-bit integer value of Property object.
     **/
    operator unsigned int ( void ) const;

//////////////////////////////////////////////////////////////////////////
// Operations and properties
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Sets and parses the key, which consists of several parts
     *          separated by key-separator symbol
     * \param   keySet  The key as a string to set and parse.
     **/
    void ParseKey( const char * keySet );

    /**
     * \brief   Sets the key of property.
     * \param   Key     The key value to set.
     **/
    void SetKey( const CEPropertyKey & Key );

    /**
     * \brief   Returns Key value of the property
     **/
    const CEPropertyKey & GetKey( void ) const;

    /**
     * \brief   Returns string value of the Key. 
     *          If Key consists of several sections, they are concatenated 
     *          and separated by key-separators symbol
     **/
    CEString GetKeyString( void ) const;

    /**
     * \brief   Sets and parses the value of property.
     * \param   valueSet    The value as a string to set and parse.
     **/
    void ParseValue( const char * valueSet );

    /**
     * \brief   Sets Value of the property.
     **/
    void SetValue( const CEPropertyValue & Value );

    /**
     * \brief   Returns Value of the property
     **/
    const CEPropertyValue & GetValue( void ) const;

    /**
     * \brief   Returns Value of Property as a string.
     *          If Value consists of several perts, all parts are concatenated by value-separator symbol.
     **/
    CEString GetValueString( void ) const;

    /**
     * \brief   Sets property comment, which is optional.
     **/
    void SetCommend( const char * comment );

    /**
     * \brief   Adds comment to the property
     **/
    void AddComment( const char * comment );

    /**
     * \brief   Returns comment of the property
     **/
    const char * GetComment( void ) const;

    /**
     * \brief   Sets the Key-Value pair of the property.
     * \param   newPair The Key-Value pair to set
     **/
    void SetPropertyPair( const CEProperty::Property & newPair );

    /**
     * \brief   Returns Key-Value pair of the property.
     **/
    const CEProperty::Property & GetPropertyPair( void ) const;

    /**
     * \brief   Returns true if Property is valid.
     *          Valid property does not have empty Key.
     **/
    bool IsValid( void ) const;

    /**
     * \brief   Parses given string, extracts Key and Value parts
     *          and initializes Property
     * \param   strProperties   The string, which contains data to set for Key and Value.
     * \return  Returns true if parsing succeeded and could extract property data.
     **/
    bool ParseProperty( const char * strProperties );

    /**
     * \brief   Converts Key-Value pair data to the string.
     **/
    CEString ToString( void ) const;

    /**
     * \brief   Resets and invalidates Property
     **/
    void ResetData( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Comment of Property
     **/
    CEString                mComment;
#if _MSC_VER
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   Key-Value pair of the Property
     **/
    CEProperty::Property    mProperty;
#if _MSC_VER
    #pragma warning(default: 4251)
#endif  // _MSC_VER
};

#endif  // AREG_PERSIST_CEPROPERTY_HPP
