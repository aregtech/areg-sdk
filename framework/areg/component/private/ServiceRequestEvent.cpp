/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceRequestEvent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Request Event implementation.
 *
 ************************************************************************/
#include "areg/component/ServiceRequestEvent.hpp"

#include "areg/component/ProxyAddress.hpp"
#include "areg/component/StubAddress.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ServiceRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

ServiceRequestEvent::ServiceRequestEvent( const ProxyAddress & proxyAddress
                                        , const StubAddress  & target
                                        , uint32_t reqId
                                        , areg::RequestType reqType
                                        , areg::EventType eventType
                                        , uint32_t initSize /*= 0u*/ )
    : StubEvent (target, eventType, initSize)
{
    areg::EventHeader* hdr{ header() };
    if (hdr != nullptr)
    {
        proxyAddress.to_event(*hdr);
        hdr->messageId = reqId;
        hdr->result    = static_cast<uint32_t>(reqType);
    }
}

ServiceRequestEvent::ServiceRequestEvent( const EventEnvelope & envelope ) noexcept
    : StubEvent (envelope)
{
}

} // namespace areg
