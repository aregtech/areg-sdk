#ifndef AREG_PERSIST_CEPROPERTYKEY_HPP
#define AREG_PERSIST_CEPROPERTYKEY_HPP
/************************************************************************
 * \file        areg/persist/CEPropertyKey.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Property Key object to persist data.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/CEString.hpp"

//////////////////////////////////////////////////////////////////////////
// CEPropertyKey class declaration
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
class AREG_API CEPropertyKey
{
//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    CEPropertyKey( void );
    /**
     * \brief   Parses and initializes Key.
     * \param   key     The Key as a string to parse.
     **/
    CEPropertyKey( const CEString & key );
    /**
     * \brief   Copies data from given source
     * \param   source  The source to copy data
     **/
    CEPropertyKey( const CEPropertyKey & source );
    /**
     * \brief   Destructor
     **/
    ~CEPropertyKey( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies property Key from given source
     * \param   source  The source to copy data.
     **/
    const CEPropertyKey & operator = ( const CEPropertyKey & source );

    /**
     * \brief   Copies property Key from given source as a string
     * \param   source  The source as string to parse and copy data.
     **/
    const CEPropertyKey & operator = ( const CEString & params );

    /**
     * \brief   Checks equality of two Key objects.
     * \param   other   The Key object to check.
     * \return  Returns true if Key objects are equal.
     **/
    bool operator == ( const CEPropertyKey & other ) const;

    /**
     * \brief   Checks inequality of two Key objects.
     * \param   other   The Key object to check.
     * \return  Returns true if Key objects are not equal.
     **/
    bool operator != ( const CEPropertyKey & other ) const;

    /**
     * \brief   Converts and returns 32-bit integer value of Key object.
     **/
    operator unsigned int ( void ) const;

//////////////////////////////////////////////////////////////////////////
// Operations and properties
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Converts Key data to the string.
     *          If Key consists of several parts, each part is concatenated by key-separator symbol.
     **/
    CEString ToString( void ) const;

    /**
     * \brief   Sets Key data, which consists of section, property, module and position sections.
     *          Key 'module' and 'position' are optional and can be empty.
     * \param   section     The section value of Key
     * \param   property    The property value of Key
     * \param   module      The module value of Key
     * \param   property    The position value of Key
     **/
    void SetValues( const char * section, const char * property, const char * module = NULL, const char * position = NULL);

    /**
     * \brief   Returns section part of the Key
     **/
    const CEString & GetSection( void ) const;

    /**
     * \brief   Returns property part of the Key
     **/
    const CEString & GetProperty( void ) const;

    /**
     * \brief   Returns module part of the Key
     **/
    const CEString & GetModule( void ) const;

    /**
     * \brief   Returns position part of the Key
     **/
    const CEString & GetPosition( void ) const;

    /**
     * \brief   Returns true if the Key is valid.
     *          The Key is valid if at least section and property parts are not empty.
     **/
    bool IsValid( void ) const;

    /**
     * \brief   Returns true if Key has module part.
     **/
    bool HasModule( void ) const;

    /**
     * \brief   Returns true if Key has position part.
     **/
    bool HasPosition( void ) const;

    /**
     * \brief   Parses given string, extracts and initializes Key data.
     * \param   key     The string, which contains data for Key.
     * \return  Returns true if parsing succeeded and could extract property data.
     **/
    bool ParseKey( const CEString & key );

    /**
     * \brief   Resets and invalidates Key
     **/
    void ResetKey( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Section part of the key, which is at position 1
     **/
    CEString mSection;      // pos 1
    /**
     * \brief   Property part of the key, which is at position 2
     **/
    CEString mProperty;     // pos 2
    /**
     * \brief   Module part of the key, which is at position 3
     **/
    CEString mModule;       // pos 3
    /**
     * \brief   Position part of the key, which is at position 4
     **/
    CEString mPosition;     // pos 4
};

#endif  // AREG_PERSIST_CEPROPERTYKEY_HPP
