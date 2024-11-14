/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        mcrouter/service/private/ServiceRegistry.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform Service Manager
 ************************************************************************/
#include "mcrouter/service/private/ServiceRegistry.hpp"

#include "areg/logging/GELog.h"

//////////////////////////////////////////////////////////////////////////
// ServiceRegistry class implementation
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(mcrouter_service_private_ServiceRegistry_registerServiceProxy);
DEF_LOG_SCOPE(mcrouter_service_private_ServiceRegistry_unregisterServiceProxy);
DEF_LOG_SCOPE(mcrouter_service_private_ServiceRegistry_registerServiceStub);
DEF_LOG_SCOPE(mcrouter_service_private_ServiceRegistry_unregisterServiceStub);
DEF_LOG_SCOPE(mcrouter_service_private_ServiceRegistry_getServiceList);
DEF_LOG_SCOPE(mcrouter_service_private_ServiceRegistry_getServiceSources);
DEF_LOG_SCOPE(mcrouter_service_private_ServiceRegistry_disconnectProxy);

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
    MAPPOS pos = findService( addrService );
    return ( isValidPosition(pos) ? keyAtPosition(pos) : ServiceRegistry::InvalidStubService);
}

const ListServiceProxies & ServiceRegistry::getProxyServiceList( const ServiceAddress & addrService ) const
{
    MAPPOS pos = findService( static_cast<const ServiceAddress &>(addrService) );
    return (isValidPosition(pos) ? valueAtPosition(pos) : ServiceRegistry::EmptyProxiesList );
}

const ServiceProxy & ServiceRegistry::getProxyService(const ProxyAddress & addProxy) const
{
    return getProxyServiceList( static_cast<const ServiceAddress &>(addProxy) ).getService(addProxy);
}

NEService::eServiceConnection ServiceRegistry::getServiceStatus(const StubAddress & addrStub) const
{
    return getStubService(addrStub).getServiceStatus();
}

NEService::eServiceConnection ServiceRegistry::getServiceStatus(const ProxyAddress & addrProxy) const
{
    return getProxyService(addrProxy).getServiceStatus();
}

const ServiceStub & ServiceRegistry::registerServiceProxy(const ProxyAddress & addrProxy, ServiceProxy & out_proxyService)
{
    LOG_SCOPE(mcrouter_service_private_ServiceRegistry_registerServiceProxy);
    std::pair<MAPPOS, bool> pos = addIfUnique(ServiceStub(addrProxy), ServiceRegistry::EmptyProxiesList);
    ASSERT(isValidPosition(pos.first));

    const ServiceStub & result = keyAtPosition(pos.first);
    ListServiceProxies& proxies = valueAtPosition(pos.first);
    if ( pos.second )
    {
        LOG_DBG("Proxy [ %s ] registers new entry and wait for service"
                    , ProxyAddress::convAddressToPath(addrProxy).getString());

        out_proxyService = proxies.registerService( addrProxy );
    }
    else
    {
        out_proxyService = proxies.registerService(addrProxy, result);

        LOG_DBG("Proxy [ %s ] is registered for service with status [ %s ]"
                        , ProxyAddress::convAddressToPath(addrProxy).getString()
                        , NEService::getString(result.getServiceStatus()));
    }

    return result;
}

const ServiceStub & ServiceRegistry::unregisterServiceProxy(const ProxyAddress & addrProxy, ServiceProxy & out_proxyService)
{
    LOG_SCOPE(mcrouter_service_private_ServiceRegistry_unregisterServiceProxy);

    MAPPOS pos = findService( static_cast<const ServiceAddress &>(addrProxy) );
    if ( isValidPosition(pos) )
    {
        const ServiceStub & stub = keyAtPosition(pos);
        ListServiceProxies & proxies = valueAtPosition(pos);
        out_proxyService = proxies.unregisterService(addrProxy);
        if ( proxies.isEmpty() && (stub.isValid() == false) )
        {
            LOG_INFO("Proxy [ %s ] is unregistered, remove empty and invalid service entry with status [ %s ]"
                            , ProxyAddress::convAddressToPath(addrProxy).getString()
                            , NEService::getString(stub.getServiceStatus()));

            removePosition(pos);
            pos = invalidPosition(); // should not be the last
        }
        else
        {
            LOG_INFO("Unregistered proxy [ %s ], there are [ %d ] proxies left, service status [ %s ]"
                            , ProxyAddress::convAddressToPath(addrProxy).getString()
                            , proxies.getSize()
                            , NEService::getString(stub.getServiceStatus()));
        }
    }
    else
    {
        LOG_WARN("The proxy [ %s ] is not in registration list, ignore unregistering"
                    , ProxyAddress::convAddressToPath(addrProxy).getString() );
    }

    return (isValidPosition( pos ) ? keyAtPosition( pos ) : ServiceRegistry::InvalidStubService);
}

const ServiceStub & ServiceRegistry::registerServiceStub(const StubAddress & addrStub, ListServiceProxies & out_listProxies)
{
    LOG_SCOPE(mcrouter_service_private_ServiceRegistry_registerServiceStub);

    std::pair<MAPPOS, bool> pos = addIfUnique( ServiceStub(addrStub), ServiceRegistry::EmptyProxiesList);
    ASSERT(isValidPosition(pos.first));
    ServiceStub& result = keyAtPosition(pos.first);
    ListServiceProxies& proxies = valueAtPosition(pos.first);

    if ( pos.second )
    {
        LOG_DBG("Registered new service [ %s ], there are no proxies yet waiting for service"
                    , StubAddress::convAddressToPath(addrStub).getString());

        result.setServiceStatus( NEService::eServiceConnection::ServiceConnected );
        out_listProxies = proxies;
    }
    else
    {
        result.setService( addrStub, NEService::eServiceConnection::ServiceConnected );
        proxies.stubServiceAvailable(addrStub);
        out_listProxies = proxies;

        LOG_DBG("Registered service [ %s ] availability, [ %d ] proxies will be notified"
                    , StubAddress::convAddressToPath(addrStub).getString()
                    , out_listProxies.getSize());
    }

    return result;
}

const ServiceStub & ServiceRegistry::unregisterServiceStub(const StubAddress & addrStub, ListServiceProxies & out_listProxies)
{
    LOG_SCOPE(mcrouter_service_private_ServiceRegistry_unregisterServiceStub);

    MAPPOS pos = findService( static_cast<const ServiceAddress &>(addrStub) );
    if ( isValidPosition(pos) )
    {
        ServiceStub & stub = keyAtPosition(pos);
        ListServiceProxies & proxies = valueAtPosition(pos);

        stub.setServiceStatus( NEService::eServiceConnection::ServicePending );
        proxies.stubServiceUnavailable( );
        if ( proxies.isEmpty() )
        {
            LOG_INFO("Service [ %s ] is unregistered and has no proxies, deleting registry entry"
                        , StubAddress::convAddressToPath(addrStub).getString());

            removePosition(pos);
            pos = invalidPosition();
            out_listProxies.clear();
        }
        else
        {
            out_listProxies = proxies;
            LOG_INFO("Service [ %s ] is unregistered, there are [ %d ] service clients"
                        , StubAddress::convAddressToPath(addrStub).getString()
                        , out_listProxies.getSize());
        }
    }
    else
    {
        LOG_WARN("Service [ %s ] was not in registered list, ignoring"
                    , StubAddress::convAddressToPath(addrStub).getString());
    }

    return (isValidPosition(pos) ? keyAtPosition(pos) : ServiceRegistry::InvalidStubService);
}

ServiceRegistry::MAPPOS ServiceRegistry::findService( const ServiceAddress & addrService ) const
{
    return find(ServiceStub(addrService));
}

void ServiceRegistry::getServiceList(const ITEM_ID & cookie , TEArrayList<StubAddress> & OUT out_stubServiceList, TEArrayList<ProxyAddress> & OUT out_proxyServiceList ) const
{
    LOG_SCOPE(mcrouter_service_private_ServiceRegistry_getServiceList);
    LOG_DBG("Filter service list for cookie [ %u ]", static_cast<unsigned int>(cookie));

    for (ServiceRegistryBase::MAPPOS posMap = firstPosition(); isValidPosition(posMap); posMap = nextPosition(posMap) )
    {
        const ServiceStub & svcStub  = keyAtPosition(posMap);
        const StubAddress & addrStub = svcStub.getServiceAddress();
        const ListServiceProxies & listProxies = valueAtPosition(posMap);

        if ( svcStub.isValid() && ((cookie == NEService::COOKIE_ANY) || (addrStub.getCookie() == cookie)) )
        {
            LOG_INFO("The cookie [ %u ] of service [ %s ] with status [ %s ] match criteria."
                        , static_cast<unsigned int>(addrStub.getCookie())
                        , StubAddress::convAddressToPath(addrStub).getString()
                        , NEService::getString(svcStub.getServiceStatus()));

            out_stubServiceList.add(addrStub);
        }
        else
        {
            LOG_DBG("Ignore stub [ %s ] with cookie [ %u ] and status [ %s ]"
                        , StubAddress::convAddressToPath(addrStub).getString()
                        , static_cast<unsigned int>(addrStub.getCookie())
                        , NEService::getString(svcStub.getServiceStatus()));
        }

        for (ListServiceProxiesBase::LISTPOS posList = listProxies.firstPosition(); listProxies.isValidPosition(posList); posList = listProxies.nextPosition(posList) )
        {
            const ServiceProxy & svcProxy   = listProxies.valueAtPosition(posList);
            const ProxyAddress & addrProxy  = svcProxy.getServiceAddress();
            if ( svcProxy.isValid() && ((cookie == NEService::COOKIE_ANY) || (addrProxy.getCookie() == cookie)) )
            {
                LOG_INFO("The cookie [ %u ] of proxy [ %s ] with status [ %s ] match criteria."
                            , static_cast<unsigned int>(addrProxy.getCookie())
                            , ProxyAddress::convAddressToPath(addrProxy).getString()
                            , NEService::getString(svcProxy.getServiceStatus()));

                out_proxyServiceList.add(addrProxy);
            }
            else
            {
                LOG_DBG("Ignore proxy [ %s ] with cookie [ %u ] and status [ %s ]"
                            , ProxyAddress::convAddressToPath(addrProxy).getString()
                            , static_cast<unsigned int>(addrProxy.getCookie())
                            , NEService::getString(svcProxy.getServiceStatus()));
            }
        }
    }
}

void ServiceRegistry::getServiceSources(const ITEM_ID & cookie, TEArrayList<StubAddress> & OUT stubSource, TEArrayList<ProxyAddress> & OUT proxySources)
{
    LOG_SCOPE(mcrouter_service_private_ServiceRegistry_getServiceSources);
    LOG_DBG("Pickup services with [ %u ] sources ", static_cast<unsigned int>(cookie));

    for (ServiceRegistry::MAPPOS posMap = firstPosition(); isValidPosition(posMap); posMap = nextPosition(posMap) )
    {
        const ServiceStub & svcStub  = keyAtPosition(posMap);
        const StubAddress & addrStub = svcStub.getServiceAddress();
        const ListServiceProxies & listProxies = valueAtPosition(posMap);

        if (svcStub.isValid() && (cookie == addrStub.getSource()))
        {
            LOG_INFO("Found stub [ %s ] of source [ %u ]", addrStub.convToString().getString(), static_cast<unsigned int>(cookie));
            stubSource.add(addrStub);
        }
        else
        {
            LOG_DBG("Ignore stub [ %s ], it is either invalid or different source than [ %u ]"
                        , addrStub.convToString().getString()
                        , static_cast<unsigned int>(cookie));
        }

        for (ListServiceProxiesBase::LISTPOS posList = listProxies.firstPosition(); listProxies.isValidPosition(posList); posList = listProxies.nextPosition(posList) )
        {
            const ServiceProxy & svcProxy   = listProxies.valueAtPosition(posList);
            const ProxyAddress & addrProxy  = svcProxy.getServiceAddress();

            if (svcProxy.isValid() && (cookie == addrProxy.getSource()))
            {
                LOG_INFO("Found proxy [ %s ] of source [ %u ]", addrProxy.convToString().getString(), static_cast<unsigned int>(cookie));
                proxySources.add(addrProxy);
            }
            else
            {
                LOG_DBG("Ignore proxy [ %s ], it is either invalid or has different source than [ %u ]", addrProxy.convToString().getString(), static_cast<unsigned int>(cookie));
            }
        }
    }
}

const ServiceStub & ServiceRegistry::disconnectProxy(const ProxyAddress & IN addrProxy)
{
    LOG_SCOPE(mcrouter_service_private_ServiceRegistry_disconnectProxy);
    MAPPOS pos = findService( static_cast<const ServiceAddress &>(addrProxy) );
    if ( isValidPosition(pos) )
    {
        ListServiceProxies & proxies = valueAtPosition(pos);
        ServiceProxy * svcProxy = proxies.getService(addrProxy);
        if ((svcProxy != nullptr) && svcProxy->isValid())
        {
            LOG_INFO("Found service of proxy [ %s ] to disconnect, current state [ %s ]", addrProxy.convToString().getString(), NEService::getString(svcProxy->getServiceStatus()));
            svcProxy->stubUnavailable();
        }
        else
        {
            LOG_DBG("Nor service of proxy [ %s ] found, ignore disconnect", addrProxy.convToString().getString());
        }
    }
    else
    {
        LOG_WARN("No proxy [ %s ] is in registration list, ignore disconnect", addrProxy.convToString().getString() );
    }

    return ( isValidPosition(pos) ? keyAtPosition(pos) : ServiceRegistry::InvalidStubService);
}
