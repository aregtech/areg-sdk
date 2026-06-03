/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceResponseEvent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Response Event implementation.
 *
 ************************************************************************/
#include "areg/component/ServiceResponseEvent.hpp"
namespace areg {

ServiceResponseEvent::ServiceResponseEvent( const ProxyAddress & target
                                          , areg::ResultType result
                                          , uint32_t respId
                                          , areg::EventType eventType
                                          , const SequenceNumber & seqNr    /*= areg::SEQUENCE_NUMBER_NOTIFY*/)
    : ProxyEvent    (target, eventType)
{
    areg::EventHeader* hdr{ header() };
    if (hdr != nullptr)
    {
        hdr->messageId  = respId;
        hdr->result     = static_cast<uint32_t>(result);
        hdr->sequenceNr = seqNr;
    }
}

ServiceResponseEvent::ServiceResponseEvent( const EventEnvelope & envelope ) noexcept
    : ProxyEvent    (envelope)
{
}

ServiceResponseEvent::ServiceResponseEvent( const ProxyAddress& target, const ServiceResponseEvent& src )
    : ProxyEvent    (target, src.envelope().clone())
{
}

ServiceResponseEvent::ServiceResponseEvent( const ProxyAddress& target, EventEnvelope&& env )
    : ProxyEvent    (target, std::move(env))
{
}

ServiceResponseEvent ServiceResponseEvent::clone_for_target( const ProxyAddress & target ) const
{
    return ServiceResponseEvent(target, envelope().clone());
}

} // namespace areg
