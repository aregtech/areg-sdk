/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        mtrouter/service/private/ListServiceProxies.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Proxy Service object
 ************************************************************************/
#include "mtrouter/service/private/ListServiceProxies.hpp"
#include "mtrouter/service/private/ServiceStub.hpp"
#include "areg/component/StubAddress.hpp"

const ServiceProxy     ListServiceProxies::InvalidProxyService;

const ServiceProxy & ListServiceProxies::service( const ProxyAddress & addrProxy ) const
{
    ListServiceProxies::LISTPOS pos = _findProxy(addrProxy);
    return ( is_valid_position(pos) ? static_cast<const ServiceProxy &>(value_at_position(pos)) : ListServiceProxies::InvalidProxyService );
}

ServiceProxy * ListServiceProxies::service( const ProxyAddress & addrProxy )
{
    ListServiceProxies::LISTPOS pos = _findProxy(addrProxy);
    return ( is_valid_position(pos) ? static_cast<ServiceProxy *>(&value_at_position(pos)) : nullptr );
}

ServiceProxy & ListServiceProxies::registerService( const ProxyAddress & addrProxy )
{
    ListServiceProxies::LISTPOS pos = _findProxy(addrProxy);
    if ( is_invalid_position(pos) )
    {
        push_last(ServiceProxy(addrProxy));
        pos = last_position();
    }

    return static_cast<ServiceProxy &>(value_at_position(pos));
}

ServiceProxy & ListServiceProxies::registerService(const ProxyAddress & addrProxy, const ServiceStub & stubService)
{
    ListServiceProxies::LISTPOS pos = _findProxy(addrProxy);
    if (is_invalid_position(pos))
    {
        push_last(ServiceProxy(addrProxy));
        pos = last_position();
    }

    const StubAddress & addrStub = stubService.service_address();
    ServiceProxy & proxyService = value_at_position(pos);
    if ( addrStub == addrProxy)
    {
        if ( stubService.getServiceStatus() == NEService::ServiceConnectionState::Connected )
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
    for (ListServiceProxies::LISTPOS pos = first_position( ); is_valid_position(pos); pos = next_position(pos) )
    {
        const ServiceProxy & proxyService = value_at_position(pos);
        if ( proxyService == addrProxy )
        {
            result = proxyService;
            remove_at(pos);
            break;
        }
    }

    return result;
}

int32_t ListServiceProxies::stubServiceAvailable( const StubAddress & addrStub )
{
    int32_t result = 0;
    for ( LISTPOS pos = first_position(); is_valid_position(pos); pos = next_position(pos) )
    {
        ServiceProxy & proxyService = value_at_position(pos);
        result += proxyService.stubAvailable(addrStub) ? 1 : 0;
    }

    return result;
}

int32_t ListServiceProxies::stubServiceUnavailable()
{
    int32_t result = 0;
    for ( LISTPOS pos = first_position(); is_valid_position(pos); pos = next_position(pos) )
    {
        ServiceProxy & proxyService = value_at_position(pos);
        result += proxyService.stubUnavailable() ? 1 : 0;
    }
    return result;
}

int32_t ListServiceProxies::getSpecificService(ListServiceProxies & out_listProxies, const ITEM_ID & cookie)
{
    int32_t result = 0;
    for ( LISTPOS pos = first_position( ); is_valid_position(pos); pos = next_position(pos) )
    {
        ServiceProxy & proxyService = value_at_position(pos);
        if ( proxyService.service_address().cookie() == cookie )
        {
            result += 1;
            out_listProxies.push_last(proxyService);
        }
    }
    return result;
}

ListServiceProxies::LISTPOS ListServiceProxies::_findProxy(const ProxyAddress & addrProxy) const
{
    LISTPOS pos = first_position( );
    for ( ; is_valid_position(pos); pos = next_position(pos))
    {
        if ( value_at_position(pos) == addrProxy )
            break;
    }

    return pos;
}
