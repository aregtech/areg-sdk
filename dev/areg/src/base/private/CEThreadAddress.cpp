/************************************************************************
 * \file        areg/src/base/private/CEThreadAddress.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Thread Address class
 ************************************************************************/
#include "areg/src/base/CEThreadAddress.hpp"
#include "areg/src/base/IEIOStream.hpp"
#include "areg/src/base/CEThread.hpp"
#include "areg/src/base/CEProcess.hpp"
#include "areg/src/base/NEMath.hpp"

//////////////////////////////////////////////////////////////////////////
// CEThreadAddress class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// CEThreadAddress
/************************************************************************/
/**
 * \brief   Invalid thread name
 **/
const char* const       CEThreadAddress::INVALID_THREAD_NAME    = static_cast<const char *>("INVALID_THREAD_NAME");
/**
 * \brief   Invalid thread address constant.
 **/
const CEThreadAddress   CEThreadAddress::INVALID_THREAD_ADDRESS(CEThreadAddress::INVALID_THREAD_NAME);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEThreadAddress::CEThreadAddress( void )
    : mThreadName   ( CEThreadAddress::INVALID_THREAD_NAME )
{
    ; // do nothing
}

CEThreadAddress::CEThreadAddress( const char* threadName )
    : mThreadName   ( threadName != NULL ? threadName : CEThreadAddress::INVALID_THREAD_NAME )
{
    mThreadName.Truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
}

CEThreadAddress::CEThreadAddress( const CEThreadAddress & src )
    : mThreadName   ( src.mThreadName )
{
    ; // do nothing
}

CEThreadAddress::CEThreadAddress( const IEInStream & stream )
    : mThreadName   ( stream )
{
    ;
}

CEThreadAddress::~CEThreadAddress( void )
{
    ; // do nothing
}

bool CEThreadAddress::IsValid( void ) const
{
    return (mThreadName != CEThreadAddress::INVALID_THREAD_NAME);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
CEString CEThreadAddress::ConvertAddressToPath( const CEThreadAddress& threadAddress )
{
    return threadAddress.mThreadName;
}

CEThreadAddress CEThreadAddress::ConvertPathToAddress( const char* threadPath, const char** out_nextPart /*= NULL*/ )
{
    CEThreadAddress result;
    const char* strSource   = threadPath;
    if (out_nextPart != NULL)
        *out_nextPart = threadPath;

    result.mThreadName  = CEString::GetSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);

    if (out_nextPart != NULL)
        *out_nextPart = strSource;

    return result;
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

bool CEThreadAddress::operator == ( const CEThreadAddress & other ) const
{
    return mThreadName == other.mThreadName;
}

bool CEThreadAddress::operator != ( const CEThreadAddress & other ) const
{
    return mThreadName != other.mThreadName;
}

CEThreadAddress::operator unsigned int( void ) const
{
    return (IsValid() ? NEMath::Crc32Calculate(mThreadName.GetBuffer()) : NEMath::CHECKSUM_IGNORE);
}
