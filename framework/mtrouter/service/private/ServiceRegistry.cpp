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

const mtrouter::ServiceStub         ServiceRegistry::InvalidStubService;

const ListServiceProxies  ServiceRegistry::EmptyProxiesList;

//////////////////////////////////////////////////////////////////////////
// ServiceRegistry class methods
//////////////////////////////////////////////////////////////////////////

bool ServiceRegistry::isServiceRegistered(const areg::StubAddress & addrStub) const
{
    return contains(mtrouter::ServiceStub(addrStub));
}

bool ServiceRegistry::isServiceRegistered(const areg::ProxyAddress & addrProxy) const
{
    return getProxyServiceList( static_cast<const areg::ServiceAddress &>(addrProxy) ).isServiceRegistered(addrProxy);
}

const mtrouter::ServiceStub & ServiceRegistry::getStubService( const areg::ServiceAddress & addrService ) const
{
    MAPPOS pos = findService( addrService );
    return ( isValidPosition(pos) ? keyAtPosition(pos) : ServiceRegistry::InvalidStubService);
}

const ListServiceProxies & ServiceRegistry::getProxyServiceList( const areg::ServiceAddress & addrService ) const
{
    MAPPOS pos = findService( static_cast<const areg::ServiceAddress &>(addrService) );
    return (isValidPosition(pos) ? valueAtPosition(pos) : ServiceRegistry::EmptyProxiesList );
}

const ServiceProxy & ServiceRegistry::getProxyService(const areg::ProxyAddress & addProxy) const
{
    return getProxyServiceList( static_cast<const areg::ServiceAddress &>(addProxy) ).getService(addProxy);
}

areg::ServiceConnectionState ServiceRegistry::getServiceStatus(const areg::StubAddress & addrStub) const
{
    return getStubService(addrStub).getServiceStatus();
}

areg::ServiceConnectionState ServiceRegistry::getServiceStatus(const areg::ProxyAddress & addrProxy) const
{
    return getProxyService(addrProxy).getServiceStatus();
}

const mtrouter::ServiceStub & ServiceRegistry::registerServiceProxy(const areg::ProxyAddress & addrProxy, ServiceProxy & out_proxyService)
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_registerServiceProxy);
    std::pair<MAPPOS, bool> pos = addIfUnique(mtrouter::ServiceStub(addrProxy), ServiceRegistry::EmptyProxiesList);
    ASSERT(isValidPosition(pos.first));

    const mtrouter::ServiceStub & result = keyAtPosition(pos.first);
    ListServiceProxies& proxies = valueAtPosition(pos.first);
    if ( pos.second )
    {
        LOG_DBG("Proxy [ %s ] registers new entry and wait for service"
                    , areg::ProxyAddress::convAddressToPath(addrProxy).getString());

        out_proxyService = proxies.registerService( addrProxy );
    }
    else
    {
        out_proxyService = proxies.registerService(addrProxy, result);

        LOG_DBG("Proxy [ %s ] is registered for service with status [ %s ]"
                        , areg::ProxyAddress::convAddressToPath(addrProxy).getString()
                        , areg::getString(result.getServiceStatus()));
    }

    return result;
}

const mtrouter::ServiceStub & ServiceRegistry::unregisterServiceProxy(const areg::ProxyAddress & addrProxy, ServiceProxy & out_proxyService)
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_unregisterServiceProxy);

    MAPPOS pos = findService( static_cast<const areg::ServiceAddress &>(addrProxy) );
    if ( isValidPosition(pos) )
    {
        const mtrouter::ServiceStub & stub = keyAtPosition(pos);
        ListServiceProxies & proxies = valueAtPosition(pos);
        out_proxyService = proxies.unregisterService(addrProxy);
        if ( proxies.isEmpty() && (stub.isValid() == false) )
        {
            LOG_INFO("Proxy [ %s ] is unregistered, remove empty and invalid service entry with status [ %s ]"
                            , areg::ProxyAddress::convAddressToPath(addrProxy).getString()
                            , areg::getString(stub.getServiceStatus()));

            removePosition(pos);
            pos = invalidPosition(); // should not be the last
        }
        else
        {
            LOG_INFO("Unregistered proxy [ %s ], there are [ %d ] proxies left, service status [ %s ]"
                            , areg::ProxyAddress::convAddressToPath(addrProxy).getString()
                            , proxies.getSize()
                            , areg::getString(stub.getServiceStatus()));
        }
    }
    else
    {
        LOG_WARN("The proxy [ %s ] is not in registration list, ignore unregistering"
                    , areg::ProxyAddress::convAddressToPath(addrProxy).getString() );
    }

    return (isValidPosition( pos ) ? keyAtPosition( pos ) : ServiceRegistry::InvalidStubService);
}

const mtrouter::ServiceStub & ServiceRegistry::registerServiceStub(const areg::StubAddress & addrStub, ListServiceProxies & out_listProxies)
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_registerServiceStub);

    std::pair<MAPPOS, bool> pos = addIfUnique( mtrouter::ServiceStub(addrStub), ServiceRegistry::EmptyProxiesList);
    ASSERT(isValidPosition(pos.first));
    mtrouter::ServiceStub& result = keyAtPosition(pos.first);
    ListServiceProxies& proxies = valueAtPosition(pos.first);

    if ( pos.second )
    {
        LOG_DBG("Registered new service [ %s ], there are no proxies yet waiting for service"
                    , areg::StubAddress::convAddressToPath(addrStub).getString());

        result.setServiceStatus( areg::ServiceConnectionState::Connected );
        out_listProxies = proxies;
    }
    else
    {
        result.setService( addrStub, areg::ServiceConnectionState::Connected );
        proxies.stubServiceAvailable(addrStub);
        out_listProxies = proxies;

        LOG_DBG("Registered service [ %s ] availability, [ %d ] proxies will be notified"
                    , areg::StubAddress::convAddressToPath(addrStub).getString()
                    , out_listProxies.getSize());
    }

    return result;
}

const mtrouter::ServiceStub & ServiceRegistry::unregisterServiceStub(const areg::StubAddress & addrStub, ListServiceProxies & out_listProxies)
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_unregisterServiceStub);

    MAPPOS pos = findService( static_cast<const areg::ServiceAddress &>(addrStub) );
    if ( isValidPosition(pos) )
    {
        mtrouter::ServiceStub & stub = keyAtPosition(pos);
        ListServiceProxies & proxies = valueAtPosition(pos);

        stub.setServiceStatus( areg::ServiceConnectionState::Pending );
        proxies.stubServiceUnavailable( );
        if ( proxies.isEmpty() )
        {
            LOG_INFO("Service [ %s ] is unregistered and has no proxies, deleting registry entry"
                        , areg::StubAddress::convAddressToPath(addrStub).getString());

            removePosition(pos);
            pos = invalidPosition();
            out_listProxies.clear();
        }
        else
        {
            out_listProxies = proxies;
            LOG_INFO("Service [ %s ] is unregistered, there are [ %d ] service clients"
                        , areg::StubAddress::convAddressToPath(addrStub).getString()
                        , out_listProxies.getSize());
        }
    }
    else
    {
        LOG_WARN("Service [ %s ] was not in registered list, ignoring"
                    , areg::StubAddress::convAddressToPath(addrStub).getString());
    }

    return (isValidPosition(pos) ? keyAtPosition(pos) : ServiceRegistry::InvalidStubService);
}

ServiceRegistry::MAPPOS ServiceRegistry::findService( const areg::ServiceAddress & addrService ) const
{
    return find(mtrouter::ServiceStub(addrService));
}

void ServiceRegistry::getServiceList(const ITEM_ID & cookie , areg::ArrayList<areg::StubAddress> & listProviders, areg::ArrayList<areg::ProxyAddress> & listConsumers ) const
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_getServiceList);
    LOG_DBG("Filter service list for cookie [ %u ]", static_cast<uint32_t>(cookie));

    for (ServiceRegistryBase::MAPPOS posMap = firstPosition(); isValidPosition(posMap); posMap = nextPosition(posMap) )
    {
        const mtrouter::ServiceStub & svcStub  = keyAtPosition(posMap);
        const areg::StubAddress & addrStub = svcStub.getServiceAddress();
        const ListServiceProxies & listProxies = valueAtPosition(posMap);

        if ( svcStub.isValid() && ((cookie == areg::COOKIE_ANY) || (addrStub.getCookie() == cookie)) )
        {
            LOG_INFO("The cookie [ %u ] of service [ %s ] with status [ %s ] match criteria."
                        , static_cast<uint32_t>(addrStub.getCookie())
                        , areg::StubAddress::convAddressToPath(addrStub).getString()
                        , areg::getString(svcStub.getServiceStatus()));

            listProviders.add(addrStub);
        }
        else
        {
            LOG_DBG("Ignore stub [ %s ] with cookie [ %u ] and status [ %s ]"
                        , areg::StubAddress::convAddressToPath(addrStub).getString()
                        , static_cast<uint32_t>(addrStub.getCookie())
                        , areg::getString(svcStub.getServiceStatus()));
        }

        for (ListServiceProxiesBase::LISTPOS posList = listProxies.firstPosition(); listProxies.isValidPosition(posList); posList = listProxies.nextPosition(posList) )
        {
            const ServiceProxy & svcProxy   = listProxies.valueAtPosition(posList);
            const areg::ProxyAddress & addrProxy  = svcProxy.getServiceAddress();
            if ( svcProxy.isValid() && ((cookie == areg::COOKIE_ANY) || (addrProxy.getCookie() == cookie)) )
            {
                LOG_INFO("The cookie [ %u ] of proxy [ %s ] with status [ %s ] match criteria."
                            , static_cast<uint32_t>(addrProxy.getCookie())
                            , areg::ProxyAddress::convAddressToPath(addrProxy).getString()
                            , areg::getString(svcProxy.getServiceStatus()));

                listConsumers.add(addrProxy);
            }
            else
            {
                LOG_DBG("Ignore proxy [ %s ] with cookie [ %u ] and status [ %s ]"
                            , areg::ProxyAddress::convAddressToPath(addrProxy).getString()
                            , static_cast<uint32_t>(addrProxy.getCookie())
                            , areg::getString(svcProxy.getServiceStatus()));
            }
        }
    }
}

void ServiceRegistry::getServiceSources(const ITEM_ID & cookie, areg::ArrayList<areg::StubAddress> & stubSource, areg::ArrayList<areg::ProxyAddress> & proxySources)
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_getServiceSources);
    LOG_DBG("Pickup services with [ %u ] sources ", static_cast<uint32_t>(cookie));

    for (ServiceRegistry::MAPPOS posMap = firstPosition(); isValidPosition(posMap); posMap = nextPosition(posMap) )
    {
        const mtrouter::ServiceStub & svcStub  = keyAtPosition(posMap);
        const areg::StubAddress & addrStub = svcStub.getServiceAddress();
        const ListServiceProxies & listProxies = valueAtPosition(posMap);

        if (svcStub.isValid() && (cookie == addrStub.getSource()))
        {
            LOG_INFO("Found stub [ %s ] of source [ %u ]", addrStub.convToString().getString(), static_cast<uint32_t>(cookie));
            stubSource.add(addrStub);
        }
        else
        {
            LOG_DBG("Ignore stub [ %s ], it is either invalid or different source than [ %u ]"
                        , addrStub.convToString().getString()
                        , static_cast<uint32_t>(cookie));
        }

        for (ListServiceProxiesBase::LISTPOS posList = listProxies.firstPosition(); listProxies.isValidPosition(posList); posList = listProxies.nextPosition(posList) )
        {
            const ServiceProxy & svcProxy   = listProxies.valueAtPosition(posList);
            const areg::ProxyAddress & addrProxy  = svcProxy.getServiceAddress();

            if (svcProxy.isValid() && (cookie == addrProxy.getSource()))
            {
                LOG_INFO("Found proxy [ %s ] of source [ %u ]", addrProxy.convToString().getString(), static_cast<uint32_t>(cookie));
                proxySources.add(addrProxy);
            }
            else
            {
                LOG_DBG("Ignore proxy [ %s ], it is either invalid or has different source than [ %u ]", addrProxy.convToString().getString(), static_cast<uint32_t>(cookie));
            }
        }
    }
}

const mtrouter::ServiceStub & ServiceRegistry::disconnectProxy(const areg::ProxyAddress & addrProxy)
{
    LOG_SCOPE(mtrouter_service_private_ServiceRegistry_disconnectProxy);
    MAPPOS pos = findService( static_cast<const areg::ServiceAddress &>(addrProxy) );
    if ( isValidPosition(pos) )
    {
        ListServiceProxies & proxies = valueAtPosition(pos);
        ServiceProxy * svcProxy = proxies.getService(addrProxy);
        if ((svcProxy != nullptr) && svcProxy->isValid())
        {
            LOG_INFO("Found service of proxy [ %s ] to disconnect, current state [ %s ]", addrProxy.convToString().getString(), areg::getString(svcProxy->getServiceStatus()));
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
