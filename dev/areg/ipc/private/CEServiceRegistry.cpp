/************************************************************************
 * \file        areg/ipc/private/CEServiceRegistry.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Service Manager
 ************************************************************************/
#include "areg/ipc/private/CEServiceRegistry.hpp"

const CEServiceStub         CEServiceRegistry::InvalidStubService;

const CEListServiceProxies  CEServiceRegistry::EmptyProxiesList;

CEServiceRegistry::CEServiceRegistry(void)
    : TEHashMap<CEServiceStub, CEListServiceProxies, const CEServiceStub &, const CEListServiceProxies &, CEServiceRegistryImpl>  ( )
{
    ; // do nothing
}

CEServiceRegistry::~CEServiceRegistry(void)
{
    ; // do nothing
}

bool CEServiceRegistry::IsServiceRegistered(const CEStubAddress & addrStub) const
{
    return ( findService(static_cast<const CEServiceAddress &>(addrStub)) != NULL );
}

bool CEServiceRegistry::IsServiceRegistered(const CEProxyAddress & addrProxy) const
{
    return GetProxyServiceList( static_cast<const CEServiceAddress &>(addrProxy) ).IsServiceRegistered(addrProxy);
}

const CEServiceStub & CEServiceRegistry::GetStubService( const CEServiceAddress & addrService ) const
{
    MAPPOS pos = findService( addrService );
    return ( pos != NULL ? GetKeyAt(pos) : CEServiceRegistry::InvalidStubService );
}

const CEListServiceProxies & CEServiceRegistry::GetProxyServiceList( const CEServiceAddress & addrService ) const
{
    MAPPOS pos = findService( static_cast<const CEServiceAddress &>(addrService) );
    return ( pos != NULL ? GetValueAt(pos) : CEServiceRegistry::EmptyProxiesList );
}

const CEServiceProxy & CEServiceRegistry::GetProxyService(const CEProxyAddress & addProxy) const
{
    return GetProxyServiceList( static_cast<const CEServiceAddress &>(addProxy) ).GetService(addProxy);
}

NEService::eServiceConnection CEServiceRegistry::GetServiceStatus(const CEStubAddress & addrStub) const
{
    return GetStubService(addrStub).GetServiceStatus();
}

NEService::eServiceConnection CEServiceRegistry::GetServiceStatus(const CEProxyAddress & addrProxy) const
{
    return GetProxyService(addrProxy).GetServiceStatus();
}

const CEServiceStub & CEServiceRegistry::RegisterServiceProxy(const CEProxyAddress & addrProxy, CEServiceProxy & out_proxyService)
{
    MAPPOS pos = findService( static_cast<const CEServiceAddress &>(addrProxy) );
    if ( pos == NULL )
    {
        CEListServiceProxies proxies;
        out_proxyService = proxies.RegisterService( addrProxy );
        pos = SetKey( CEServiceStub(addrProxy) , proxies, false );
    }
    else
    {
        ServiceBlock * block = reinterpret_cast<ServiceBlock *>(pos);
        CEListServiceProxies & proxies = block->mValue;
        out_proxyService = proxies.RegisterService(addrProxy, block->mKey);
    }
    return ( pos != NULL ? GetKeyAt(pos) : CEServiceRegistry::InvalidStubService );
}

const CEServiceStub & CEServiceRegistry::UnregisterServiceProxy(const CEProxyAddress & addrProxy, CEServiceProxy & out_proxyService)
{
    MAPPOS pos = findService( static_cast<const CEServiceAddress &>(addrProxy) );
    if ( pos != NULL )
    {
        ServiceBlock * block = reinterpret_cast<ServiceBlock *>(pos);
        CEListServiceProxies & proxies = block->mValue;
        out_proxyService = proxies.UnregisterService(addrProxy);
        if ( proxies.IsEmpty() && (block->mKey.IsValid() == false) )
        {
            RemoveAt(pos);
            pos = NULL;
        }
    }
    return ( pos != NULL ? GetKeyAt(pos) : CEServiceRegistry::InvalidStubService );
}

const CEServiceStub & CEServiceRegistry::RegisterServiceStub(const CEStubAddress & addrStub, CEListServiceProxies & out_listProxies)
{
    MAPPOS pos = findService( static_cast<const CEServiceAddress &>(addrStub) );
    if ( pos == NULL )
    {
        CEServiceStub stubService( addrStub );
        stubService.SetServiceStatus( NEService::ServiceConnected );
        pos = SetKey( stubService, CEServiceRegistry::EmptyProxiesList, false);
        out_listProxies = CEServiceRegistry::EmptyProxiesList;
    }
    else
    {
        ServiceBlock * block = reinterpret_cast<ServiceBlock *>(pos);
        block->mKey.SetService( addrStub, NEService::ServiceConnected );
        block->mValue.StubServiceAvailable(addrStub);
        out_listProxies = block->mValue;
    }
    return ( pos != NULL ? GetKeyAt(pos) : CEServiceRegistry::InvalidStubService );
}

const CEServiceStub & CEServiceRegistry::UnregisterServiceStub(const CEStubAddress & addrStub, CEListServiceProxies & out_listProxies)
{
    MAPPOS pos = findService( static_cast<const CEServiceAddress &>(addrStub) );
    if ( pos != NULL )
    {
        ServiceBlock * block = reinterpret_cast<ServiceBlock *>(pos);
        block->mKey.SetServiceStatus( NEService::ServicePending );
        block->mValue.StubServiceUnavailable( );
        out_listProxies = block->mValue;
        if ( block->mValue.IsEmpty() )
        {
            RemoveAt(pos);
            pos = NULL;
        }
    }
    return ( pos != NULL ? GetKeyAt(pos) : CEServiceRegistry::InvalidStubService );
}

MAPPOS CEServiceRegistry::findService( const CEServiceAddress & addrService ) const
{
    const unsigned int hash = static_cast<unsigned int>(addrService);
    CEBlock ** result       = &mHashTable[hash % mHashTableSize];

    for ( ; *result != NULL; result = &(*result)->mNext)
    {
        if ( hash == (*result)->mHash && static_cast<const CEServiceAddress &>((*result)->mKey.GetServiceAddress()) == addrService )
            break;
    }
    return static_cast<MAPPOS>(*result);
}

int CEServiceRegistry::GetServiceList( ITEM_ID cookie, TEArrayList<CEStubAddress, const CEStubAddress &> out_stubServiceList ) const
{
    int result = 0;
    for ( MAPPOS pos = GetStartPosition(); pos != NULL; pos = GetNextPosition(pos) )
    {
        const CEServiceStub & stubService = GetKeyAt(pos);
        if ( stubService.GetServiceAddress().GetCookie() == cookie && stubService.IsValid() )
        {
            out_stubServiceList.Add(stubService.GetServiceAddress());
            result += 1;
        }
    }
    return result;
}

int CEServiceRegistry::GetServiceList( ITEM_ID cookie, TEArrayList<CEProxyAddress, const CEProxyAddress &> out_proxyServiceList ) const
{
    int result = 0;
    for ( MAPPOS posMap = GetStartPosition(); posMap != NULL; posMap = GetNextPosition(posMap) )
    {
        const CEListServiceProxies & listProxies = GetValueAt(posMap);
        for ( LISTPOS posList = listProxies.GetHeadPosition(); posList != NULL; posList = listProxies.GetNextPosition(posList) )
        {
            const CEServiceProxy & proxyService = listProxies.GetAt(posList);
            if ( proxyService.GetServiceAddress().GetCookie() == cookie && proxyService.IsValid() )
            {
                out_proxyServiceList.Add(proxyService.GetServiceAddress());
                result += 1;
            }
        }
    }
    return result;
}

int CEServiceRegistry::GetRemoteServiceList( TEArrayList<CEStubAddress, const CEStubAddress &> & out_stubServiceList ) const
{
    int result = 0;
    for ( MAPPOS pos = GetStartPosition(); pos != NULL; pos = GetNextPosition(pos) )
    {
        const CEServiceStub & stubService = GetKeyAt(pos);
        if ( stubService.GetServiceAddress().IsServiceRemote() && stubService.IsValid() )
        {
            out_stubServiceList.Add(stubService.GetServiceAddress());
            result += 1;
        }
    }
    return result;
}

int CEServiceRegistry::GetRemoteServiceList( TEArrayList<CEProxyAddress, const CEProxyAddress &> & out_proxyServiceList ) const
{
    int result = 0;
    for ( MAPPOS posMap = GetStartPosition(); posMap != NULL; posMap = GetNextPosition(posMap) )
    {
        const CEListServiceProxies & listProxies = GetValueAt(posMap);
        for ( LISTPOS posList = listProxies.GetHeadPosition(); posList != NULL; posList = listProxies.GetNextPosition(posList) )
        {
            const CEServiceProxy & proxyService = listProxies.GetAt(posList);
            if ( proxyService.GetServiceAddress().IsServiceRemote() && proxyService.IsValid() )
            {
                out_proxyServiceList.Add(proxyService.GetServiceAddress());
                result += 1;
            }
        }
    }
    return result;
}
