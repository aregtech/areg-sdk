#ifndef AREG_COMPONENT_REQUESTEVENTS_HPP
#define AREG_COMPONENT_REQUESTEVENTS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/RequestEvents.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Generic Local and Remote Request 
 *              Event object for local and remote communication
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/ServiceRequestEvent.hpp"

#include "areg/base/SharedBuffer.hpp"
namespace areg {

/************************************************************************
 * List of declared classes:
 ************************************************************************/
// ServiceRequestEvent;
    class LocalRequestEvent;
    class RemoteRequestEvent;
    class LocalNotifyRequestEvent;
    class RemoteNotifyRequestEvent;
/************************************************************************
 * \brief   In this file following request event classes are declared:
 *              1. Request function call events:
 *                  a.      LocalRequestEvent
 *                  b.      RemoteRequestEvent
 *              2. Notification request events:
 *                  a.      LocalNotifyRequestEvent
 *                  b.      RemoteNotifyRequestEvent
 *          These are base classes for communication, used to send
 *          Data in Request objects for local and remote communication.
 *          For more information, see description bellow.
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// LocalRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * LocalRequestEvent class, sends request event
 ************************************************************************/
/**
 * \brief   Event for triggering a service request on the stub side within the same process.
 **/
class AREG_API LocalRequestEvent  : public    ServiceRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes a request event with no data. Use when the request has no parameters.
     *
     * \param   fromSource      The address of the source proxy.
     * \param   toTarget        The address of the target stub.
     * \param   reqId           The ID of the request.
     * \param   initSize        The payload initial size to reserve. If zero, nothing is reserved.
     **/
    inline LocalRequestEvent(const ProxyAddress & fromSource, const StubAddress & toTarget, uint32_t reqId, uint32_t initSize = 0);

    /**
     * \brief   Initializes a request event with data. Use when the request has parameters.
     *
     * \param   args            Serialized request parameters (SharedBuffer payload, copied in).
     * \param   fromSource      The address of the source proxy.
     * \param   toTarget        The address of the target stub.
     * \param   reqId           The ID of the request.
     **/
    inline LocalRequestEvent( const SharedBuffer & args
                            , const ProxyAddress & fromSource
                            , const StubAddress & toTarget
                            , uint32_t reqId);

    /**
     * \brief   Constructs from a received MessageEnvelope
     *
     * \param   envelope    The received event envelope containing header and payload.
     **/
    explicit inline LocalRequestEvent(const MessageEnvelope& envelope) noexcept;

    LocalRequestEvent(const LocalRequestEvent& /*src*/) = default;

    LocalRequestEvent(LocalRequestEvent&& /*src*/) noexcept = default;

    ~LocalRequestEvent() override = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LocalRequestEvent() = delete;
};

//////////////////////////////////////////////////////////////////////////
// RemoteRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * RemoteRequestEvent class, sends request event
 ************************************************************************/
/**
 * \brief   Generic Remote Request Event object to trigger requests on Stub side within different
 *          processes via service routing module.
 **/
class AREG_API RemoteRequestEvent : public    ServiceRequestEvent
{
    friend class RemoteEventFactory;
    friend class StubBase;   // needs new RemoteRequestEvent(envelope) on IPC receive path

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates event with no data. Initializes event source, target address, and message ID.
     *          Use if request has no parameters.
     *
     * \param   fromSource      The address of source Proxy.
     * \param   toTarget        The address of Stub target
     * \param   reqId           The ID of request.
     * \param   initSize        Payload bytes to reserve after the header for serialized parameters.
     **/
    inline RemoteRequestEvent(const ProxyAddress & fromSource, const StubAddress & toTarget, uint32_t reqId, uint32_t initSize = 0u);

    /**
     * \brief   Creates event with data. Initializes event source, target information, and message ID.
     *          Use if request with parameters should be sent.
     *
     * \param   args            Serialized request parameters (SharedBuffer payload, copied in).
     * \param   fromSource      The address of Proxy source.
     * \param   toTarget        The address of Stub target.
     * \param   reqId           The ID of request.
     **/
    inline RemoteRequestEvent( const SharedBuffer & args
                             , const ProxyAddress & fromSource
                             , const StubAddress & toTarget
                             , uint32_t reqId);

    /**
     * \brief   Constructs from a received MessageEnvelope.
     **/
    explicit inline RemoteRequestEvent( const MessageEnvelope & envelope ) noexcept;

    RemoteRequestEvent(const RemoteRequestEvent& /*src*/) = default;

    RemoteRequestEvent(RemoteRequestEvent&& /*src*/) = default;

    ~RemoteRequestEvent() override = default;

//////////////////////////////////////////////////////////////////////////////
// Protected operations
//////////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Sets the target communication Channel object.
     *
     * \param   channel     The communication channel object to set.
     **/
    inline void set_target_channel( const Channel & channel ) noexcept;

    /**
     * \brief   Returns target communication channel object.
     **/
    inline Channel target_channel() const noexcept;

    /**
     * \brief   Sets the source communication Channel object.
     *
     * \param   channel     The communication channel object to set.
     **/
    inline void set_source_channel( const Channel & channel ) noexcept;

    /**
     * \brief   Returns source communication channel object.
     **/
    inline Channel source_channel() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    RemoteRequestEvent() = delete;
};

//////////////////////////////////////////////////////////////////////////
// LocalNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Event for sending notification subscription/unsubscription requests from a proxy to a
 *          stub within the same process.
 **/
class AREG_API LocalNotifyRequestEvent    : public    ServiceRequestEvent
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes a notification request event for starting or stopping notifications
     *          within the same process.
     *
     * \param   fromProxy   The address of the source proxy requesting notifications.
     * \param   toStub      The address of the target stub to start or stop sending notifications.
     * \param   msgId       The ID of the message (attribute, broadcast, or response). Request IDs cannot be notified.
     * \param   reqType     The request type (subscribe or unsubscribe).
     **/
    inline LocalNotifyRequestEvent( const ProxyAddress & fromProxy
                                  , const StubAddress & toStub
                                  , uint32_t msgId
                                  , areg::RequestType reqType );

    /**
     * \brief   Constructs from a received MessageEnvelope.
     **/
    explicit inline LocalNotifyRequestEvent(const MessageEnvelope& envelope) noexcept;

    LocalNotifyRequestEvent(const LocalNotifyRequestEvent& /*src*/) = default;

    LocalNotifyRequestEvent(LocalNotifyRequestEvent&& /*src*/) noexcept = default;

    ~LocalNotifyRequestEvent() override = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LocalNotifyRequestEvent() = delete;
};

//////////////////////////////////////////////////////////////////////////
// RemoteNotifyRequestEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Remote notification request event sent by a proxy to a stub to start or stop
 *          attribute/broadcast/response notifications.
 **/
class AREG_API RemoteNotifyRequestEvent    : public    ServiceRequestEvent
{
    friend class RemoteEventFactory;
    friend class StubBase;   // needs new RemoteNotifyRequestEvent(envelope) on IPC receive path

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes a notification request event for starting or stopping notifications
     *          between the specified proxy and stub (cross-process).
     *
     * \param   fromProxy       The address of the source proxy requesting notifications.
     * \param   toStub          The address of the target stub to start or stop sending notifications.
     * \param   msgId           The ID of the message (attribute, broadcast, or response). Request IDs cannot be notified.
     * \param   reqType         The request type (subscribe or unsubscribe).
     **/
    inline RemoteNotifyRequestEvent( const ProxyAddress & fromProxy
                                   , const StubAddress & toStub
                                   , uint32_t msgId
                                   , areg::RequestType reqType );

    /**
     * \brief   Constructs from a received MessageEnvelope.
     **/
    explicit inline RemoteNotifyRequestEvent( const MessageEnvelope & envelope ) noexcept;

    RemoteNotifyRequestEvent(const RemoteNotifyRequestEvent& /*src*/) = default;

    RemoteNotifyRequestEvent(RemoteNotifyRequestEvent&& /*src*/) noexcept = default;

    ~RemoteNotifyRequestEvent() = default;

//////////////////////////////////////////////////////////////////////////////
// Protected operations
//////////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Sets the target communication channel.
     *
     * \param   channel     The communication channel to set.
     **/
    inline void set_target_channel( const Channel & channel ) noexcept;

    /**
     * \brief   Returns the target communication channel.
     **/
    inline Channel target_channel() const noexcept;

    /**
     * \brief   Sets the source communication channel.
     *
     * \param   channel     The communication channel to set.
     **/
    inline void set_source_channel( const Channel & channel ) noexcept;

    /**
     * \brief   Returns the source communication channel.
     **/
    inline Channel source_channel() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    RemoteNotifyRequestEvent() = delete;
};

//////////////////////////////////////////////////////////////////////////
// LocalRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
inline LocalRequestEvent::LocalRequestEvent(const ProxyAddress& fromSource, const StubAddress& toTarget, uint32_t reqId, uint32_t initSize /*= 0*/)
    : ServiceRequestEvent(fromSource, toTarget, reqId, areg::RequestType::CallFunction, areg::EventType::EventLocalRequest, initSize)
{
}

inline LocalRequestEvent::LocalRequestEvent( const SharedBuffer& args
                                           , const ProxyAddress& fromSource
                                           , const StubAddress& toTarget
                                           , uint32_t reqId)
    : ServiceRequestEvent(fromSource, toTarget, reqId, areg::RequestType::CallFunction, areg::EventType::EventLocalRequest, 0u)
{
    if (args.is_valid())
        write_data(args.buffer(), args.size_used());
}

inline LocalRequestEvent::LocalRequestEvent(const MessageEnvelope& envelope) noexcept
    : ServiceRequestEvent(envelope)
{
}

//////////////////////////////////////////////////////////////////////////
// RemoteRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
inline RemoteRequestEvent::RemoteRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, uint32_t reqId, uint32_t initSize /*= 0u*/ )
    : ServiceRequestEvent(fromSource, toTarget, reqId, areg::RequestType::CallFunction, areg::EventType::EventRemoteRequest, initSize)
{
}

inline RemoteRequestEvent::RemoteRequestEvent( const SharedBuffer & args
                                             , const ProxyAddress & fromSource
                                             , const StubAddress & toTarget
                                             , uint32_t reqId)
    : ServiceRequestEvent(fromSource, toTarget, reqId, areg::RequestType::CallFunction, areg::EventType::EventRemoteRequest, 0u)
{
    if (args.is_valid())
        write_data(args.buffer(), args.size_used());
}

inline RemoteRequestEvent::RemoteRequestEvent( const MessageEnvelope & envelope ) noexcept
    : ServiceRequestEvent( envelope )
{
}

//////////////////////////////////////////////////////////////////////////
// RemoteRequestEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline void RemoteRequestEvent::set_target_channel(const Channel & channel) noexcept
{
    areg::EventHeader* hdr{ header() };
    ASSERT(hdr != nullptr);
    hdr->target          = channel.cookie();
    hdr->channel         = channel.target();
    hdr->provider.id     = channel.cookie();
    hdr->provider.thread = channel.source();
}

inline Channel RemoteRequestEvent::target_channel() const noexcept
{
    const areg::EventHeader* hdr{ header() };
    ASSERT(hdr != nullptr);
    return Channel(hdr->provider.thread, hdr->channel, hdr->provider.id);
}

inline void RemoteRequestEvent::set_source_channel( const Channel & channel ) noexcept
{
    areg::EventHeader* hdr{ header() };
    ASSERT(hdr != nullptr);
    hdr->source          = channel.target();
    hdr->consumer.id     = channel.cookie();
    hdr->consumer.thread = channel.source();
}

inline Channel RemoteRequestEvent::source_channel() const noexcept
{
    const areg::EventHeader* hdr{ header() };
    ASSERT(hdr != nullptr);
    return Channel(hdr->consumer.thread, hdr->source, hdr->consumer.id);
}

//////////////////////////////////////////////////////////////////////////
// LocalNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
inline LocalNotifyRequestEvent::LocalNotifyRequestEvent( const ProxyAddress& fromProxy
                                                       , const StubAddress& toStub
                                                       , uint32_t msgId
                                                       , areg::RequestType reqType)
    : ServiceRequestEvent(fromProxy, toStub, msgId, reqType, areg::EventType::EventLocalNotifyRequest)
{
}

inline LocalNotifyRequestEvent::LocalNotifyRequestEvent(const MessageEnvelope& envelope) noexcept
    : ServiceRequestEvent(envelope)
{
}

//////////////////////////////////////////////////////////////////////////
// RemoteNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
inline RemoteNotifyRequestEvent::RemoteNotifyRequestEvent( const ProxyAddress& fromProxy
                                                         , const StubAddress& toStub
                                                         , uint32_t msgId
                                                         , areg::RequestType reqType)
    : ServiceRequestEvent(fromProxy, toStub, msgId, reqType, areg::EventType::EventRemoteNotifyRequest)
{
}

inline RemoteNotifyRequestEvent::RemoteNotifyRequestEvent(const MessageEnvelope& envelope) noexcept
    : ServiceRequestEvent(envelope)
{
}

//////////////////////////////////////////////////////////////////////////
// RemoteNotifyRequestEvent class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline void RemoteNotifyRequestEvent::set_target_channel(const Channel & channel) noexcept
{
    areg::EventHeader* hdr{ header() };
    ASSERT(hdr != nullptr);
    hdr->target          = channel.cookie();
    hdr->channel         = channel.target();
    hdr->provider.id     = channel.cookie();
    hdr->provider.thread = channel.source();
}

inline Channel RemoteNotifyRequestEvent::target_channel() const noexcept
{
    const areg::EventHeader* hdr{ header() };
    ASSERT(hdr != nullptr);
    return Channel(hdr->provider.thread, hdr->channel, hdr->provider.id);
}

inline void RemoteNotifyRequestEvent::set_source_channel( const Channel & channel ) noexcept
{
    areg::EventHeader* hdr{ header() };
    ASSERT(hdr != nullptr);
    hdr->source          = channel.target();
    hdr->consumer.id     = channel.cookie();
    hdr->consumer.thread = channel.source();
}

inline Channel RemoteNotifyRequestEvent::source_channel() const noexcept
{
    const areg::EventHeader* hdr{ header() };
    ASSERT(hdr != nullptr);
    return Channel(hdr->consumer.thread, hdr->source, hdr->consumer.id);
}

} // namespace areg
#endif  // AREG_COMPONENT_REQUESTEVENTS_HPP
