/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ProxyAddress.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
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

#include <string_view>
#include <utility>

namespace
{
//////////////////////////////////////////////////////////////////////////
// Predefined constants
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The name of invalid proxy.
     *          None of service interfaces should contain this name.
     **/
    constexpr std::string_view  INVALID_PROXY_NAME      { "INVALID_PROXY" };

    /**
     * \brief   ProxyAddress::EXTENTION_PROXY
     *          Extension, used in the path to convert Proxy Address to string object.
     **/
    constexpr std::string_view  EXTENTION_PROXY         { "proxy" };

}

//////////////////////////////////////////////////////////////////////////
// ProxyAddress class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Static variables
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Invalid proxy address.
 **/
const ProxyAddress & ProxyAddress::getInvalidProxyAddress()
{
    static const ProxyAddress _invalidProxyAddress;
    return _invalidProxyAddress;
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
String ProxyAddress::convAddressToPath( const ProxyAddress & proxyAddress )
{
    return proxyAddress.convToString();
}

ProxyAddress ProxyAddress::convPathToAddress( const char* pathProxy, const char** out_nextPart /*= nullptr*/ )
{
    ProxyAddress result;
    result.convFromString(pathProxy, out_nextPart);
    return result;
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

ProxyAddress::ProxyAddress( void )
    : ServiceAddress( ServiceItem(), INVALID_PROXY_NAME )
    , mThreadName   ( ThreadAddress::getInvalidThreadAddress().getThreadName() )
    , mChannel      ( )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
}

ProxyAddress::ProxyAddress( const String & serviceName
                          , const Version & serviceVersion
                          , NEService::eServiceType serviceType
                          , const String & roleName
                          , const String & threadName /*= String::getEmptyString()*/ )
    : ServiceAddress( serviceName, serviceVersion, serviceType, roleName )
    , mThreadName   ( threadName )
    , mChannel      ( )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    setThread( threadName );
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(NEService::COOKIE_LOCAL);
}

ProxyAddress::ProxyAddress( const ServiceItem & service, const String & roleName, const String & threadName /*= String::getEmptyString()*/ )
    : ServiceAddress( service, roleName )
    , mThreadName   ( "" )
    , mChannel      ( )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    setThread( threadName );
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(NEService::COOKIE_LOCAL);
}

ProxyAddress::ProxyAddress(const NEService::SInterfaceData & siData, const String & roleName, const String & threadName /*= String::getEmptyString()*/)
    : ServiceAddress( siData.idServiceName, siData.idVersion, siData.idServiceType, roleName )
    , mThreadName   ( "" )
    , mChannel      ( )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    setThread(threadName);
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(NEService::COOKIE_LOCAL);
}

ProxyAddress::ProxyAddress( const ProxyAddress & source )
    : ServiceAddress( static_cast<const ServiceAddress &>(source) )
    , mThreadName   ( source.mThreadName )
    , mChannel      ( source.mChannel )
    , mMagicNum     ( source.mMagicNum )
{
}

ProxyAddress::ProxyAddress( ProxyAddress && source ) noexcept
    : ServiceAddress( static_cast<ServiceAddress &&>(source) )
    , mThreadName   ( std::move(source.mThreadName) )
    , mChannel      ( std::move(source.mChannel) )
    , mMagicNum     ( source.mMagicNum )
{
}

ProxyAddress::ProxyAddress(const ServiceAddress & source)
    : ServiceAddress(static_cast<const ServiceAddress&>(source))
    , mThreadName   ("")
    , mChannel      ( )
    , mMagicNum     (static_cast<unsigned int>(source))
{
}

ProxyAddress::ProxyAddress( ServiceAddress && source)
    : ServiceAddress(std::move(source))
    , mThreadName   ("")
    , mChannel      ( )
    , mMagicNum     (static_cast<unsigned int>(static_cast<const ServiceAddress &>(self())))
{
}

ProxyAddress::ProxyAddress( const IEInStream & stream )
    : ServiceAddress( stream )
    , mThreadName   ( stream )
    , mChannel      ( )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    ITEM_ID cookie = NEService::COOKIE_LOCAL;
    stream >> cookie;
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(cookie);

    mMagicNum = ProxyAddress::_magicNumber(*this);
}

bool ProxyAddress::isStubCompatible(const StubAddress & addrStub ) const
{
    return addrStub.isProxyCompatible(*this);
}

void ProxyAddress::setThread( const String & threadName )
{
    Thread * thread = threadName.isEmpty() ? Thread::getCurrentThread() : Thread::findThreadByName(threadName);
    DispatcherThread * dispatcher = RUNTIME_CAST( thread, DispatcherThread);
    if ( (dispatcher != nullptr) && dispatcher->isValid() )
    {
        mThreadName = dispatcher->getAddress().getThreadName();
        mMagicNum   = ProxyAddress::_magicNumber(*this);
        mChannel.setSource( dispatcher->getId() );
    }
    else
    {
        mMagicNum   = NEMath::CHECKSUM_IGNORE;
        mThreadName = ThreadAddress::getInvalidThreadAddress().getThreadName();
    }
}

bool ProxyAddress::deliverServiceEvent(ServiceRequestEvent & stubEvent) const
{
    return ProxyAddress::_deliverEvent( static_cast<Event &>(stubEvent), mChannel.getTarget());
}

bool ProxyAddress::deliverServiceEvent(ServiceResponseEvent & proxyEvent) const
{
    return ProxyAddress::_deliverEvent( static_cast<Event &>(proxyEvent), mChannel.getSource());
}

bool ProxyAddress::_deliverEvent(Event & serviceEvent, const ITEM_ID & idTarget)
{
    bool result{ false };
    Thread* thread = idTarget != NEService::TARGET_UNKNOWN ? Thread::findThreadById(static_cast<id_type>(idTarget)) : nullptr;
    DispatcherThread* dispatcher = thread != nullptr ? RUNTIME_CAST(thread, DispatcherThread) : nullptr;
    if (dispatcher != nullptr)
    {
        result = serviceEvent.registerForThread(dispatcher);
        serviceEvent.deliverEvent();
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

unsigned int ProxyAddress::_magicNumber(const ProxyAddress & proxy)
{
    unsigned int result     = NEMath::CHECKSUM_IGNORE;

    if ( proxy.isValidated() )
    {
        result = NEMath::crc32Init();
        result = NEMath::crc32Start( result, proxy.mServiceName.getString() );
        result = NEMath::crc32Start( result, static_cast<unsigned char>(proxy.mServiceType) );
        result = NEMath::crc32Start( result, proxy.mRoleName.getString() );
        result = NEMath::crc32Start( result, proxy.mThreadName.getString() );
        result = NEMath::crc32Finish(result);
    }

    return result;
}

String ProxyAddress::convToString(void) const
{
    String result(static_cast<uint32_t>(0xFF));

    result.append(EXTENTION_PROXY)
          .append(NECommon::COMPONENT_PATH_SEPARATOR)
          .append(ServiceAddress::convToString( ))
          .append(NECommon::COMPONENT_PATH_SEPARATOR)
          .append(mThreadName)
          .append(NECommon::COMPONENT_PATH_SEPARATOR)
          .append(mChannel.convToString());

    return result;
}

void ProxyAddress::convFromString(const char * pathProxy, const char** out_nextPart /*= nullptr*/)
{
    const char* strSource = pathProxy;
    if ( String::getSubstring(strSource, NECommon::COMPONENT_PATH_SEPARATOR.data(), &strSource) == EXTENTION_PROXY )
    {
        ServiceAddress::convFromString(strSource, &strSource);
        mThreadName  = String::getSubstring(strSource, NECommon::COMPONENT_PATH_SEPARATOR.data( ), &strSource);
        mChannel.convFromString( String::getSubstring(strSource, NECommon::COMPONENT_PATH_SEPARATOR.data( ), &strSource) );

        mMagicNum = ProxyAddress::_magicNumber(*this);
    }
    else
    {
        *this = ProxyAddress::getInvalidProxyAddress();
    }

    if (out_nextPart != nullptr)
        *out_nextPart = strSource;
}

bool ProxyAddress::isValidated(void) const
{
    return ServiceAddress::isValidated() && (mThreadName.isEmpty() == false) && (mThreadName != ThreadAddress::getInvalidThreadAddress().getThreadName());
}

AREG_API_IMPL const IEInStream & operator >> ( const IEInStream & stream, ProxyAddress & input )
{
    ITEM_ID cookie = NEService::COOKIE_LOCAL;
    stream >> static_cast<ServiceAddress &>(input);
    stream >> input.mThreadName;
    stream >> cookie;

    input.setCookie(cookie);
    input.mMagicNum = ProxyAddress::_magicNumber(input);

    return stream;
}

AREG_API_IMPL IEOutStream & operator << ( IEOutStream & stream, const ProxyAddress & output)
{
    stream << static_cast<const ServiceAddress &>(output);
    stream << output.mThreadName;
    stream << output.mChannel.getCookie();
    
    return stream;
}
