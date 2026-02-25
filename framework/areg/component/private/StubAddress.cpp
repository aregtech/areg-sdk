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
areg::String StubAddress::convAddressToPath( const StubAddress & stubAddress )
{
    return stubAddress.convToString();
}

StubAddress StubAddress::convPathToAddress( const char* pathStub, const char** out_nextPart /*= nullptr*/ )
{
    StubAddress result(StubAddress::getInvalidStubAddress());
    result.convFromString(pathStub, out_nextPart);
    return result;
}

const StubAddress & StubAddress::getInvalidStubAddress()
{
    static const StubAddress _invalidStubAddress;
    return _invalidStubAddress;
}

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
StubAddress::StubAddress()
    : ServiceAddress( )
    , mThreadName   ( ThreadAddress::getInvalidThreadAddress().getThreadName() )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
}

StubAddress::StubAddress( const areg::String & serviceName
                        , const areg::Version & serviceVersion
                        , areg::ServiceType serviceType
                        , const areg::String & roleName
                        , const areg::String & threadName   /*= areg::String::getEmptyString()*/ )
    : ServiceAddress( serviceName, serviceVersion, serviceType, roleName )
    , mThreadName   ( )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    setThread(threadName); // don't change this to fix channel source.
    if ( ServiceAddress::isValid() )
    {
        mChannel.setCookie(areg::COOKIE_LOCAL);
    }

    mMagicNum = StubAddress::_magicNumber(*this);
}

StubAddress::StubAddress(const ServiceItem & service, const areg::String & roleName, const areg::String & threadName /*= areg::String::getEmptyString() */)
    : ServiceAddress( service, roleName )
    , mThreadName   ( )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    setThread(threadName); // don't change this to fix channel source.
    if ( ServiceAddress::isValid() )
    {
        mChannel.setCookie(areg::COOKIE_LOCAL);
    }

    mMagicNum = StubAddress::_magicNumber(*this);
}

StubAddress::StubAddress(const areg::InterfaceData & siData, const areg::String & roleName, const areg::String & threadName /*= areg::String::getEmptyString() */)
    : ServiceAddress( siData.idServiceName, siData.idVersion, siData.idServiceType, roleName )
    , mThreadName   ( )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    setThread(threadName); // don't change this to fix channel source.
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(areg::COOKIE_LOCAL);

    mMagicNum = StubAddress::_magicNumber(*this);
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
    , mThreadName   (ThreadAddress::getInvalidThreadAddress().getThreadName())
    , mChannel      ( )
    , mMagicNum     (static_cast<uint32_t>(source))
{
    if (ServiceAddress::isValid())
        mChannel.setCookie(areg::COOKIE_LOCAL);
}

StubAddress::StubAddress( ServiceAddress && source)
    : ServiceAddress(std::move(source))
    , mThreadName   (ThreadAddress::getInvalidThreadAddress().getThreadName())
    , mChannel      ( )
    , mMagicNum     (static_cast<uint32_t>(static_cast<const ServiceAddress &>(self())))
{
    if (ServiceAddress::isValid())
        mChannel.setCookie(areg::COOKIE_LOCAL);
}

StubAddress::StubAddress( const InStream & stream )
    : ServiceAddress( stream )
    , mThreadName   ( stream )
    , mChannel      ( )
    , mMagicNum     ( areg::CHECKSUM_IGNORE )
{
    ITEM_ID cookie = areg::COOKIE_LOCAL;
    stream >> cookie;
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(cookie);

    mMagicNum = StubAddress::_magicNumber(*this);
}

bool StubAddress::isProxyCompatible(const ProxyAddress & proxyAddress) const
{
    if (isValid() && proxyAddress.isValid())
    {
        return (mRoleName == proxyAddress.getRoleName() && isServiceCompatible(static_cast<const ServiceItem&>(proxyAddress)));
    }
    else
    {
        return false;
    }
}

void StubAddress::setThread(const areg::String & threadName)
{
    Thread * thread = threadName.isEmpty() ? Thread::getCurrentThread() : Thread::findThreadByName(threadName);
    DispatcherThread * dispatcher = AREG_RUNTIME_CAST( thread, DispatcherThread);
    if ( (dispatcher != nullptr) && dispatcher->isValid())
    {
        mThreadName = dispatcher->getAddress().getThreadName();
        mMagicNum   = StubAddress::_magicNumber(*this);
        mChannel.setSource( dispatcher->getId() );
    }
    else
    {
        mMagicNum   = areg::CHECKSUM_IGNORE;
        mThreadName = ThreadAddress::getInvalidThreadAddress().getThreadName();
    }
}

bool StubAddress::deliverServiceEvent( ServiceRequestEvent & serviceEvent ) const
{
    bool result{ false };

    const ITEM_ID & target{ mChannel.getSource() };
    Thread* thread = target != areg::TARGET_UNKNOWN ? Thread::findThreadById(static_cast<id_type>(target)) : nullptr;
    DispatcherThread* dispatcher = thread != nullptr ? AREG_RUNTIME_CAST(thread, DispatcherThread) : nullptr;
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

void StubAddress::invalidateChannel()
{
    mChannel.invalidate();
}

bool StubAddress::isValid() const
{
    return mChannel.isValid();
}

areg::String StubAddress::convToString() const
{
    areg::String result(static_cast<uint32_t>(0xFF));

    result.append(EXTENTION_STUB)
          .append(areg::COMPONENT_PATH_SEPARATOR)
          .append(ServiceAddress::convToString())
          .append(areg::COMPONENT_PATH_SEPARATOR)
          .append(mThreadName)
          .append(areg::COMPONENT_PATH_SEPARATOR)
          .append(mChannel.convToString());

    return result;
}

void StubAddress::convFromString(const char* pathStub, const char** out_nextPart /*= nullptr*/)
{
    const char* strSource = pathStub;
    if ( areg::String::getSubstring(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource) == EXTENTION_STUB.data() )
    {
        ServiceAddress::convFromString(strSource, &strSource);
        mThreadName  = areg::String::getSubstring(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource);
        mChannel.convFromString( areg::String::getSubstring(strSource, areg::COMPONENT_PATH_SEPARATOR.data(), &strSource).getString() );

        mMagicNum   = StubAddress::_magicNumber(*this);
    }

    if (out_nextPart != nullptr)
        *out_nextPart = strSource;
}

uint32_t StubAddress::_magicNumber(const StubAddress & addrStub)
{
    uint32_t result = areg::CHECKSUM_IGNORE;

    if (addrStub.isValidated())
    {
        result = areg::crc32Init();
        result = areg::crc32Start( result, addrStub.mServiceName.getString() );
        result = areg::crc32Start( result, static_cast<uint8_t>(addrStub.mServiceType) );
        result = areg::crc32Start( result, addrStub.mRoleName.getString() );
        result = areg::crc32Start( result, addrStub.mThreadName.getString() );
        result = areg::crc32Finish(result);
    }

    return result;
}

bool StubAddress::isValidated() const
{
    return ServiceAddress::isValidated() && (mThreadName.isEmpty() == false) && (mThreadName != ThreadAddress::getInvalidThreadAddress().getThreadName());
}

AREG_API_IMPL const InStream & operator >> ( const InStream & stream, StubAddress & input )
{
    ITEM_ID cookie = areg::COOKIE_LOCAL;
    stream >> static_cast<ServiceAddress &>(input);
    stream >> input.mThreadName;
    stream >> cookie;

    input.setCookie(cookie);
    input.mMagicNum = StubAddress::_magicNumber(input);
    
    return stream;
}

AREG_API_IMPL OutStream & operator << ( OutStream & stream, const StubAddress & output)
{
    stream << static_cast<const ServiceAddress &>(output);
    stream << output.mThreadName;
    stream << output.mChannel.getCookie();
    return stream;
}
