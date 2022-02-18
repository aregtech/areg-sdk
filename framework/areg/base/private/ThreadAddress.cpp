/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/ThreadAddress.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Thread Address class
 ************************************************************************/
#include "areg/base/ThreadAddress.hpp"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/NEMath.hpp"

#include <utility>

//////////////////////////////////////////////////////////////////////////
// ThreadAddress class implementation
//////////////////////////////////////////////////////////////////////////

namespace
{
/**
 * \brief   Invalid thread name
 **/
constexpr std::string_view   INVALID_THREAD_NAME     { "INVALID_THREAD_NAME" };

}

/************************************************************************/
// ThreadAddress
/************************************************************************/
/**
 * \brief   Invalid thread address constant.
 **/
const ThreadAddress   ThreadAddress::INVALID_THREAD_ADDRESS;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ThreadAddress::ThreadAddress( void )
    : mThreadName   ( INVALID_THREAD_NAME.data() )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
}

ThreadAddress::ThreadAddress( const char * threadName )
    : mThreadName   ( threadName != nullptr ? threadName : INVALID_THREAD_NAME.data() )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mThreadName.resize( NEUtilities::ITEM_NAMES_MAX_LENGTH );
    mMagicNum    = ThreadAddress::_magicNumber(*this);
}

ThreadAddress::ThreadAddress( const ThreadAddress & src )
    : mThreadName   ( src.mThreadName )
    , mMagicNum     ( src.mMagicNum )
{
}

ThreadAddress::ThreadAddress( ThreadAddress && src ) noexcept
    : mThreadName   ( std::move(src.mThreadName) )
    , mMagicNum     ( src.mMagicNum )
{
    src.mMagicNum   = NEMath::CHECKSUM_IGNORE;
}

ThreadAddress::ThreadAddress( const IEInStream & stream )
    : mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    String name(stream);
    mThreadName = std::string(name);
}

bool ThreadAddress::isValid( void ) const
{
    return (mMagicNum != NEMath::CHECKSUM_IGNORE);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
std::string ThreadAddress::convAddressToPath( const ThreadAddress& threadAddress )
{
    return threadAddress.convToString();
}

ThreadAddress ThreadAddress::convPathToAddress( const char* threadPath, const char** out_nextPart /*= nullptr*/ )
{
    ThreadAddress result;
    result.convFromString(threadPath, out_nextPart);
    return result;
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

void ThreadAddress::convFromString(const char * threadPath, const char** OUT out_nextPart /*= nullptr*/)
{
    const char* strSource   = threadPath;
    if (out_nextPart != nullptr)
    {
        *out_nextPart = threadPath;
    }

    mThreadName  = String::getSubstring(strSource, NECommon::COMPONENT_PATH_SEPARATOR.data(), &strSource);
    mMagicNum    = ThreadAddress::_magicNumber(*this);

    if (out_nextPart != nullptr)
    {
        *out_nextPart = strSource;
    }
}

unsigned int ThreadAddress::_magicNumber(const ThreadAddress & addrThread)
{
    unsigned int result = NEMath::CHECKSUM_IGNORE;
    if ((addrThread.mThreadName.empty() == false) && (addrThread.mThreadName != INVALID_THREAD_NAME.data( )))
    {
        result = NEMath::crc32Calculate(addrThread.mThreadName.c_str());
    }

    return result;
}
