/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ProxyAddress.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Proxy Address class implementation.
 *
 ************************************************************************/
#include "areg/component/ProxyAddress.hpp"

#include "areg/base/UtilityDefs.hpp"
#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/ServiceRequestEvent.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/logging/areg_log.h"

#include <string_view>
#include <utility>
namespace {
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
} // namespace

namespace areg {

//////////////////////////////////////////////////////////////////////////
// ProxyAddress class implementation
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(areg_component_private_ProxyAddress, _deliver_event);

//////////////////////////////////////////////////////////////////////////
// Static variables
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Invalid proxy address.
 **/
const ProxyAddress & ProxyAddress::invalid_proxy_address() noexcept
{
    static const ProxyAddress _invalidProxyAddress;
    return _invalidProxyAddress;
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
String ProxyAddress::to_path( const ProxyAddress & proxyAddress )
{
    return proxyAddress.to_string();
}

ProxyAddress ProxyAddress::from_path( const char* pathProxy, const char** nextPart /*= nullptr*/ )
{
    ProxyAddress result;
    result.from_string(pathProxy, nextPart);
    return result;
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

ProxyAddress::ProxyAddress()
    : ServiceAddress( )
    , mThread       ( ThreadAddress::invalid_thread_address() )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_INVALID )
{
}

ProxyAddress::ProxyAddress( const String & serviceName
                          , const Version & serviceVersion
                          , areg::ServiceType serviceType
                          , const String & roleName
                          , const String & threadName /*= String::empty_string()*/ )
    : ServiceAddress( serviceName, serviceVersion, serviceType, roleName )
    , mThread       ( )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_INVALID )
{
    set_thread( threadName );  // don't change this to fix channel source.
    mChannel.set_cookie(ServiceAddress::is_valid() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN);
}

ProxyAddress::ProxyAddress(const UniqueNumber serviceNum, const Version& serviceVersion, areg::ServiceType serviceType, const UniqueNumber roleNum, const UniqueNumber threadNum, const UniqueNumber proxyNum)
    : ServiceAddress( serviceNum, serviceVersion, serviceType, roleNum )
    , mThread       ( threadNum )
    , mChannel      ( )
    , mMagicNum     ( proxyNum )
{
    mChannel.set_cookie(ServiceAddress::is_valid() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN);
}

ProxyAddress::ProxyAddress( const ServiceItem & service, const String & roleName, const String & threadName /*= String::empty_string()*/ )
    : ServiceAddress( service, roleName )
    , mThread       ( )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_INVALID )
{
    set_thread( threadName );  // don't change this to fix channel source.
    mChannel.set_cookie(ServiceAddress::is_valid() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN);
}

ProxyAddress::ProxyAddress(const ServiceItem& service, const UniqueNumber roleNum, const UniqueNumber threadNum, const UniqueNumber proxyNum)
    : ServiceAddress( service, roleNum )
    , mThread       ( threadNum )
    , mChannel      ( )
    , mMagicNum     ( proxyNum )
{
    mChannel.set_cookie(ServiceAddress::is_valid() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN);
}

ProxyAddress::ProxyAddress(const areg::InterfaceData & siData, const String & roleName, const String & threadName /*= String::empty_string()*/)
    : ServiceAddress( siData.idServiceName, siData.idVersion, siData.idServiceType, roleName )
    , mThread       ( )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_INVALID )
{
    set_thread(threadName);  // don't change this to fix channel source.
    mChannel.set_cookie(ServiceAddress::is_valid() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN);
}

ProxyAddress::ProxyAddress( ProxyAddress && source ) noexcept
    : ServiceAddress( static_cast<ServiceAddress &&>(source) )
    , mThread       ( std::move(source.mThread) )
    , mChannel      ( std::move(source.mChannel) )
    , mMagicNum     ( source.mMagicNum )
{
    source.mMagicNum = areg::CHECKSUM_IGNORE;
}

ProxyAddress::ProxyAddress(const ServiceAddress & source)
    : ServiceAddress(static_cast<const ServiceAddress&>(source))
    , mThread       (ThreadAddress::invalid_thread_address())
    , mChannel      ( )
    , mMagicNum     (areg::CHECKSUM_IGNORE)
{
    mChannel.set_cookie(ServiceAddress::is_valid() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN);
}

ProxyAddress::ProxyAddress(const StubAddress& source)
    : ServiceAddress(static_cast<const ServiceAddress&>(source))
    , mThread       (ThreadAddress::invalid_thread_address())
    , mChannel      ( )
    , mMagicNum     (areg::CHECKSUM_IGNORE)
{
    mChannel.set_cookie(ServiceAddress::is_valid() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN);
}


ProxyAddress::ProxyAddress( ServiceAddress && source)
    : ServiceAddress(std::move(source))
    , mThread       (ThreadAddress::invalid_thread_address())
    , mChannel      ( )
    , mMagicNum     (areg::CHECKSUM_IGNORE)
{
    mChannel.set_cookie(ServiceAddress::is_valid() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN);
}

ProxyAddress::ProxyAddress(const areg::RawService& rawService, const areg::Endpoint& endPoint)
    : ServiceAddress( rawService, endPoint )
    , mThread       ( endPoint )
    , mChannel      ( )
    , mMagicNum     ( endPoint.number )
{
    mChannel.set_cookie(endPoint.id);
    mChannel.set_source(endPoint.thread);
}

ProxyAddress::ProxyAddress(const areg::EventHeader& header)
    : ServiceAddress( header.rawService, header.consumer )
    , mThread       ( header.consumer )
    , mChannel      ( )
    , mMagicNum     ( header.consumer.number )
{
    mChannel.set_cookie(header.consumer.id);
    mChannel.set_source(header.consumer.thread);
}

bool ProxyAddress::is_provider_compatible(const StubAddress & addrStub ) const noexcept
{
    return addrStub.is_proxy_compatible(*this);
}

void ProxyAddress::set_thread( const ThreadAddress & thread )
{
    DispatcherThread& dispatcher = DispatcherThread::dispatcher_thread(thread);
    if (dispatcher.is_valid())
    {
        mChannel.set_source(static_cast<uint32_t>(dispatcher.address()));
        mThread   = dispatcher.address();
        mMagicNum = ProxyAddress::_magic_number(*this);
    }
    else
    {
        mMagicNum = areg::CHECKSUM_IGNORE;
        mThread   = ThreadAddress::invalid_thread_address();
    }
}

void ProxyAddress::set_thread( const String & threadName )
{
    DispatcherThread& dispatcher = DispatcherThread::dispatcher_thread(
        static_cast<UniqueNumber>(areg::crc32_calculate(threadName.as_string())));
    if (dispatcher.is_valid())
    {
        mChannel.set_source(static_cast<uint32_t>(dispatcher.address()));
        mThread   = dispatcher.address();
        mMagicNum = ProxyAddress::_magic_number(*this);
    }
    else
    {
        mMagicNum = areg::CHECKSUM_IGNORE;
        mThread   = ThreadAddress::invalid_thread_address();
    }
}

bool ProxyAddress::deliver_service_event(ServiceRequestEvent & stubEvent) const
{
    return ProxyAddress::_deliver_event( static_cast<Event &>(stubEvent), mChannel.target());
}

bool ProxyAddress::deliver_service_event(ServiceResponseEvent & proxyEvent) const
{
    return ProxyAddress::_deliver_event( static_cast<Event &>(proxyEvent), mChannel.source());
}

bool ProxyAddress::_deliver_event(Event & serviceEvent, const ITEM_ID & idTarget)
{
    LOG_SCOPE(areg_component_private_ProxyAddress, _deliver_event);

    DispatcherThread& dispatcher = DispatcherThread::dispatcher_thread(static_cast<UniqueNumber>(idTarget));
    if (dispatcher.is_running() && serviceEvent.register_for_thread(&dispatcher))
    {
        LOG_DBG("Delivering event [ %u ] to dispatcher [ %s ]", serviceEvent.event_id(), dispatcher.address().to_string().as_string());
        return dispatcher.event_dispatcher().post_event(serviceEvent);
    }

    LOG_WARN("Failed to deliver event [ %u ], no running dispatcher for ID [ %u ]", serviceEvent.event_id(), static_cast<uint32_t>(idTarget));
    return false;
}

uint32_t ProxyAddress::_magic_number(const ProxyAddress & proxy) noexcept
{
    uint32_t result{ areg::CHECKSUM_IGNORE };

    if (proxy.mThread.is_valid() && proxy.ServiceAddress::is_valid())
    {
        result = areg::crc32_init();
        result = areg::crc32_start( result, EXTENTION_PROXY );
        result = areg::crc32_start( result, proxy.mServiceName.as_string() );
        result = areg::crc32_start( result, static_cast<uint8_t>(proxy.mServiceType) );
        result = areg::crc32_start( result, proxy.mRoleName.as_string() );
        result = areg::crc32_start( result, proxy.mThread.name().as_string() );
        result = areg::crc32_finish(result);
    }

    return result;
}

String ProxyAddress::to_string() const
{
    String result(static_cast<uint32_t>(0xFF));

    result.append(EXTENTION_PROXY)
          .append(areg::COMPONENT_PATH_SEPARATOR)
          .append(String::make_string(mMagicNum))
          .append(areg::COMPONENT_PATH_SEPARATOR)
          .append(ServiceAddress::to_string())
          .append(areg::COMPONENT_PATH_SEPARATOR)
          .append(mThread.name())
          .append(areg::COMPONENT_PATH_SEPARATOR)
          .append(mChannel.to_string());

    return result;
}

void ProxyAddress::from_string(const char * pathProxy, const char** nextPart /*= nullptr*/)
{
    const char* strSource = pathProxy;
    if ( String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource) == EXTENTION_PROXY )
    {
        String num = String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource);
        mMagicNum  = num.to_uint32();
        ServiceAddress::from_string(strSource, &strSource);
        mThread.from_string(strSource, &strSource);
        mChannel.from_string( String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource).as_string() );
    }

    if (nextPart != nullptr)
        *nextPart = strSource;
}

} // namespace areg
