/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/StubConnectEvent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Stub Connection event class implementation.
 *
 ************************************************************************/
#include "areg/component/private/StubConnectEvent.hpp"

#include "areg/component/StubAddress.hpp"
#include "areg/component/ProxyAddress.hpp"
namespace areg {

AREG_IMPLEMENT_RUNTIME_EVENT(StubConnectEvent, ServiceRequestEvent)

StubConnectEvent::StubConnectEvent(const StubAddress & stubTarget, areg::ServiceConnectionState connectStatus)
    : ServiceRequestEvent   ( ProxyAddress::invalid_proxy_address()
                            , stubTarget
                            , static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection)
                            , areg::RequestType::ServiceConnection
                            , areg::EventType::EventLocalProviderConnect)
{
    areg::EventHeader* hdr{ header() };
    if (hdr != nullptr)
    {
        hdr->result = static_cast<uint32_t>(connectStatus);
        hdr->eventId = StubConnectEvent::CLASS_ID;
    }
}

StubConnectEvent::StubConnectEvent(const ProxyAddress & proxyClient, const StubAddress & stubTarget, areg::ServiceConnectionState connectStatus)
    : ServiceRequestEvent   ( proxyClient
                            , stubTarget
                            , static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection)
                            , areg::RequestType::ClientConnection
                            , areg::EventType::EventLocalProviderConnect)
{
    areg::EventHeader* hdr{ header() };
    if (hdr != nullptr)
    {
        hdr->result = static_cast<uint32_t>(connectStatus);
        hdr->eventId = StubConnectEvent::CLASS_ID;
    }
}

} // namespace areg
