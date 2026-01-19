#ifndef AREG_COMPONENT_STUBADDRESS_HPP
#define AREG_COMPONENT_STUBADDRESS_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/StubAddress.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Stub Address class.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/ServiceAddress.hpp"
#include "areg/component/Channel.hpp"

#include <utility>

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEInStream;
class ProxyAddress;
class ServiceRequestEvent;

//////////////////////////////////////////////////////////////////////////
// StubAddress class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Stub address class.
 *              This class defines address of stub object. All events
 *              sent to Stub object should have as target defined
 *              this Stub Address.
 * 
 * \details     The Stub Address contains Component Address of Stub holder
 *              and name of implemented service interface. The events 
 *              sent to Stub object should contain this address as a target
 *              of event delivery and the address should not be equal to
 *              invalid stub address. Check validation
 *              of Stub address before sending event.
 *
 **/
class AREG_API StubAddress    : public    ServiceAddress
{
//////////////////////////////////////////////////////////////////////////
// Static operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts Stub address to string as a Address Path, containing
     *          path separator.
     * \param   addrStub    The Stub address containing information to crate path
     * \return  Returns converted path of Stub as string, containing Stub address information
     **/
    static String convAddressToPath( const StubAddress & addrStub );

    /**
     * \brief   Instantiate Stub Address from given address path.
     *          If out_nextPart is not nullptr, on output this will contain remaining part
     *          from Stub path.
     * \param   pathStub        The path of Stub object, containing information for address.
     * \param   out_nextPart    If not nullptr, on output this will contain remaining part of Stub path
     * \return  Returns initialized StubAddress object, containing information taken from path
     **/
    static StubAddress convPathToAddress(const char* pathStub, const char** out_nextPart = nullptr);

    /**
     * \brief   Returns predefined invalid stub address object.
     **/
    static const StubAddress & getInvalidStubAddress(void);

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initially creates invalid Stub address
     **/
    StubAddress( void );

    /**
     * \brief   Initialize Stub address from given service interface name, version, type, role name and holder thread.
     * \param   serviceName     The implemented Service Interface name of Stub
     * \param   serviceVersion  The implemented version of Service Interface
     * \param   serviceType     The type of service
     * \param   roleName        The role name of holder component
     * \param   threadName      Optional thread name of Stub. If nullptr, the current thread where Stub instantiated is set.
     **/
    StubAddress( const String & serviceName
               , const Version & serviceVersion
               , NEService::eServiceType serviceType
               , const String & roleName
               , const String & threadName = String::getEmptyString() );

    /**
     * \brief   Initialize Stub address from given service item, role name and holder thread.
     * \param   service     Service item, which contains basic information of service
     * \param   roleName    The role name of holder component
     * \param   threadName  Optional thread name of Stub. If nullptr, the current thread where Stub instantiated is set.
     **/
    StubAddress( const ServiceItem & service, const String & roleName, const String & threadName = String::getEmptyString() );

    /**
     * \brief   Initialize Stub address from given service data, role name and holder thread.
     * \param   siData          Service data, which contains basic information of service
     * \param   roleName        The role name of holder component
     * \param   threadName      Optional thread name of Stub. If nullptr, the current thread where Stub instantiated is set.
     **/
    StubAddress( const NEService::SInterfaceData & siData, const String & roleName, const String & threadName = String::getEmptyString() );

    /**
     * \brief   Copy constructor.
     * \param   source  The source of data to copy.
     **/
    StubAddress( const StubAddress & source );

    /**
     * \brief   Move constructor.
     * \param   source  The source of data to move.
     **/
    StubAddress( StubAddress && source ) noexcept;

    /**
     * \brief   Initializes stub address by copying service address data.
     * \param   source  The service address source of data to copy.
     **/
    explicit StubAddress( const ServiceAddress & source );

    /**
     * \brief   Initializes stub address by moving service address data.
     * \param   source  The service address source of data to move.
     **/
    explicit StubAddress( ServiceAddress && source );

    /**
     * \brief   Initialize Stub address from stream.
     * \param   stream  The streaming object to read data.
     **/
    StubAddress( const IEInStream & stream);

    /**
     * \brief   Destructor.
     **/
    virtual ~StubAddress( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Basic operators
/************************************************************************/

    /**
     * \brief   Copies Stub Address data from given source.
     * \param   source  The source of stub address to copy
     **/
    inline StubAddress & operator = ( const StubAddress & source );

    /**
     * \brief   Moves Stub Address data from given source.
     * \param   source  The source of stub address to move.
     **/
    inline StubAddress & operator = ( StubAddress && source ) noexcept;

    /**
     * \brief   Copies Stub Address data from given service address.
     * \param   addrService The service address as a source of basic information.
     **/
    inline StubAddress & operator = ( const ServiceAddress & addrService );

    /**
     * \brief   Copies Stub Address data from given service address.
     * \param   addrService The service address as a source of basic information.
     **/
    inline StubAddress & operator = ( ServiceAddress && addrService ) noexcept;

    /**
     * \brief   Checks equality of 2 stub address and returns true if objects are equal.
     * \param   other   The stub address to compare.
     **/
    inline bool operator == ( const StubAddress & other ) const;

    /**
     * \brief   Checks compatibility of stub and proxy addresses and returns true if addresses of objects are compatible.
     * \param   addrProxy   The proxy address to check compatibility.
     **/
    inline bool operator == ( const ProxyAddress & addrProxy ) const;

    /**
     * \brief   Checks inequality of 2 stub address and returns true if objects are not equal.
     * \param   other   The stub address to compare.
     **/
    inline bool operator != ( const StubAddress & other ) const;

    /**
     * \brief   Converts the stub address to 32-bit integer.
     **/
    inline explicit operator unsigned int ( void ) const;

/************************************************************************/
// Friend global operators for streaming
/************************************************************************/

    /**
     * \brief   Streaming operator. Reads and initialize proxy address from stream.
     * \param   stream  The streaming object to read data.
     * \param   input   Service address object to initialize data.
     **/
    friend AREG_API const IEInStream & operator >> ( const IEInStream & stream, StubAddress & input );

    /**
     * \brief   Streaming operator. Writes proxy address into stream.
     * \param   stream  The streaming object to write data.
     * \param   output  Service address object to serialize.
     **/
    friend AREG_API IEOutStream & operator << ( IEOutStream & stream, const StubAddress & output);

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if stub address is local
     **/
    inline bool isLocalAddress( void ) const;

    /**
     * \brief   Returns true if stub address is remote
     **/
    inline bool isRemoteAddress( void ) const;

    /**
     * \brief   Returns true if the source of communication channel is local, i.e. it is the same process.
     **/
    inline bool isSourceLocal( void ) const;

    /**
     * \brief   Returns true if the source of communication channel is public, i.e. it is external process.
     **/
    inline bool isSourcePublic( void ) const;

    /**
     * \brief   Returns true if the target of communication channel is local, i.e. it is the same process.
     **/
    inline bool isTargetLocal( void ) const;

    /**
     * \brief   Returns true if the target of communication channel is public, i.e. it is external process.
     **/
    inline bool isTargetPublic( void ) const;

    /**
     * \brief   Returns stub communication channel
     **/
    inline const Channel & getChannel( void ) const;

    /**
     * \brief   Sets stub communication channel.
     **/
    inline void setChannel( const Channel & channel );

    /**
     * \brief   Returns stub cookies value
     **/
    inline const ITEM_ID & getCookie( void ) const;

    /**
     * \brief   Sets stub cookie value
     **/
    inline void setCookie(const ITEM_ID & cookie );

    /**
     * \brief   Returns the ID of source set in communication channel
     **/
    inline const ITEM_ID & getSource( void ) const;

    /**
     * \brief   Sets the ID of source in communication channel.
     **/
    inline void setSource(const ITEM_ID & source );

    /**
     * \brief   Returns the service owner thread name.
     **/
    inline const String & getThread( void ) const;

    /**
     * \brief   Sets the service owner thread name.
     * \param   threadName  The thread name to set.
     **/
    void setThread( const String & threadName );

    /**
     * \brief   Returns validity of stub address. 
     *          Returns true if Stub Address is not invalid.
     **/
    bool isValid( void ) const;

    /**
     * \brief   Invalidates communication channel
     **/
    void invalidateChannel( void );

    /**
     * \brief   Checks compatibility of given proxy address.
     * \param   proxyAddress    The address of proxy to check compatibility
     * \return  Returns true if proxy is compatible with proxy. Otherwise it returns false.
     **/
    bool isProxyCompatible( const ProxyAddress & proxyAddress ) const;

    /**
     * \brief   Triggered to deliver given service event to target. Returns true if event is delivered.
     * \param   serviceEvent    The servicing event to deliver to target.
     * \return  Returns true if event is delivered to target for further dispatching.
     * \note    For remote events, it returns true if event is queued to be delivered and returned boolean value does not
     *          indicate that the target component received event.
     **/
    bool deliverServiceEvent( ServiceRequestEvent & serviceEvent ) const;

    /**
     * \brief   Converts Stub address to string as a Address Path, containing
     *          path separator.
     * \return  Returns converted path of Stub as string, containing Stub address information
     **/
    String convToString( void ) const;

    /**
     * \brief   Instantiate Stub Address from given address path.
     *          If out_nextPart is not nullptr, on output this will contain remaining part
     *          from Stub path.
     * \param   pathStub        The path of Stub object, containing information for address.
     * \param   out_nextPart    If not nullptr, on output this will contain remaining part of Stub path
     **/
    void convFromString(const char* pathStub, const char** out_nextPart = nullptr);

protected:
    /**
     * \brief   Returns true if stub address data is valid.
     **/
    bool isValidated( void ) const;

private:
    /**
     * \brief   Returns own object.
     **/
    inline StubAddress& self(void);
    /**
     * \brief   Returns the calculated hash-key value of specified stub address object.
     **/
    static unsigned int _magicNumber( const StubAddress & addrStub );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The name of owner thread.
     **/
    String          mThreadName;
    /**
     * \brief   The communication channel.
     **/
    Channel         mChannel;

private:
    /**
     * \brief   The calculated number of stub address.
     **/
    unsigned int    mMagicNum;
};

//////////////////////////////////////////////////////////////////////////
// Hasher of StubAddress class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the StubAddress.
 */
namespace std
{
    //! Calculates the hash value of the StubAddress object
    template<> struct hash<StubAddress>
    {
        //! A function operator to convert StubAddress object to hash value.
        inline unsigned int operator()(const StubAddress& key) const
        {
            return static_cast<unsigned int>(key);
        }
    };

    //!< Compares 2 StubAddress objects
    template<> struct equal_to<StubAddress>
    {
        //! A function operator to compare 2 StubAddress objects.
        inline bool operator() (const StubAddress& key1, const StubAddress& key2) const
        {
            return static_cast<const ServiceAddress&>(key1) == static_cast<const ServiceAddress&>(key2);
        }
    };
}

//////////////////////////////////////////////////////////////////////////
// StubAddress class inline functions implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

inline StubAddress & StubAddress::operator = ( const StubAddress & source )
{
    if (this != &source)
    {
        static_cast<ServiceAddress &>(*this) = static_cast<const ServiceAddress &>(source);
        mThreadName = source.mThreadName;
        mChannel    = source.mChannel;
        mMagicNum   = source.mMagicNum;
    }

    return (*this);
}

inline StubAddress & StubAddress::operator = ( StubAddress && source ) noexcept
{
    if ( this != &source )
    {
        static_cast<ServiceAddress &>(*this) = static_cast<ServiceAddress &&>(source);
        mThreadName = std::move(source.mThreadName);
        mChannel    = std::move(source.mChannel);
        mMagicNum   = source.mMagicNum;
    }

    return (*this);
}

inline StubAddress & StubAddress::operator = (const ServiceAddress & addrService)
{
    if ( static_cast<const ServiceAddress *>(this) != &addrService)
    {
        static_cast<ServiceAddress &>(*this) = static_cast<const ServiceAddress &>(addrService);
        mThreadName = String::getEmptyString();
        mChannel    = Channel();
        mMagicNum   = StubAddress::_magicNumber(*this);
    }

    return (*this);
}

inline StubAddress & StubAddress::operator = ( ServiceAddress && addrService ) noexcept
{
    if ( static_cast<const ServiceAddress *>(this) != &addrService )
    {
        static_cast<ServiceAddress &>(*this) = static_cast<ServiceAddress &&>(addrService);
        mThreadName = String::getEmptyString();
        mChannel    = Channel( );
        mMagicNum   = StubAddress::_magicNumber( *this );
    }

    return (*this);
}

inline bool StubAddress::operator == ( const StubAddress & other ) const
{
    return (mMagicNum == other.mMagicNum) && (mChannel.getCookie() == other.mChannel.getCookie());
}

inline bool StubAddress::operator != ( const StubAddress& other ) const
{
    return (mMagicNum != other.mMagicNum) || (mChannel.getCookie() != other.mChannel.getCookie());
}

inline bool StubAddress::operator == ( const ProxyAddress & addrProxy ) const
{
    return isProxyCompatible(addrProxy);
}

inline StubAddress::operator unsigned int ( void ) const
{
    return mMagicNum;
}

inline bool StubAddress::isLocalAddress(void) const
{
    return mChannel.getCookie() == NEService::COOKIE_LOCAL;
}

inline bool StubAddress::isRemoteAddress(void) const
{
    return (mChannel.getCookie() >= NEService::COOKIE_ANY);
}

inline bool StubAddress::isSourceLocal( void ) const
{
    return (mChannel.getCookie( ) == NEService::COOKIE_LOCAL) && (mChannel.getSource( ) != 0);
}

inline bool StubAddress::isSourcePublic( void ) const
{
    return (mChannel.getCookie( ) >= NEService::COOKIE_REMOTE_SERVICE) && (mChannel.getSource( ) != 0);
}

inline bool StubAddress::isTargetLocal( void ) const
{
    return (mChannel.getCookie( ) == NEService::COOKIE_LOCAL) && (mChannel.getTarget( ) != 0);
}

inline bool StubAddress::isTargetPublic( void ) const
{
    return (mChannel.getCookie( ) >= NEService::COOKIE_LOCAL) && (mChannel.getTarget( ) != 0);
}

inline const String & StubAddress::getThread( void ) const
{
    return mThreadName;
}

inline const Channel & StubAddress::getChannel(void) const
{
    return mChannel;
}

inline void StubAddress::setChannel(const Channel & channel)
{
    mChannel = channel;
}

inline const ITEM_ID & StubAddress::getCookie( void ) const
{
    return mChannel.getCookie();
}

inline void StubAddress::setCookie(const ITEM_ID & cookie )
{
    mChannel.setCookie(cookie);
}

inline const ITEM_ID & StubAddress::getSource( void ) const
{
    return mChannel.getSource();
}

inline void StubAddress::setSource(const ITEM_ID & source )
{
    return mChannel.setSource(source);
}

inline StubAddress& StubAddress::self(void)
{
    return (*this);
}

#endif  // AREG_COMPONENT_STUBADDRESS_HPP
