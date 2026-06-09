#ifndef AREG_COMPONENT_RESPONSEEVENTS_HPP
#define AREG_COMPONENT_RESPONSEEVENTS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/ResponseEvents.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Generic Local and Remote Request 
 *              Event object for local and remote communication
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/ServiceResponseEvent.hpp"

#include "areg/base/SharedBuffer.hpp"
namespace areg {

/************************************************************************
 * List of declared classes:
 ************************************************************************/
// ServiceRequestEvent;
    class LocalResponseEvent;
    class RemoteResponseEvent;

/************************************************************************
 * \brief   In this file following classes are declared:
 *              1.      LocalResponseEvent
 *              2.      RemoteResponseEvent
 *          These are base classes for communication, used to send
 *          Data in Response objects for local and remote communication.
 *          For more information, see description bellow.
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// LocalResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * LocalResponseEvent class, used to send responses
 ************************************************************************/
/**
 * \brief   Generic Local Response Event object for triggering response or attribute update events
 *          on Proxy side within same process.
 **/
class AREG_API LocalResponseEvent : public    ServiceResponseEvent
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes local event with target address, result type, and response ID.
     *
     * \param   proxyTarget     The address of target Proxy
     * \param   result          The type of result to indicate whether it is response or update
     *                          event, as well as to specify message validation flag.
     * \param   respId          The ID of response. Can also be update ID.
     * \param   seqNr           The call sequence number.
     * \param   initSize        Payload bytes to reserve after the header for serialized parameters.
     **/
    inline LocalResponseEvent( const ProxyAddress & proxyTarget
                             , areg::ResultType result
                             , uint32_t respId
                             , const SequenceNumber & seqNr = areg::SEQUENCE_NUMBER_NOTIFY
                             , uint32_t initSize = 0);

    /**
     * \brief   Initializes local event with serialized arguments, target address, result type, and response ID.
     *
     * \param   args            Shared Buffer object with information of serialized parameters.
     * \param   proxyTarget     The address of target Proxy
     * \param   result          The type of result to indicate whether it is response or update
     *                          event, as well as to specify message validation flag.
     * \param   respId          The ID of response. Can also be update ID.
     * \param   seqNr           The call sequence number.
     **/
    inline LocalResponseEvent( const SharedBuffer & args
                             , const ProxyAddress & proxyTarget
                             , areg::ResultType result
                             , uint32_t respId
                             , const SequenceNumber & seqNr = areg::SEQUENCE_NUMBER_NOTIFY);

    /**
     * \brief   Clones existing information, except target Proxy address to send same message to different Proxies.
     *
     * \param   proxyTarget     The address of target Proxy
     * \param   src             The Event source to copy data.
     **/
    inline LocalResponseEvent(const ProxyAddress& proxyTarget, const LocalResponseEvent& src);

    /**
     * \brief   Initializes local event with target consumer data, result type, and response ID.
     *
     * \param   consumer        The target consumer structure
     * \param   service         Indicates the service to send the event
     * \param   result          The type of result to indicate whether it is response or update
     *                          event, as well as to specify message validation flag.
     * \param   respId          The ID of response. Can also be update ID.
     * \param   seqNr           The call sequence number.
     * \param   initSize        Payload bytes to reserve after the header for serialized parameters.
     **/
    inline LocalResponseEvent( const areg::Endpoint & consumer
                             , const areg::RawService& service
                             , areg::ResultType result
                             , uint32_t respId
                             , const SequenceNumber & seqNr = areg::SEQUENCE_NUMBER_NOTIFY
                             , uint32_t initSize = 0);

    /**
     * \brief   Initializes local event with serialized arguments, target consumer data, result type, and response ID.
     *
     * \param   args            Shared Buffer object with information of serialized parameters.
     * \param   consumer        The target consumer structure
     * \param   service         Indicates the service to send the event
     * \param   result          The type of result to indicate whether it is response or update
     *                          event, as well as to specify message validation flag.
     * \param   respId          The ID of response. Can also be update ID.
     * \param   seqNr           The call sequence number.
     **/
    inline LocalResponseEvent( const SharedBuffer & args
                             , const areg::Endpoint & consumer
                             , const areg::RawService& service
                             , areg::ResultType result
                             , uint32_t respId
                             , const SequenceNumber & seqNr = areg::SEQUENCE_NUMBER_NOTIFY);

    /**
     * \brief   Clones existing information, except target consumer and service information to send same message to different Proxies.
     *
     * \param   proxyTarget     The address of target Proxy
     * \param   src             The Event source to copy data.
     **/
    inline LocalResponseEvent(const areg::Endpoint& consumer, const areg::RawService& service, const LocalResponseEvent& src);

    /**
     * \brief   Constructs from a received MessageEnvelope.
     **/
    explicit inline LocalResponseEvent(const MessageEnvelope& envelope) noexcept;

    LocalResponseEvent(const LocalResponseEvent& /*src*/) = default;

    LocalResponseEvent(LocalResponseEvent&& /*src*/) noexcept = default;

    ~LocalResponseEvent() override = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LocalResponseEvent() = delete;
};

//////////////////////////////////////////////////////////////////////////
// RemoteResponseEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * RemoteResponseEvent class, used to send responses
 ************************************************************************/
/**
 * \brief   Generic Remote Response Event object for triggering response or attribute update events
 *          on Proxy side.
 **/
class AREG_API RemoteResponseEvent: public    ServiceResponseEvent
{
    friend class RemoteEventFactory;
    friend class ProxyBase;   // needs new RemoteResponseEvent(envelope) on IPC receive path
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes remote event with target address, result type, and response ID.
     *
     * \param   proxyTarget     The address of target Proxy
     * \param   result          The type of result to indicate whether it is response or update
     *                          event, as well as to specify message validation flag.
     * \param   respId          The ID of response. Can also be update ID.
     * \param   seqNr           The call sequence number.
     * \param   initSize        Payload bytes to reserve after the header for serialized parameters.
     **/
    inline RemoteResponseEvent( const ProxyAddress & proxyTarget
                              , areg::ResultType result
                              , uint32_t respId
                              , const SequenceNumber & seqNr = areg::SEQUENCE_NUMBER_NOTIFY
                              , uint32_t initSize = 0u );

    /**
     * \brief   Initializes remote event with serialized arguments, target address, result type, and response ID.
     *
     * \param   args            Shared Buffer object with information of serialized parameters.
     * \param   proxyTarget     The address of target Proxy
     * \param   result          The type of result to indicate whether it is response or update
     *                          event, as well as to specify message validation flag.
     * \param   respId          The ID of response. Can also be update ID.
     * \param   seqNr           The call sequence number.
     **/
    inline RemoteResponseEvent( const SharedBuffer & args
                              , const ProxyAddress & proxyTarget
                              , areg::ResultType result
                              , uint32_t respId
                              , const SequenceNumber & seqNr = areg::SEQUENCE_NUMBER_NOTIFY);

    /**
     * \brief   Clones existing information, except target Proxy address to send same message to
     *          different Proxies.
     *
     * \param   proxyTarget     The address of target Proxy
     * \param   src             The Event source to copy data.
     **/
    inline RemoteResponseEvent(const ProxyAddress & proxyTarget, const RemoteResponseEvent & src);

    /**
     * \brief   Initializes remote event with target consumer data, result type, and response ID.
     *
     * \param   consumer        The target consumer structure
     * \param   service         Indicates the service to send the event
     * \param   result          The type of result to indicate whether it is response or update
     *                          event, as well as to specify message validation flag.
     * \param   respId          The ID of response. Can also be update ID.
     * \param   seqNr           The call sequence number.
     * \param   initSize        Payload bytes to reserve after the header for serialized parameters.
     **/
    inline RemoteResponseEvent( const areg::Endpoint & consumer
                              , const areg::RawService& service
                              , areg::ResultType result
                              , uint32_t respId
                              , const SequenceNumber & seqNr = areg::SEQUENCE_NUMBER_NOTIFY
                              , uint32_t initSize = 0);

    /**
     * \brief   Initializes remote event with serialized arguments, target consumer data, result type, and response ID.
     *
     * \param   args            Shared Buffer object with information of serialized parameters.
     * \param   consumer        The target consumer structure
     * \param   service         Indicates the service to send the event
     * \param   result          The type of result to indicate whether it is response or update
     *                          event, as well as to specify message validation flag.
     * \param   respId          The ID of response. Can also be update ID.
     * \param   seqNr           The call sequence number.
     **/
    inline RemoteResponseEvent( const SharedBuffer & args
                              , const areg::Endpoint & consumer
                              , const areg::RawService& service
                              , areg::ResultType result
                              , uint32_t respId
                              , const SequenceNumber & seqNr = areg::SEQUENCE_NUMBER_NOTIFY);

    /**
     * \brief   Clones existing information, except target consumer and service information to send same message to different Proxies.
     *
     * \param   proxyTarget     The address of target Proxy
     * \param   src             The Event source to copy data.
     **/
    inline RemoteResponseEvent(const areg::Endpoint& consumer, const areg::RawService& service, const RemoteResponseEvent& src);

    /**
     * \brief   Constructs from a received MessageEnvelope.
     **/
    explicit inline RemoteResponseEvent( const MessageEnvelope & envelope ) noexcept;

    RemoteResponseEvent(const RemoteResponseEvent& /*src*/) = default;

    RemoteResponseEvent(RemoteResponseEvent&& /*src*/) noexcept = default;

    ~RemoteResponseEvent() override = default;

//////////////////////////////////////////////////////////////////////////////
// Protected operations
//////////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Sets the target communication channel object.
     *
     * \param   channel     The communication channel to set in remote event.
     **/
    inline void set_target_channel( const Channel & channel ) noexcept;

    /**
     * \brief   Returns the event communication channel object.
     **/
    inline Channel target_channel() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    RemoteResponseEvent() = delete;
};

//////////////////////////////////////////////////////////////////////////
// LocalResponseEvent class Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
inline LocalResponseEvent::LocalResponseEvent( const ProxyAddress & proxyTarget
                                             , areg::ResultType result
                                             , uint32_t respId
                                             , const SequenceNumber & seqNr    /*= areg::SEQUENCE_NUMBER_NOTIFY*/
                                             , uint32_t initSize /*= 0*/)
    : ServiceResponseEvent(proxyTarget, result, respId, areg::EventType::EventLocalResponse, seqNr, initSize)
{
}

inline LocalResponseEvent::LocalResponseEvent( const SharedBuffer & args
                                             , const ProxyAddress & proxyTarget
                                             , areg::ResultType result
                                             , uint32_t respId
                                             , const SequenceNumber & seqNr  /*= areg::SEQUENCE_NUMBER_NOTIFY*/)
    : ServiceResponseEvent(proxyTarget, result, respId, areg::EventType::EventLocalResponse, seqNr, 0u)
{
    if (args.is_valid())
        write_data(args.buffer(), args.size_used());
}

inline LocalResponseEvent::LocalResponseEvent(const ProxyAddress& proxyTarget, const LocalResponseEvent& src)
    : ServiceResponseEvent(proxyTarget, static_cast<const ServiceResponseEvent&>(src))
{
}

inline LocalResponseEvent::LocalResponseEvent( const areg::Endpoint& consumer
                                             , const areg::RawService& service
                                             , areg::ResultType result
                                             , uint32_t respId
                                             , const SequenceNumber& seqNr
                                             , uint32_t initSize)
    : ServiceResponseEvent(consumer, service, result, respId, areg::EventType::EventLocalResponse, seqNr, initSize)
{
}

inline LocalResponseEvent::LocalResponseEvent( const SharedBuffer& args
                                             , const areg::Endpoint& consumer
                                             , const areg::RawService& service
                                             , areg::ResultType result
                                             , uint32_t respId
                                             , const SequenceNumber& seqNr)
    : ServiceResponseEvent(consumer, service, result, respId, areg::EventType::EventLocalResponse, seqNr, 0u)
{
    if (args.is_valid())
        write_data(args.buffer(), args.size_used());
}

inline LocalResponseEvent::LocalResponseEvent(const areg::Endpoint& consumer, const areg::RawService& service, const LocalResponseEvent& src)
    : ServiceResponseEvent(consumer, service, src)
{
}

inline LocalResponseEvent::LocalResponseEvent(const MessageEnvelope& envelope) noexcept
    : ServiceResponseEvent(envelope)
{
}

//////////////////////////////////////////////////////////////////////////
// RemoteResponseEvent class Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
inline RemoteResponseEvent::RemoteResponseEvent( const ProxyAddress & proxyTarget
                                               , areg::ResultType result
                                               , uint32_t respId
                                               , const SequenceNumber & seqNr  /*= areg::SEQUENCE_NUMBER_NOTIFY*/
                                               , uint32_t initSize             /*= 0u*/ )
    : ServiceResponseEvent(proxyTarget, result, respId, areg::EventType::EventRemoteResponse, seqNr, initSize)
{
}

inline RemoteResponseEvent::RemoteResponseEvent( const SharedBuffer & args
                                               , const ProxyAddress & proxyTarget
                                               , areg::ResultType result
                                               , uint32_t respId
                                               , const SequenceNumber & seqNr  /*= areg::SEQUENCE_NUMBER_NOTIFY*/)
    : ServiceResponseEvent(proxyTarget, result, respId, areg::EventType::EventRemoteResponse, seqNr)
{
    if (args.is_valid())
        write_data(args.buffer(), args.size_used());
}

inline RemoteResponseEvent::RemoteResponseEvent( const ProxyAddress& proxyTarget, const RemoteResponseEvent & src )
    : ServiceResponseEvent(proxyTarget, static_cast<const ServiceResponseEvent&>(src))
{
}

inline RemoteResponseEvent::RemoteResponseEvent( const areg::Endpoint& consumer
                                             , const areg::RawService& service
                                             , areg::ResultType result
                                             , uint32_t respId
                                             , const SequenceNumber& seqNr
                                             , uint32_t initSize)
    : ServiceResponseEvent(consumer, service, result, respId, areg::EventType::EventRemoteResponse, seqNr, initSize)
{
}

inline RemoteResponseEvent::RemoteResponseEvent( const SharedBuffer& args
                                             , const areg::Endpoint& consumer
                                             , const areg::RawService& service
                                             , areg::ResultType result
                                             , uint32_t respId
                                             , const SequenceNumber& seqNr)
    : ServiceResponseEvent(consumer, service, result, respId, areg::EventType::EventRemoteResponse, seqNr, 0u)
{
    if (args.is_valid())
        write_data(args.buffer(), args.size_used());
}

inline RemoteResponseEvent::RemoteResponseEvent(const areg::Endpoint& consumer, const areg::RawService& service, const RemoteResponseEvent& src)
    : ServiceResponseEvent(consumer, service, src)
{
}

inline RemoteResponseEvent::RemoteResponseEvent( const MessageEnvelope & envelope ) noexcept
    : ServiceResponseEvent( envelope )
{
}

//////////////////////////////////////////////////////////////////////////
// RemoteResponseEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline void RemoteResponseEvent::set_target_channel(const Channel & channel) noexcept
{
    areg::EventHeader* hdr{ header() };
    ASSERT(hdr != nullptr);
    hdr->target         = channel.cookie();
    hdr->channel        = channel.target();
    hdr->consumer.id    = channel.cookie();
    hdr->consumer.thread= channel.source();
}

inline Channel RemoteResponseEvent::target_channel() const noexcept
{
    const areg::EventHeader* hdr{ header() };
    ASSERT(hdr != nullptr);
    return Channel(hdr->consumer.thread, hdr->channel, hdr->consumer.id);
}

} // namespace areg
#endif  // AREG_COMPONENT_RESPONSEEVENTS_HPP
