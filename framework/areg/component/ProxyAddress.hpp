#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/ProxyAddress.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Proxy Address class.
 *              Every Proxy is reached by unique address, which is a 
 *              part of Proxy object.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/NEService.hpp"
#include "areg/component/ServiceAddress.hpp"
#include "areg/component/Channel.hpp"

#include <utility>

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEInStream;
class StubAddress;
class Event;
class ServiceRequestEvent;
class ServiceResponseEvent;

//////////////////////////////////////////////////////////////////////////
// ProxyAddress class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Every Proxy contains Proxy Address. The Proxy Address is needed
 *          to identify Proxy. The proxy address is unique and contains 
 *          information of thread, connected component, and the service interface.
 **/
class AREG_API ProxyAddress   : public    ServiceAddress
{
public:
    /**
     * \brief   ProxyAddress::INVALID_PROXY_ADDRESS
     *          Invalid proxy address.
     *          Need to verify Proxy Address validation.
     **/
    static const ProxyAddress INVALID_PROXY_ADDRESS;

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Creates proxy address path from given proxy address.
     *          Every part of proxy address has a special path separator
     *          and path contains information of process ID, thread name,
     *          connected component role name and supported service name,
     *          and special extension identifying proxy.
     * \param	addrProxy   The proxy address to generate path
     * \return  Returns converted path of Proxy as string, containing Proxy address information
     **/
    static String convAddressToPath( const ProxyAddress & addrProxy );

    /**
     * \brief	Pars proxy path string and retrieves proxy address data from path.
     * \param	pathProxy	    The proxy path as a string.
     * \param	out_nextPart	If not a nullptr, on output this will contain remaining
     *                          part after getting proxy path. On output usually
     *                          should be nullptr.
     * \return	Proxy address object.
     **/
    static ProxyAddress convPathToAddress(const char * pathProxy, const char** out_nextPart = nullptr);

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Default constructor. By default created Invalid Proxy Address
     **/
    ProxyAddress( void );

    /**
     * \brief	Creates Proxy address according required connected component role name,
     *          service name and thread address of Proxy.
     * \param   serviceName     The name of service for Proxy
     * \param   serviceVersion  The version of service
     * \param   serviceType     The type of service (local, remote, etc.)
     * \param   roleName        Assigned role name of Proxy
     * \param   threadName      The name of thread where Proxy should act. If null, it is processed in current thread.
     **/
    ProxyAddress( const char * serviceName, const Version & serviceVersion, NEService::eServiceType serviceType, const char * roleName, const char * threadName = nullptr );
    /**
     * \brief	Creates Proxy address according required connected component role name,
     *          service name and thread address of Proxy.
     * \param   service         The service item object, which contains service name, version and type
     * \param   roleName        Assigned role name of Proxy
     * \param   threadName      The name of thread where Proxy should act. If null, it is processed in current thread.
     **/
    ProxyAddress( const ServiceItem & service, const char * roleName, const char * threadName = nullptr );
    /**
     * \brief	Creates Proxy address according required connected component role name,
     *          service name and thread address of Proxy.
     * \param   siData          The service interface data to assign to Proxy.
     * \param   roleName        Assigned role name of Proxy
     * \param   threadName      The name of thread where Proxy should act. If null, it is processed in current thread.
     **/
    ProxyAddress( const NEService::SInterfaceData & siData, const char * roleName, const char * threadName = nullptr );

    /**
     * \brief   Copy constructor.
     * \param   source  The source of data to copy.
     **/
    ProxyAddress( const ProxyAddress & source );

    /**
     * \brief   Move constructor.
     * \param   source  The source of data to move.
     **/
    ProxyAddress( ProxyAddress && source ) noexcept;

    /**
     * \brief   Initialize proxy address from streaming object.
     **/
    ProxyAddress(const IEInStream & stream);

    /**
     * \brief   Destructor.
     **/
    virtual ~ProxyAddress( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Basic operators
/************************************************************************/

    /**
     * \brief   Copies proxy address from given source.
     * \param   source  The source of Proxy address to copy
     **/
    inline ProxyAddress & operator = ( const ProxyAddress & source );

    /**
     * \brief   Moves proxy address from given source.
     * \param   source  The source of Proxy address to move.
     **/
    inline ProxyAddress & operator = ( ProxyAddress && source ) noexcept;

    /**
     * \brief   Checks equality of 2 proxy address objects. Returns true if 2 proxy addresses are equal.
     * \param   other   The Proxy address object to compare
     **/
    inline bool operator == ( const ProxyAddress & other ) const;

    /**
     * \brief   Returns true if passed stub address is compatible with existing proxy address
     * \param   addrStub    Address of stub to compare.
     **/
    inline bool operator == (const StubAddress & addrStub ) const;

    /**
     * \brief   Checks inequality of 2 proxy addresses. Returns true if 2 proxy addresses are not equal.
     * \param   other   The Proxy address object to compare
     **/
    inline bool operator != ( const ProxyAddress & other ) const;

    /**
     * \brief   Converts ProxyAddress object to 32-bit unsigned int value.
     **/
    inline explicit operator unsigned int ( void ) const;

/************************************************************************/
// Friend global operators for streaming
/************************************************************************/

    /**
     * \brief   Reads and initialize proxy address from stream.
     * \param   stream  The streaming object to read data.
     * \param   input   Service address object to initialize data.
     **/
    friend AREG_API const IEInStream & operator >> ( const IEInStream & stream, ProxyAddress & input );

    /**
     * \brief   Writes proxy address into stream.
     * \param   stream  The streaming object to write data.
     * \param   output  Service address object to serialize.
     **/
    friend AREG_API IEOutStream & operator << ( IEOutStream & stream, const ProxyAddress & output);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if existing Proxy address is local
     **/
    inline bool isLocalAddress( void ) const;

    /**
     * \brief   Returns true if existing Proxy address is local
     **/
    inline bool isRemoteAddress( void ) const;

    /**
     * \brief   Returns thread name of processed Proxy
     **/
    inline const String & getThread( void ) const;
    /**
     * \brief   Sets the thread name of processed Proxy
     **/
    void setThread( const char * threadName );
    /**
     * \brief   Returns Proxy communication channel object
     **/
    inline const Channel & getChannel( void ) const;
    /**
     * \brief   Sets proxy communication channel
     **/
    inline void setChannel( const Channel & channel );
    /**
     * \brief   Returns Proxy cookie value
     **/
    inline ITEM_ID getCookie( void ) const;
    /**
     * \brief   Sets Proxy cookie value
     **/
    inline void setCookie( ITEM_ID cookie );
    /**
     * \brief   Returns Proxy source ID
     **/
    inline ITEM_ID getSource( void ) const;
    /**
     * \brief   Sets Proxy source ID
     **/
    inline void setSource( ITEM_ID source );
    /**
     * \brief   Returns Proxy target ID
     **/
    inline ITEM_ID getTarget( void ) const;
    /**
     * \brief   Sets Proxy target ID
     **/
    inline void setTarget( ITEM_ID target);

    /**
     * \brief   Returns true if proxy address is valid.
     *          The validation verified according of data in Invalid Proxy address.
     **/
    bool isValid( void ) const;

    /**
     * \brief   Invalidates communication channel
     **/
    void invalidateChannel( void );

    /**
     * \brief   Returns true if specified Stub address is compatible with Proxy
     * \param   addStub     The Stub address to check
     **/
    bool isStubCompatible( const StubAddress & addrStub ) const;

    /**
     * \brief   Delivers specified service request event to target Stub
     * \param   stubEvent   The service request event to deliver
     * \return  Returns true if event was successfully delivered. If the specified Event is
     *          for remote target, the returned boolean value indicates whether event was successfully
     *          queued in the communication channel to deliver.
     **/
    bool deliverServiceEvent( ServiceRequestEvent & stubEvent ) const;

    /**
     * \brief   Delivers specified service response event to target Stub
     * \param   proxyEvent   The service response event to deliver
     * \return  Returns true if event was successfully delivered. If the specified Event is
     *          for remote target, the returned boolean value indicates whether event was successfully
     *          queued in the communication channel to deliver.
     **/
    bool deliverServiceEvent( ServiceResponseEvent & proxyEvent ) const;

    /**
     * \brief	Creates proxy address path as a string.
     *          Every part of proxy address has a special path separator
     *          and path contains information of process ID, thread name,
     *          connected component role name and supported service name,
     *          and special extension identifying proxy.
     * \return  Returns converted path of Proxy as string, containing Proxy address information
     **/
    String convToString( void ) const;

    /**
     * \brief	Parses proxy path string and retrieves proxy address data from path.
     * \param	pathProxy	    The proxy path as a string.
     * \param	out_nextPart	If not a nullptr, on output this will contain remaining
     *                          part after getting proxy path. On output usually
     *                          should be nullptr.
     **/
    void convFromString(const char * pathProxy, const char** out_nextPart = nullptr);

protected:
    /**
     * \brief   Returns true if proxy address data is valid.
     **/
    bool isValidated( void ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Delivers specified service event to target.
     * \param   serviceEvent    The service event to deliver
     * \param   idTarget        The ID of target service to deliver event
     **/
    static bool _deliverEvent( Event & serviceEvent, ITEM_ID idTarget );

    /**
     * \brief   Returns the calculated hash-key value of specified proxy address object.
     **/
    static unsigned int _magicNumber( const ProxyAddress & proxy );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Thread name of Proxy
     **/
    String          mThreadName;
    /**
     * \brief   Communication channel of Proxy.
     **/
    Channel         mChannel;

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The calculated number of proxy address
     **/
    unsigned int    mMagicNum;
};

//////////////////////////////////////////////////////////////////////////
// ProxyAddress class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool ProxyAddress::operator == ( const StubAddress & addrStub ) const
{
    return isStubCompatible(addrStub);
}

inline ProxyAddress & ProxyAddress::operator = ( const ProxyAddress & source )
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

inline ProxyAddress & ProxyAddress::operator = ( ProxyAddress && source )noexcept
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

inline bool ProxyAddress::operator == ( const ProxyAddress & other ) const
{
    return (mMagicNum == other.mMagicNum) && (mChannel.getCookie() == other.mChannel.getCookie());
}

inline bool ProxyAddress::operator != ( const ProxyAddress & other ) const
{
    return (mMagicNum != other.mMagicNum) || (mChannel.getCookie() != other.mChannel.getCookie());
}

inline ProxyAddress::operator unsigned int(void) const
{
    return mMagicNum;
}

inline bool ProxyAddress::isLocalAddress(void) const
{
    return (mChannel.getCookie() == NEService::COOKIE_LOCAL);
}

inline bool ProxyAddress::isRemoteAddress(void) const
{
    return (mChannel.getCookie() > NEService::COOKIE_ROUTER );
}

inline const String & ProxyAddress::getThread(void) const
{
    return mThreadName;
}

inline const Channel & ProxyAddress::getChannel( void ) const
{
    return mChannel;
}

inline void ProxyAddress::setChannel( const Channel & channel )
{
    mChannel = channel;
}

inline ITEM_ID ProxyAddress::getCookie( void ) const
{
    return mChannel.getCookie();
}

inline void ProxyAddress::setCookie( ITEM_ID cookie )
{
    mChannel.setCookie(cookie);
}

inline ITEM_ID ProxyAddress::getSource( void ) const
{
    return mChannel.getSource();
}

inline void ProxyAddress::setSource( ITEM_ID source )
{
    return mChannel.setSource(source);
}

inline ITEM_ID ProxyAddress::getTarget( void ) const
{
    return mChannel.getTarget();
}

inline void ProxyAddress::setTarget( ITEM_ID target )
{
    return mChannel.setTarget(target);
}
