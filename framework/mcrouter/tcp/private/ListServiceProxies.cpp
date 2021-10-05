/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        mcrouter/tcp/private/ListServiceProxies.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Proxy Service object
 ************************************************************************/
#include "mcrouter/tcp/private/ListServiceProxies.hpp"
#include "mcrouter/tcp/private/ServiceStub.hpp"
#include "areg/component/StubAddress.hpp"

const ServiceProxy     ListServiceProxies::InvalidProxyService;

const ServiceProxy & ListServiceProxies::getService( const ProxyAddress & addrProxy ) const
{
    LISTPOS pos = _findProxy(addrProxy);
    return ( pos != nullptr ? static_cast<const ServiceProxy &>(getAt(pos)) : ListServiceProxies::InvalidProxyService );
}

ServiceProxy * ListServiceProxies::getService( const ProxyAddress & addrProxy )
{
    LISTPOS pos = _findProxy(addrProxy);
    ListServiceProxies::Block * block = reinterpret_cast<ListServiceProxies::Block *>(pos);
    return ( block != nullptr ? static_cast<ServiceProxy *>(&(block->mValue)) : nullptr );
}

ServiceProxy & ListServiceProxies::registerService( const ProxyAddress & addrProxy )
{
    LISTPOS pos = _findProxy(addrProxy);
    if ( pos == nullptr )
    {
        pos = pushLast( ServiceProxy( addrProxy ) );
    }

    return static_cast<ServiceProxy &>(getAt(pos));
}

ServiceProxy & ListServiceProxies::registerService(const ProxyAddress & addrProxy, const ServiceStub & stubService)
{
    LISTPOS pos = _findProxy(addrProxy);
    if ( pos == nullptr )
    {
        pos = pushLast( ServiceProxy( addrProxy ) );
    }

    const StubAddress & addrStub = stubService.getServiceAddress();
    ServiceProxy & proxyService = getAt(pos);
    if ( addrStub == addrProxy)
    {
        if ( stubService.getServiceStatus() == NEService::eServiceConnection::ServiceConnected )
        {
            proxyService.stubAvailable( addrStub );
        }
        else
        {
            proxyService.stubUnavailable( );
        }
    }
    return proxyService;
}

ServiceProxy ListServiceProxies::unregisterService( const ProxyAddress & addrProxy )
{
    ServiceProxy result;
    for ( LISTPOS pos = firstPosition( ); pos != nullptr; pos = nextPosition(pos) )
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
    for ( LISTPOS pos = firstPosition(); pos != nullptr; pos = nextPosition(pos) )
    {
        ServiceProxy & proxyService = getAt(pos);
        result += proxyService.stubAvailable(addrStub) ? 1 : 0;
    }
    return result;
}

int ListServiceProxies::stubServiceUnavailable( void )
{
    int result = 0;
    for ( LISTPOS pos = firstPosition(); pos != nullptr; pos = nextPosition(pos) )
    {
        ServiceProxy & proxyService = getAt(pos);
        result += proxyService.stubUnavailable() ? 1 : 0;
    }
    return result;
}

int ListServiceProxies::getSpecificService(ListServiceProxies & out_listProxies, ITEM_ID cookie)
{
    int result = 0;
    for ( LISTPOS pos = firstPosition( ); pos != nullptr; pos = nextPosition(pos) )
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
    for ( ; pos != nullptr; pos = nextPosition(pos) )
    {
        if ( getAt(pos) == addrProxy )
            break;
    }
    return pos;
}
