/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/StubAddress.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Stub Address implementation.
 *
 ************************************************************************/
#include "areg/component/StubAddress.hpp"

#include "areg/component/ServiceRequestEvent.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/base/ThreadAddress.hpp"
#include "areg/base/IOStream.hpp"
#include "areg/base/UtilityDefs.hpp"

#include <string_view>

namespace
{
    //!< Extension to add to Stub path.
    constexpr std::string_view  EXTENTION_STUB{ "stub" };
}

namespace areg {

//////////////////////////////////////////////////////////////////////////
// ProxyAddress class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Static functions
//////////////////////////////////////////////////////////////////////////
String StubAddress::to_path( const StubAddress & stubAddress )
{
    return stubAddress.to_string();
}

StubAddress StubAddress::from_path( const char* pathStub, const char** out_nextPart /*= nullptr*/ )
{
    StubAddress result(StubAddress::invalid_stub_address());
    result.from_string(pathStub, out_nextPart);
    return result;
}

const StubAddress & StubAddress::invalid_stub_address()
{
    static const StubAddress _invalidStubAddress;
    return _invalidStubAddress;
}

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
StubAddress::StubAddress()
    : ServiceAddress( )
    , mThread       ( ThreadAddress::invalid_thread_address() )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_INVALID )
{
}

StubAddress::StubAddress( const String & serviceName, const Version & serviceVersion, areg::ServiceType serviceType, const String & roleName, const String & threadName   /*= String::empty_string()*/ )
    : ServiceAddress( serviceName, serviceVersion, serviceType, roleName )
    , mThread       ( )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_INVALID )
{
    set_thread(threadName); // don't change this to fix channel source.
    mChannel.set_cookie(ServiceAddress::is_valid() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN);
}

StubAddress::StubAddress(const UniqueNumber serviceNum, const Version& serviceVersion, areg::ServiceType serviceType, const UniqueNumber roleNum, const UniqueNumber threadNum, const UniqueNumber stubNum)
    : ServiceAddress( serviceNum, serviceVersion, serviceType, roleNum )
    , mThread       ( threadNum )
    , mChannel      ( )
    , mMagicNum     (stubNum)
{
    mChannel.set_cookie(ServiceAddress::is_valid() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN);
}

StubAddress::StubAddress(const ServiceItem & service, const String & roleName, const String & threadName /*= String::empty_string() */)
    : ServiceAddress( service, roleName )
    , mThread       ( )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    set_thread(threadName); // don't change this to fix channel source.
    mChannel.set_cookie(ServiceAddress::is_valid() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN);
}

StubAddress::StubAddress(const ServiceAddress& service, const UniqueNumber threadNum, const UniqueNumber stubNum)
    : ServiceAddress( service )
    , mThread       ( threadNum )
    , mChannel      ( )
    , mMagicNum     ( stubNum )
{
    mChannel.set_cookie(ServiceAddress::is_valid() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN);
}


StubAddress::StubAddress(const areg::InterfaceData & siData, const String & roleName, const String & threadName /*= String::empty_string() */)
    : ServiceAddress( siData.idServiceName, siData.idVersion, siData.idServiceType, roleName )
    , mThread       ( )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    set_thread(threadName); // don't change this to fix channel source.
    mChannel.set_cookie(ServiceAddress::is_valid() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN);
}

StubAddress::StubAddress(const areg::RawService& rawService, const areg::Endpoint& endPoint)
    : ServiceAddress( rawService, endPoint )
    , mThread       ( endPoint )
    , mChannel      ( )
    , mMagicNum     ( endPoint.number )
{
    mChannel.set_cookie(endPoint.id);
    mChannel.set_source(endPoint.thread);
}

StubAddress::StubAddress(const areg::EventHeader& header)
    : ServiceAddress( header.rawService, header.provider )
    , mThread       ( header.provider )
    , mChannel      ( )
    , mMagicNum     ( header.provider.number )
{
    mChannel.set_cookie(header.provider.id);
    mChannel.set_source(header.provider.thread);
}


StubAddress::StubAddress( StubAddress && source ) noexcept
    : ServiceAddress( std::move(source) )
    , mThread       ( std::move(source.mThread) )
    , mChannel      ( std::move(source.mChannel) )
    , mMagicNum     ( source.mMagicNum )
{
    source.mMagicNum = areg::CHECKSUM_IGNORE;
}

StubAddress::StubAddress(const ServiceAddress & source)
    : ServiceAddress(static_cast<const ServiceAddress&>(source))
    , mThread       (ThreadAddress::invalid_thread_address())
    , mChannel      ( )
    , mMagicNum     (areg::CHECKSUM_IGNORE)
{
    mChannel.set_cookie(ServiceAddress::is_valid() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN);
}

StubAddress::StubAddress(const ProxyAddress& proxy)
    : ServiceAddress(static_cast<const ServiceAddress&>(proxy))
    , mThread       (ThreadAddress::invalid_thread_address())
    , mChannel      ( )
    , mMagicNum     (areg::CHECKSUM_IGNORE)
{
    mChannel.set_cookie(ServiceAddress::is_valid() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN);
}

StubAddress::StubAddress( ServiceAddress && source)
    : ServiceAddress(std::move(source))
    , mThread       (ThreadAddress::invalid_thread_address())
    , mChannel      ( )
    , mMagicNum     (static_cast<uint32_t>(static_cast<const ServiceAddress &>(self())))
{
    mChannel.set_cookie(ServiceAddress::is_valid() ? areg::COOKIE_LOCAL : areg::COOKIE_UNKNOWN);
}

bool StubAddress::is_proxy_compatible(const ProxyAddress & proxyAddress) const noexcept
{
    return (is_valid() && proxyAddress.is_valid() &&
           (static_cast<const ServiceAddress &>(*this) == static_cast<const ServiceAddress&>(proxyAddress)) &&
           is_service_compatible(static_cast<const ServiceItem&>(proxyAddress))) ;
}

void StubAddress::set_thread(const ThreadAddress& thread)
{
    DispatcherThread& dispatcher = DispatcherThread::dispatcher_thread(thread);
    if ( dispatcher.is_valid())
    {
        mChannel.set_source(static_cast<uint32_t>(dispatcher.address()));
        mThread     = dispatcher.address();
        mMagicNum   = StubAddress::_magic_number(*this);
    }
    else
    {
        mMagicNum   = areg::CHECKSUM_IGNORE;
        mThread     = ThreadAddress::invalid_thread_address();
    }
}

void StubAddress::set_thread(const String& thread)
{
    DispatcherThread& dispatcher = DispatcherThread::dispatcher_thread(static_cast<UniqueNumber>(areg::crc32_calculate(thread.as_string())));
    if (dispatcher.is_valid())
    {
        mChannel.set_source(static_cast<uint32_t>(dispatcher.address()));
        mThread     = dispatcher.address();
        mMagicNum   = StubAddress::_magic_number(*this);
    }
    else
    {
        mMagicNum   = areg::CHECKSUM_IGNORE;
        mThread     = ThreadAddress::invalid_thread_address();
    }
}

bool StubAddress::deliver_service_event( ServiceRequestEvent & serviceEvent ) const
{
    const ITEM_ID & target{ mChannel.source() };
    DispatcherThread& dispatcher = DispatcherThread::dispatcher_thread(static_cast<UniqueNumber>(target));
    if (dispatcher.is_running() && serviceEvent.register_for_thread(&dispatcher))
    {
        return dispatcher.event_dispatcher().post_event(serviceEvent);
    }

    return false;
}

String StubAddress::to_string() const
{
    String result(static_cast<uint32_t>(0xFF));

    result.append(EXTENTION_STUB)
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

void StubAddress::from_string(const char* pathStub, const char** out_nextPart /*= nullptr*/)
{
    const char* strSource = pathStub;
    if ( String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource) == EXTENTION_STUB )
    {
        String num = String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource);
        mMagicNum  = num.to_uint32();
        ServiceAddress::from_string(strSource, &strSource);
        mThread.from_string(strSource, &strSource);
        mChannel.from_string( String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource).as_string() );
    }

    if (out_nextPart != nullptr)
        *out_nextPart = strSource;
}

uint32_t StubAddress::_magic_number(const StubAddress & addrStub) noexcept
{
    uint32_t result{ areg::CHECKSUM_IGNORE };

    if (addrStub.mThread.is_valid() && addrStub.ServiceAddress::is_valid())
    {
        result = areg::crc32_init();
        result = areg::crc32_start( result, EXTENTION_STUB );
        result = areg::crc32_start( result, addrStub.mServiceName.as_string() );
        result = areg::crc32_start( result, static_cast<uint8_t>(addrStub.mServiceType) );
        result = areg::crc32_start( result, addrStub.mRoleName.as_string() );
        result = areg::crc32_start( result, addrStub.mThread.name().as_string());
        result = areg::crc32_finish(result);
    }

    return result;
}

} // namespace areg
