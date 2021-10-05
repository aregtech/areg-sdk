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
 * \file        mcrouter/tcp/private/ServiceProxy.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Proxy Service object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/ProxyAddress.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class StubAddress;

//////////////////////////////////////////////////////////////////////////
// ServiceProxy class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The remote servicing proxy object, which contains the remote proxy address.
 *          The entry is used by server connection service to list available proxy objects in network.
 **/
class ServiceProxy
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    ServiceProxy( void );

    /**
     * \brief   The remote proxy address to set in object
     * \param   addrProxy   The address of remote proxy object.
     **/
    explicit ServiceProxy( const ProxyAddress & addrProxy );

    /**
     * \brief   The remote proxy address to set in object
     * \param   addrProxy   The address of remote proxy object.
     **/
    ServiceProxy( ProxyAddress && addrProxy ) noexcept;

    /**
     * \brief   Copies data from given source.
     * \param   serviceProxy    The source object to copy data.
     **/
    explicit ServiceProxy( const ServiceProxy & serviceProxy );

    /**
     * \brief   Moves data from given source.
     * \param   serviceProxy    The source object to move data.
     **/
    ServiceProxy( ServiceProxy && serviceProxy ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~ServiceProxy( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copies data from given source
     * \param   serviceProxy    The source of remote servicing data to copy
     **/
    ServiceProxy & operator = ( const ServiceProxy & serviceProxy );

    /**
     * \brief   Moves data from given source.
     * \param   serviceProxy    The source of remote servicing data to move.
     **/
    ServiceProxy & operator = ( ServiceProxy && serviceProxy ) noexcept;

    /**
     * \brief   Copies remote servicing proxy address data from given source
     * \param   addrProxy   The address of remote proxy object to set in object.
     **/
    ServiceProxy & operator = ( const ProxyAddress & addrProxy );

    /**
     * \brief   Moves remote servicing proxy address data from given source.
     * \param   addrProxy   The address of remote proxy object to set in object.
     **/
    ServiceProxy & operator = ( ProxyAddress && addrProxy ) noexcept;

    /**
     * \brief   Checks equality of 2 service proxy objects.
     *          2 remote servicing proxies are equal if the generated cookie
     *          and instantiated thread values are equal.
     * \param   serviceProxy    Another remote servicing proxy object to compare.
     * \return  Returns true if 2 objects are equal.
     **/
    bool operator == ( const ServiceProxy & serviceProxy ) const;

    /**
     * \brief   Checks equality of 2 service proxy addresses.
     *          2 remote servicing proxy addresses are equal if the generated cookie
     *          and instantiated thread values are equal.
     * \param   addrProxy       The address of remote servicing proxy to check.
     * \return  Returns true if 2 objects are equal.
     **/
    bool operator == ( const ProxyAddress & addrProxy ) const;

    /**
     * \brief   Checks equality of service proxy and remote stub objects.
     *          Remote servicing proxy and stub are equal if proxy address is equal
     *          to stub address, i.e. they have logical connection.
     * \param   addrStub        The address of remote servicing stub to check.
     * \return  Returns true if 2 objects are equal.
     **/
    bool operator == ( const StubAddress & addrStub ) const;

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if a remote proxy object is valid, i.e. the proxy address is valid.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Returns true if a remote proxy object status is connected
     **/
    inline bool isConnected( void ) const;

    /**
     * \brief   Returns true if a remote proxy object status is waiting
     **/
    inline bool isWaiting( void ) const;

    /**
     * \brief   Returns true the service connection status value
     **/
    inline NEService::eServiceConnection getServiceStatus( void ) const;

    /**
     * \brief   Returns the address of remote proxy
     **/
    inline const ProxyAddress & getServiceAddress( void ) const;

    /**
     * \brief   Called when the remote service stub of proxy is available.
     *          The system sets the stub address as target address of proxy.
     * \param   addrStub    The stub address to set.
     * \return  Returns true if proxy entry status is connected, 
     *          i.e. at least of proxy client was waiting for connection
     **/
    bool stubAvailable( const StubAddress & addrStub );

    /**
     * \brief   Called when the remote service stub of proxy is not available anymore.
     *          The system resets the stub address as target address of proxy.
     * \return  Returns true if proxy entry status is waiting for connection,
     *          i.e. there was at least one client proxy connected or waiting for a connection.
     **/
    bool stubUnavailable( void );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Sets servicing new status. The call is ignored if proxy target is unknown.
     * \param   newStatus   New status of proxy to set.
     **/
    void _setServiceStatus( NEService::eServiceConnection newStatus );
    /**
     * \brief   Sets servicing proxy address and the connection status.
     * \param   addrProxy       The remote servicing proxy address to set.
     * \param   connectStatus   The connection status to update.
     **/
    void _setService( const ProxyAddress & addrProxy, NEService::eServiceConnection connectStatus = NEService::eServiceConnection::ServicePending );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The remote servicing proxy address
     **/
    ProxyAddress                    mProxyAddress;
    /**
     * \brief   The remote servicing connection status.
     **/
    NEService::eServiceConnection   mConnectStatus;
};

//////////////////////////////////////////////////////////////////////////
// ServiceProxy class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const ProxyAddress & ServiceProxy::getServiceAddress(void) const
{
    return mProxyAddress;
}

inline NEService::eServiceConnection ServiceProxy::getServiceStatus(void) const
{
    return mConnectStatus;
}

inline bool ServiceProxy::isValid(void) const
{
    return mProxyAddress.isValid();
}

inline bool ServiceProxy::isConnected( void ) const
{
    return ( mConnectStatus == NEService::eServiceConnection::ServiceConnected );
}

inline bool ServiceProxy::isWaiting( void ) const
{
    return ( mConnectStatus == NEService::eServiceConnection::ServicePending );
}
