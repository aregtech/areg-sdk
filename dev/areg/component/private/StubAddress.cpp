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
    return stubAddress.convToString();
}

StubAddress StubAddress::convPathToAddress( const char* pathStub, const char** out_nextPart /*= NULL*/ )
{
    StubAddress result(StubAddress::INVALID_STUB_ADDRESS);
    result.convFromString(pathStub, out_nextPart);
    return result;
}

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
StubAddress::StubAddress( void )
    : ServiceAddress( ServiceItem(), StubAddress::INVALID_STUB_NAME )
    , mThreadName   ( ThreadAddress::INVALID_THREAD_ADDRESS.getThreadName() )
    , mChannel      ( )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    ; // do nothing
}

StubAddress::StubAddress( const char * serviceName
                        , const Version & serviceVersion
                        , NEService::eServiceType serviceType
                        , const char * roleName
                        , const char * threadName   /*= NULL*/ )
    : ServiceAddress( serviceName, serviceVersion, serviceType, roleName )
    , mThreadName   ( threadName )
    , mChannel      ( )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mThreadName.truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(NEService::COOKIE_LOCAL);

    mMagicNum = StubAddress::_magicNumber(*this);
}

StubAddress::StubAddress(const ServiceItem & service, const char * roleName, const char * threadName /*= NULL */)
    : ServiceAddress( service, roleName )
    , mThreadName   ( threadName )
    , mChannel      ( )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mThreadName.truncate( NEUtilities::ITEM_NAMES_MAX_LENGTH );
    if ( ServiceAddress::isValid() )
        mChannel.setCookie(NEService::COOKIE_LOCAL);

    mMagicNum = StubAddress::_magicNumber(*this);
}

StubAddress::StubAddress(const NEService::SInterfaceData & siData, const char * roleName, const char * threadName /*= NULL */)
    : ServiceAddress( siData.idServiceName, siData.idVersion, siData.idServiceType, roleName )
    , mThreadName   ( String::EmptyString )
    , mChannel      ( )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    setThread(threadName);
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
    ; // do nothing
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

StubAddress::~StubAddress( void )
{
    ; // do nothing
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
    if ( (dispatcher != NULL) && dispatcher->isValid())
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
        Thread * thread = Thread::findThreadById(target);
        DispatcherThread * dispatcher = thread != NULL ? RUNTIME_CAST(thread, DispatcherThread) : NULL;
        if ( dispatcher != NULL )
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
    String result = "";

    result += EXTENTION_STUB;
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += ServiceAddress::convToString( );
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += mThreadName;
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += mChannel.convToString();

    return result;
}

void StubAddress::convFromString(const char* pathStub, const char** out_nextPart /*= NULL*/)
{
    const char* strSource = pathStub;
    if ( String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource) == StubAddress::EXTENTION_STUB )
    {
        ServiceAddress::convFromString(strSource, &strSource);
        mThreadName  = String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);
        mChannel.convFromString( String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource).getString() );

        mMagicNum   = StubAddress::_magicNumber(*this);
    }

    if (out_nextPart != NULL)
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
