/************************************************************************
 * \file        areg/src/component/private/CEStubAddress.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Stub Address implementation.
 *
 ************************************************************************/
#include "areg/src/component/CEStubAddress.hpp"

#include "areg/src/component/CEServiceRequestEvent.hpp"
#include "areg/src/component/CEDispatcherThread.hpp"
#include "areg/src/component/CEProxyAddress.hpp"
#include "areg/src/base/CEThreadAddress.hpp"
#include "areg/src/base/IEIOStream.hpp"
#include "areg/src/base/NEUtilities.hpp"

//////////////////////////////////////////////////////////////////////////
// CEProxyAddress class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Predefined constants and statics
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The invalid service interface name (invalid stub name). None of service interfaces should contain this name.
 **/
const char * const  CEStubAddress::INVALID_STUB_NAME    = "INVALID_STUB";
/**
 * \brief   Extension to add to Stub path.
 **/
const char * const  CEStubAddress::EXTENTION_STUB       = "stub";
/**
 * \brief   Predefined Invalid Stub address.
 **/
const CEStubAddress CEStubAddress::INVALID_STUB_ADDRESS;

//////////////////////////////////////////////////////////////////////////
// Static functions
//////////////////////////////////////////////////////////////////////////
CEString CEStubAddress::ConvertAddressToPath( const CEStubAddress & stubAddress )
{
    CEString result = "";
    
    result += EXTENTION_STUB;
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += CEServiceAddress::ConvertAddressToPath( static_cast<const CEServiceAddress &>(stubAddress));
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += stubAddress.mThreadName;
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += stubAddress.mChannel.ToString();

    return result;
}

CEStubAddress CEStubAddress::ConvertPathToAddress( const char* pathStub, const char** out_nextPart /*= NULL*/ )
{
    CEString stubKey;
    CEStubAddress result(CEStubAddress::INVALID_STUB_ADDRESS);
    const char* strSource = pathStub;

    if (out_nextPart != NULL)
        *out_nextPart = pathStub;

    if ( CEString::GetSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource) == CEStubAddress::EXTENTION_STUB )
    {
        static_cast<CEServiceItem &>(result)= CEServiceAddress::ConvertPathToAddress(strSource, &strSource);
        result.mThreadName  = CEString::GetSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);
        result.mChannel.FromString( CEString::GetSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource).GetBuffer() );

        if (out_nextPart != NULL)
            *out_nextPart = strSource;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CEStubAddress::CEStubAddress( void )
    : CEServiceAddress  ( CEServiceItem(), CEStubAddress::INVALID_STUB_NAME )
    , mThreadName       ( CEThreadAddress::INVALID_THREAD_ADDRESS.GetThreadName() )
    , mChannel          ( )
{
    ; // do nothing
}

CEStubAddress::CEStubAddress( const char * serviceName
                            , const CEVersion & serviceVersion
                            , NEService::eServiceType serviceType
                            , const char * roleName
                            , const char * threadName   /*= NULL*/ )
    : CEServiceAddress  ( serviceName, serviceVersion, serviceType, roleName )
    , mThreadName       ( threadName )
    , mChannel          ( )
{
    mThreadName.Truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
    if ( CEServiceAddress::IsValid() )
        mChannel.SetCookie(NEService::COOKIE_LOCAL);
}

CEStubAddress::CEStubAddress(const CEServiceItem & service, const char * roleName, const char * threadName /*= NULL */)
    : CEServiceAddress  ( service, roleName )
    , mThreadName       ( threadName )
    , mChannel          ( )
{
    mThreadName.Truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
    if ( CEServiceAddress::IsValid() )
        mChannel.SetCookie(NEService::COOKIE_LOCAL);
}

CEStubAddress::CEStubAddress(const NEService::SInterfaceData & siData, const char * roleName, const char * threadName /*= NULL */)
    : CEServiceAddress  ( siData.idServiceName, siData.idVersion, siData.idServiceType, roleName )
    , mThreadName       ( "" )
    , mChannel          ( )
{
    SetThread(threadName);
    if ( CEServiceAddress::IsValid() )
        mChannel.SetCookie(NEService::COOKIE_LOCAL);
}

CEStubAddress::CEStubAddress( const CEStubAddress & source )
    : CEServiceAddress  ( static_cast<const CEServiceAddress &>(source) )
    , mThreadName       ( source.mThreadName )
    , mChannel          ( source.mChannel )
{
    ; // do nothing
}

CEStubAddress::CEStubAddress( const IEInStream & stream )
    : CEServiceAddress  ( stream )
    , mThreadName       ( stream )
    , mChannel          ( )
{
    ITEM_ID cookie = NEService::COOKIE_LOCAL;
    stream >> cookie;
    if ( CEServiceAddress::IsValid() )
        mChannel.SetCookie(cookie);
}

CEStubAddress::~CEStubAddress( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

const CEStubAddress & CEStubAddress::operator = ( const CEStubAddress & source )
{
    static_cast<CEServiceAddress &>(*this) = static_cast<const CEServiceAddress &>(source);
    mThreadName = source.mThreadName;
    mChannel    = source.mChannel;
    return (*this);
}

const CEStubAddress & CEStubAddress::operator = (const CEServiceAddress & addrService)
{
    static_cast<CEServiceAddress &>(*this) = static_cast<const CEServiceAddress &>(addrService);
    mThreadName = "";
    mChannel    = CEChannel();
    return (*this);
}

bool CEStubAddress::operator == ( const CEStubAddress & other ) const
{
    return ( (static_cast<const CEServiceAddress &>(*this) == static_cast<const CEServiceAddress &>(other)) && 
             (mThreadName == other.mThreadName) &&  
             (mChannel.GetCookie() == other.mChannel.GetCookie()) );
}

inline bool CEStubAddress::operator != ( const CEStubAddress& other ) const
{
    return ( (static_cast<const CEServiceAddress &>(*this) != static_cast<const CEServiceAddress &>(other)) || 
             (mThreadName != other.mThreadName) || 
             (mChannel.GetCookie() != other.mChannel.GetCookie()) );
}

bool CEStubAddress::IsProxyCompatible(const CEProxyAddress & proxyAddress) const
{
    if ( IsValid() && proxyAddress.IsValid() )
        return (mRoleName == proxyAddress.GetRoleName() && IsServiceCompatible( static_cast<const CEServiceItem &>(proxyAddress) ) );
    else
        return false;
}

void CEStubAddress::SetThread(const char * threadName)
{
    CEThread * thread = threadName == NULL ? CEThread::GetCurrentThread() : CEThread::FindThreadByName(threadName);
    CEDispatcherThread * dispatcher = RUNTIME_CAST( thread, CEDispatcherThread);
    if ( dispatcher != NULL )
    {
        mThreadName = dispatcher->GetThreadAddress().GetThreadName();
        mChannel.SetSource( dispatcher->GetThreadId() );
    }

    mThreadName.Truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
}

bool CEStubAddress::DeliverServiceEvent( CEServiceRequestEvent & serviceEvent ) const
{
    bool result = false;
    ITEM_ID target = mChannel.GetSource();
    if ( target != NEService::TARGET_UNKNOWN )
    {
        CEThread * thread = CEThread::FindThreadById(target);
        CEDispatcherThread * dispatcher = thread != NULL ? RUNTIME_CAST(thread, CEDispatcherThread) : NULL;
        if ( dispatcher != NULL )
        {
            result = serviceEvent.RegisterForThread(dispatcher);
            serviceEvent.SendEvent();
        }
        else
        {
            serviceEvent.Destroy();
        }
    }
    else
    {
        serviceEvent.Destroy();
    }
    return result;
}

void CEStubAddress::InvalidateChannel( void )
{
    mChannel.Invalidate();
}

bool CEStubAddress::IsValid( void ) const
{
    return mChannel.IsValid();
}

AREG_API const IEInStream & operator >> ( const IEInStream & stream, CEStubAddress & input )
{
    ITEM_ID cookie = NEService::COOKIE_LOCAL;
    stream >> static_cast<CEServiceAddress &>(input);
    stream >> input.mThreadName;
    stream >> cookie;
    input.SetCookie(cookie);
    return stream;
}

AREG_API IEOutStream & operator << ( IEOutStream & stream, const CEStubAddress & output)
{
    stream << static_cast<const CEServiceAddress &>(output);
    stream << output.mThreadName;
    stream << output.mChannel.GetCookie();
    return stream;
}
