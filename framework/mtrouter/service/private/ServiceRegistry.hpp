#ifndef AREG_mtrouter_SERVICE_PRIVATE_SERVICEREGISTRY_HPP
#define AREG_mtrouter_SERVICE_PRIVATE_SERVICEREGISTRY_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        mtrouter/service/private/ServiceRegistry.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Service Manager
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/HashMap.hpp"
#include "mtrouter/service/private/ServiceStub.hpp"
#include "mtrouter/service/private/ListServiceProxies.hpp"
#include "areg/base/ArrayList.hpp"

//////////////////////////////////////////////////////////////////////////
// ServiceRegistry class declaration
//////////////////////////////////////////////////////////////////////////
using ServiceRegistryBase = areg::HashMap<ServiceStub, ListServiceProxies>;

/**
 * \brief   The remote services registration map, which is a map of stub and list of connected proxies.
 **/
class ServiceRegistry   : public ServiceRegistryBase
{
//////////////////////////////////////////////////////////////////////////
// Predefined types and constants
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   ServiceRegistry::InvalidProviderService
     *          Defines invalid provider service
     **/
    static const ServiceStub          InvalidProviderService;

    /**
     * \brief   ServiceRegistry::EmptyProxiesList
     *          Defines empty list of remote connected proxies.
     **/
    static const ListServiceProxies   EmptyProxiesList;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    ServiceRegistry() = default;
    ~ServiceRegistry() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if passed stub service is already registered.
     *
     * \param   addrStub    The remote servicing stub object to check.
     **/
    [[nodiscard]]
    bool is_service_registered( const areg::StubAddress & addrStub ) const;

    /**
     * \brief   Returns true if passed proxy address is already registered.
     *
     * \param   addrProxy       The address of proxy to check.
     **/
    [[nodiscard]]
    bool is_service_registered( const areg::ProxyAddress & addrProxy ) const;

    /**
     * \brief   By given remote servicing address finds the address of stub, if there is any, or
     *          returns invalid stub address.
     *
     * \param   addrService     The servicing address to check.
     * \return  Returns valid Stub address if it is registered. Otherwise, returns invalid Stub
     *          address.
     **/
    const ServiceStub & stub_service( const areg::ServiceAddress & addrService ) const;

    /**
     * \brief   By given remote servicing Stub address checks and returns the service connection
     *          status.
     *
     * \param   addrStub    The address of remote Stub service to check the status.
     * \return  If found registered remote Stub address, returns the actual service connection
     *          status. Otherwise, returns unknown service connection status.
     **/
    areg::ServiceConnectionState service_status( const areg::StubAddress & addrStub ) const;

    /**
     * \brief   By given remote servicing Proxy address checks and returns the service connection
     *          status.
     *
     * \param   addrProxy       The address of remote Proxy service to check the status.
     * \return  If found registered remote Stub address as Proxy target, returns the actual service
     *          connection status. Otherwise, returns unknown service connection status.
     **/
    areg::ServiceConnectionState service_status( const areg::ProxyAddress & addrProxy ) const;

    /**
     * \brief   Returns the list of connected proxies address or empty list if no proxy is connected
     *          yet.
     *
     * \param   addrService     The address of remote servicing stub to check.
     * \return  Returns not empty list if there is any proxy requested connection. Otherwise,
     *          returns empty list.
     **/
    const ListServiceProxies & proxy_service_list( const areg::ServiceAddress & addrService ) const;

    /**
     * \brief   By given Proxy address, searches and returns the registered proxy service object.
     *          Returns invalid remote service proxy object if could not find any.
     *
     * \param   addProxy    The address of remote servicing proxy to check.
     * \return  Returns valid servicing proxy object if found entry registered. Otherwise, returns
     *          invalid object.
     **/
    const ServiceProxy & proxy_service( const areg::ProxyAddress & addProxy ) const;

    /**
     * \brief   Registers remote Proxy address and returns the registered Stub and Proxy service
     *          object. On output, the 'out_proxyService' parameter contains the data of remote
     *          proxy service object.
     *
     * \param   addrProxy           The address of remote servicing proxy to register.
     * \param[out] out_proxyService    On output, contains data of remote proxy service.
     * \return  Returns servicing stub object with remote stub address and connection status data.
     **/
    const ServiceStub & register_service_proxy( const areg::ProxyAddress & addrProxy, ServiceProxy & out_proxyService );

    /**
     * \brief   Unregisters remote Proxy address and returns the Stub and Proxy service object. On
     *          output, the 'out_proxyService' parameter contains the data of remote proxy service
     *          object.
     *
     * \param   addrProxy           The address of remote servicing proxy to unregister.
     * \param[out] out_proxyService    On output, contains data of remote proxy service.
     * \return  Returns servicing stub object with remote stub address and connection status data.
     **/
    const ServiceStub & unregister_service_proxy( const areg::ProxyAddress & addrProxy, ServiceProxy & out_proxyService );

    /**
     * \brief   Registers remote Stub address and returns the registered stub service object. On
     *          output, the 'out_listProxies' parameter contains the list of proxies waiting for
     *          Stub registration.
     *
     * \param   addrStub            The address of remote servicing Stub to register.
     * \param[out] out_listProxies     On output, contains list of remote service proxy waiting for
     *                                 connection
     * \return  Returns servicing stub object with remote stub address and connection status data.
     **/
    const ServiceStub & register_service_provider( const areg::StubAddress & addrStub, ListServiceProxies & out_listProxies );

    /**
     * \brief   Unregisters remote Stub address and returns the registered stub service object. On
     *          output, the 'out_listProxies' parameter contains the list of proxies previously
     *          connected to Stub.
     *
     * \param   addrStub            The address of remote servicing Stub to unregister.
     * \param[out] out_listProxies     On output, contains list of remote service proxy previously
     *                                 connected to Stub
     * \return  Returns servicing stub object with remote stub address and connection status data.
     **/
    const ServiceStub & unregister_service_provider( const areg::StubAddress & addrStub, ListServiceProxies & out_listProxies );

    /**
     * \brief   Call to receive list of registered remote stub and proxy services, which connection
     *          cookie is equal to specified value. In output listProviders and listConsumers
     *          contain list of remote stub and proxy addresses.
     *
     * \param   cookie              The cookie to filter. Pass areg::COOKIE_ANY to ignore
     *                              filtering
     * \param[out] listProviders       On output this will contain list of remote stub addresses
     *                                 connected with specified cookie value.
     * \param[out] listConsumers       On output this will contain list of remote proxy addresses
     *                                 connected with specified cookie value.
     **/
    void service_list(const ITEM_ID & cookie, areg::ArrayList<areg::StubAddress> & listProviders, areg::ArrayList<areg::ProxyAddress> & listConsumers ) const;

    /**
     * \brief   Call to get list of registered remote stub and proxy services of specified cookie
     *          source. In output listProviders and listConsumers contain list of remote stub and
     *          proxy addresses.
     *
     * \param   cookie              The cookie to filter. Pass areg::COOKIE_ANY to ignore
     *                              filtering
     * \param[out] listProviders       On output the list contains stub address objects that have
     *                                 sources of specified cookie.
     * \param[out] listConsumers       On output the list contains proxy address objects that have
     *                                 sources of specified cookie.
     **/
    void service_sources(const ITEM_ID & cookie, areg::ArrayList<areg::StubAddress> & listProviders, areg::ArrayList<areg::ProxyAddress> & listConsumers);

    /**
     * \brief   Call to disconnect proxy service specified by proxy address. It sets the connection
     *          state to pending and resets channel data.
     *
     * \param   addrProxy       The address of proxy to disconnect.
     * \return  Returns instance of valid stub service. Otherwise, the service stub is invalid.
     **/
    const ServiceStub & disconnect_proxy( const areg::ProxyAddress & addrProxy );

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Searches the entry of registered servicing by given address of service.
     *
     * \param   addrService     The address of remote service.
     * \return  Returns valid position value if could find an entry matching given servicing
     *          address. Otherwise, returns nullptr.
     **/
    MAPPOS find_service( const areg::ServiceAddress & addrService ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( ServiceRegistry );
};

#endif  // AREG_mtrouter_SERVICE_PRIVATE_SERVICEREGISTRY_HPP
