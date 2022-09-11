/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/StubAddress.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
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

#include <string_view>

namespace
{
/************************************************************************/
// Hidden constants
/************************************************************************/

    /**
     * \brief   The invalid service interface name (invalid stub name)
     *          None of service interfaces should contain this name.
     **/
    constexpr std::string_view  INVALID_STUB_NAME   { "INVALID_STUB" };

    /**
     * \brief   Extension to add to Stub path.
     **/
    constexpr std::string_view  EXTENTION_STUB      { "stub" };
}

//////////////////////////////////////////////////////////////////////////
// ProxyAddress class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Predefined constants and statics
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Predefined Invalid Stub address.
 **/
const StubAddress StubAddress::INVALID_STUB_ADDRESS;

//////////////////////////////////////////////////////////////////////////
// Static functions
//////////////////////////////////////////////////////////////////////////
String StubAddress::convAddressToPath( const StubAddress & stubAddress )
{
    return stubAddress.convToString();
}

StubAddress StubAddress::convPathToAddress( const char* pathStub, const char** out_nextPart /*= nullptr*/ )
{
    StubAddress result(StubAddress::INVALID_STUB_ADDRESS);
    result.convFromString(pathStub, out_nextPart);
    return result;
}

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
StubAddress::StubAddress( void )
    : ServiceAddress( ServiceItem(), INVALID_STUB_NAME )
    , mThreadName   ( ThreadAddress::INVALID_THREAD_ADDRESS.getThreadName() )
    , mChannel      ( )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
}

StubAddress::StubAddress( const String & serviceName
                        , const Version & serviceVersion
                        , NEService::eServiceType serviceType
                        , const String & roleName
                        , const String & threadName   /*= String::EmptyString*/ )
    : ServiceAddress( serviceName, serviceVersion, serviceType, roleName )
    , mThreadName   ( )
    , mChannel      ( )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    setThread(threadName); // don't change this to fix channel source.
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(NEService::COOKIE_LOCAL);

    mMagicNum = StubAddress::_magicNumber(*this);
}

StubAddress::StubAddress(const ServiceItem & service, const String & roleName, const String & threadName /*= String::EmptyString */)
    : ServiceAddress( service, roleName )
    , mThreadName   ( )
    , mChannel      ( )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    setThread(threadName); // don't change this to fix channel source.
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(NEService::COOKIE_LOCAL);

    mMagicNum = StubAddress::_magicNumber(*this);
}

StubAddress::StubAddress(const NEService::SInterfaceData & siData, const String & roleName, const String & threadName /*= String::EmptyString */)
    : ServiceAddress( siData.idServiceName, siData.idVersion, siData.idServiceType, roleName )
    , mThreadName   ( )
    , mChannel      ( )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    setThread(threadName); // don't change this to fix channel source.
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(NEService::COOKIE_LOCAL);

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
    , mThreadName   (ThreadAddress::INVALID_THREAD_ADDRESS.getThreadName())
    , mChannel      ( )
    , mMagicNum     (static_cast<unsigned int>(source))
{
    if (ServiceAddress::isValid())
        mChannel.setCookie(NEService::COOKIE_LOCAL);
}

StubAddress::StubAddress( ServiceAddress && source)
    : ServiceAddress(std::move(source))
    , mThreadName   (ThreadAddress::INVALID_THREAD_ADDRESS.getThreadName())
    , mChannel      ( )
    , mMagicNum     (static_cast<unsigned int>(static_cast<const ServiceAddress &>(self())))
{
    if (ServiceAddress::isValid())
        mChannel.setCookie(NEService::COOKIE_LOCAL);
}

StubAddress::StubAddress( const IEInStream & stream )
    : ServiceAddress( stream )
    , mThreadName   ( stream )
    , mChannel      ( )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    ITEM_ID cookie = NEService::COOKIE_LOCAL;
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

void StubAddress::setThread(const String & threadName)
{
    Thread * thread = threadName.isEmpty() ? Thread::getCurrentThread() : Thread::findThreadByName(threadName);
    DispatcherThread * dispatcher = RUNTIME_CAST( thread, DispatcherThread);
    if ( (dispatcher != nullptr) && dispatcher->isValid())
    {
        mThreadName = dispatcher->getAddress().getThreadName();
        mMagicNum   = StubAddress::_magicNumber(*this);
        mChannel.setSource( dispatcher->getId() );
    }
    else
    {
        mMagicNum   = NEMath::CHECKSUM_IGNORE;
        mThreadName = ThreadAddress::INVALID_THREAD_ADDRESS.getThreadName();
    }
}

bool StubAddress::deliverServiceEvent( ServiceRequestEvent & serviceEvent ) const
{
    bool result = false;
    ITEM_ID target = mChannel.getSource();
    if ( target != NEService::TARGET_UNKNOWN )
    {
        Thread * thread = Thread::findThreadById(static_cast<id_type>(target));
        DispatcherThread * dispatcher = thread != nullptr ? RUNTIME_CAST(thread, DispatcherThread) : nullptr;
        if ( dispatcher != nullptr )
        {
            result = serviceEvent.registerForThread(dispatcher);
            serviceEvent.deliverEvent();
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

String StubAddress::convToString(void) const
{
    String result(static_cast<uint32_t>(0xFF));

    result.append(EXTENTION_STUB)
          .append(NECommon::COMPONENT_PATH_SEPARATOR)
          .append(ServiceAddress::convToString())
          .append(NECommon::COMPONENT_PATH_SEPARATOR)
          .append(mThreadName)
          .append(NECommon::COMPONENT_PATH_SEPARATOR)
          .append(mChannel.convToString());

    return result;
}

void StubAddress::convFromString(const char* pathStub, const char** out_nextPart /*= nullptr*/)
{
    const char* strSource = pathStub;
    if ( String::getSubstring(strSource, NECommon::COMPONENT_PATH_SEPARATOR.data(), &strSource) == EXTENTION_STUB.data() )
    {
        ServiceAddress::convFromString(strSource, &strSource);
        mThreadName  = String::getSubstring(strSource, NECommon::COMPONENT_PATH_SEPARATOR.data(), &strSource);
        mChannel.convFromString( String::getSubstring(strSource, NECommon::COMPONENT_PATH_SEPARATOR.data(), &strSource).getString() );

        mMagicNum   = StubAddress::_magicNumber(*this);
    }

    if (out_nextPart != nullptr)
        *out_nextPart = strSource;
}

unsigned int StubAddress::_magicNumber(const StubAddress & addrStub)
{
    unsigned int result = NEMath::CHECKSUM_IGNORE;

    if (addrStub.isValidated())
    {
        result = NEMath::crc32Init();
        result = NEMath::crc32Start( result, addrStub.mServiceName.getString() );
        result = NEMath::crc32Start( result, static_cast<unsigned char>(addrStub.mServiceType) );
        result = NEMath::crc32Start( result, addrStub.mRoleName.getString() );
        result = NEMath::crc32Start( result, addrStub.mThreadName.getString() );
        result = NEMath::crc32Finish(result);
    }

    return result;
}

bool StubAddress::isValidated(void) const
{
    return ServiceAddress::isValidated() && (mThreadName.isEmpty() == false) && (mThreadName != ThreadAddress::INVALID_THREAD_ADDRESS.getThreadName());
}

AREG_API const IEInStream & operator >> ( const IEInStream & stream, StubAddress & input )
{
    ITEM_ID cookie = NEService::COOKIE_LOCAL;
    stream >> static_cast<ServiceAddress &>(input);
    stream >> input.mThreadName;
    stream >> cookie;

    input.setCookie(cookie);
    input.mMagicNum = StubAddress::_magicNumber(input);
    
    return stream;
}

AREG_API IEOutStream & operator << ( IEOutStream & stream, const StubAddress & output)
{
    stream << static_cast<const ServiceAddress &>(output);
    stream << output.mThreadName;
    stream << output.mChannel.getCookie();
    return stream;
}
