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
#include "areg/base/IOStream.hpp"
#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/ServiceRequestEvent.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/StubAddress.hpp"

#include <string_view>
#include <utility>
namespace areg {

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
const ProxyAddress & ProxyAddress::invalid_proxy_address()
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

ProxyAddress ProxyAddress::from_path( const char* pathProxy, const char** out_nextPart /*= nullptr*/ )
{
    ProxyAddress result;
    result.from_string(pathProxy, out_nextPart);
    return result;
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

ProxyAddress::ProxyAddress()
    : ServiceAddress( ServiceItem(), INVALID_PROXY_NAME )
    , mThreadName   ( ThreadAddress::invalid_thread_address().name() )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
}

ProxyAddress::ProxyAddress( const String & serviceName
                          , const Version & serviceVersion
                          , areg::ServiceType serviceType
                          , const String & roleName
                          , const String & threadName /*= String::empty_string()*/ )
    : ServiceAddress( serviceName, serviceVersion, serviceType, roleName )
    , mThreadName   ( threadName )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    set_thread( threadName );
    if ( ServiceAddress::is_valid() )
        mChannel.set_cookie(areg::COOKIE_LOCAL);
}

ProxyAddress::ProxyAddress( const ServiceItem & service, const String & roleName, const String & threadName /*= String::empty_string()*/ )
    : ServiceAddress( service, roleName )
    , mThreadName   ( "" )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    set_thread( threadName );
    if ( ServiceAddress::is_valid() )
        mChannel.set_cookie(areg::COOKIE_LOCAL);
}

ProxyAddress::ProxyAddress(const areg::InterfaceData & siData, const String & roleName, const String & threadName /*= String::empty_string()*/)
    : ServiceAddress( siData.idServiceName, siData.idVersion, siData.idServiceType, roleName )
    , mThreadName   ( "" )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    set_thread(threadName);
    if ( ServiceAddress::is_valid() )
        mChannel.set_cookie(areg::COOKIE_LOCAL);
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
    , mMagicNum     (static_cast<uint32_t>(source))
{
}

ProxyAddress::ProxyAddress( ServiceAddress && source)
    : ServiceAddress(std::move(source))
    , mThreadName   ("")
    , mChannel      ( )
    , mMagicNum     (static_cast<uint32_t>(static_cast<const ServiceAddress &>(self())))
{
}

ProxyAddress::ProxyAddress( const InStream & stream )
    : ServiceAddress( stream )
    , mThreadName   ( stream )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    ITEM_ID cookie = areg::COOKIE_LOCAL;
    stream >> cookie;
    if ( ServiceAddress::is_valid() )
        mChannel.set_cookie(cookie);

    mMagicNum = ProxyAddress::_magic_number(*this);
}

bool ProxyAddress::is_stub_compatible(const StubAddress & addrStub ) const
{
    return addrStub.is_proxy_compatible(*this);
}

void ProxyAddress::set_thread( const String & threadName )
{
    Thread * thread = threadName.is_empty() ? Thread::current_thread() : Thread::find_by_name(threadName);
    DispatcherThread * dispatcher = AREG_RUNTIME_CAST( thread, DispatcherThread);
    if ( (dispatcher != nullptr) && dispatcher->is_valid() )
    {
        mThreadName = dispatcher->address().name();
        mMagicNum   = ProxyAddress::_magic_number(*this);
        mChannel.set_source( dispatcher->id() );
    }
    else
    {
        mMagicNum   = areg::CHECKSUM_IGNORE;
        mThreadName = ThreadAddress::invalid_thread_address().name();
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
    bool result{ false };
    Thread* thread = idTarget != areg::TARGET_UNKNOWN ? Thread::find_by_id(static_cast<id_type>(idTarget)) : nullptr;
    DispatcherThread* dispatcher = thread != nullptr ? AREG_RUNTIME_CAST(thread, DispatcherThread) : nullptr;
    if (dispatcher != nullptr)
    {
        result = serviceEvent.register_for_thread(dispatcher);
        serviceEvent.deliver_event();
    }
    else
    {
        serviceEvent.destroy();
    }

    return result;
}

bool ProxyAddress::is_valid() const noexcept
{
    return mChannel.is_valid();
}

void ProxyAddress::invalidate_channel() noexcept
{
    mChannel.invalidate();
}

uint32_t ProxyAddress::_magic_number(const ProxyAddress & proxy) noexcept
{
    uint32_t result     = areg::CHECKSUM_IGNORE;

    if ( proxy.is_validated() )
    {
        result = areg::crc32_init();
        result = areg::crc32_start( result, proxy.mServiceName.as_string() );
        result = areg::crc32_start( result, static_cast<uint8_t>(proxy.mServiceType) );
        result = areg::crc32_start( result, proxy.mRoleName.as_string() );
        result = areg::crc32_start( result, proxy.mThreadName.as_string() );
        result = areg::crc32_finish(result);
    }

    return result;
}

String ProxyAddress::to_string() const
{
    String result(static_cast<uint32_t>(0xFF));

    result.append(EXTENTION_PROXY)
          .append(areg::COMPONENT_PATH_SEPARATOR)
          .append(ServiceAddress::to_string( ))
          .append(areg::COMPONENT_PATH_SEPARATOR)
          .append(mThreadName)
          .append(areg::COMPONENT_PATH_SEPARATOR)
          .append(mChannel.to_string());

    return result;
}

void ProxyAddress::from_string(const char * pathProxy, const char** out_nextPart /*= nullptr*/)
{
    const char* strSource = pathProxy;
    if ( String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource) == EXTENTION_PROXY )
    {
        ServiceAddress::from_string(strSource, &strSource);
        mThreadName  = String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data( ), &strSource);
        mChannel.from_string( String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data( ), &strSource) );

        mMagicNum = ProxyAddress::_magic_number(*this);
    }
    else
    {
        *this = ProxyAddress::invalid_proxy_address();
    }

    if (out_nextPart != nullptr)
        *out_nextPart = strSource;
}

bool ProxyAddress::is_validated() const noexcept
{
    return ServiceAddress::is_validated() && !mThreadName.is_empty() && (mThreadName != ThreadAddress::invalid_thread_address().name());
}

AREG_API_IMPL const InStream & operator >> ( const InStream & stream, ProxyAddress & input )
{
    ITEM_ID cookie = areg::COOKIE_LOCAL;
    stream >> static_cast<ServiceAddress &>(input);
    stream >> input.mThreadName;
    stream >> cookie;

    input.set_cookie(cookie);
    input.mMagicNum = ProxyAddress::_magic_number(input);

    return stream;
}

AREG_API_IMPL OutStream & operator << ( OutStream & stream, const ProxyAddress & output)
{
    stream << static_cast<const ServiceAddress &>(output);
    stream << output.mThreadName;
    stream << output.mChannel.cookie();
    
    return stream;
}

} // namespace areg
