/************************************************************************
 * \file        areg/base/private/ThreadAddress.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Thread Address class
 ************************************************************************/
#include "areg/base/ThreadAddress.hpp"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/NEMath.hpp"

//////////////////////////////////////////////////////////////////////////
// ThreadAddress class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// ThreadAddress
/************************************************************************/
/**
 * \brief   Invalid thread name
 **/
const char* const       ThreadAddress::INVALID_THREAD_NAME    = static_cast<const char *>("INVALID_THREAD_NAME");
/**
 * \brief   Invalid thread address constant.
 **/
const ThreadAddress   ThreadAddress::INVALID_THREAD_ADDRESS(ThreadAddress::INVALID_THREAD_NAME);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ThreadAddress::ThreadAddress( void )
    : mThreadName   ( ThreadAddress::INVALID_THREAD_NAME )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    ; // do nothing
}

ThreadAddress::ThreadAddress( const char* threadName )
    : mThreadName   ( threadName != NULL ? threadName : ThreadAddress::INVALID_THREAD_NAME )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mThreadName.truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
    mMagicNum    = ThreadAddress::_magicNumber(*this);
}

ThreadAddress::ThreadAddress( const ThreadAddress & src )
    : mThreadName   ( src.mThreadName )
    , mMagicNum     ( src.mMagicNum )
{
    ; // do nothing
}

ThreadAddress::ThreadAddress( const IEInStream & stream )
    : mThreadName   ( stream )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mMagicNum    = ThreadAddress::_magicNumber(*this);
}

ThreadAddress::~ThreadAddress( void )
{
    mMagicNum = NEMath::CHECKSUM_IGNORE;
}

bool ThreadAddress::isValid( void ) const
{
    return (mMagicNum != NEMath::CHECKSUM_IGNORE);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
String ThreadAddress::convAddressToPath( const ThreadAddress& threadAddress )
{
    return threadAddress.convToString();
}

ThreadAddress ThreadAddress::convPathToAddress( const char* threadPath, const char** out_nextPart /*= NULL*/ )
{
    ThreadAddress result;
    result.convFromString(threadPath, out_nextPart);
    return result;
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

void ThreadAddress::convFromString(const char * threadPath, const char** OUT out_nextPart /*= NULL*/)
{
    const char* strSource   = threadPath;
    if (out_nextPart != NULL)
        *out_nextPart = threadPath;

    mThreadName  = String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);
    mMagicNum    = ThreadAddress::_magicNumber(*this);

    if (out_nextPart != NULL)
        *out_nextPart = strSource;
}

unsigned int ThreadAddress::_magicNumber(const ThreadAddress & addrThread)
{
    unsigned int result = NEMath::CHECKSUM_IGNORE;
    if ((addrThread.mThreadName.isEmpty() == false) && (addrThread.mThreadName != ThreadAddress::INVALID_THREAD_NAME))
    {
        result = NEMath::crc32Calculate(addrThread.mThreadName.getString());
    }

    return result;
}
