/************************************************************************
 * \file        areg/src/base/private/CEIdentifier.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Object Identifier
 *
 ************************************************************************/
#include "areg/src/base/CEIdentifier.hpp"

//////////////////////////////////////////////////////////////////////////
// CEIdentifier class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Types and constants
//////////////////////////////////////////////////////////////////////////
const unsigned int  CEIdentifier::BAD_IDENTIFIER_VALUE = static_cast<unsigned int>(0x80000000); // static_cast<unsigned int>(-6719744); /* 4288247552 or 0xFF997700*/
const char* const   CEIdentifier::BAD_IDENTIFIER_NAME  = "_BAD_IDENTIFIER";
const CEIdentifier  CEIdentifier::BAD_IDENTIFIER(CEIdentifier::BAD_IDENTIFIER_VALUE, CEIdentifier::BAD_IDENTIFIER_NAME );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEIdentifier::CEIdentifier( void )
    : mValue    (CEIdentifier::BAD_IDENTIFIER_VALUE)
    , mName     (CEIdentifier::BAD_IDENTIFIER_NAME)
{
    ; // do nothing
}

CEIdentifier::CEIdentifier( unsigned int idValue, const char * idName )
    : mValue    (idValue)
    , mName     (idName)
{
    ; // do nothing
}

CEIdentifier::CEIdentifier( const CEIdentifier & src )
    : mValue    (src.mValue)
    , mName     (src.mName)
{
    ; // do nothing
}

CEIdentifier::~CEIdentifier( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

const CEIdentifier& CEIdentifier::operator = ( const CEIdentifier & src )
{
    if ( static_cast<const CEIdentifier *>(this) != &src )
    {
        this->mValue    = src.mValue;
        this->mName     = src.mName;
    }
    return (*this);
}
