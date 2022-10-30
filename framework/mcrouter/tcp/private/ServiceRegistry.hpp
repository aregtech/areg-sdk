#ifndef AREG_MCROUTER_TCP_PRIVATE_SERVICEREGISTRY_HPP
#define AREG_MCROUTER_TCP_PRIVATE_SERVICEREGISTRY_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        mcrouter/tcp/private/ServiceRegistry.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Service Manager
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TEHashMap.hpp"
#include "mcrouter/tcp/private/ServiceStub.hpp"
#include "mcrouter/tcp/private/ListServiceProxies.hpp"
#include "areg/base/TEArrayList.hpp"

//////////////////////////////////////////////////////////////////////////
// ServiceRegistry class declaration
//////////////////////////////////////////////////////////////////////////
using ServiceRegistryBase = TEHashMap<ServiceStub, ListServiceProxies>;

/**
 * \brief   The remote services registration map, which is a map of stub and list of connected proxies.
 **/
class ServiceRegistry   : public ServiceRegistryBase
{
//////////////////////////////////////////////////////////////////////////
// Predefined types and constants
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   ServiceRegistry::InvalidStubService
     *          Defines invalid stub service
     **/
    static const ServiceStub          InvalidStubService;

    /**
     * \brief   ServiceRegistry::EmptyProxiesList
     *          Defines empty list of remote connected proxies.
     **/
    static const ListServiceProxies   EmptyProxiesList;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    ServiceRegistry( void ) = default;
    /**
     * \brief   Destructor
     **/
    ~ServiceRegistry( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if passed stub service is already registered.
     * \param   addrStub    The remote servicing stub object to check.
     **/
    bool isServiceRegistered( const StubAddress & addrStub ) const;

    /**
     * \brief   Returns true if passed proxy address is already registered.
     * \param   addrProxy   The address of proxy to check.
     **/
    bool isServiceRegistered( const ProxyAddress & addrProxy ) const;

    /**
     * \brief   By given remote servicing address finds the address of stub,
     *          if there is any, or returns invalid stub address.
     * \param   addrService     The servicing address to check.
     * \return  Returns valid Stub address if it is registered. Otherwise, returns invalid Stub address.
     **/
    const ServiceStub & getStubService( const ServiceAddress & addrService ) const;

    /**
     * \brief   By given remote servicing Stub address checks and returns the service connection status.
     * \param   addrStub        The address of remote Stub service to check the status.
     * \return  If found registered remote Stub address, returns the actual service connection status.
     *          Otherwise, returns unknown service connection status.
     **/
    NEService::eServiceConnection getServiceStatus( const StubAddress & addrStub ) const;

    /**
     * \brief   By given remote servicing Proxy address checks and returns the service connection status.
     * \param   addrProxy       The address of remote Proxy service to check the status.
     * \return  If found registered remote Stub address as Proxy target, returns the actual service connection status.
     *          Otherwise, returns unknown service connection status.
     **/
    NEService::eServiceConnection getServiceStatus( const ProxyAddress & addrProxy ) const;

    /**
     * \brief   Returns the list of connected proxies address or empty list if no proxy is connected yet.
     * \param   addrService     The address of remote servicing stub to check.
     * \param   Returns not empty list if there is any proxy requested connection.
     *          Otherwise, returns empty list.
     **/
    const ListServiceProxies & getProxyServiceList( const ServiceAddress & addrService ) const;

    /**
     * \brief   By given Proxy address, searches and returns the registered proxy service object.
     *          Returns invalid remote service proxy object if could not find any.
     * \param   addProxy     The address of remote servicing proxy to check.
     * \return  Returns valid servicing proxy object if found entry registered.
     *          Otherwise, returns invalid object.
     **/
    const ServiceProxy & getProxyService( const ProxyAddress & addProxy ) const;

    /**
     * \brief   Registers remote Proxy address and returns the registered Stub and Proxy service object.
     *          On output, the 'out_proxyService' parameter contains the data of remote proxy service object.
     * \param   addProxy            The address of remote servicing proxy to register.
     * \param   out_proxyService    On output, contains data of remote proxy service.
     * \param   Returns servicing stub object with remote stub address and connection status data.
     **/
    const ServiceStub & registerServiceProxy( const ProxyAddress & addrProxy, ServiceProxy & out_proxyService );

    /**
     * \brief   Unregisters remote Proxy address and returns the Stub and Proxy service object.
     *          On output, the 'out_proxyService' parameter contains the data of remote proxy service object.
     * \param   addProxy            The address of remote servicing proxy to register.
     * \param   out_proxyService    On output, contains data of remote proxy service.
     * \param   Returns servicing stub object with remote stub address and connection status data.
     **/
    const ServiceStub & unregisterServiceProxy( const ProxyAddress & addrProxy, ServiceProxy & out_proxyService );

    /**
     * \brief   Registers remote Stub address and returns the registered stub service object.
     *          On output, the 'out_listProxies' parameter contains the list of proxies waiting for Stub registration
     * \param   addrStub            The address of remote servicing Stub to register.
     * \param   out_listProxies     On output, contains list of remote service proxy waiting for connection
     * \param   Returns servicing stub object with remote stub address and connection status data.
     **/
    const ServiceStub & registerServiceStub( const StubAddress & addrStub, ListServiceProxies & out_listProxies );

    /**
     * \brief   Unregisters remote Stub address and returns the registered stub service object.
     *          On output, the 'out_listProxies' parameter contains the list of proxies previously connected to Stub
     * \param   addrStub            The address of remote servicing Stub to register.
     * \param   out_listProxies     On output, contains list of remote service proxy previously connected to Stub
     * \param   Returns servicing stub object with remote stub address and connection status data.
     **/
    const ServiceStub & unregisterServiceStub( const StubAddress & addrStub, ListServiceProxies & out_listProxies );

    /**
     * \brief   Call to receive list of registered remote stub and proxy services, which connection cookie is equal to 
     *          specified value. In output out_listStubs and out_lisProxies contain list of remote stub and proxy addresses.
     * \param   cookie          The cookie to filter. Pass NEService::COOKIE_ANY to ignore filtering
     * \param   out_listStubs   On output this will contain list of remote stub addresses connected with specified cookie value.
     * \param   out_lisProxies  On output this will contain list of remote proxy addresses connected with specified cookie value.
     **/
    void getServiceList( ITEM_ID cookie, TEArrayList<StubAddress> & OUT out_listStubs, TEArrayList<ProxyAddress> & OUT out_lisProxies ) const;

    /**
     * \brief   Call to get list of registered remote stub and proxy services of specified cookie source.
     *          In output out_listStubs and out_lisProxies contain list of remote stub and proxy addresses.
     * \param   cookie          The cookie to filter. Pass NEService::COOKIE_ANY to ignore filtering
     * \param   stubSource      On output the list contains stub address objects that have sources of specified cookie.
     * \param   proxySources    On output the list contains proxy address objects that have sources of specified cookie.
     **/
    void getServiceSources( ITEM_ID cookie, TEArrayList<StubAddress> & OUT stubSource, TEArrayList<ProxyAddress> & OUT proxySources);

    /**
     * \brief   Call to disconnect proxy service specified by proxy address.
     *          It sets the connection state to pending and resets channel data.
     * \param   addrProxy   The address of proxy to disconnect.
     * \return  Returns instance of valid stub service. Otherwise, the service stub is invalid.
     **/
    const ServiceStub & disconnectProxy( const ProxyAddress & IN addrProxy );

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Searches the entry of registered servicing by given address of service.
     * \param   addrService     The address of remote service.
     * \return  Returns valid position value if could find an entry matching given servicing address.
     *          Otherwise, returns nullptr.
     **/
    MAPPOS findService( const ServiceAddress & addrService ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( ServiceRegistry );
};

#endif  // AREG_MCROUTER_TCP_PRIVATE_SERVICEREGISTRY_HPP
