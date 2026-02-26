#ifndef AREG_mtrouter_SERVICE_PRIVATE_SERVICEPROXY_HPP
#define AREG_mtrouter_SERVICE_PRIVATE_SERVICEPROXY_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        mtrouter/service/private/ServiceProxy.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Proxy Service object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/ProxyAddress.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg { class StubAddress; }

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
    ServiceProxy();

    /**
     * \brief   The remote proxy address to set in object
     * \param   addrProxy   The address of remote proxy object.
     **/
    ServiceProxy( const areg::ProxyAddress & addrProxy );

    /**
     * \brief   The remote proxy address to set in object
     * \param   addrProxy   The address of remote proxy object.
     **/
    ServiceProxy( areg::ProxyAddress && addrProxy ) noexcept;

    /**
     * \brief   Extracts relevant data from given Stub address and initializes Proxy service object.
     * \param   addrStub    The Stub address to extract information.
     **/
    explicit ServiceProxy(const areg::StubAddress & addrStub);

    /**
     * \brief   Extracts relevant data from given Stub address and initializes Stub service object.
     * \param   addrStub    The Stub address to extract information.
     **/
    explicit ServiceProxy(areg::StubAddress && addrStub) noexcept;

    /**
     * \brief   Copies data from given source.
     * \param   serviceProxy    The source object to copy data.
     **/
    ServiceProxy( const ServiceProxy & serviceProxy );

    /**
     * \brief   Moves data from given source.
     * \param   serviceProxy    The source object to move data.
     **/
    ServiceProxy( ServiceProxy && serviceProxy ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~ServiceProxy() = default;

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
    ServiceProxy & operator = ( const areg::ProxyAddress & addrProxy );

    /**
     * \brief   Moves remote servicing proxy address data from given source.
     * \param   addrProxy   The address of remote proxy object to set in object.
     **/
    ServiceProxy & operator = ( areg::ProxyAddress && addrProxy ) noexcept;

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
    bool operator == ( const areg::ProxyAddress & addrProxy ) const;

    /**
     * \brief   Checks equality of service proxy and remote stub objects.
     *          Remote servicing proxy and stub are equal if proxy address is equal
     *          to stub address, i.e. they have logical connection.
     * \param   addrStub        The address of remote servicing stub to check.
     * \return  Returns true if 2 objects are equal.
     **/
    bool operator == ( const areg::StubAddress & addrStub ) const;

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if a remote proxy object is valid, i.e. the proxy address is valid.
     **/
    inline bool isValid() const;

    /**
     * \brief   Returns true if a remote proxy object status is connected
     **/
    inline bool isConnected() const;

    /**
     * \brief   Returns true if a remote proxy object status is waiting
     **/
    inline bool isWaiting() const;

    /**
     * \brief   Returns true the service connection status value
     **/
    inline areg::ServiceConnectionState getServiceStatus() const;

    /**
     * \brief   Returns the address of remote proxy
     **/
    inline const areg::ProxyAddress & getServiceAddress() const;

    /**
     * \brief   Called when the remote service stub of proxy is available.
     *          The system sets the stub address as target address of proxy.
     * \param   addrStub    The stub address to set.
     * \return  Returns true if proxy entry status is connected, 
     *          i.e. at least of proxy client was waiting for connection
     **/
    bool stubAvailable( const areg::StubAddress & addrStub );

    /**
     * \brief   Called when the remote service stub of proxy is not available anymore.
     *          The system resets the stub address as target address of proxy.
     * \return  Returns true if proxy entry status is waiting for connection,
     *          i.e. there was at least one client proxy connected or waiting for a connection.
     **/
    bool stubUnavailable();

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Sets servicing new status. The call is ignored if proxy target is unknown.
     * \param   newStatus   New status of proxy to set.
     **/
    void _setServiceStatus( areg::ServiceConnectionState newStatus );
    /**
     * \brief   Sets servicing proxy address and the connection status.
     * \param   addrProxy       The remote servicing proxy address to set.
     * \param   connectStatus   The connection status to update.
     **/
    void _setService( const areg::ProxyAddress & addrProxy, areg::ServiceConnectionState connectStatus = areg::ServiceConnectionState::Pending );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The remote servicing proxy address
     **/
    areg::ProxyAddress                    mProxyAddress;
    /**
     * \brief   The remote servicing connection status.
     **/
    areg::ServiceConnectionState   mConnectStatus;
};

//////////////////////////////////////////////////////////////////////////
// ServiceProxy class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const areg::ProxyAddress & ServiceProxy::getServiceAddress() const
{
    return mProxyAddress;
}

inline areg::ServiceConnectionState ServiceProxy::getServiceStatus() const
{
    return mConnectStatus;
}

inline bool ServiceProxy::isValid() const
{
    return mProxyAddress.isValid();
}

inline bool ServiceProxy::isConnected() const
{
    return ( mConnectStatus == areg::ServiceConnectionState::Connected );
}

inline bool ServiceProxy::isWaiting() const
{
    return ( mConnectStatus == areg::ServiceConnectionState::Pending );
}

#endif  // AREG_mtrouter_SERVICE_PRIVATE_SERVICEPROXY_HPP
