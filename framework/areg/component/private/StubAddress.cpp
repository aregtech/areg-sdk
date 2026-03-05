/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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
namespace areg {

namespace
{
/************************************************************************/
// Hidden constants
/************************************************************************/

    /**
     * \brief   Extension to add to Stub path.
     **/
    constexpr std::string_view  EXTENTION_STUB      { "stub" };
}

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
    , mThreadName   ( ThreadAddress::invalid_thread_address().name() )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
}

StubAddress::StubAddress( const String & serviceName
                        , const Version & serviceVersion
                        , areg::ServiceType serviceType
                        , const String & roleName
                        , const String & threadName   /*= String::empty_string()*/ )
    : ServiceAddress( serviceName, serviceVersion, serviceType, roleName )
    , mThreadName   ( )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    set_thread(threadName); // don't change this to fix channel source.
    if ( ServiceAddress::is_valid() )
    {
        mChannel.set_cookie(areg::COOKIE_LOCAL);
    }

    mMagicNum = StubAddress::_magic_number(*this);
}

StubAddress::StubAddress(const ServiceItem & service, const String & roleName, const String & threadName /*= String::empty_string() */)
    : ServiceAddress( service, roleName )
    , mThreadName   ( )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    set_thread(threadName); // don't change this to fix channel source.
    if ( ServiceAddress::is_valid() )
    {
        mChannel.set_cookie(areg::COOKIE_LOCAL);
    }

    mMagicNum = StubAddress::_magic_number(*this);
}

StubAddress::StubAddress(const areg::InterfaceData & siData, const String & roleName, const String & threadName /*= String::empty_string() */)
    : ServiceAddress( siData.idServiceName, siData.idVersion, siData.idServiceType, roleName )
    , mThreadName   ( )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    set_thread(threadName); // don't change this to fix channel source.
    if ( ServiceAddress::is_valid() )
        mChannel.set_cookie(areg::COOKIE_LOCAL);

    mMagicNum = StubAddress::_magic_number(*this);
}

StubAddress::StubAddress( const StubAddress & source )
    : ServiceAddress( static_cast<const ServiceAddress &>(source) )
    , mThreadName   ( source.mThreadName )
    , mChannel      ( source.mChannel )
    , mMagicNum     ( source.mMagicNum )
{
}

StubAddress::StubAddress( StubAddress && source ) noexcept
    : ServiceAddress( static_cast<ServiceAddress &&>(source) )
    , mThreadName   ( std::move(source.mThreadName) )
    , mChannel      ( std::move(source.mChannel) )
    , mMagicNum     ( source.mMagicNum )
{
}

StubAddress::StubAddress(const ServiceAddress & source)
    : ServiceAddress(static_cast<const ServiceAddress&>(source))
    , mThreadName   (ThreadAddress::invalid_thread_address().name())
    , mChannel      ( )
    , mMagicNum     (static_cast<uint32_t>(source))
{
    if (ServiceAddress::is_valid())
        mChannel.set_cookie(areg::COOKIE_LOCAL);
}

StubAddress::StubAddress( ServiceAddress && source)
    : ServiceAddress(std::move(source))
    , mThreadName   (ThreadAddress::invalid_thread_address().name())
    , mChannel      ( )
    , mMagicNum     (static_cast<uint32_t>(static_cast<const ServiceAddress &>(self())))
{
    if (ServiceAddress::is_valid())
        mChannel.set_cookie(areg::COOKIE_LOCAL);
}

StubAddress::StubAddress( const InStream & stream )
    : ServiceAddress( stream )
    , mThreadName   ( stream )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    ITEM_ID cookie = areg::COOKIE_LOCAL;
    stream >> cookie;
    if ( ServiceAddress::is_valid() )
        mChannel.set_cookie(cookie);

    mMagicNum = StubAddress::_magic_number(*this);
}

bool StubAddress::is_proxy_compatible(const ProxyAddress & proxyAddress) const
{
    if (is_valid() && proxyAddress.is_valid())
    {
        return (mRoleName == proxyAddress.role_name() && is_service_compatible(static_cast<const ServiceItem&>(proxyAddress)));
    }
    else
    {
        return false;
    }
}

void StubAddress::set_thread(const String & threadName)
{
    Thread * thread = threadName.is_empty() ? Thread::current_thread() : Thread::find_by_name(threadName);
    DispatcherThread * dispatcher = AREG_RUNTIME_CAST( thread, DispatcherThread);
    if ( (dispatcher != nullptr) && dispatcher->is_valid())
    {
        mThreadName = dispatcher->address().name();
        mMagicNum   = StubAddress::_magic_number(*this);
        mChannel.set_source( dispatcher->id() );
    }
    else
    {
        mMagicNum   = areg::CHECKSUM_IGNORE;
        mThreadName = ThreadAddress::invalid_thread_address().name();
    }
}

bool StubAddress::deliver_service_event( ServiceRequestEvent & serviceEvent ) const
{
    bool result{ false };

    const ITEM_ID & target{ mChannel.source() };
    Thread* thread = target != areg::TARGET_UNKNOWN ? Thread::find_by_id(static_cast<id_type>(target)) : nullptr;
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

void StubAddress::invalidate_channel()
{
    mChannel.invalidate();
}

bool StubAddress::is_valid() const
{
    return mChannel.is_valid();
}

String StubAddress::to_string() const
{
    String result(static_cast<uint32_t>(0xFF));

    result.append(EXTENTION_STUB)
          .append(areg::COMPONENT_PATH_SEPARATOR)
          .append(ServiceAddress::to_string())
          .append(areg::COMPONENT_PATH_SEPARATOR)
          .append(mThreadName)
          .append(areg::COMPONENT_PATH_SEPARATOR)
          .append(mChannel.to_string());

    return result;
}

void StubAddress::from_string(const char* pathStub, const char** out_nextPart /*= nullptr*/)
{
    const char* strSource = pathStub;
    if ( String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource) == EXTENTION_STUB )
    {
        ServiceAddress::from_string(strSource, &strSource);
        mThreadName  = String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource);
        mChannel.from_string( String::substr(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource).as_string() );

        mMagicNum   = StubAddress::_magic_number(*this);
    }

    if (out_nextPart != nullptr)
        *out_nextPart = strSource;
}

uint32_t StubAddress::_magic_number(const StubAddress & addrStub)
{
    uint32_t result = areg::CHECKSUM_IGNORE;

    if (addrStub.is_validated())
    {
        result = areg::crc32_init();
        result = areg::crc32_start( result, addrStub.mServiceName.as_string() );
        result = areg::crc32_start( result, static_cast<uint8_t>(addrStub.mServiceType) );
        result = areg::crc32_start( result, addrStub.mRoleName.as_string() );
        result = areg::crc32_start( result, addrStub.mThreadName.as_string() );
        result = areg::crc32_finish(result);
    }

    return result;
}

bool StubAddress::is_validated() const
{
    return ServiceAddress::is_validated() && (mThreadName.is_empty() == false) && (mThreadName != ThreadAddress::invalid_thread_address().name());
}

AREG_API_IMPL const InStream & operator >> ( const InStream & stream, StubAddress & input )
{
    ITEM_ID cookie = areg::COOKIE_LOCAL;
    stream >> static_cast<ServiceAddress &>(input);
    stream >> input.mThreadName;
    stream >> cookie;

    input.set_cookie(cookie);
    input.mMagicNum = StubAddress::_magic_number(input);
    
    return stream;
}

AREG_API_IMPL OutStream & operator << ( OutStream & stream, const StubAddress & output)
{
    stream << static_cast<const ServiceAddress &>(output);
    stream << output.mThreadName;
    stream << output.mChannel.cookie();
    return stream;
}

} // namespace areg
