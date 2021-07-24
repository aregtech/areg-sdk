/************************************************************************
 * \file        mcrouter/tcp/private/ListServiceProxies.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Proxy Service object
 ************************************************************************/
#include "mcrouter/tcp/private/ListServiceProxies.hpp"
#include "mcrouter/tcp/private/ServiceStub.hpp"
#include "areg/component/StubAddress.hpp"

const ServiceProxy     ListServiceProxies::InvalidProxyService;

ListServiceProxies::ListServiceProxies( void )
    : TELinkedList<ServiceProxy, const ServiceProxy &> ( )
{
    ; // do nothing
}

ListServiceProxies::ListServiceProxies( const ListServiceProxies & source )
    : TELinkedList<ServiceProxy, const ServiceProxy &>( static_cast<const TELinkedList<ServiceProxy, const ServiceProxy &> &>(source) )
{
    ; // do nothing
}

ListServiceProxies::~ListServiceProxies(void)
{
    ; // do nothing
}

const ListServiceProxies & ListServiceProxies::operator = ( const ListServiceProxies & source )
{
    if ( static_cast<const ListServiceProxies *>(this) != &source )
    {
        removeAll();
        static_cast<TELinkedList<ServiceProxy, const ServiceProxy &> &>(*this) = static_cast<const TELinkedList<ServiceProxy, const ServiceProxy &> &>(source);
    }
    return (*this);
}

const ServiceProxy & ListServiceProxies::getService( const ProxyAddress & addrProxy ) const
{
    LISTPOS pos = _findProxy(addrProxy);
    return ( pos != NULL ? static_cast<const ServiceProxy &>(getAt(pos)) : ListServiceProxies::InvalidProxyService );
}

ServiceProxy * ListServiceProxies::getService( const ProxyAddress & addrProxy )
{
    LISTPOS pos = _findProxy(addrProxy);
    ListServiceProxies::Block * block = reinterpret_cast<ListServiceProxies::Block *>(pos);
    return ( block != NULL ? static_cast<ServiceProxy *>(&(block->mValue)) : NULL );
}

ServiceProxy & ListServiceProxies::registerService( const ProxyAddress & addrProxy )
{
    LISTPOS pos = _findProxy(addrProxy);
    if ( pos == NULL )
        pos = pushLast( ServiceProxy(addrProxy) );

    return static_cast<ServiceProxy &>(getAt(pos));
}

ServiceProxy & ListServiceProxies::registerService(const ProxyAddress & addrProxy, const ServiceStub & stubService)
{
    LISTPOS pos = _findProxy(addrProxy);
    if ( pos == NULL )
        pos = pushLast( ServiceProxy(addrProxy) );

    const StubAddress & addrStub = stubService.getServiceAddress();
    ServiceProxy & proxyService = getAt(pos);
    if ( addrStub == addrProxy)
    {
        if ( stubService.getServiceStatus() == NEService::ServiceConnected )
            proxyService.stubAvailable( addrStub );
        else
            proxyService.stubUnavailable();
    }
    return proxyService;
}

ServiceProxy ListServiceProxies::unregisterService( const ProxyAddress & addrProxy )
{
    ServiceProxy result;
    for ( LISTPOS pos = firstPosition( ); pos != NULL; pos = nextPosition(pos) )
    {
        const ServiceProxy & proxyService = getAt(pos);
        if ( proxyService == addrProxy )
        {
            result = proxyService;
            removeAt(pos);
            break;
        }
    }
    return result;
}

int ListServiceProxies::stubServiceAvailable( const StubAddress & addrStub )
{
    int result = 0;
    for ( LISTPOS pos = firstPosition(); pos != NULL; pos = nextPosition(pos) )
    {
        ServiceProxy & proxyService = getAt(pos);
        result += proxyService.stubAvailable(addrStub) ? 1 : 0;
    }
    return result;
}

int ListServiceProxies::stubServiceUnavailable( void )
{
    int result = 0;
    for ( LISTPOS pos = firstPosition(); pos != NULL; pos = nextPosition(pos) )
    {
        ServiceProxy & proxyService = getAt(pos);
        result += proxyService.stubUnavailable() ? 1 : 0;
    }
    return result;
}

int ListServiceProxies::getSpecificService(ListServiceProxies & out_listProxies, ITEM_ID cookie)
{
    int result = 0;
    for ( LISTPOS pos = firstPosition( ); pos != NULL; pos = nextPosition(pos) )
    {
        ServiceProxy & proxyService = getAt(pos);
        if ( proxyService.getServiceAddress().getCookie() == cookie )
        {
            result += 1;
            out_listProxies.pushLast(proxyService);
        }
    }
    return result;
}

LISTPOS ListServiceProxies::_findProxy(const ProxyAddress & addrProxy) const
{
    LISTPOS pos = firstPosition( );
    for ( ; pos != NULL; pos = nextPosition(pos) )
    {
        if ( getAt(pos) == addrProxy )
            break;
    }
    return pos;
}
