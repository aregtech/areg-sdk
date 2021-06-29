#ifndef AREG_BASE_IDENTIFIER_HPP
#define AREG_BASE_IDENTIFIER_HPP
/************************************************************************
 * \file        areg/base/Identifier.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Object Identifier class.
 *              The Identifier is binding string and integer values to
 *              provide conversion from string to integer and from 
 *              integer to string.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/String.hpp"

//////////////////////////////////////////////////////////////////////////
// Identifier class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The Identifier is binding string and integer values to
 *              provide conversion from string to integer and from 
 *              integer to string. It is used to pass enum values as strings.
 **/
class AREG_API Identifier
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Identifier::BAD_IDENTIFIER_VALUE
     *          Bad Identifier integer value. 
     *          The values is reserved and should not be used
     *          at least together with BAD_IDENTIFIER_NAME
     **/
    static const unsigned int   BAD_IDENTIFIER_VALUE    /*= static_cast<unsigned int>(0x80000000)*/;
    /**
     * \brief   Identifier::BAD_IDENTIFIER_NAME
     *          Bad Identifier name. The name is reserved and
     *          should not be used at leas together with BAD_IDENTIFIER_VALUE
     **/
    static const char* const    BAD_IDENTIFIER_NAME     /*= "_BAD_IDENTIFIER"*/;

public:
    /**
     * \brief   Identifier::BAD_IDENTIFIER
     *          Bad Identifier object, which integer value is BAD_IDENTIFIER_VALUE
     *          and string value is BAD_IDENTIFIER_NAME
     **/
    static const Identifier   BAD_IDENTIFIER;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. 
     *          By default, initial values are equal to BAD_DENTIFIER
     *          and the identifier object is invalid
     **/
    Identifier( void );

    /**
     * \brief   Initialization constructor. Sets integer value and name
     * \param   idValue     The integer value of identifier
     * \param   idName      The name of identifier associated with integer value.
     **/
    Identifier(unsigned int idValue, const char * idName);

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    Identifier(const Identifier & src);

    /**
     * \brief   Destructor
     **/
    ~Identifier( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assigning operator. Copies identifier data from given source
     **/
    const Identifier& operator = (const Identifier & src);

    /**
     * \brief   Comparing operator. 
     *          Returns true if 2 identifier objects are equal.
     **/
    inline bool operator == (const Identifier & other) const;

    /**
     * \brief   Comparing operator. 
     *          Returns true if the name of identifier is equal
     *          to the string on right side.
     **/
    inline bool operator == (const char * rhs) const;

    /**
     * \brief   Comparing operator. 
     *          Returns true if the integer value of identifier is equal
     *          to the value on right side.
     **/
    inline bool operator == (unsigned int rhs) const;

/************************************************************************
 * Friend functions. Declare to make it streamable
 * It is streamable object and the values can be streamed between different threads
 ************************************************************************/
    /**
     * \brief   Reads identifier value from stream.
     * \param   stream  Streaming object to read identifier value
     * \param   input   Identifier to write values
     * \return  Returns instance of read streaming object
     **/
    friend inline const IEInStream & operator >> (const IEInStream & stream, Identifier & input);

    /**
     * \brief   Writes identifier value to stream.
     * \param   stream  Streaming object to write identifier value
     * \param   input   Identifier to read values
     * \return  Returns instance of write streaming object
     **/
    friend inline IEOutStream & operator << (IEOutStream & stream, const Identifier & output);

//////////////////////////////////////////////////////////////////////////
// Operations / Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if Identifier is valid. 
     *          Invalid Identifier has values of BAD_DENTIFIER.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Reset Identifier data. Set to BAD_DENTIFIER.
     **/
    inline void invalidate( void );

    /**
     * \brief   Returns Identifier string value
     **/
    inline const char* getName( void ) const;

    /**
     * \brief   Returns Identifier integer value
     **/
    inline unsigned int getValue( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Integer value of Identifier
     **/
    unsigned int    mValue;
    /**
     * \brief   String value of Identifier
     **/
    String          mName;
};

//////////////////////////////////////////////////////////////////////////
// Identifier class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool Identifier::operator == ( const Identifier & other ) const
{   return (this != &other ? mValue == other.mValue && mName == other.mName : true);    }

inline bool Identifier::operator == ( const char * rhs ) const
{   return (rhs != NULL ? mName == rhs : false);    }

inline bool Identifier::operator == ( unsigned int rhs ) const
{   return (mValue == rhs);     }

inline bool Identifier::isValid( void ) const
{   return (*this == BAD_IDENTIFIER ? false : true); }

inline void Identifier::invalidate( void )
{   this->mValue = BAD_IDENTIFIER.mValue; this->mName = BAD_IDENTIFIER.mName; }

inline const char* Identifier::getName( void ) const
{   return static_cast<const char*>(mName);   }

inline unsigned int Identifier::getValue( void ) const
{   return mValue;  }

/************************************************************************
 * Friend functions. Declare to make it streamable
 * It is streamable object and the values can be streamed between different threads
 ************************************************************************/
inline const IEInStream& operator >> (const IEInStream& stream, Identifier& input)
{   stream >> input.mValue; stream >> input.mName;  return stream;  }

inline IEOutStream & operator << (IEOutStream& stream, const Identifier& output)
{   stream << output.mValue; stream << output.mName; return stream; }

#endif  // AREG_BASE_IDENTIFIER_HPP
