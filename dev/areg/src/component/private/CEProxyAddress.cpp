/************************************************************************
 * \file        areg/src/component/private/CEProxyAddress.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Proxy Address class implementation.
 *
 ************************************************************************/
#include "areg/src/component/CEProxyAddress.hpp"

#include "areg/src/base/NEUtilities.hpp"
#include "areg/src/base/IEIOStream.hpp"
#include "areg/src/component/CEServiceResponseEvent.hpp"
#include "areg/src/component/CEServiceRequestEvent.hpp"
#include "areg/src/component/CEDispatcherThread.hpp"
#include "areg/src/component/CEStubAddress.hpp"

//////////////////////////////////////////////////////////////////////////
// CEProxyAddress class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Static variables
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   The name of invalid proxy.
 **/
const char * const      CEProxyAddress::INVALID_PROXY_NAME      = "INVALID_PROXY";
/**
 * \brief   Extension, used in the path to convert Proxy Address to string object.
 **/
const char * const      CEProxyAddress::EXTENTION_PROXY         = "proxy";
/**
 * \brief   Invalid proxy address.
 **/
const CEProxyAddress    CEProxyAddress::INVALID_PROXY_ADDRESS;

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
CEString CEProxyAddress::ConvertAddressToPath( const CEProxyAddress & proxyAddress )
{
    CEString result = "";

    result += CEProxyAddress::EXTENTION_PROXY;
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += CEServiceAddress::ConvertAddressToPath( static_cast<const CEServiceAddress &>(proxyAddress));
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += proxyAddress.mThreadName;
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += proxyAddress.mChannel.ToString();

    return result;
}

CEProxyAddress CEProxyAddress::ConvertPathToAddress( const char* pathProxy, const char** out_nextPart /*= NULL*/ )
{
    CEProxyAddress result(CEProxyAddress::INVALID_PROXY_ADDRESS);
    const char* strSource = pathProxy;

    if (out_nextPart != NULL)
        *out_nextPart = pathProxy;

    if ( CEString::GetSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource) == CEProxyAddress::EXTENTION_PROXY )
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
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

CEProxyAddress::CEProxyAddress( void )
    : CEServiceAddress  ( CEServiceItem(), CEProxyAddress::INVALID_PROXY_NAME )
    , mThreadName       ( CEThreadAddress::INVALID_THREAD_ADDRESS.GetThreadName() )
    , mChannel          ( )
{
    ; // do nothing
}

CEProxyAddress::CEProxyAddress( const char * serviceName, const CEVersion & serviceVersion, NEService::eServiceType serviceType, const char * roleName, const char * threadName /*= NULL*/ )
    : CEServiceAddress  ( serviceName, serviceVersion, serviceType, roleName )
    , mThreadName       ( threadName )
    , mChannel          ( )
{
    SetThread( threadName );
    if ( CEServiceAddress::IsValid() )
        mChannel.SetCookie(NEService::COOKIE_LOCAL);
}

CEProxyAddress::CEProxyAddress( const CEServiceItem & service, const char * roleName, const char * threadName /*= NULL*/ )
    : CEServiceAddress  ( service, roleName )
    , mThreadName       ( "" )
    , mChannel          ( )
{
    SetThread( threadName );
    if ( CEServiceAddress::IsValid() )
        mChannel.SetCookie(NEService::COOKIE_LOCAL);
}

CEProxyAddress::CEProxyAddress(const NEService::SInterfaceData & siData, const char * roleName, const char * threadName /*= NULL */)
    : CEServiceAddress  ( siData.idServiceName, siData.idVersion, siData.idServiceType, roleName )
    , mThreadName       ( "" )
    , mChannel          ( )
{
    SetThread(threadName);
    if ( CEServiceAddress::IsValid() )
        mChannel.SetCookie(NEService::COOKIE_LOCAL);
}

CEProxyAddress::CEProxyAddress( const CEProxyAddress & source )
    : CEServiceAddress  ( static_cast<const CEServiceAddress &>(source) )
    , mThreadName       ( source.mThreadName )
    , mChannel          ( source.mChannel )
{
    ; // do nothing
}

CEProxyAddress::CEProxyAddress( const IEInStream & stream )
    : CEServiceAddress  ( stream )
    , mThreadName       ( stream )
    , mChannel          ( )
{
    ITEM_ID cookie = NEService::COOKIE_LOCAL;
    stream >> cookie;
    if ( CEServiceAddress::IsValid() )
        mChannel.SetCookie(cookie);
}

CEProxyAddress::~CEProxyAddress( void )
{
    ; // do nothing
}

const CEProxyAddress & CEProxyAddress::operator = ( const CEProxyAddress & source )
{
    static_cast<CEServiceAddress &>(*this) = static_cast<const CEServiceAddress &>(source);
    mThreadName = source.mThreadName;
    mChannel    = source.mChannel;
    return (*this);
}

bool CEProxyAddress::operator == ( const CEProxyAddress & other ) const
{
    return ( static_cast<const CEServiceAddress &>(*this) == static_cast<const CEServiceAddress &>(other) && 
             mThreadName== other.mThreadName &&  mChannel.GetCookie() == other.mChannel.GetCookie() );
}

bool CEProxyAddress::operator != ( const CEProxyAddress & other ) const
{
    return ( (static_cast<const CEServiceAddress &>(*this) != static_cast<const CEServiceAddress &>(other)) || 
             (mThreadName != mThreadName) || 
             (mChannel.GetCookie() != other.mChannel.GetCookie()) );
}


bool CEProxyAddress::IsStubCompatible(const CEStubAddress & addrStub ) const
{
    return addrStub.IsProxyCompatible(*this);
}

void CEProxyAddress::SetThread( const char * threadName )
{
    CEThread * thread = NEString::isEmpty<char>(threadName) ? CEThread::GetCurrentThread() : CEThread::FindThreadByName(threadName);
    CEDispatcherThread * dispatcher = RUNTIME_CAST( thread, CEDispatcherThread);
    if ( dispatcher != NULL )
    {
        mThreadName = dispatcher->GetThreadAddress().GetThreadName();
        mChannel.SetSource( dispatcher->GetThreadId() );
    }
    else
    {
        mThreadName = CEThreadAddress::INVALID_THREAD_ADDRESS.GetThreadName();
    }

    mThreadName.Truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
}

bool CEProxyAddress::DeliverServiceEvent(CEServiceRequestEvent & stubEvent) const
{
    return CEProxyAddress::_deliverEvent( static_cast<CEEvent &>(stubEvent), mChannel.GetTarget());
}

bool CEProxyAddress::DeliverServiceEvent(CEServiceResponseEvent & proxyEvent) const
{
    return CEProxyAddress::_deliverEvent( static_cast<CEEvent &>(proxyEvent), mChannel.GetSource());
}

bool CEProxyAddress::_deliverEvent(CEEvent & serviceEvent, ITEM_ID idTarget)
{
    bool result = false;
    if ( idTarget != NEService::TARGET_UNKNOWN )
    {
        CEThread * thread = CEThread::FindThreadById(idTarget);
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

bool CEProxyAddress::IsValid( void ) const
{
    return mChannel.IsValid();
}

void CEProxyAddress::InvalidateChannel( void )
{
    mChannel.Invalidate();
}

AREG_API const IEInStream & operator >> ( const IEInStream & stream, CEProxyAddress & input )
{
    ITEM_ID cookie = NEService::COOKIE_LOCAL;
    stream >> static_cast<CEServiceAddress &>(input);
    stream >> input.mThreadName;
    stream >> cookie;

    input.SetCookie(cookie);
    return stream;
}

AREG_API IEOutStream & operator << ( IEOutStream & stream, const CEProxyAddress & output)
{
    stream << static_cast<const CEServiceAddress &>(output);
    stream << output.mThreadName;
    stream << output.mChannel.GetCookie();
    return stream;
}
