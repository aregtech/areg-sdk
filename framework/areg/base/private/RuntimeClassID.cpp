/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/RuntimeClassID.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Runtime Class ID
 *              This class contains information of Runtime Class ID.
 *              Every Runtime Class contains class ID to identify and
 *              cast class using ID. As an ID of Runtime class used class name.
 *
 ************************************************************************/
#include "areg/base/RuntimeClassID.hpp"
#include "areg/base/StringDefs.hpp"
#include "areg/base/MathDefs.hpp"

#include <string_view>

namespace
{
    /**
     * \brief   RuntimeClassID::BAD_CLASS_ID
     *          Bad Class ID. Defined as constant. Used to indicate invalid class ID name.
     **/
    constexpr std::string_view BAD_CLASS_ID { "_BAD_RUNTIME_CLASS_ID_" };
}

//////////////////////////////////////////////////////////////////////////
// RuntimeClassID class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
RuntimeClassID::RuntimeClassID()
    : mClassName(BAD_CLASS_ID)
    , mMagicNum (NEMath::CHECKSUM_IGNORE)
{
}

RuntimeClassID::RuntimeClassID( const char * className )
    : mClassName(BAD_CLASS_ID)
    , mMagicNum (NEMath::CHECKSUM_IGNORE)
{
    if (NEString::is_empty<char>(className) == false)
    {
        mClassName  = className;
        mMagicNum   = NEMath::crc32_calculate(className);
    }
}

RuntimeClassID::RuntimeClassID( const String& className )
    : mClassName(BAD_CLASS_ID)
    , mMagicNum (NEMath::CHECKSUM_IGNORE)
{
    if (className.is_empty() == false)
    {
        mClassName  = className;
        mMagicNum   = NEMath::crc32_calculate(className);
    }
}

RuntimeClassID::RuntimeClassID( const RuntimeClassID & src )
    : mClassName(src.mClassName)
    , mMagicNum (src.mMagicNum)
{
    ASSERT(src.mClassName.is_empty() == false);
}

RuntimeClassID::RuntimeClassID( RuntimeClassID && src ) noexcept
    : mClassName( std::move(src.mClassName) )
    , mMagicNum ( src.mMagicNum )
{
    ASSERT( src.mClassName.is_empty( ) == false );
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void RuntimeClassID::set_name( const String& className )
{
    if ( className.is_empty() || (className == BAD_CLASS_ID))
    {
        mClassName  = BAD_CLASS_ID;
        mMagicNum   = NEMath::CHECKSUM_IGNORE;
    }
    else
    {
        mClassName  = className;
        mMagicNum   = NEMath::crc32_calculate(className.as_string());
    }
}

void RuntimeClassID::set_name( const char* className )
{
    if ( NEString::is_empty<char>(className) || (BAD_CLASS_ID == className) )
    {
        mClassName  = BAD_CLASS_ID;
        mMagicNum   = NEMath::CHECKSUM_IGNORE;
    }
    else
    {
        mClassName  = className;
        mMagicNum   = NEMath::crc32_calculate(className);
    }
}
