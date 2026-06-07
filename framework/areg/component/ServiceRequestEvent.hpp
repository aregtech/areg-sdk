#ifndef AREG_COMPONENT_SERVICEREQUESTEVENT_HPP
#define AREG_COMPONENT_SERVICEREQUESTEVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/ServiceRequestEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Request Event.
 *              Base Service Request event class to send events to Stub
 *              and trigger function call.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/StubEvent.hpp"

#include "areg/component/ProxyAddress.hpp"
#include "areg/component/ServiceDefs.hpp"
namespace areg {

/************************************************************************
 * Dependencies.
 ************************************************************************/
class StubAddress;
class ComponentAddress;

//////////////////////////////////////////////////////////////////////////
// ServiceRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for all requests sent from Proxy to Stub. All data lives in
 *          the EventHeader; no extra member variables are stored.
 *
 *          EventHeader layout used by this class:
 *            - provider  endpoint : StubAddress  (via StubAddress::to_event)
 *            - consumer  endpoint : ProxyAddress (via ProxyAddress::to_event)
 *            - messageId          : request message ID
 *            - result             : RequestType (cast from/to areg::RequestType)
 *            - sequenceNr         : sequence number
 **/
class AREG_API ServiceRequestEvent : public StubEvent
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Service Event object and stores all fields in EventHeader.
     *
     * \param   proxyAddress    The source Proxy Address, which sent event.
     * \param   target          The target Stub Address, which should process event
     * \param   reqId           The request message ID to process.
     * \param   reqType         The request type.
     * \param   eventType       The type of event.
     * \param   initSize        Payload bytes to reserve after the header for the serialized request
     *                          parameters. 0 keeps the default block size.
     **/
    inline ServiceRequestEvent( const ProxyAddress & proxyAddress
                              , const StubAddress & target
                              , uint32_t reqId
                              , areg::RequestType reqType
                              , areg::EventType eventType
                              , uint32_t initSize = 0u );

    /**
     * \brief   Constructs from a received MessageEnvelope.
     *          All routing data is read from EventHeader fields; payload contains serialized params.
     **/
    explicit inline ServiceRequestEvent( const MessageEnvelope & envelope ) noexcept;

    ServiceRequestEvent(const ServiceRequestEvent& /*src*/) = default;

    ServiceRequestEvent(ServiceRequestEvent&& /*src*/) noexcept = default;

    ~ServiceRequestEvent() override = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the address of Proxy event source, reconstructed from consumer endpoint.
     **/
    [[nodiscard]]
    inline ProxyAddress event_source() const noexcept;

    /**
     * \brief   Returns request message ID from EventHeader.
     **/
    [[nodiscard]]
    inline uint32_t request_id() const noexcept;

    /**
     * \brief   Returns request type from EventHeader result field.
     **/
    [[nodiscard]]
    inline areg::RequestType request_type() const noexcept;

    /**
     * \brief   Returns sequence number from EventHeader.
     **/
    [[nodiscard]]
    inline const SequenceNumber & sequence_number() const noexcept;

    /**
     * \brief   Sets new sequence number in EventHeader.
     *
     * \param   newSeqNr    The new sequence number to set.
     **/
    inline void set_sequence_number(const SequenceNumber & newSeqNr) noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceRequestEvent() = delete;
};

//////////////////////////////////////////////////////////////////////////
// ServiceRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

inline ServiceRequestEvent::ServiceRequestEvent( const ProxyAddress & proxyAddress
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

inline ServiceRequestEvent::ServiceRequestEvent( const MessageEnvelope & envelope ) noexcept
    : StubEvent (envelope)
{
}

//////////////////////////////////////////////////////////////////////////
// ServiceRequestEvent class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline ProxyAddress ServiceRequestEvent::event_source() const noexcept
{
    const areg::EventHeader* hdr{ header() };
    return (hdr != nullptr ? ProxyAddress(*hdr) : ProxyAddress::invalid_proxy_address());
}

inline uint32_t ServiceRequestEvent::request_id() const noexcept
{
    return message_id();
}

inline areg::RequestType ServiceRequestEvent::request_type() const noexcept
{
    return static_cast<areg::RequestType>(result());
}

inline const SequenceNumber & ServiceRequestEvent::sequence_number() const noexcept
{
    return sequence();
}

inline void ServiceRequestEvent::set_sequence_number(const SequenceNumber & newSeqNr ) noexcept
{
    set_sequence(newSeqNr);
}

} // namespace areg
#endif  // AREG_COMPONENT_SERVICEREQUESTEVENT_HPP
