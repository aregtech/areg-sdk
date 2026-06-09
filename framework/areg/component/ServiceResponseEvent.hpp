#ifndef AREG_COMPONENT_SERVICERESPONSEEVENT_HPP
#define AREG_COMPONENT_SERVICERESPONSEEVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/ServiceResponseEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Response Event.
 *              Base Service Response Event class to send events to
 *              Proxy, trigger updates and notification calls.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/ProxyEvent.hpp"
#include "areg/component/ServiceDefs.hpp"
namespace areg {

/************************************************************************
 * Dependencies.
 ************************************************************************/
class ProxyAddress;

/**
 * \brief       The Service Response class is a base class for all kind
 *              response events sent from Stub to Proxy after processing
 *              request calls. Normally, this is either request call 
 *              response or Attribute update notification. It is derived
 *              from ProxyEvent.
 * 
 * \details     This class contains response ID, result, service interface
 *              version and executed request sequence number. As well as
 *              this should be response call to notify Proxy about server
 *              connect and disconnect status update.
 *
 **/
//////////////////////////////////////////////////////////////////////////
// ServiceResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for all response events sent from Stub to Proxy after processing request
 *          calls. Handles response delivery and attribute update notifications.
 **/
class AREG_API ServiceResponseEvent    : public ProxyEvent
{
//////////////////////////////////////////////////////////////////////////
// Friend declarations
//////////////////////////////////////////////////////////////////////////
    friend class StubBase;   // needs delete on heap-allocated ServiceResponseEvent shells

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Creates service response event and sets parameters.
     *
     * \param   target      The event target proxy address
     * \param   result      The response result
     * \param   respId      The response message ID
     * \param   eventType   The type of event.
     * \param   seqNr       The sequence number of call.
     * \param   initSize    Payload bytes to reserve after the header for serialized parameters.
     **/
    inline ServiceResponseEvent( const ProxyAddress & target
                               , areg::ResultType result
                               , uint32_t respId
                               , areg::EventType eventType
                               , const SequenceNumber & seqNr = areg::SEQUENCE_NUMBER_NOTIFY
                               , uint32_t initSize = 0u );

    /**
     * \brief   Constructs from a received MessageEnvelope.
     **/
    explicit inline ServiceResponseEvent( const MessageEnvelope & envelope ) noexcept;

    /**
     * \brief   Copies all data from given source, except the target proxy address. This is used if
     *          proxy needs to clone event data to send to different target proxy objects.
     *
     * \param   target      The target proxy address
     * \param   src         The service response source to copy data.
     **/
    inline ServiceResponseEvent(const ProxyAddress & target, const ServiceResponseEvent & src );

    /**
     * \brief   Constructs from a cloned MessageEnvelope and redirects the consumer endpoint to target.
     *          Used by clone_for_target() to create a lightweight copy sharing EventHeader data.
     *
     * \param   target  The new consumer proxy address (written into the cloned header).
     * \param   env     A deep-copied MessageEnvelope; its consumer endpoint is updated to target.
     **/
    inline ServiceResponseEvent(const ProxyAddress & target, MessageEnvelope && env);

    /**
     * \brief   Creates service response event and sets parameters.
     *
     * \param   consumer    The target consumer to send event
     * \param   service     The raw service structure
     * \param   result      The response result
     * \param   respId      The response message ID
     * \param   eventType   The type of event.
     * \param   seqNr       The sequence number of call.
     * \param   initSize    Payload bytes to reserve after the header for serialized parameters.
     **/
    inline ServiceResponseEvent( const areg::Endpoint& consumer
                               , const areg::RawService& service
                               , areg::ResultType result
                               , uint32_t respId
                               , areg::EventType eventType
                               , const SequenceNumber & seqNr = areg::SEQUENCE_NUMBER_NOTIFY
                               , uint32_t initSize = 0u );

    inline ServiceResponseEvent(const areg::Endpoint& consumer, const areg::RawService& service, const ServiceResponseEvent& src);

    inline ServiceResponseEvent(const areg::Endpoint& consumer, const areg::RawService& service, MessageEnvelope&& env);

    ServiceResponseEvent(const ServiceResponseEvent& /*src*/) = default;

    ServiceResponseEvent(ServiceResponseEvent&& /*src*/) noexcept = default;

    ~ServiceResponseEvent() override = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns response message ID.
     **/
    [[nodiscard]]
    inline uint32_t response_id() const noexcept;

    /**
     * \brief   Returns sequence number stored in the EventHeader.
     **/
    [[nodiscard]]
    inline const SequenceNumber & sequence_number() const noexcept;

    /**
     * \brief   Sets new sequence number stored in the EventHeader.
     **/
    inline void set_sequence_number(const SequenceNumber & newSeqNr) noexcept;

    /**
     * \brief   Returns the IPC routing cookie of the consumer (proxy) endpoint.
     **/
    [[nodiscard]]
    inline uint32_t target_cookie() const noexcept;

    /**
     * \brief   Returns a deep-copied event retargeted to the given proxy. The clone shares no
     *          buffer with the original (independent serialization state).
     *
     * \param   target      Proxy address for the cloned event's consumer endpoint.
     * \return  A new ServiceResponseEvent value with an independent buffer and updated consumer.
     **/
    [[nodiscard]]
    inline ServiceResponseEvent clone_for_target(const ProxyAddress & target) const;

    inline ServiceResponseEvent clone_for_target(const areg::Endpoint& consumer, const areg::RawService& service) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceResponseEvent() = delete;
};

//////////////////////////////////////////////////////////////////////////
// ServiceResponseEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline ServiceResponseEvent::ServiceResponseEvent( const ProxyAddress & target
                                                 , areg::ResultType result
                                                 , uint32_t respId
                                                 , areg::EventType eventType
                                                 , const SequenceNumber & seqNr    /*= areg::SEQUENCE_NUMBER_NOTIFY*/
                                                 , uint32_t initSize               /*= 0u*/ )
    : ProxyEvent    (target, eventType, initSize)
{
    areg::EventHeader* hdr{ header() };
    if (hdr != nullptr)
    {
        hdr->messageId  = respId;
        hdr->result     = static_cast<uint32_t>(result);
        hdr->sequenceNr = seqNr;
    }
}

inline ServiceResponseEvent::ServiceResponseEvent( const MessageEnvelope & envelope ) noexcept
    : ProxyEvent    (envelope)
{
}

inline ServiceResponseEvent::ServiceResponseEvent( const ProxyAddress& target, const ServiceResponseEvent& src )
    : ProxyEvent    (target, src.envelope().clone())
{
}

inline ServiceResponseEvent::ServiceResponseEvent( const ProxyAddress& target, MessageEnvelope&& env )
    : ProxyEvent    (target, std::move(env))
{
}

inline ServiceResponseEvent::ServiceResponseEvent( const areg::Endpoint& consumer
                                                 , const areg::RawService& service
                                                 , areg::ResultType result
                                                 , uint32_t respId
                                                 , areg::EventType eventType
                                                 , const SequenceNumber& seqNr
                                                 , uint32_t initSize)
    : ProxyEvent(consumer, service, eventType, initSize)
{
    areg::EventHeader* hdr{ header() };
    if (hdr != nullptr)
    {
        hdr->messageId = respId;
        hdr->result = static_cast<uint32_t>(result);
        hdr->sequenceNr = seqNr;
    }
}

inline ServiceResponseEvent::ServiceResponseEvent(const areg::Endpoint& consumer, const areg::RawService& service, const ServiceResponseEvent& src)
    : ProxyEvent(consumer, service, src.envelope().clone())
{
}

inline ServiceResponseEvent::ServiceResponseEvent(const areg::Endpoint& consumer, const areg::RawService& service, MessageEnvelope&& env)
    : ProxyEvent(consumer, service, std::move(env))
{
}

inline ServiceResponseEvent ServiceResponseEvent::clone_for_target( const ProxyAddress & target ) const
{
    return ServiceResponseEvent(target, envelope().clone());
}

inline ServiceResponseEvent ServiceResponseEvent::clone_for_target(const areg::Endpoint& consumer, const areg::RawService& service) const
{
    return ServiceResponseEvent(consumer, service, envelope().clone());
}

inline uint32_t ServiceResponseEvent::response_id() const noexcept
{
    return message_id();
}

inline const SequenceNumber & ServiceResponseEvent::sequence_number() const noexcept
{
    return sequence();
}

inline void ServiceResponseEvent::set_sequence_number(const SequenceNumber & newSeqNr) noexcept
{
    set_sequence(newSeqNr);
}

inline uint32_t ServiceResponseEvent::target_cookie() const noexcept
{
    return consumer_id();
}

} // namespace areg
#endif  // AREG_COMPONENT_SERVICERESPONSEEVENT_HPP
