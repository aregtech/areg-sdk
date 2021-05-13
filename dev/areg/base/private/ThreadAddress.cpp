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
{
    ; // do nothing
}

ThreadAddress::ThreadAddress( const char* threadName )
    : mThreadName   ( threadName != NULL ? threadName : ThreadAddress::INVALID_THREAD_NAME )
{
    mThreadName.truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
}

ThreadAddress::ThreadAddress( const ThreadAddress & src )
    : mThreadName   ( src.mThreadName )
{
    ; // do nothing
}

ThreadAddress::ThreadAddress( const IEInStream & stream )
    : mThreadName   ( stream )
{
    ;
}

ThreadAddress::~ThreadAddress( void )
{
    ; // do nothing
}

bool ThreadAddress::isValid( void ) const
{
    return (mThreadName != ThreadAddress::INVALID_THREAD_NAME);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
String ThreadAddress::convAddressToPath( const ThreadAddress& threadAddress )
{
    return threadAddress.mThreadName;
}

ThreadAddress ThreadAddress::convPathToAddress( const char* threadPath, const char** out_nextPart /*= NULL*/ )
{
    ThreadAddress result;
    const char* strSource   = threadPath;
    if (out_nextPart != NULL)
        *out_nextPart = threadPath;

    result.mThreadName  = String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);

    if (out_nextPart != NULL)
        *out_nextPart = strSource;

    return result;
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

bool ThreadAddress::operator == ( const ThreadAddress & other ) const
{
    return mThreadName == other.mThreadName;
}

bool ThreadAddress::operator != ( const ThreadAddress & other ) const
{
    return mThreadName != other.mThreadName;
}

ThreadAddress::operator unsigned int( void ) const
{
    return (isValid() ? NEMath::crc32Calculate(mThreadName.getString()) : NEMath::CHECKSUM_IGNORE);
}
