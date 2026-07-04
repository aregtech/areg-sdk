/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        mtrouter/service/private/ListServiceProxies.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Proxy Service object
 ************************************************************************/
#include "mtrouter/service/private/ListServiceProxies.hpp"
#include "mtrouter/service/private/ServiceStub.hpp"
#include "areg/component/StubAddress.hpp"

const ServiceProxy     ListServiceProxies::InvalidProxyService;

const ServiceProxy & ListServiceProxies::service( const areg::ProxyAddress & addrProxy ) const
{
    ListServiceProxies::LISTPOS pos = _find_proxy(addrProxy);
    return ( is_valid_position(pos) ? static_cast<const ServiceProxy &>(value_at(pos)) : ListServiceProxies::InvalidProxyService );
}

ServiceProxy * ListServiceProxies::service( const areg::ProxyAddress & addrProxy )
{
    ListServiceProxies::LISTPOS pos = _find_proxy(addrProxy);
    return ( is_valid_position(pos) ? static_cast<ServiceProxy *>(&value_at(pos)) : nullptr );
}

ServiceProxy & ListServiceProxies::register_service( const areg::ProxyAddress & addrProxy )
{
    ListServiceProxies::LISTPOS pos = _find_proxy(addrProxy);
    if ( !is_valid_position(pos) )
    {
        push_last(ServiceProxy(addrProxy));
        pos = last_position();
    }

    return static_cast<ServiceProxy &>(value_at(pos));
}

ServiceProxy & ListServiceProxies::register_service(const areg::ProxyAddress & addrProxy, const ServiceStub & stubService)
{
    ListServiceProxies::LISTPOS pos = _find_proxy(addrProxy);
    if (!is_valid_position(pos))
    {
        push_last(ServiceProxy(addrProxy));
        pos = last_position();
    }

    const areg::StubAddress & addrStub = stubService.service_address();
    ServiceProxy & proxyService = value_at(pos);
    if ( addrStub == addrProxy)
    {
        if ( stubService.service_status() == areg::ServiceConnectionState::Connected )
        {
            proxyService.stub_available( addrStub );
        }
        else
        {
            proxyService.stub_unavailable( );
        }
    }
    return proxyService;
}

ServiceProxy ListServiceProxies::unregister_service( const areg::ProxyAddress & addrProxy )
{
    ServiceProxy result;
    for (ListServiceProxies::LISTPOS pos = first_position( ); is_valid_position(pos); pos = next_position(pos) )
    {
        const ServiceProxy & proxyService = value_at(pos);
        if ( proxyService == addrProxy )
        {
            result = proxyService;
            remove_at(pos);
            break;
        }
    }

    return result;
}

int32_t ListServiceProxies::stub_service_available( const areg::StubAddress & addrStub )
{
    int32_t result = 0;
    for ( LISTPOS pos = first_position(); is_valid_position(pos); pos = next_position(pos) )
    {
        ServiceProxy & proxyService = value_at(pos);
        result += proxyService.stub_available(addrStub) ? 1 : 0;
    }

    return result;
}

int32_t ListServiceProxies::stub_service_unavailable()
{
    int32_t result = 0;
    for ( LISTPOS pos = first_position(); is_valid_position(pos); pos = next_position(pos) )
    {
        ServiceProxy & proxyService = value_at(pos);
        result += proxyService.stub_unavailable() ? 1 : 0;
    }
    return result;
}

int32_t ListServiceProxies::specific_service(ListServiceProxies & out_listProxies, const ITEM_ID & cookie)
{
    int32_t result = 0;
    for ( LISTPOS pos = first_position( ); is_valid_position(pos); pos = next_position(pos) )
    {
        ServiceProxy & proxyService = value_at(pos);
        if ( proxyService.service_address().cookie() == cookie )
        {
            result += 1;
            out_listProxies.push_last(proxyService);
        }
    }
    return result;
}

ListServiceProxies::LISTPOS ListServiceProxies::_find_proxy(const areg::ProxyAddress & addrProxy) const
{
    LISTPOS pos = first_position( );
    for ( ; is_valid_position(pos); pos = next_position(pos))
    {
        if (value_at(pos) == addrProxy )
            break;
    }

    return pos;
}
