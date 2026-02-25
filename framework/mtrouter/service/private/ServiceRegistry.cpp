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

#include "areg/logging/GELog.h"

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

bool ServiceRegistry::isServiceRegistered(const StubAddress & addrStub) const
{
    return contains(ServiceStub(addrStub));
}

bool ServiceRegistry::isServiceRegistered(const ProxyAddress & addrProxy) const
{
    return getProxyServiceList( static_cast<const ServiceAddress &>(addrProxy) ).isServiceRegistered(addrProxy);
}

const ServiceStub & ServiceRegistry::getStubService( const ServiceAddress & addrService ) const
{
    MAPPOS pos = find_service( addrService );
    return ( is_valid_position(pos) ? key_at_position(pos) : ServiceRegistry::InvalidStubService);
}

const ListServiceProxies & ServiceRegistry::getProxyServiceList( const ServiceAddress & addrService ) const
{
    MAPPOS pos = find_service( static_cast<const ServiceAddress &>(addrService) );
    return (is_valid_position(pos) ? value_at_position(pos) : ServiceRegistry::EmptyProxiesList );
}

const ServiceProxy & ServiceRegistry::getProxyService(const ProxyAddress & addProxy) const
{
    return getProxyServiceList( static_cast<const ServiceAddress &>(addProxy) ).service(addProxy);
}

NEService::ServiceConnectionState ServiceRegistry::getServiceStatus(const StubAddress & addrStub) const
{
    return getStubService(addrStub).getServiceStatus();
}

NEService::ServiceConnectionState ServiceRegistry::getServiceStatus(const ProxyAddress & addrProxy) const
{
    return getProxyService(addrProxy).getServiceStatus();
}

const ServiceStub & ServiceRegistry::registerServiceProxy(const ProxyAddress & addrProxy, ServiceProxy & out_proxyService)
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_registerServiceProxy);
    std::pair<MAPPOS, bool> pos = add_if_unique(ServiceStub(addrProxy), ServiceRegistry::EmptyProxiesList);
    ASSERT(is_valid_position(pos.first));

    const ServiceStub & result = key_at_position(pos.first);
    ListServiceProxies& proxies = value_at_position(pos.first);
    if ( pos.second )
    {
        LOG_DBG("Proxy [ %s ] registers new entry and wait for service"
                    , ProxyAddress::to_path(addrProxy).as_string());

        out_proxyService = proxies.registerService( addrProxy );
    }
    else
    {
        out_proxyService = proxies.registerService(addrProxy, result);

        LOG_DBG("Proxy [ %s ] is registered for service with status [ %s ]"
                        , ProxyAddress::to_path(addrProxy).as_string()
                        , NEService::as_string(result.getServiceStatus()));
    }

    return result;
}

const ServiceStub & ServiceRegistry::unregisterServiceProxy(const ProxyAddress & addrProxy, ServiceProxy & out_proxyService)
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_unregisterServiceProxy);

    MAPPOS pos = find_service( static_cast<const ServiceAddress &>(addrProxy) );
    if ( is_valid_position(pos) )
    {
        const ServiceStub & stub = key_at_position(pos);
        ListServiceProxies & proxies = value_at_position(pos);
        out_proxyService = proxies.unregisterService(addrProxy);
        if ( proxies.is_empty() && (stub.is_valid() == false) )
        {
            LOG_INFO("Proxy [ %s ] is unregistered, remove empty and invalid service entry with status [ %s ]"
                            , ProxyAddress::to_path(addrProxy).as_string()
                            , NEService::as_string(stub.getServiceStatus()));

            remove_position(pos);
            pos = invalid_position(); // should not be the last
        }
        else
        {
            LOG_INFO("Unregistered proxy [ %s ], there are [ %d ] proxies left, service status [ %s ]"
                            , ProxyAddress::to_path(addrProxy).as_string()
                            , proxies.size()
                            , NEService::as_string(stub.getServiceStatus()));
        }
    }
    else
    {
        LOG_WARN("The proxy [ %s ] is not in registration list, ignore unregistering"
                    , ProxyAddress::to_path(addrProxy).as_string() );
    }

    return (is_valid_position( pos ) ? key_at_position( pos ) : ServiceRegistry::InvalidStubService);
}

const ServiceStub & ServiceRegistry::registerServiceStub(const StubAddress & addrStub, ListServiceProxies & out_listProxies)
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_registerServiceStub);

    std::pair<MAPPOS, bool> pos = add_if_unique( ServiceStub(addrStub), ServiceRegistry::EmptyProxiesList);
    ASSERT(is_valid_position(pos.first));
    ServiceStub& result = key_at_position(pos.first);
    ListServiceProxies& proxies = value_at_position(pos.first);

    if ( pos.second )
    {
        LOG_DBG("Registered new service [ %s ], there are no proxies yet waiting for service"
                    , StubAddress::to_path(addrStub).as_string());

        result.setServiceStatus( NEService::ServiceConnectionState::Connected );
        out_listProxies = proxies;
    }
    else
    {
        result.setService( addrStub, NEService::ServiceConnectionState::Connected );
        proxies.stubServiceAvailable(addrStub);
        out_listProxies = proxies;

        LOG_DBG("Registered service [ %s ] availability, [ %d ] proxies will be notified"
                    , StubAddress::to_path(addrStub).as_string()
                    , out_listProxies.size());
    }

    return result;
}

const ServiceStub & ServiceRegistry::unregisterServiceStub(const StubAddress & addrStub, ListServiceProxies & out_listProxies)
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_unregisterServiceStub);

    MAPPOS pos = find_service( static_cast<const ServiceAddress &>(addrStub) );
    if ( is_valid_position(pos) )
    {
        ServiceStub & stub = key_at_position(pos);
        ListServiceProxies & proxies = value_at_position(pos);

        stub.setServiceStatus( NEService::ServiceConnectionState::Pending );
        proxies.stubServiceUnavailable( );
        if ( proxies.is_empty() )
        {
            LOG_INFO("Service [ %s ] is unregistered and has no proxies, deleting registry entry"
                        , StubAddress::to_path(addrStub).as_string());

            remove_position(pos);
            pos = invalid_position();
            out_listProxies.clear();
        }
        else
        {
            out_listProxies = proxies;
            LOG_INFO("Service [ %s ] is unregistered, there are [ %d ] service clients"
                        , StubAddress::to_path(addrStub).as_string()
                        , out_listProxies.size());
        }
    }
    else
    {
        LOG_WARN("Service [ %s ] was not in registered list, ignoring"
                    , StubAddress::to_path(addrStub).as_string());
    }

    return (is_valid_position(pos) ? key_at_position(pos) : ServiceRegistry::InvalidStubService);
}

ServiceRegistry::MAPPOS ServiceRegistry::find_service( const ServiceAddress & addrService ) const
{
    return find(ServiceStub(addrService));
}

void ServiceRegistry::service_list(const ITEM_ID & cookie , ArrayList<StubAddress> & listProviders, ArrayList<ProxyAddress> & listConsumers ) const
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_getServiceList);
    LOG_DBG("Filter service list for cookie [ %u ]", static_cast<uint32_t>(cookie));

    for (ServiceRegistryBase::MAPPOS posMap = first_position(); is_valid_position(posMap); posMap = next_position(posMap) )
    {
        const ServiceStub & svcStub  = key_at_position(posMap);
        const StubAddress & addrStub = svcStub.service_address();
        const ListServiceProxies & listProxies = value_at_position(posMap);

        if ( svcStub.is_valid() && ((cookie == NEService::COOKIE_ANY) || (addrStub.cookie() == cookie)) )
        {
            LOG_INFO("The cookie [ %u ] of service [ %s ] with status [ %s ] match criteria."
                        , static_cast<uint32_t>(addrStub.cookie())
                        , StubAddress::to_path(addrStub).as_string()
                        , NEService::as_string(svcStub.getServiceStatus()));

            listProviders.add(addrStub);
        }
        else
        {
            LOG_DBG("Ignore stub [ %s ] with cookie [ %u ] and status [ %s ]"
                        , StubAddress::to_path(addrStub).as_string()
                        , static_cast<uint32_t>(addrStub.cookie())
                        , NEService::as_string(svcStub.getServiceStatus()));
        }

        for (ListServiceProxiesBase::LISTPOS posList = listProxies.first_position(); listProxies.is_valid_position(posList); posList = listProxies.next_position(posList) )
        {
            const ServiceProxy & svcProxy   = listProxies.value_at_position(posList);
            const ProxyAddress & addrProxy  = svcProxy.service_address();
            if ( svcProxy.is_valid() && ((cookie == NEService::COOKIE_ANY) || (addrProxy.cookie() == cookie)) )
            {
                LOG_INFO("The cookie [ %u ] of proxy [ %s ] with status [ %s ] match criteria."
                            , static_cast<uint32_t>(addrProxy.cookie())
                            , ProxyAddress::to_path(addrProxy).as_string()
                            , NEService::as_string(svcProxy.getServiceStatus()));

                listConsumers.add(addrProxy);
            }
            else
            {
                LOG_DBG("Ignore proxy [ %s ] with cookie [ %u ] and status [ %s ]"
                            , ProxyAddress::to_path(addrProxy).as_string()
                            , static_cast<uint32_t>(addrProxy.cookie())
                            , NEService::as_string(svcProxy.getServiceStatus()));
            }
        }
    }
}

void ServiceRegistry::getServiceSources(const ITEM_ID & cookie, ArrayList<StubAddress> & stubSource, ArrayList<ProxyAddress> & proxySources)
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_getServiceSources);
    LOG_DBG("Pickup services with [ %u ] sources ", static_cast<uint32_t>(cookie));

    for (ServiceRegistry::MAPPOS posMap = first_position(); is_valid_position(posMap); posMap = next_position(posMap) )
    {
        const ServiceStub & svcStub  = key_at_position(posMap);
        const StubAddress & addrStub = svcStub.service_address();
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
            const ProxyAddress & addrProxy  = svcProxy.service_address();

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

const ServiceStub & ServiceRegistry::disconnectProxy(const ProxyAddress & addrProxy)
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_disconnectProxy);
    MAPPOS pos = find_service( static_cast<const ServiceAddress &>(addrProxy) );
    if ( is_valid_position(pos) )
    {
        ListServiceProxies & proxies = value_at_position(pos);
        ServiceProxy * svcProxy = proxies.service(addrProxy);
        if ((svcProxy != nullptr) && svcProxy->is_valid())
        {
            LOG_INFO("Found service of proxy [ %s ] to disconnect, current state [ %s ]", addrProxy.to_string().as_string(), NEService::as_string(svcProxy->getServiceStatus()));
            svcProxy->stubUnavailable();
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
