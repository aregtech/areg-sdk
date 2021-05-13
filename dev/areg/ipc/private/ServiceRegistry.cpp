/************************************************************************
 * \file        areg/ipc/private/ServiceRegistry.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Service Manager
 ************************************************************************/
#include "areg/ipc/private/ServiceRegistry.hpp"

const ServiceStub         ServiceRegistry::InvalidStubService;

const ListServiceProxies  ServiceRegistry::EmptyProxiesList;

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
    MAPPOS pos = findService( static_cast<const ServiceAddress &>(addrProxy) );
    if ( pos == NULL )
    {
        ListServiceProxies proxies;
        out_proxyService = proxies.registerService( addrProxy );
        pos = setAt( ServiceStub(addrProxy) , proxies, false );
    }
    else
    {
        ServiceBlock * block = reinterpret_cast<ServiceBlock *>(pos);
        ListServiceProxies & proxies = block->mValue;
        out_proxyService = proxies.registerService(addrProxy, block->mKey);
    }
    return ( pos != NULL ? keyAtPosition(pos) : ServiceRegistry::InvalidStubService );
}

const ServiceStub & ServiceRegistry::unregisterServiceProxy(const ProxyAddress & addrProxy, ServiceProxy & out_proxyService)
{
    MAPPOS pos = findService( static_cast<const ServiceAddress &>(addrProxy) );
    if ( pos != NULL )
    {
        ServiceBlock * block = reinterpret_cast<ServiceBlock *>(pos);
        ListServiceProxies & proxies = block->mValue;
        out_proxyService = proxies.unregisterService(addrProxy);
        if ( proxies.isEmpty() && (block->mKey.isValid() == false) )
        {
            removePosition(pos);
            pos = NULL;
        }
    }
    return ( pos != NULL ? keyAtPosition(pos) : ServiceRegistry::InvalidStubService );
}

const ServiceStub & ServiceRegistry::registerServiceStub(const StubAddress & addrStub, ListServiceProxies & out_listProxies)
{
    MAPPOS pos = findService( static_cast<const ServiceAddress &>(addrStub) );
    if ( pos == NULL )
    {
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
    }
    return ( pos != NULL ? keyAtPosition(pos) : ServiceRegistry::InvalidStubService );
}

const ServiceStub & ServiceRegistry::unregisterServiceStub(const StubAddress & addrStub, ListServiceProxies & out_listProxies)
{
    MAPPOS pos = findService( static_cast<const ServiceAddress &>(addrStub) );
    if ( pos != NULL )
    {
        ServiceBlock * block = reinterpret_cast<ServiceBlock *>(pos);
        block->mKey.setServiceStatus( NEService::ServicePending );
        block->mValue.stubServiceUnavailable( );
        out_listProxies = block->mValue;
        if ( block->mValue.isEmpty() )
        {
            removePosition(pos);
            pos = NULL;
        }
    }
    return ( pos != NULL ? keyAtPosition(pos) : ServiceRegistry::InvalidStubService );
}

MAPPOS ServiceRegistry::findService( const ServiceAddress & addrService ) const
{
    const unsigned int hash = static_cast<unsigned int>(addrService);
    Block ** result       = &mHashTable[hash % mHashTableSize];

    for ( ; *result != NULL; result = &(*result)->mNext)
    {
        if ( hash == (*result)->mHash && static_cast<const ServiceAddress &>((*result)->mKey.getServiceAddress()) == addrService )
            break;
    }
    return static_cast<MAPPOS>(*result);
}

int ServiceRegistry::getServiceList( ITEM_ID cookie, TEArrayList<StubAddress, const StubAddress &> out_stubServiceList ) const
{
    int result = 0;
    for ( MAPPOS pos = firstPosition(); pos != NULL; pos = nextPosition(pos) )
    {
        const ServiceStub & stubService = keyAtPosition(pos);
        if ( stubService.getServiceAddress().getCookie() == cookie && stubService.isValid() )
        {
            out_stubServiceList.add(stubService.getServiceAddress());
            result += 1;
        }
    }
    return result;
}

int ServiceRegistry::getServiceList( ITEM_ID cookie, TEArrayList<ProxyAddress, const ProxyAddress &> out_proxyServiceList ) const
{
    int result = 0;
    for ( MAPPOS posMap = firstPosition(); posMap != NULL; posMap = nextPosition(posMap) )
    {
        const ListServiceProxies & listProxies = valueAtPosition(posMap);
        for ( LISTPOS posList = listProxies.firstPosition(); posList != NULL; posList = listProxies.nextPosition(posList) )
        {
            const ServiceProxy & proxyService = listProxies.getAt(posList);
            if ( proxyService.getServiceAddress().getCookie() == cookie && proxyService.isValid() )
            {
                out_proxyServiceList.add(proxyService.getServiceAddress());
                result += 1;
            }
        }
    }
    return result;
}

int ServiceRegistry::getRemoteServiceList( TEArrayList<StubAddress, const StubAddress &> & out_stubServiceList ) const
{
    int result = 0;
    for ( MAPPOS pos = firstPosition(); pos != NULL; pos = nextPosition(pos) )
    {
        const ServiceStub & stubService = keyAtPosition(pos);
        if ( stubService.getServiceAddress().isServiceRemote() && stubService.isValid() )
        {
            out_stubServiceList.add(stubService.getServiceAddress());
            result += 1;
        }
    }
    return result;
}

int ServiceRegistry::getRemoteServiceList( TEArrayList<ProxyAddress, const ProxyAddress &> & out_proxyServiceList ) const
{
    int result = 0;
    for ( MAPPOS posMap = firstPosition(); posMap != NULL; posMap = nextPosition(posMap) )
    {
        const ListServiceProxies & listProxies = valueAtPosition(posMap);
        for ( LISTPOS posList = listProxies.firstPosition(); posList != NULL; posList = listProxies.nextPosition(posList) )
        {
            const ServiceProxy & proxyService = listProxies.getAt(posList);
            if ( proxyService.getServiceAddress().isServiceRemote() && proxyService.isValid() )
            {
                out_proxyServiceList.add(proxyService.getServiceAddress());
                result += 1;
            }
        }
    }
    return result;
}
