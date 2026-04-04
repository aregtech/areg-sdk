/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/ThreadAddress.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Thread Address class
 ************************************************************************/
#include "areg/base/ThreadAddress.hpp"
#include "areg/base/IOStream.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/MathDefs.hpp"

#include <utility>

//////////////////////////////////////////////////////////////////////////
// ThreadAddress class implementation
//////////////////////////////////////////////////////////////////////////

namespace
{
    //!< Invalid thread name constant
constexpr std::string_view   INVALID_THREAD_NAME     { "INVALID_THREAD_NAME" };
}

namespace areg {

/************************************************************************/
// ThreadAddress
/************************************************************************/

const ThreadAddress & ThreadAddress::invalid_thread_address()
{
    static const ThreadAddress _invalidAddress;
    return _invalidAddress;
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ThreadAddress::ThreadAddress()
    : mThreadName   ( INVALID_THREAD_NAME )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
}

ThreadAddress::ThreadAddress( const char * threadName )
    : mThreadName   ( threadName != nullptr ? threadName : INVALID_THREAD_NAME )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    mThreadName.truncate( areg::ITEM_NAMES_MAX_LENGTH );
    mMagicNum    = ThreadAddress::_magic_number(*this);
}

ThreadAddress::ThreadAddress( const String & threadName )
    : mThreadName   ( threadName )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    mMagicNum    = ThreadAddress::_magic_number(*this);
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
    src.mMagicNum   = areg::CHECKSUM_IGNORE;
}

ThreadAddress::ThreadAddress( const InStream & stream )
    : mThreadName   ( stream )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    mMagicNum    = ThreadAddress::_magic_number(*this);
}

bool ThreadAddress::is_valid() const noexcept
{
    return (mMagicNum != areg::CHECKSUM_IGNORE);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
String ThreadAddress::to_path( const ThreadAddress& threadAddress )
{
    return threadAddress.to_string();
}

ThreadAddress ThreadAddress::from_path( const char* threadPath, const char** nextPart /*= nullptr*/ )
{
    ThreadAddress result;
    result.from_string(threadPath, nextPart);
    return result;
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

void ThreadAddress::from_string(const char * threadPath, const char** nextPart /*= nullptr*/)
{
    const char* strSource   = threadPath;
    if (nextPart != nullptr)
    {
        *nextPart = threadPath;
    }

    mThreadName  = String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource);
    mMagicNum    = ThreadAddress::_magic_number(*this);

    if (nextPart != nullptr)
    {
        *nextPart = strSource;
    }
}

uint32_t ThreadAddress::_magic_number(const ThreadAddress & addrThread)
{
    return (!addrThread.mThreadName.is_empty() && (addrThread.mThreadName != INVALID_THREAD_NAME))
            ? areg::crc32_calculate(addrThread.mThreadName.as_string())
            : areg::CHECKSUM_IGNORE;
}

} // namespace areg
