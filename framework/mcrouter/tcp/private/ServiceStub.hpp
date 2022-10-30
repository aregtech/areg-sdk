#ifndef AREG_MCROUTER_TCP_PRIVATE_SERVICESTUB_HPP
#define AREG_MCROUTER_TCP_PRIVATE_SERVICESTUB_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        mcrouter/tcp/private/ServiceStub.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Stub Service object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/StubAddress.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class ProxyAddress;

//////////////////////////////////////////////////////////////////////////
// ServiceStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Stub service object to register at message router.
 **/
class ServiceStub
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor, creates invalid Stub service object
     **/
    ServiceStub( void );

    /**
     * \brief   Initializes Stub address of remote service
     * \param   addrStub    The Stub address to set.
     **/
    ServiceStub( const StubAddress & addrStub );

    /**
     * \brief   Initializes Stub address of remote service
     * \param   addrStub    The Stub address to set.
     **/
    ServiceStub( StubAddress && addrStub ) noexcept;

    /**
     * \brief   Extracts relevant data from given Proxy address and initializes Stub service object.
     * \param   addrProxy   The Proxy address to extract information.
     **/
    explicit ServiceStub( const ProxyAddress & addrProxy );

    /**
     * \brief   Copies Stub service data from given source.
     * \param   stubService     The source of Stub service object to copy data.
     **/
    ServiceStub( const ServiceStub & stubService );

    /**
     * \brief   Copies Stub service data from given source.
     * \param   stubService     The source of Stub service object to copy data.
     **/
    ServiceStub( ServiceStub && stubService ) noexcept;

    /**
     * \brief   Initializes Service stub object by copying the service address data.
     * \param   addrService     Service address that contains data to copy.
     **/
    explicit ServiceStub( const ServiceAddress & addrService );

    /**
     * \brief   Initializes Service stub object by moving the service address data.
     * \param   addrService     Service address that contains data to move.
     **/
    explicit ServiceStub( ServiceAddress && addrService );

    /**
     * \brief   Destructor
     **/
    ~ServiceStub( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copies Stub address data from given source
     * \param   addrStub    The Stub address data to copy
     **/
    ServiceStub & operator = ( const StubAddress & addrStub );

    /**
     * \brief   Moves Stub address data from given source.
     * \param   addrStub    The Stub address data to move.
     **/
    ServiceStub & operator = ( StubAddress && addrStub ) noexcept;

    /**
     * \brief   Creates and copies Stub address data out of Proxy address.
     * \param   addrProxy   The Proxy address to generate Stub address information
     **/
    ServiceStub & operator = ( const ProxyAddress & addrProxy );

    /**
     * \brief   Copies data from given source
     * \param   stubService The source of Stub service to copy data.
     **/
    ServiceStub & operator = ( const ServiceStub & stubService );

    /**
     * \brief   Moves data from given source
     * \param   stubService The source of Stub service to move data.
     **/
    ServiceStub & operator = ( ServiceStub && stubService ) noexcept;

    /**
     * \brief   Checks equality of address set in Service and given Stub address
     * \param   addrStub    The address of Stub to check
     **/
    bool operator == ( const StubAddress & addrStub ) const;

    /**
     * \brief   Checks equality of Stub address set in service and Proxy address.
     *          Stub and Proxy addresses are equal if they are compatible.
     * \param   addrProxy   The Proxy address to check.
     **/
    bool operator == ( const ProxyAddress & addrProxy ) const;

    /**
     * \brief   Check equality of Stub service objects.
     * \param   stubService The Stub service object to check equality.
     **/
    bool operator == ( const ServiceStub & stubService ) const;

    /**
     * \brief   Coverts data of Stub service object into 32-bit integer value.
     **/
    explicit operator unsigned int ( void ) const;

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if connection status of Stub service is connected
     **/
    inline bool isConnected( void ) const;

    /**
     * \brief   Returns true if connection status of Stub service is pending
     **/
    inline bool isWaiting( void ) const;

    /**
     * \brief   Returns the current connection status of Stub service
     **/
    inline NEService::eServiceConnection getServiceStatus( void ) const;

    /**
     * \brief   Returns Stub remote address of service
     **/
    inline const StubAddress & getServiceAddress( void ) const;

    /**
     * \brief   Returns true if Stub service object is valid.
     **/
    bool isValid( void ) const;

    /**
     * \brief   Sets new connection status of Stub service
     **/
    void setServiceStatus( NEService::eServiceConnection newStatus );

    /**
     * \brief   Sets service data. Sets Stub address and connection status.
     * \param   addrStub        The address of remote Stub to set.
     * \param   connectStatus   The connection status to set.
     **/
    void setService( const StubAddress & addrStub, NEService::eServiceConnection connectStatus = NEService::eServiceConnection::ServiceConnected );

private:
    /**
     * \brief   The address of remote Stub
     **/
    StubAddress                     mStubAddress;
    /**
     * \brief   The connection status of service.
     **/
    NEService::eServiceConnection   mConnectStatus;
};

//////////////////////////////////////////////////////////////////////////
// Hasher of ServiceStub class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the ServiceStub.
 */
namespace std
{
    //! Calculates the hash value of the ServiceStub object
    template<> struct hash<ServiceStub>
    {
        //! A function to convert ServiceStub object to unsigned int.
        inline unsigned int operator()(const ServiceStub& key) const
        {
            return static_cast<unsigned int>(static_cast<const ServiceAddress &>(key.getServiceAddress()));
        }
    };

    //!< Compares 2 ServiceStub objects
    template<> struct equal_to<ServiceStub>
    {
        //! A function operator to compare 2 StubAddress objects.
        inline bool operator() (const ServiceStub& key1, const ServiceStub& key2) const
        {
            return static_cast<const ServiceAddress&>(key1.getServiceAddress()) == static_cast<const ServiceAddress&>(key2.getServiceAddress());
        }
    };
}

//////////////////////////////////////////////////////////////////////////
// ServiceStub class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool ServiceStub::isConnected( void ) const
{
    return ( mConnectStatus == NEService::eServiceConnection::ServiceConnected );
}

inline bool ServiceStub::isWaiting( void ) const
{
    return ( mConnectStatus == NEService::eServiceConnection::ServicePending );
}

inline NEService::eServiceConnection ServiceStub::getServiceStatus( void ) const
{
    return mConnectStatus;
}

inline const StubAddress & ServiceStub::getServiceAddress( void ) const
{
    return mStubAddress;
}

#endif  // AREG_MCROUTER_TCP_PRIVATE_SERVICESTUB_HPP
