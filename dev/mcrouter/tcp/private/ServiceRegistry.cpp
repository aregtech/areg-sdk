/************************************************************************
 * \file        mcrouter/tcp/private/ServiceRegistry.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Service Manager
 ************************************************************************/
#include "mcrouter/tcp/private/ServiceRegistry.hpp"

#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// ServiceRegistry class implementation
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceRegistry_registerServiceProxy);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceRegistry_unregisterServiceProxy);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceRegistry_registerServiceStub);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceRegistry_unregisterServiceStub);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceRegistry_getServiceList);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceRegistry_getServiceSources);
DEF_TRACE_SCOPE(mcrouter_tcp_private_ServiceRegistry_disconnectProxy);

//////////////////////////////////////////////////////////////////////////
// ServiceRegistry statics
//////////////////////////////////////////////////////////////////////////

const ServiceStub         ServiceRegistry::InvalidStubService;

const ListServiceProxies  ServiceRegistry::EmptyProxiesList;

//////////////////////////////////////////////////////////////////////////
// ServiceRegistry class methods
//////////////////////////////////////////////////////////////////////////

ServiceRegistry::ServiceRegistry(void)
    : TEHashMap<ServiceStub, ListServiceProxies, const ServiceStub &, const ListServiceProxies &, ServiceRegistryImpl>  ( )
{
    ; // do nothing
}

ServiceRegistry::~ServiceRegistry(void)
{
    ; // do nothing
}

bool ServiceRegistry::isServiceRegistered(const StubAddress & addrStub) const
{
    return ( findService(static_cast<const ServiceAddress &>(addrStub)) != NULL );
}

bool ServiceRegistry::isServiceRegistered(const ProxyAddress & addrProxy) const
{
    return getProxyServiceList( static_cast<const ServiceAddress &>(addrProxy) ).isServiceRegistered(addrProxy);
}

const ServiceStub & ServiceRegistry::getStubService( const ServiceAddress & addrService ) const
{
    MAPPOS pos = findService( addrService );
    return ( pos != NULL ? keyAtPosition(pos) : ServiceRegistry::InvalidStubService );
}

const ListServiceProxies & ServiceRegistry::getProxyServiceList( const ServiceAddress & addrService ) const
{
    MAPPOS pos = findService( static_cast<const ServiceAddress &>(addrService) );
    return ( pos != NULL ? valueAtPosition(pos) : ServiceRegistry::EmptyProxiesList );
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
    TRACE_SCOPE(mcrouter_tcp_private_ServiceRegistry_registerServiceProxy);
    MAPPOS pos = findService( static_cast<const ServiceAddress &>(addrProxy) );
    if ( pos == NULL )
    {
        TRACE_DBG("Proxy [ %s ] registers new entry and wait for service"
                    , ProxyAddress::convAddressToPath(addrProxy).getString());
        
        ListServiceProxies proxies;
        out_proxyService = proxies.registerService( addrProxy );
        pos = setAt( ServiceStub(addrProxy) , proxies, false );
    }
    else
    {
        ServiceBlock * block = reinterpret_cast<ServiceBlock *>(pos);
        ListServiceProxies & proxies = block->mValue;
        out_proxyService = proxies.registerService(addrProxy, block->mKey);

        TRACE_DBG("Proxy [ %s ] is registered for service with status [ %s ]"
                        , ProxyAddress::convAddressToPath(addrProxy).getString()
                        , NEService::getString(block->mKey.getServiceStatus()));
    }

    return ( pos != NULL ? keyAtPosition(pos) : ServiceRegistry::InvalidStubService );
}

const ServiceStub & ServiceRegistry::unregisterServiceProxy(const ProxyAddress & addrProxy, ServiceProxy & out_proxyService)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceRegistry_unregisterServiceProxy);
    MAPPOS pos = findService( static_cast<const ServiceAddress &>(addrProxy) );
    if ( pos != NULL )
    {
        ServiceBlock * block = reinterpret_cast<ServiceBlock *>(pos);
        ListServiceProxies & proxies = block->mValue;
        out_proxyService = proxies.unregisterService(addrProxy);
        if ( proxies.isEmpty() && (block->mKey.isValid() == false) )
        {
            TRACE_INFO("Proxy [ %s ] is unregistered, remove empty and invalid service entry with status [ %s ]"
                            , ProxyAddress::convAddressToPath(addrProxy).getString()
                            , NEService::getString(block->mKey.getServiceStatus()));

            removePosition(pos);
            pos = NULL;
        }
        else
        {
            TRACE_INFO("Unregistered proxy [ %s ], there are [ %d ] proxies left, service status [ %s ]"
                            , ProxyAddress::convAddressToPath(addrProxy).getString()
                            , proxies.getSize()
                            , NEService::getString(block->mKey.getServiceStatus()));
        }
    }
    else
    {
        TRACE_WARN("The proxy [ %s ] is not in registration list, ignore unregistering"
                    , ProxyAddress::convAddressToPath(addrProxy).getString() );
    }

    return ( pos != NULL ? keyAtPosition(pos) : ServiceRegistry::InvalidStubService );
}

const ServiceStub & ServiceRegistry::registerServiceStub(const StubAddress & addrStub, ListServiceProxies & out_listProxies)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceRegistry_registerServiceStub);

    MAPPOS pos = findService( static_cast<const ServiceAddress &>(addrStub) );
    if ( pos == NULL )
    {
        TRACE_DBG("Registered new service [ %s ], there are no proxies yet waiting for service"
                    , StubAddress::convAddressToPath(addrStub).getString());
        
        ServiceStub stubService( addrStub );
        stubService.setServiceStatus( NEService::ServiceConnected );
        pos = setAt( stubService, ServiceRegistry::EmptyProxiesList, false);
        out_listProxies = ServiceRegistry::EmptyProxiesList;
    }
    else
    {
        ServiceBlock * block = reinterpret_cast<ServiceBlock *>(pos);
        block->mKey.setService( addrStub, NEService::ServiceConnected );
        block->mValue.stubServiceAvailable(addrStub);
        out_listProxies = block->mValue;

        TRACE_DBG("Registered service [ %s ] availability, [ %d ] proxies will be notified"
                    , StubAddress::convAddressToPath(addrStub).getString()
                    , out_listProxies.getSize());
    }

    return ( pos != NULL ? keyAtPosition(pos) : ServiceRegistry::InvalidStubService );
}

const ServiceStub & ServiceRegistry::unregisterServiceStub(const StubAddress & addrStub, ListServiceProxies & out_listProxies)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceRegistry_unregisterServiceStub);
    MAPPOS pos = findService( static_cast<const ServiceAddress &>(addrStub) );
    if ( pos != NULL )
    {
        ServiceBlock * block = reinterpret_cast<ServiceBlock *>(pos);
        block->mKey.setServiceStatus( NEService::ServicePending );
        block->mValue.stubServiceUnavailable( );
        out_listProxies = block->mValue;
        if ( block->mValue.isEmpty() )
        {
            TRACE_INFO("Service [ %s ] is unregistered and has no proxies, deleting registry entry"
                        , StubAddress::convAddressToPath(addrStub).getString());
            removePosition(pos);
            pos = NULL;
        }
        else
        {
            TRACE_INFO("Service [ %s ] is unregistered, there are [ %d ] service clients"
                        , StubAddress::convAddressToPath(addrStub).getString()
                        , out_listProxies.getSize());
        }
    }
    else
    {
        TRACE_WARN("Service [ %s ] was not in registered list, ignoring"
                    , StubAddress::convAddressToPath(addrStub).getString());
    }

    return ( pos != NULL ? keyAtPosition(pos) : ServiceRegistry::InvalidStubService );
}

MAPPOS ServiceRegistry::findService( const ServiceAddress & addrService ) const
{
    const unsigned int hash = static_cast<unsigned int>(addrService);
    Block ** result         = &mHashTable[hash % mHashTableSize];

    for ( ; *result != NULL; result = &(*result)->mNext)
    {
        if ( hash == (*result)->mHash && static_cast<const ServiceAddress &>((*result)->mKey.getServiceAddress()) == addrService )
            break;
    }
    return static_cast<MAPPOS>(*result);
}

void ServiceRegistry::getServiceList( ITEM_ID cookie , TEArrayList<StubAddress, const StubAddress &> & OUT out_stubServiceList, TEArrayList<ProxyAddress, const ProxyAddress &> & OUT out_proxyServiceList ) const
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceRegistry_getServiceList);
    TRACE_DBG("Filter service list for cookie [ %u ]", static_cast<unsigned int>(cookie));

    for ( MAPPOS posMap = firstPosition(); posMap != NULL; posMap = nextPosition(posMap) )
    {
        const ServiceStub & svcStub  = keyAtPosition(posMap);
        const StubAddress & addrStub = svcStub.getServiceAddress();
        const ListServiceProxies & listProxies = valueAtPosition(posMap);

        if ( svcStub.isValid() && ((cookie == NEService::COOKIE_ANY) || (addrStub.getCookie() == cookie)) )
        {
            TRACE_INFO("The cookie [ %u ] of service [ %s ] with status [ %s ] match criteria."
                        , static_cast<unsigned int>(addrStub.getCookie())
                        , StubAddress::convAddressToPath(addrStub).getString()
                        , NEService::getString(svcStub.getServiceStatus()));
            
            out_stubServiceList.add(addrStub);
        }
        else
        {
            TRACE_DBG("Ignore stub [ %s ] with cookie [ %u ] and status [ %s ]"
                        , StubAddress::convAddressToPath(addrStub).getString()
                        , static_cast<unsigned int>(addrStub.getCookie())
                        , NEService::getString(svcStub.getServiceStatus()));
        }

        for ( LISTPOS posList = listProxies.firstPosition(); posList != NULL; posList = listProxies.nextPosition(posList) )
        {
            const ServiceProxy & svcProxy   = listProxies.getAt(posList);
            const ProxyAddress & addrProxy  = svcProxy.getServiceAddress();
            if ( svcProxy.isValid() && ((cookie == NEService::COOKIE_ANY) || (addrProxy.getCookie() == cookie)) )
            {
                TRACE_INFO("The cookie [ %u ] of proxy [ %s ] with status [ %s ] match criteria."
                            , static_cast<unsigned int>(addrProxy.getCookie())
                            , ProxyAddress::convAddressToPath(addrProxy).getString()
                            , NEService::getString(svcProxy.getServiceStatus()));
            
                out_proxyServiceList.add(addrProxy);
            }
            else
            {
                TRACE_DBG("Ignore proxy [ %s ] with cookie [ %u ] and status [ %s ]"
                            , ProxyAddress::convAddressToPath(addrProxy).getString()
                            , static_cast<unsigned int>(addrProxy.getCookie())
                            , NEService::getString(svcProxy.getServiceStatus()));
            }
        }
    }
}

void ServiceRegistry::getServiceSources(ITEM_ID cookie, TEArrayList<StubAddress, const StubAddress &> & OUT stubSource, TEArrayList<ProxyAddress, const ProxyAddress &> & OUT proxySources)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceRegistry_getServiceSources);
    TRACE_DBG("Pickup services with [ %u ] sources ", static_cast<unsigned int>(cookie));

    for ( MAPPOS posMap = firstPosition(); posMap != NULL; posMap = nextPosition(posMap) )
    {
        const ServiceStub & svcStub  = keyAtPosition(posMap);
        const StubAddress & addrStub = svcStub.getServiceAddress();
        const ListServiceProxies & listProxies = valueAtPosition(posMap);

        if (svcStub.isValid() && (cookie == addrStub.getSource()))
        {
            TRACE_INFO("Found stub [ %s ] of source [ %u ]", addrStub.convToString().getString(), static_cast<unsigned int>(cookie));
            stubSource.add(addrStub);
        }
        else
        {
            TRACE_DBG("Ignore stub [ %s ], it is either invalid or differet source than [ %u ]", addrStub.convToString().getString(), static_cast<unsigned int>(cookie));
        }

        for ( LISTPOS posList = listProxies.firstPosition(); posList != NULL; posList = listProxies.nextPosition(posList) )
        {
            const ServiceProxy & svcProxy   = listProxies.getAt(posList);
            const ProxyAddress & addrProxy  = svcProxy.getServiceAddress();

            if (svcProxy.isValid() && (cookie == addrProxy.getSource()))
            {
                TRACE_INFO("Found proxy [ %s ] of source [ %u ]", addrProxy.convToString().getString(), static_cast<unsigned int>(cookie));
                proxySources.add(addrProxy);
            }
            else
            {
                TRACE_DBG("Ignore proxy [ %s ], it is either invalid or has different source than [ %u ]", addrProxy.convToString().getString(), static_cast<unsigned int>(cookie));
            }
        }
    }
}

const ServiceStub & ServiceRegistry::disconnectProxy(const ProxyAddress & IN addrProxy)
{
    TRACE_SCOPE(mcrouter_tcp_private_ServiceRegistry_disconnectProxy);
    MAPPOS pos = findService( static_cast<const ServiceAddress &>(addrProxy) );
    ServiceBlock * block = reinterpret_cast<ServiceBlock *>(pos);
    if ( block != NULL )
    {
        ListServiceProxies & proxies = block->mValue;
        ServiceProxy * svcProxy = proxies.getService(addrProxy);
        if ((svcProxy != NULL) && svcProxy->isValid())
        {
            TRACE_INFO("Found service of proxy [ %s ] to disconnect, current state [ %s ]", addrProxy.convToString().getString(), NEService::getString(svcProxy->getServiceStatus()));
            svcProxy->stubUnavailable();
        }
        else
        {
            TRACE_DBG("Nor service of proxy [ %s ] found, ignore disconnect", addrProxy.convToString().getString());
        }
    }
    else
    {
        TRACE_WARN("No proxy [ %s ] is in registration list, ignore disconect", addrProxy.convToString().getString() );
    }

    return ( block != NULL ? block->mKey : ServiceRegistry::InvalidStubService );
}
