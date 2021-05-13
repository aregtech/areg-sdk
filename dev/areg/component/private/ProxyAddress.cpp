/************************************************************************
 * \file        areg/component/private/ProxyAddress.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Proxy Address class implementation.
 *
 ************************************************************************/
#include "areg/component/ProxyAddress.hpp"

#include "areg/base/NEUtilities.hpp"
#include "areg/base/IEIOStream.hpp"
#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/ServiceRequestEvent.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/StubAddress.hpp"

//////////////////////////////////////////////////////////////////////////
// ProxyAddress class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Static variables
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   The name of invalid proxy.
 **/
const char * const      ProxyAddress::INVALID_PROXY_NAME      = "INVALID_PROXY";
/**
 * \brief   Extension, used in the path to convert Proxy Address to string object.
 **/
const char * const      ProxyAddress::EXTENTION_PROXY         = "proxy";
/**
 * \brief   Invalid proxy address.
 **/
const ProxyAddress    ProxyAddress::INVALID_PROXY_ADDRESS;

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
String ProxyAddress::convAddressToPath( const ProxyAddress & proxyAddress )
{
    String result = "";

    result += ProxyAddress::EXTENTION_PROXY;
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += ServiceAddress::convAddressToPath( static_cast<const ServiceAddress &>(proxyAddress));
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += proxyAddress.mThreadName;
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += proxyAddress.mChannel.convToString();

    return result;
}

ProxyAddress ProxyAddress::convPathToAddress( const char* pathProxy, const char** out_nextPart /*= NULL*/ )
{
    ProxyAddress result(ProxyAddress::INVALID_PROXY_ADDRESS);
    const char* strSource = pathProxy;

    if (out_nextPart != NULL)
        *out_nextPart = pathProxy;

    if ( String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource) == ProxyAddress::EXTENTION_PROXY )
    {
        static_cast<ServiceItem &>(result)= ServiceAddress::convPathToAddress(strSource, &strSource);
        result.mThreadName  = String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);
        result.mChannel.convFromString( String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource).getString() );

        if (out_nextPart != NULL)
            *out_nextPart = strSource;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

ProxyAddress::ProxyAddress( void )
    : ServiceAddress  ( ServiceItem(), ProxyAddress::INVALID_PROXY_NAME )
    , mThreadName       ( ThreadAddress::INVALID_THREAD_ADDRESS.getThreadName() )
    , mChannel          ( )
{
    ; // do nothing
}

ProxyAddress::ProxyAddress( const char * serviceName, const Version & serviceVersion, NEService::eServiceType serviceType, const char * roleName, const char * threadName /*= NULL*/ )
    : ServiceAddress  ( serviceName, serviceVersion, serviceType, roleName )
    , mThreadName       ( threadName )
    , mChannel          ( )
{
    setThread( threadName );
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(NEService::COOKIE_LOCAL);
}

ProxyAddress::ProxyAddress( const ServiceItem & service, const char * roleName, const char * threadName /*= NULL*/ )
    : ServiceAddress  ( service, roleName )
    , mThreadName       ( "" )
    , mChannel          ( )
{
    setThread( threadName );
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(NEService::COOKIE_LOCAL);
}

ProxyAddress::ProxyAddress(const NEService::SInterfaceData & siData, const char * roleName, const char * threadName /*= NULL */)
    : ServiceAddress  ( siData.idServiceName, siData.idVersion, siData.idServiceType, roleName )
    , mThreadName       ( "" )
    , mChannel          ( )
{
    setThread(threadName);
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(NEService::COOKIE_LOCAL);
}

ProxyAddress::ProxyAddress( const ProxyAddress & source )
    : ServiceAddress  ( static_cast<const ServiceAddress &>(source) )
    , mThreadName       ( source.mThreadName )
    , mChannel          ( source.mChannel )
{
    ; // do nothing
}

ProxyAddress::ProxyAddress( const IEInStream & stream )
    : ServiceAddress  ( stream )
    , mThreadName       ( stream )
    , mChannel          ( )
{
    ITEM_ID cookie = NEService::COOKIE_LOCAL;
    stream >> cookie;
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(cookie);
}

ProxyAddress::~ProxyAddress( void )
{
    ; // do nothing
}

const ProxyAddress & ProxyAddress::operator = ( const ProxyAddress & source )
{
    static_cast<ServiceAddress &>(*this) = static_cast<const ServiceAddress &>(source);
    mThreadName = source.mThreadName;
    mChannel    = source.mChannel;
    return (*this);
}

bool ProxyAddress::operator == ( const ProxyAddress & other ) const
{
    return ( static_cast<const ServiceAddress &>(*this) == static_cast<const ServiceAddress &>(other) && 
             mThreadName== other.mThreadName &&  mChannel.getCookie() == other.mChannel.getCookie() );
}

bool ProxyAddress::operator != ( const ProxyAddress & other ) const
{
    return ( (static_cast<const ServiceAddress &>(*this) != static_cast<const ServiceAddress &>(other)) || 
             (mThreadName != mThreadName) || 
             (mChannel.getCookie() != other.mChannel.getCookie()) );
}


bool ProxyAddress::isStubCompatible(const StubAddress & addrStub ) const
{
    return addrStub.isProxyCompatible(*this);
}

void ProxyAddress::setThread( const char * threadName )
{
    Thread * thread = NEString::isEmpty<char>(threadName) ? Thread::getCurrentThread() : Thread::findThreadByName(threadName);
    DispatcherThread * dispatcher = RUNTIME_CAST( thread, DispatcherThread);
    if ( dispatcher != NULL )
    {
        mThreadName = dispatcher->getAddress().getThreadName();
        mChannel.setSource( dispatcher->getId() );
    }
    else
    {
        mThreadName = ThreadAddress::INVALID_THREAD_ADDRESS.getThreadName();
    }

    mThreadName.truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
}

bool ProxyAddress::deliverServiceEvent(ServiceRequestEvent & stubEvent) const
{
    return ProxyAddress::_deliverEvent( static_cast<Event &>(stubEvent), mChannel.getTarget());
}

bool ProxyAddress::deliverServiceEvent(ServiceResponseEvent & proxyEvent) const
{
    return ProxyAddress::_deliverEvent( static_cast<Event &>(proxyEvent), mChannel.getSource());
}

bool ProxyAddress::_deliverEvent(Event & serviceEvent, ITEM_ID idTarget)
{
    bool result = false;
    if ( idTarget != NEService::TARGET_UNKNOWN )
    {
        Thread * thread = Thread::findThreadById(idTarget);
        DispatcherThread * dispatcher = thread != NULL ? RUNTIME_CAST(thread, DispatcherThread) : NULL;
        if ( dispatcher != NULL )
        {
            result = serviceEvent.registerForThread(dispatcher);
            serviceEvent.sendEvent();
        }
        else
        {
            serviceEvent.destroy();
        }
    }
    else
    {
        serviceEvent.destroy();
    }
    return result;
}

bool ProxyAddress::isValid( void ) const
{
    return mChannel.isValid();
}

void ProxyAddress::invalidateChannel( void )
{
    mChannel.invalidate();
}

AREG_API const IEInStream & operator >> ( const IEInStream & stream, ProxyAddress & input )
{
    ITEM_ID cookie = NEService::COOKIE_LOCAL;
    stream >> static_cast<ServiceAddress &>(input);
    stream >> input.mThreadName;
    stream >> cookie;

    input.setCookie(cookie);
    return stream;
}

AREG_API IEOutStream & operator << ( IEOutStream & stream, const ProxyAddress & output)
{
    stream << static_cast<const ServiceAddress &>(output);
    stream << output.mThreadName;
    stream << output.mChannel.getCookie();
    return stream;
}
