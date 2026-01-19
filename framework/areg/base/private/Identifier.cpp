/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/Identifier.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Object Identifier
 *
 ************************************************************************/
#include "areg/base/Identifier.hpp"

#include <utility>

//////////////////////////////////////////////////////////////////////////
// Identifier class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Types and constants
//////////////////////////////////////////////////////////////////////////
const Identifier    Identifier::BAD_IDENTIFIER{ Identifier::BAD_IDENTIFIER_VALUE, Identifier::BAD_IDENTIFIER_NAME };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Identifier::Identifier( void )
    : mValue    (Identifier::BAD_IDENTIFIER_VALUE)
    , mName     (Identifier::BAD_IDENTIFIER_NAME)
{
}

Identifier::Identifier( unsigned int idValue, const char * idName )
    : mValue    (idValue)
    , mName     (idName)
{
}

Identifier::Identifier( unsigned int idValue, const std::string_view& idName )
    : mValue    (idValue)
    , mName     (idName)
{
}

Identifier::Identifier( unsigned int idValue, const String& idName )
    : mValue    (idValue)
    , mName     (idName)
{
}

Identifier::Identifier( const Identifier & src )
    : mValue    (src.mValue)
    , mName     (src.mName)
{
}

Identifier::Identifier( Identifier && src ) noexcept
    : mValue    ( std::move(src.mValue) )
    , mName     ( std::move(src.mName) )
{
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

Identifier& Identifier::operator = ( const Identifier & src )
{
    this->mValue    = src.mValue;
    this->mName     = src.mName;

    return (*this);
}

Identifier & Identifier::operator = ( Identifier && src )  noexcept
{
    this->mValue    = std::move( src.mValue );
    this->mName     = std::move( src.mName );

    return (*this);
}
