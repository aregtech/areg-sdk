/************************************************************************
 * \file        areg/base/private/Identifier.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Object Identifier
 *
 ************************************************************************/
#include "areg/base/Identifier.hpp"

//////////////////////////////////////////////////////////////////////////
// Identifier class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Types and constants
//////////////////////////////////////////////////////////////////////////
const unsigned int  Identifier::BAD_IDENTIFIER_VALUE = static_cast<unsigned int>(0x80000000); // static_cast<unsigned int>(-6719744); /* 4288247552 or 0xFF997700*/
const char* const   Identifier::BAD_IDENTIFIER_NAME  = "_BAD_IDENTIFIER";
const Identifier    Identifier::BAD_IDENTIFIER(Identifier::BAD_IDENTIFIER_VALUE, Identifier::BAD_IDENTIFIER_NAME );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Identifier::Identifier( void )
    : mValue    (Identifier::BAD_IDENTIFIER_VALUE)
    , mName     (Identifier::BAD_IDENTIFIER_NAME)
{
    ; // do nothing
}

Identifier::Identifier( unsigned int idValue, const char * idName )
    : mValue    (idValue)
    , mName     (idName)
{
    ; // do nothing
}

Identifier::Identifier( const Identifier & src )
    : mValue    (src.mValue)
    , mName     (src.mName)
{
    ; // do nothing
}

Identifier::~Identifier( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

const Identifier& Identifier::operator = ( const Identifier & src )
{
    if ( static_cast<const Identifier *>(this) != &src )
    {
        this->mValue    = src.mValue;
        this->mName     = src.mName;
    }
    return (*this);
}
