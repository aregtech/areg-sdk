/************************************************************************
 * \file        areg/component/private/StubAddress.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Stub Address implementation.
 *
 ************************************************************************/
#include "areg/component/StubAddress.hpp"

#include "areg/component/ServiceRequestEvent.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/base/ThreadAddress.hpp"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/NEUtilities.hpp"

//////////////////////////////////////////////////////////////////////////
// ProxyAddress class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Predefined constants and statics
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The invalid service interface name (invalid stub name). None of service interfaces should contain this name.
 **/
const char * const  StubAddress::INVALID_STUB_NAME    = "INVALID_STUB";
/**
 * \brief   Extension to add to Stub path.
 **/
const char * const  StubAddress::EXTENTION_STUB       = "stub";
/**
 * \brief   Predefined Invalid Stub address.
 **/
const StubAddress StubAddress::INVALID_STUB_ADDRESS;

//////////////////////////////////////////////////////////////////////////
// Static functions
//////////////////////////////////////////////////////////////////////////
String StubAddress::convAddressToPath( const StubAddress & stubAddress )
{
    String result = "";
    
    result += EXTENTION_STUB;
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += ServiceAddress::convAddressToPath( static_cast<const ServiceAddress &>(stubAddress));
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += stubAddress.mThreadName;
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += stubAddress.mChannel.convToString();

    return result;
}

StubAddress StubAddress::convPathToAddress( const char* pathStub, const char** out_nextPart /*= NULL*/ )
{
    String stubKey;
    StubAddress result(StubAddress::INVALID_STUB_ADDRESS);
    const char* strSource = pathStub;

    if (out_nextPart != NULL)
        *out_nextPart = pathStub;

    if ( String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource) == StubAddress::EXTENTION_STUB )
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
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
StubAddress::StubAddress( void )
    : ServiceAddress  ( ServiceItem(), StubAddress::INVALID_STUB_NAME )
    , mThreadName       ( ThreadAddress::INVALID_THREAD_ADDRESS.getThreadName() )
    , mChannel          ( )
{
    ; // do nothing
}

StubAddress::StubAddress( const char * serviceName
                            , const Version & serviceVersion
                            , NEService::eServiceType serviceType
                            , const char * roleName
                            , const char * threadName   /*= NULL*/ )
    : ServiceAddress  ( serviceName, serviceVersion, serviceType, roleName )
    , mThreadName       ( threadName )
    , mChannel          ( )
{
    mThreadName.truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(NEService::COOKIE_LOCAL);
}

StubAddress::StubAddress(const ServiceItem & service, const char * roleName, const char * threadName /*= NULL */)
    : ServiceAddress  ( service, roleName )
    , mThreadName       ( threadName )
    , mChannel          ( )
{
    mThreadName.truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(NEService::COOKIE_LOCAL);
}

StubAddress::StubAddress(const NEService::SInterfaceData & siData, const char * roleName, const char * threadName /*= NULL */)
    : ServiceAddress  ( siData.idServiceName, siData.idVersion, siData.idServiceType, roleName )
    , mThreadName       ( "" )
    , mChannel          ( )
{
    setThread(threadName);
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(NEService::COOKIE_LOCAL);
}

StubAddress::StubAddress( const StubAddress & source )
    : ServiceAddress  ( static_cast<const ServiceAddress &>(source) )
    , mThreadName       ( source.mThreadName )
    , mChannel          ( source.mChannel )
{
    ; // do nothing
}

StubAddress::StubAddress( const IEInStream & stream )
    : ServiceAddress  ( stream )
    , mThreadName       ( stream )
    , mChannel          ( )
{
    ITEM_ID cookie = NEService::COOKIE_LOCAL;
    stream >> cookie;
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(cookie);
}

StubAddress::~StubAddress( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

const StubAddress & StubAddress::operator = ( const StubAddress & source )
{
    static_cast<ServiceAddress &>(*this) = static_cast<const ServiceAddress &>(source);
    mThreadName = source.mThreadName;
    mChannel    = source.mChannel;
    return (*this);
}

const StubAddress & StubAddress::operator = (const ServiceAddress & addrService)
{
    static_cast<ServiceAddress &>(*this) = static_cast<const ServiceAddress &>(addrService);
    mThreadName = "";
    mChannel    = Channel();
    return (*this);
}

bool StubAddress::operator == ( const StubAddress & other ) const
{
    return ( (static_cast<const ServiceAddress &>(*this) == static_cast<const ServiceAddress &>(other)) && 
             (mThreadName == other.mThreadName) &&  
             (mChannel.getCookie() == other.mChannel.getCookie()) );
}

inline bool StubAddress::operator != ( const StubAddress& other ) const
{
    return ( (static_cast<const ServiceAddress &>(*this) != static_cast<const ServiceAddress &>(other)) || 
             (mThreadName != other.mThreadName) || 
             (mChannel.getCookie() != other.mChannel.getCookie()) );
}

bool StubAddress::isProxyCompatible(const ProxyAddress & proxyAddress) const
{
    if ( isValid() && proxyAddress.isValid() )
        return (mRoleName == proxyAddress.getRoleName() && isServiceCompatible( static_cast<const ServiceItem &>(proxyAddress) ) );
    else
        return false;
}

void StubAddress::setThread(const char * threadName)
{
    Thread * thread = threadName == NULL ? Thread::getCurrentThread() : Thread::findThreadByName(threadName);
    DispatcherThread * dispatcher = RUNTIME_CAST( thread, DispatcherThread);
    if ( dispatcher != NULL )
    {
        mThreadName = dispatcher->getAddress().getThreadName();
        mChannel.setSource( dispatcher->getId() );
    }

    mThreadName.truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
}

bool StubAddress::deliverServiceEvent( ServiceRequestEvent & serviceEvent ) const
{
    bool result = false;
    ITEM_ID target = mChannel.getSource();
    if ( target != NEService::TARGET_UNKNOWN )
    {
        Thread * thread = Thread::findThreadById(target);
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

void StubAddress::invalidateChannel( void )
{
    mChannel.invalidate();
}

bool StubAddress::isValid( void ) const
{
    return mChannel.isValid();
}

AREG_API const IEInStream & operator >> ( const IEInStream & stream, StubAddress & input )
{
    ITEM_ID cookie = NEService::COOKIE_LOCAL;
    stream >> static_cast<ServiceAddress &>(input);
    stream >> input.mThreadName;
    stream >> cookie;
    input.SetCookie(cookie);
    return stream;
}

AREG_API IEOutStream & operator << ( IEOutStream & stream, const StubAddress & output)
{
    stream << static_cast<const ServiceAddress &>(output);
    stream << output.mThreadName;
    stream << output.mChannel.getCookie();
    return stream;
}
