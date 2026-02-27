#ifndef AREG_mtrouter_SERVICE_PRIVATE_LISTSERVICEPROXIES_HPP
#define AREG_mtrouter_SERVICE_PRIVATE_LISTSERVICEPROXIES_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        mtrouter/service/private/ListServiceProxies.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Proxy Service object, list of proxies
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/LinkedList.hpp"
#include "mtrouter/service/private/ServiceProxy.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class ServiceStub;
using ListServiceProxiesBase = LinkedList<ServiceProxy>;

//////////////////////////////////////////////////////////////////////////
// ListServiceProxies class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Container for remote service proxies registered in the network.
 **/
class ListServiceProxies  : public ListServiceProxiesBase
{
//////////////////////////////////////////////////////////////////////////
// The internal constants and types
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Invalid proxy service
     **/
    static const ServiceProxy     InvalidProxyService;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    ListServiceProxies() = default;

    /**
     * \brief   Copy constructor.
     *
     * \param   source      The source proxy list to copy.
     **/
    ListServiceProxies( const ListServiceProxies & source ) = default;

    /**
     * \brief   Move constructor.
     *
     * \param   source      The source proxy list to move.
     **/
    ListServiceProxies( ListServiceProxies && source ) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~ListServiceProxies() = default;

//////////////////////////////////////////////////////////////////////////
// Operators, operations and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copy assignment operator.
     *
     * \param   source      The source proxy list to copy.
     **/
    ListServiceProxies & operator = ( const ListServiceProxies & source ) = default;

    /**
     * \brief   Move assignment operator.
     *
     * \param   source      The source proxy list to move.
     **/
    ListServiceProxies & operator = ( ListServiceProxies && source ) noexcept = default;

    /**
     * \brief   Returns true if the specified proxy is registered in the list.
     *
     * \param   addrProxy       The proxy address to check.
     **/
    inline bool is_service_registered( const ProxyAddress & addrProxy ) const;

    /**
     * \brief   Returns the service proxy entry. Returns an invalid entry if the address is not
     *          registered.
     *
     * \param   addrProxy       The proxy address to look up.
     **/
    const ServiceProxy & service( const ProxyAddress & addrProxy ) const;
    
    /**
     * \brief   Returns pointer to the service proxy entry, or nullptr if not registered.
     *
     * \param   addrProxy       The proxy address to look up.
     **/
    ServiceProxy * service( const ProxyAddress & addrProxy );

    /**
     * \brief   Registers or retrieves a service proxy entry. Sets connection state to pending if
     *          the address is valid.
     *
     * \param   addrProxy       The proxy address to register or retrieve.
     * \return  Returns the registered or existing proxy service entry, or an invalid entry if the
     *          address is invalid.
     **/
    ServiceProxy & register_service( const ProxyAddress & addrProxy );

    /**
     * \brief   Registers or retrieves a service proxy entry with stub service availability. Sets
     *          connection state to connected if stub is available and connected.
     *
     * \param   addrProxy       The proxy address to register or retrieve.
     * \param   stubService     The stub service to associate with the proxy.
     * \return  Returns the registered or existing proxy service entry, or an invalid entry if the
     *          address is invalid.
     **/
    ServiceProxy & register_service( const ProxyAddress & addrProxy, const ServiceStub & stubService );

    /**
     * \brief   Unregisters and removes the proxy service entry. Returns an invalid entry if not
     *          found.
     *
     * \param   addrProxy       The proxy address to unregister.
     * \return  Returns the unregistered proxy service, or an invalid entry if not found.
     **/
    ServiceProxy unregister_service( const ProxyAddress & addrProxy );

    /**
     * \brief   Sets all registered proxy services to connected state and returns the number of
     *          entries modified.
     *
     * \param   addrStub    The address of the stub service that is now available.
     * \return  Returns the number of proxy entries modified; zero if the list is empty.
     **/
    int32_t stub_service_available( const StubAddress & addrStub );

    /**
     * \brief   Sets all registered proxy services to pending state and returns the number of
     *          entries modified.
     *
     * \return  Returns the number of proxy entries modified; zero if the list is empty.
     **/
    int32_t stub_service_unavailable();

    /**
     * \brief   Searches the list and collects all proxies matching the cookie value into
     *          out_listProxies.
     *
     * \param[out] out_listProxies     Output list that receives the matching proxy service entries.
     *                                 Existing entries are not removed.
     * \param   cookie              The cookie value to match.
     * \return  Returns the number of entries found; zero if no matches.
     **/
    int32_t specific_service( ListServiceProxies & out_listProxies, const ITEM_ID & cookie );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Searches the list for a service proxy matching the address.
     *
     * \param   addrProxy       The proxy address to search for.
     * \return  Returns the position of the found entry, or nullptr if not found.
     **/
    LISTPOS _find_proxy( const ProxyAddress & addrProxy ) const;
};

//////////////////////////////////////////////////////////////////////////
// ListServiceProxies class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool ListServiceProxies::is_service_registered( const ProxyAddress & addrProxy ) const
{
    return ListServiceProxiesBase::is_valid_position(_find_proxy(addrProxy));
}

#endif  // AREG_mtrouter_SERVICE_PRIVATE_LISTSERVICEPROXIES_HPP
