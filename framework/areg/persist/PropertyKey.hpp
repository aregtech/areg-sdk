#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/persist/PropertyKey.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Property Key object to persist data.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"

//////////////////////////////////////////////////////////////////////////
// PropertyKey class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Property Key object.
 *          The Property Key object consists of following parts:
 *              - Section
 *              - Property
 *              - Module
 *              - Position
 *          The Property Key is valid if at least section and property exist.
 *          The other parts are optional and can be omitted if not used.
 **/
class AREG_API PropertyKey
{
//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    PropertyKey( void ) = default;
    /**
     * \brief   Parses and initializes Key.
     * \param   key     The Key as a string to parse.
     **/
    explicit PropertyKey( const String & key );
    /**
     * \brief   Copies data from given source
     * \param   source  The source to copy data
     **/
    PropertyKey( const PropertyKey & source );
    /**
     * \brief   Moves data from given source
     * \param   source  The source to move data
     **/
    PropertyKey( PropertyKey && source ) noexcept;
    /**
     * \brief   Destructor
     **/
    ~PropertyKey( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies property Key from given source
     * \param   source  The source to copy data.
     **/
    PropertyKey & operator = ( const PropertyKey & source );

    /**
     * \brief   Moves property Key from given source
     * \param   source  The source to move data.
     **/
    PropertyKey & operator = ( PropertyKey && source ) noexcept;

    /**
     * \brief   Copies property Key from given source as a string
     * \param   source  The source as string to parse and copy data.
     **/
    PropertyKey & operator = ( const String & params );

    /**
     * \brief   Checks equality of two Key objects.
     * \param   other   The Key object to check.
     * \return  Returns true if Key objects are equal.
     **/
    bool operator == ( const PropertyKey & other ) const;

    /**
     * \brief   Checks inequality of two Key objects.
     * \param   other   The Key object to check.
     * \return  Returns true if Key objects are not equal.
     **/
    bool operator != ( const PropertyKey & other ) const;

    /**
     * \brief   Converts and returns 32-bit integer value of Key object.
     **/
    explicit operator unsigned int ( void ) const;

//////////////////////////////////////////////////////////////////////////
// Operations and properties
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Converts Key data to the string.
     *          If Key consists of several parts, each part is concatenated by key-separator symbol.
     **/
    String convToString( void ) const;

    /**
     * \brief   Sets Key data, which consists of section, property, module and position sections.
     *          Key 'module' and 'position' are optional and can be empty.
     * \param   section     The section value of Key
     * \param   property    The property value of Key
     * \param   module      The module value of Key
     * \param   property    The position value of Key
     **/
    void setValues( const char * section, const char * property, const char * module = nullptr, const char * position = nullptr);

    /**
     * \brief   Returns section part of the Key
     **/
    const String & getSection( void ) const;

    /**
     * \brief   Returns property part of the Key
     **/
    const String & getProperty( void ) const;

    /**
     * \brief   Returns module part of the Key
     **/
    const String & getModule( void ) const;

    /**
     * \brief   Returns position part of the Key
     **/
    const String & getPosition( void ) const;

    /**
     * \brief   Returns true if the Key is valid.
     *          The Key is valid if at least section and property parts are not empty.
     **/
    bool isValid( void ) const;

    /**
     * \brief   Returns true if Key has module part.
     **/
    bool hasModule( void ) const;

    /**
     * \brief   Returns true if Key has position part.
     **/
    bool hasPosition( void ) const;

    /**
     * \brief   Parses given string, extracts and initializes Key data.
     * \param   key     The string, which contains data for Key.
     * \return  Returns true if parsing succeeded and could extract property data.
     **/
    bool parseKey( const String & key );

    /**
     * \brief   Resets and invalidates Key
     **/
    void resetKey( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Section part of the key, which is at position 1
     **/
    String  mSection;      // pos 1
    /**
     * \brief   Property part of the key, which is at position 2
     **/
    String  mProperty;     // pos 2
    /**
     * \brief   Module part of the key, which is at position 3
     **/
    String  mModule;       // pos 3
    /**
     * \brief   Position part of the key, which is at position 4
     **/
    String  mPosition;     // pos 4
};

//////////////////////////////////////////////////////////////////////////
// Hasher of PropertyKey class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the PropertyKey.
 */
namespace std
{
    template<>
    struct hash<PropertyKey>
    {
        //! A function to convert PropertyKey object to unsigned int.
        inline unsigned int operator()(const PropertyKey& key) const
        {
            return static_cast<unsigned int>(key);
        }
    };
}
