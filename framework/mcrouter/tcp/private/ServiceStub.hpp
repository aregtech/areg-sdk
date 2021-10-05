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
    explicit ServiceStub( const StubAddress & addrStub );

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
     * \brief   Extracts relevant data from given Proxy address and initializes Stub service object.
     * \param   addrProxy   The Proxy address to extract information.
     **/
    explicit ServiceStub( ProxyAddress && addrProxy ) noexcept;

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
     * \brief   Creates and moves Stub address data out of Proxy address.
     * \param   addrProxy   The Proxy address to generate Stub address information
     **/
    ServiceStub & operator = ( ProxyAddress && addrProxy ) noexcept;

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
