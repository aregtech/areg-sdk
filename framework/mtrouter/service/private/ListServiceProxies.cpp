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

namespace mtrouter
{

    const mtrouter::ServiceProxy     ListServiceProxies::InvalidProxyService;

    const mtrouter::ServiceProxy & ListServiceProxies::getService( const areg::ProxyAddress & addrProxy ) const
    {
        ListServiceProxies::LISTPOS pos = _findProxy(addrProxy);
        return ( isValidPosition(pos) ? static_cast<const mtrouter::ServiceProxy &>(valueAtPosition(pos)) : ListServiceProxies::InvalidProxyService );
    }

    mtrouter::ServiceProxy * ListServiceProxies::getService( const areg::ProxyAddress & addrProxy )
    {
        ListServiceProxies::LISTPOS pos = _findProxy(addrProxy);
        return ( isValidPosition(pos) ? static_cast<mtrouter::ServiceProxy *>(&valueAtPosition(pos)) : nullptr );
    }

    mtrouter::ServiceProxy & ListServiceProxies::registerService( const areg::ProxyAddress & addrProxy )
    {
        ListServiceProxies::LISTPOS pos = _findProxy(addrProxy);
        if ( isInvalidPosition(pos) )
        {
            pushLast(mtrouter::ServiceProxy(addrProxy));
            pos = lastPosition();
        }

        return static_cast<mtrouter::ServiceProxy &>(valueAtPosition(pos));
    }

    mtrouter::ServiceProxy & ListServiceProxies::registerService(const areg::ProxyAddress & addrProxy, const mtrouter::ServiceStub & stubService)
    {
        ListServiceProxies::LISTPOS pos = _findProxy(addrProxy);
        if (isInvalidPosition(pos))
        {
            pushLast(mtrouter::ServiceProxy(addrProxy));
            pos = lastPosition();
        }

        const areg::StubAddress & addrStub = stubService.getServiceAddress();
        mtrouter::ServiceProxy & proxyService = valueAtPosition(pos);
        if ( addrStub == addrProxy)
        {
            if ( stubService.getServiceStatus() == areg::ServiceConnectionState::Connected )
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

    mtrouter::ServiceProxy ListServiceProxies::unregisterService( const areg::ProxyAddress & addrProxy )
    {
        mtrouter::ServiceProxy result;
        for (ListServiceProxies::LISTPOS pos = firstPosition( ); isValidPosition(pos); pos = nextPosition(pos) )
        {
            const mtrouter::ServiceProxy & proxyService = valueAtPosition(pos);
            if ( proxyService == addrProxy )
            {
                result = proxyService;
                removeAt(pos);
                break;
            }
        }

        return result;
    }

    int32_t ListServiceProxies::stubServiceAvailable( const areg::StubAddress & addrStub )
    {
        int32_t result = 0;
        for ( LISTPOS pos = firstPosition(); isValidPosition(pos); pos = nextPosition(pos) )
        {
            mtrouter::ServiceProxy & proxyService = valueAtPosition(pos);
            result += proxyService.stubAvailable(addrStub) ? 1 : 0;
        }

        return result;
    }

    int32_t ListServiceProxies::stubServiceUnavailable()
    {
        int32_t result = 0;
        for ( LISTPOS pos = firstPosition(); isValidPosition(pos); pos = nextPosition(pos) )
        {
            mtrouter::ServiceProxy & proxyService = valueAtPosition(pos);
            result += proxyService.stubUnavailable() ? 1 : 0;
        }
        return result;
    }

    int32_t ListServiceProxies::getSpecificService(ListServiceProxies & out_listProxies, const ITEM_ID & cookie)
    {
        int32_t result = 0;
        for ( LISTPOS pos = firstPosition( ); isValidPosition(pos); pos = nextPosition(pos) )
        {
            mtrouter::ServiceProxy & proxyService = valueAtPosition(pos);
            if ( proxyService.getServiceAddress().getCookie() == cookie )
            {
                result += 1;
                out_listProxies.pushLast(proxyService);
            }
        }
        return result;
    }

    ListServiceProxies::LISTPOS ListServiceProxies::_findProxy(const areg::ProxyAddress & addrProxy) const
    {
        LISTPOS pos = firstPosition( );
        for ( ; isValidPosition(pos); pos = nextPosition(pos))
        {
            if ( valueAtPosition(pos) == addrProxy )
                break;
        }

        return pos;
    }
} // namespace mtrouter
