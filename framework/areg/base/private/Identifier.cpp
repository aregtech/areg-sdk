/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/Identifier.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Object Identifier
 *
 ************************************************************************/
#include "areg/base/Identifier.hpp"

#include <utility>

//////////////////////////////////////////////////////////////////////////
// Identifier class implementation
//////////////////////////////////////////////////////////////////////////

namespace 
{
    /**
     * \brief   Identifier::BAD_IDENTIFIER_VALUE
     *          Bad Identifier integer value.
     *          The values is reserved and should not be used
     *          at least together with BAD_IDENTIFIER_NAME
     **/
    constexpr unsigned int       BAD_IDENTIFIER_VALUE   { static_cast<unsigned int>(0x80000000) };

    /**
     * \brief   Identifier::BAD_IDENTIFIER_NAME
     *          Bad Identifier name. The name is reserved and
     *          should not be used at leas together with BAD_IDENTIFIER_VALUE
     **/
    constexpr std::string_view   BAD_IDENTIFIER_NAME     { "_BAD_IDENTIFIER" };

}

//////////////////////////////////////////////////////////////////////////
// Types and constants
//////////////////////////////////////////////////////////////////////////
const Identifier    Identifier::BAD_IDENTIFIER( BAD_IDENTIFIER_VALUE, BAD_IDENTIFIER_NAME );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Identifier::Identifier( void )
    : mValue    (BAD_IDENTIFIER_VALUE)
    , mName     (BAD_IDENTIFIER_NAME)
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
