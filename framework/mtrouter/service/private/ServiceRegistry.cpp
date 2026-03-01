/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        mtrouter/service/private/ServiceRegistry.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Service Manager
 ************************************************************************/
#include "mtrouter/service/private/ServiceRegistry.hpp"

#include "areg/logging/areg_log.h"

//////////////////////////////////////////////////////////////////////////
// ServiceRegistry class implementation
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(mtrouter_service_private_ServiceRegistry_registerServiceProxy);
DEF_LOG_SCOPE(mtrouter_service_private_ServiceRegistry_unregisterServiceProxy);
DEF_LOG_SCOPE(mtrouter_service_private_ServiceRegistry_registerServiceStub);
DEF_LOG_SCOPE(mtrouter_service_private_ServiceRegistry_unregisterServiceStub);
DEF_LOG_SCOPE(mtrouter_service_private_ServiceRegistry_getServiceList);
DEF_LOG_SCOPE(mtrouter_service_private_ServiceRegistry_getServiceSources);
DEF_LOG_SCOPE(mtrouter_service_private_ServiceRegistry_disconnectProxy);

//////////////////////////////////////////////////////////////////////////
// ServiceRegistry statics
//////////////////////////////////////////////////////////////////////////

const ServiceStub         ServiceRegistry::InvalidStubService;

const ListServiceProxies  ServiceRegistry::EmptyProxiesList;

//////////////////////////////////////////////////////////////////////////
// ServiceRegistry class methods
//////////////////////////////////////////////////////////////////////////

bool ServiceRegistry::is_service_registered(const areg::StubAddress & addrStub) const
{
    return contains(ServiceStub(addrStub));
}

bool ServiceRegistry::is_service_registered(const areg::ProxyAddress & addrProxy) const
{
    return proxy_service_list( static_cast<const areg::ServiceAddress &>(addrProxy) ).is_service_registered(addrProxy);
}

const ServiceStub & ServiceRegistry::stub_service( const areg::ServiceAddress & addrService ) const
{
    MAPPOS pos = find_service( addrService );
    return ( is_valid_position(pos) ? key_at_position(pos) : ServiceRegistry::InvalidStubService);
}

const ListServiceProxies & ServiceRegistry::proxy_service_list( const areg::ServiceAddress & addrService ) const
{
    MAPPOS pos = find_service( static_cast<const areg::ServiceAddress &>(addrService) );
    return (is_valid_position(pos) ? value_at_position(pos) : ServiceRegistry::EmptyProxiesList );
}

const ServiceProxy & ServiceRegistry::proxy_service(const areg::ProxyAddress & addProxy) const
{
    return proxy_service_list( static_cast<const areg::ServiceAddress &>(addProxy) ).service(addProxy);
}

areg::ServiceConnectionState ServiceRegistry::service_status(const areg::StubAddress & addrStub) const
{
    return stub_service(addrStub).service_status();
}

areg::ServiceConnectionState ServiceRegistry::service_status(const areg::ProxyAddress & addrProxy) const
{
    return proxy_service(addrProxy).service_status();
}

const ServiceStub & ServiceRegistry::register_service_proxy(const areg::ProxyAddress & addrProxy, ServiceProxy & out_proxyService)
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_registerServiceProxy);
    std::pair<MAPPOS, bool> pos = add_if_unique(ServiceStub(addrProxy), ServiceRegistry::EmptyProxiesList);
    ASSERT(is_valid_position(pos.first));

    const ServiceStub & result = key_at_position(pos.first);
    ListServiceProxies& proxies = value_at_position(pos.first);
    if ( pos.second )
    {
        LOG_DBG("Proxy [ %s ] registers new entry and wait for service"
                    , areg::ProxyAddress::to_path(addrProxy).as_string());

        out_proxyService = proxies.register_service( addrProxy );
    }
    else
    {
        out_proxyService = proxies.register_service(addrProxy, result);

        LOG_DBG("Proxy [ %s ] is registered for service with status [ %s ]"
                        , areg::ProxyAddress::to_path(addrProxy).as_string()
                        , areg::as_string(result.service_status()));
    }

    return result;
}

const ServiceStub & ServiceRegistry::unregister_service_proxy(const areg::ProxyAddress & addrProxy, ServiceProxy & out_proxyService)
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_unregisterServiceProxy);

    MAPPOS pos = find_service( static_cast<const areg::ServiceAddress &>(addrProxy) );
    if ( is_valid_position(pos) )
    {
        const ServiceStub & stub = key_at_position(pos);
        ListServiceProxies & proxies = value_at_position(pos);
        out_proxyService = proxies.unregister_service(addrProxy);
        if ( proxies.is_empty() && (stub.is_valid() == false) )
        {
            LOG_INFO("Proxy [ %s ] is unregistered, remove empty and invalid service entry with status [ %s ]"
                            , areg::ProxyAddress::to_path(addrProxy).as_string()
                            , areg::as_string(stub.service_status()));

            remove_position(pos);
            pos = invalid_position(); // should not be the last
        }
        else
        {
            LOG_INFO("Unregistered proxy [ %s ], there are [ %d ] proxies left, service status [ %s ]"
                            , areg::ProxyAddress::to_path(addrProxy).as_string()
                            , proxies.size()
                            , areg::as_string(stub.service_status()));
        }
    }
    else
    {
        LOG_WARN("The proxy [ %s ] is not in registration list, ignore unregistering"
                    , areg::ProxyAddress::to_path(addrProxy).as_string() );
    }

    return (is_valid_position( pos ) ? key_at_position( pos ) : ServiceRegistry::InvalidStubService);
}

const ServiceStub & ServiceRegistry::register_service_stub(const areg::StubAddress & addrStub, ListServiceProxies & out_listProxies)
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_registerServiceStub);

    std::pair<MAPPOS, bool> pos = add_if_unique( ServiceStub(addrStub), ServiceRegistry::EmptyProxiesList);
    ASSERT(is_valid_position(pos.first));
    ServiceStub& result = key_at_position(pos.first);
    ListServiceProxies& proxies = value_at_position(pos.first);

    if ( pos.second )
    {
        LOG_DBG("Registered new service [ %s ], there are no proxies yet waiting for service"
                    , areg::StubAddress::to_path(addrStub).as_string());

        result.set_service_status( areg::ServiceConnectionState::Connected );
        out_listProxies = proxies;
    }
    else
    {
        result.set_service( addrStub, areg::ServiceConnectionState::Connected );
        proxies.stub_service_available(addrStub);
        out_listProxies = proxies;

        LOG_DBG("Registered service [ %s ] availability, [ %d ] proxies will be notified"
                    , areg::StubAddress::to_path(addrStub).as_string()
                    , out_listProxies.size());
    }

    return result;
}

const ServiceStub & ServiceRegistry::unregister_service_stub(const areg::StubAddress & addrStub, ListServiceProxies & out_listProxies)
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_unregisterServiceStub);

    MAPPOS pos = find_service( static_cast<const areg::ServiceAddress &>(addrStub) );
    if ( is_valid_position(pos) )
    {
        ServiceStub & stub = key_at_position(pos);
        ListServiceProxies & proxies = value_at_position(pos);

        stub.set_service_status( areg::ServiceConnectionState::Pending );
        proxies.stub_service_unavailable( );
        if ( proxies.is_empty() )
        {
            LOG_INFO("Service [ %s ] is unregistered and has no proxies, deleting registry entry"
                        , areg::StubAddress::to_path(addrStub).as_string());

            remove_position(pos);
            pos = invalid_position();
            out_listProxies.clear();
        }
        else
        {
            out_listProxies = proxies;
            LOG_INFO("Service [ %s ] is unregistered, there are [ %d ] service clients"
                        , areg::StubAddress::to_path(addrStub).as_string()
                        , out_listProxies.size());
        }
    }
    else
    {
        LOG_WARN("Service [ %s ] was not in registered list, ignoring"
                    , areg::StubAddress::to_path(addrStub).as_string());
    }

    return (is_valid_position(pos) ? key_at_position(pos) : ServiceRegistry::InvalidStubService);
}

ServiceRegistry::MAPPOS ServiceRegistry::find_service( const areg::ServiceAddress & addrService ) const
{
    return find(ServiceStub(addrService));
}

void ServiceRegistry::service_list(const ITEM_ID & cookie , areg::ArrayList<areg::StubAddress> & listProviders, areg::ArrayList<areg::ProxyAddress> & listConsumers ) const
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_getServiceList);
    LOG_DBG("Filter service list for cookie [ %u ]", static_cast<uint32_t>(cookie));

    for (ServiceRegistryBase::MAPPOS posMap = first_position(); is_valid_position(posMap); posMap = next_position(posMap) )
    {
        const ServiceStub & svcStub  = key_at_position(posMap);
        const areg::StubAddress & addrStub = svcStub.service_address();
        const ListServiceProxies & listProxies = value_at_position(posMap);

        if ( svcStub.is_valid() && ((cookie == areg::COOKIE_ANY) || (addrStub.cookie() == cookie)) )
        {
            LOG_INFO("The cookie [ %u ] of service [ %s ] with status [ %s ] match criteria."
                        , static_cast<uint32_t>(addrStub.cookie())
                        , areg::StubAddress::to_path(addrStub).as_string()
                        , areg::as_string(svcStub.service_status()));

            listProviders.add(addrStub);
        }
        else
        {
            LOG_DBG("Ignore stub [ %s ] with cookie [ %u ] and status [ %s ]"
                        , areg::StubAddress::to_path(addrStub).as_string()
                        , static_cast<uint32_t>(addrStub.cookie())
                        , areg::as_string(svcStub.service_status()));
        }

        for (ListServiceProxiesBase::LISTPOS posList = listProxies.first_position(); listProxies.is_valid_position(posList); posList = listProxies.next_position(posList) )
        {
            const ServiceProxy & svcProxy   = listProxies.value_at_position(posList);
            const areg::ProxyAddress & addrProxy  = svcProxy.service_address();
            if ( svcProxy.is_valid() && ((cookie == areg::COOKIE_ANY) || (addrProxy.cookie() == cookie)) )
            {
                LOG_INFO("The cookie [ %u ] of proxy [ %s ] with status [ %s ] match criteria."
                            , static_cast<uint32_t>(addrProxy.cookie())
                            , areg::ProxyAddress::to_path(addrProxy).as_string()
                            , areg::as_string(svcProxy.service_status()));

                listConsumers.add(addrProxy);
            }
            else
            {
                LOG_DBG("Ignore proxy [ %s ] with cookie [ %u ] and status [ %s ]"
                            , areg::ProxyAddress::to_path(addrProxy).as_string()
                            , static_cast<uint32_t>(addrProxy.cookie())
                            , areg::as_string(svcProxy.service_status()));
            }
        }
    }
}

void ServiceRegistry::service_sources(const ITEM_ID & cookie, areg::ArrayList<areg::StubAddress> & stubSource, areg::ArrayList<areg::ProxyAddress> & proxySources)
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_getServiceSources);
    LOG_DBG("Pickup services with [ %u ] sources ", static_cast<uint32_t>(cookie));

    for (ServiceRegistry::MAPPOS posMap = first_position(); is_valid_position(posMap); posMap = next_position(posMap) )
    {
        const ServiceStub & svcStub  = key_at_position(posMap);
        const areg::StubAddress & addrStub = svcStub.service_address();
        const ListServiceProxies & listProxies = value_at_position(posMap);

        if (svcStub.is_valid() && (cookie == addrStub.source()))
        {
            LOG_INFO("Found stub [ %s ] of source [ %u ]", addrStub.to_string().as_string(), static_cast<uint32_t>(cookie));
            stubSource.add(addrStub);
        }
        else
        {
            LOG_DBG("Ignore stub [ %s ], it is either invalid or different source than [ %u ]"
                        , addrStub.to_string().as_string()
                        , static_cast<uint32_t>(cookie));
        }

        for (ListServiceProxiesBase::LISTPOS posList = listProxies.first_position(); listProxies.is_valid_position(posList); posList = listProxies.next_position(posList) )
        {
            const ServiceProxy & svcProxy   = listProxies.value_at_position(posList);
            const areg::ProxyAddress & addrProxy  = svcProxy.service_address();

            if (svcProxy.is_valid() && (cookie == addrProxy.source()))
            {
                LOG_INFO("Found proxy [ %s ] of source [ %u ]", addrProxy.to_string().as_string(), static_cast<uint32_t>(cookie));
                proxySources.add(addrProxy);
            }
            else
            {
                LOG_DBG("Ignore proxy [ %s ], it is either invalid or has different source than [ %u ]", addrProxy.to_string().as_string(), static_cast<uint32_t>(cookie));
            }
        }
    }
}

const ServiceStub & ServiceRegistry::disconnect_proxy(const areg::ProxyAddress & addrProxy)
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_disconnectProxy);
    MAPPOS pos = find_service( static_cast<const areg::ServiceAddress &>(addrProxy) );
    if ( is_valid_position(pos) )
    {
        ListServiceProxies & proxies = value_at_position(pos);
        ServiceProxy * svcProxy = proxies.service(addrProxy);
        if ((svcProxy != nullptr) && svcProxy->is_valid())
        {
            LOG_INFO("Found service of proxy [ %s ] to disconnect, current state [ %s ]", addrProxy.to_string().as_string(), areg::as_string(svcProxy->service_status()));
            svcProxy->stub_unavailable();
        }
        else
        {
            LOG_DBG("Nor service of proxy [ %s ] found, ignore disconnect", addrProxy.to_string().as_string());
        }
    }
    else
    {
        LOG_WARN("No proxy [ %s ] is in registration list, ignore disconnect", addrProxy.to_string().as_string() );
    }

    return ( is_valid_position(pos) ? key_at_position(pos) : ServiceRegistry::InvalidStubService);
}
