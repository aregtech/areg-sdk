#ifndef AREG_COMPONENT_EVENTDEFS_HPP
#define AREG_COMPONENT_EVENTDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/EventDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, event type constants, endpoint and header structures.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/CommonDefs.hpp"
#include "areg/base/IOStream.hpp"
#include "areg/base/MemoryDefs.hpp"

#include <type_traits>

namespace areg {

    constexpr uint32_t  QUEUE_MIN_RING_CAPACITY     {                 32u };    //!< Smallest ring size.
    constexpr uint32_t  QUEUE_MAX_RING_CAPACITY     {           16777216u };    //! explicit-size upper bound (keeps round-up safe)
    constexpr uint32_t  QUEUE_DEFAULT_RING_CAPACITY {               1024u };    //!< Default Ring size to use.
    //!< Ring size of a thread whose queue carries only control events, like a socket receive thread.
    constexpr uint32_t  QUEUE_CONTROL_RING_CAPACITY {                 32u };
    //!< Ring size of a thread that serves registration and lifecycle requests, like the service manager.
    constexpr uint32_t  QUEUE_MANAGER_RING_CAPACITY {                 64u };
    //!< Ring size of a thread that serves timer requests, like the timer manager.
    constexpr uint32_t  QUEUE_TIMER_RING_CAPACITY   {                128u };
    //!< Lossless full-queue producer wait in a release build. It is a backstop for a consumer
    //!< that stopped: a healthy but slow socket gives up after SO_SNDTIMEO, which is 2500 ms.
    constexpr uint32_t  QUEUE_FULL_WAIT_RELEASE_MS  {   10 * areg::WAIT_1_SECOND };

    /**
     * \brief   Lossless full-queue producer wait in a debug build, about 24 days, so that a
     *          thread suspended in the debugger does not make the application lose a message.
     *
     * \note    Must not be areg::WAIT_INFINITE. That value already means "ignore the caller and
     *          read the value from the configuration", see _resolve_queue_wait().
     **/
    constexpr uint32_t  QUEUE_FULL_WAIT_DEBUG_MS    { 0x7FFF'FFFFu };

#ifdef DEBUG
    constexpr uint32_t  QUEUE_DEFAULT_FULL_WAIT_MS  { areg::QUEUE_FULL_WAIT_DEBUG_MS };   //!< Default lossless block timeout.
#else   // DEBUG
    constexpr uint32_t  QUEUE_DEFAULT_FULL_WAIT_MS  { areg::QUEUE_FULL_WAIT_RELEASE_MS }; //!< Default lossless block timeout.
#endif  // DEBUG

    constexpr bool      QUEUE_DROP_WHEN_FULL        {               false };    //!< Default action if queue is full, wait for free slot.

/************************************************************************
 * areg::EventType
 * Bitmask enum. Composite values OR together the primitive bits below.
 *
 *   Bit 0  (0x0001)  Internal     same-thread dispatch
 *   Bit 1  (0x0002)  External     cross-thread dispatch
 *   Bit 2  (0x0004)  Local        same-process routing
 *   Bit 3  (0x0008)  Remote       IPC routing via mtrouter
 *   Bit 4  (0x0010)  ToProvider   directed to service provider stub
 *   Bit 5  (0x0020)  ToConsumer   directed to service consumer proxy
 *   Bit 6  (0x0040)  ToService    directed to a service interface
 *   Bit 8  (0x0100)  Request      service request event
 *   Bit 9  (0x0200)  Response     service response event
 *   Bit 12 (0x1000)  Notify       notification or subscription management
 *   Bit 13 (0x2000)  Connect      lifecycle connect or disconnect
 *   Bit 14 (0x4000)  Custom       developer-defined event marker
 ************************************************************************/
enum class EventType : uint32_t
{
      EventUnknown              =     0 /*0x0000*/  //!< Unknown event type.            Bit set: 0000 0000 0000 0000

    , EventInternalMask         =     1 /*0x0001*/  //!< Internal event.                Bit set: 0000 0000 0000 0001
    , EventExternalMask         =     2 /*0x0002*/  //!< External event.                Bit set: 0000 0000 0000 0010

    , EventLocalMask            =     4 /*0x0004*/  //!< Local event.                   Bit set: 0000 0000 0000 0100
    , EventRemoteMask           =     8 /*0x0008*/  //!< Remote event.                  Bit set: 0000 0000 0000 1000

    , EventToProviderMask       =    16 /*0x0010*/  //!< Provider event.                Bit set: 0000 0000 0001 0000
    , EventToConsumerMask       =    32 /*0x0020*/  //!< Proxy event.                   Bit set: 0000 0000 0010 0000
    , EventToServiceMask        =    64 /*0x0040*/  //!< Service event.                 Bit set: 0000 0000 0100 0000

    , EventRequestMask          =   256 /*0x0100*/  //!< Service request event.         Bit set: 0000 0001 0000 0000
    , EventResponseMask         =   512 /*0x0200*/  //!< Service response event.        Bit set: 0000 0010 0000 0000

    , EventNotifyMask           =  4096 /*0x1000*/  //!< Notification event.            Bit set: 0001 0000 0000 0000
    , EventConnectMask          =  8192 /*0x2000*/  //!< Connection event.              Bit set: 0010 0000 0000 0000
    , EventCustomMask           = 16384 /*0x4000*/  //!< Custom event.                  Bit set: 0100 0000 0000 0000

    , EventNotifyClient         =  4101 /*0x1005*/  //!< Client Notification.           Bit set: 0001 0000 0000 0101 -> (EventInternalMask | EventLocalMask | EventNotifyMask)

    , EventLocalRequest         =   278 /*0x0116*/  //!< Local Service Request.         Bit set: 0000 0001 0001 0110 -> (EventExternalMask | EventLocalMask  | EventToProviderMask | EventRequestMask)
    , EventRemoteRequest        =   282 /*0x011A*/  //!< Remote Service Request.        Bit set: 0000 0001 0001 1010 -> (EventExternalMask | EventRemoteMask | EventToProviderMask | EventRequestMask)

    , EventLocalNotifyRequest   =  4374 /*0x1116*/  //!< Local Request notify.          Bit set: 0001 0001 0001 0110 -> (EventExternalMask | EventLocalMask  | EventToProviderMask | EventRequestMask | EventNotifyMask)
    , EventRemoteNotifyRequest  =  4378 /*0x111A*/  //!< Remote Request notify.         Bit set: 0001 0001 0001 1010 -> (EventExternalMask | EventRemoteMask | EventToProviderMask | EventRequestMask | EventNotifyMask)

    , EventLocalResponse        =   550 /*0x0226*/  //!< Local Service Response.        Bit set: 0000 0010 0010 0110 -> (EventExternalMask | EventLocalMask  | EventToConsumerMask | EventResponseMask)
    , EventRemoteResponse       =   554 /*0x022A*/  //!< Remote Service Response.       Bit set: 0000 0010 0010 1010 -> (EventExternalMask | EventRemoteMask | EventToConsumerMask | EventResponseMask)

    , EventLocalProviderConnect = 12310 /*0x3016*/  //!< Local Provider Connection.     Bit set: 0011 0000 0001 0110 -> (EventExternalMask | EventLocalMask  | EventToProviderMask | EventNotifyMask | EventConnectMask)
    , EventRemoteProviderConnect= 12314 /*0x301A*/  //!< Remote Provider Connection.    Bit set: 0011 0000 0001 1010 -> (EventExternalMask | EventRemoteMask | EventToProviderMask | EventNotifyMask | EventConnectMask)

    , EventLocalConsumerConnect = 12326 /*0x3026*/  //!< Local Proxy Connection.        Bit set: 0011 0000 0010 0110 -> (EventExternalMask | EventLocalMask  | EventToConsumerMask | EventNotifyMask | EventConnectMask)
    , EventRemoteConsumerConnect= 12330 /*0x302A*/  //!< Remote Proxy Connection.       Bit set: 0011 0000 0010 1010 -> (EventExternalMask | EventRemoteMask | EventToConsumerMask | EventNotifyMask | EventConnectMask)

    , EventRemoteConnection     = 12410 /*0x307A*/  //!< Remote Proxy Connection.       Bit set: 0011 0000 0111 1010 -> (EventExternalMask | EventRemoteMask | EventToConsumerMask | EventToProviderMask | EventToServiceMask | EventNotifyMask | EventConnectMask)

    , EventCustomInternal       = 16389 /*0x4005*/  //!< Custom Notification.           Bit set: 0100 0000 0000 0101 -> (EventInternalMask | EventLocalMask  | EventCustomMask)
    , EventCustomExternal       = 16390 /*0x4006*/  //!< Custom Thread event.           Bit set: 0100 0000 0000 0110 -> (EventExternalMask | EventLocalMask  | EventCustomMask)
};

[[nodiscard]]
static inline constexpr const char* as_string(areg::EventType eventType) noexcept;

/************************************************************************
 * areg::EventPriority
 ************************************************************************/
enum class EventPriority : uint32_t
{
      NormalPrio    = 0 //!< Normal priority; processed in FIFO order.
    , HighPrio      = 1 //!< High priority; processed before normal events.
    , CriticalPrio  = 2 //!< Critical priority; processed nearly immediately.
    , ExitPrio      = 4 //!< Highest priority; processed immediately and never dropped.
};

[[nodiscard]]
static inline constexpr const char* as_string(areg::EventPriority eventPrio) noexcept;

constexpr areg::EventPriority DefaultPriority { areg::EventPriority::NormalPrio };

/************************************************************************
 * areg::CallType
 * The kind of the message ID in the event header.
 *   0x10 / 0x20  registration of a consumer / a provider
 *   0x40         service interface call: the ID belongs to the interface
 ************************************************************************/
enum class CallType : uint8_t
{
      Undefined             = 0x00u //!< No call. The message ID is a system ID or empty.
    , RegisterConsumer      = 0x10u //!< A service consumer registers.
    , UnregisterConsumer    = 0x11u //!< A service consumer unregisters.
    , RegisterProvider      = 0x20u //!< A service provider registers.
    , UnregisterProvider    = 0x21u //!< A service provider unregisters.
    , Interface             = 0x40u //!< A service interface call that names no entry. The message ID is empty.
    , Request               = 0x41u //!< The message ID is a request.
    , Response              = 0x42u //!< The message ID is a response.
    , Broadcast             = 0x43u //!< The message ID is a broadcast.
    , Attribute             = 0x44u //!< The message ID is an attribute.
};

//!< The bit of a call type that marks a service interface call.
constexpr uint8_t   CALL_INTERFACE_MASK { 0x40u };

/**
 * \brief   Returns true if the call type marks a service interface call.
 **/
[[nodiscard]]
inline constexpr bool is_interface_call(uint8_t callType) noexcept;

/**
 * \brief   Returns true if the call type marks a service interface call.
 **/
[[nodiscard]]
inline constexpr bool is_interface_call(areg::CallType callType) noexcept;

/**
 * \brief   Returns true if the call type is one of the four service interface kinds.
 **/
[[nodiscard]]
inline constexpr bool is_interface_kind(areg::CallType callType) noexcept;

[[nodiscard]]
static inline constexpr const char* as_string(areg::CallType callType) noexcept;

/**
 * \brief   Returns true if the event type is matching any bit set of the give event.
 **/
[[nodiscard]]
inline constexpr bool is_matching_any(const uint32_t checkEvent, areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event type has exact matching bits of the give event.
 **/
[[nodiscard]]
inline constexpr bool is_matching_exact(const uint32_t checkEvent, uint32_t eventType) noexcept;

/**
 * \brief   Returns true if the event type is internal.
 **/
[[nodiscard]]
inline constexpr bool is_internal(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event type is external.
 **/
[[nodiscard]]
inline constexpr bool is_external(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event type is local.
 **/
[[nodiscard]]
inline constexpr bool is_local(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event type is remote.
 **/
[[nodiscard]]
inline constexpr bool is_remote(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event is directed to a service provider stub.
 **/
[[nodiscard]]
inline constexpr bool is_to_provider(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event is directed to a service proxy.
 **/
[[nodiscard]]
inline constexpr bool is_to_consumer(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event is directed to a service interface.
 **/
[[nodiscard]]
inline constexpr bool is_to_service(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event carries the service request bit.
 **/
[[nodiscard]]
inline constexpr bool is_request(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event carries the service response bit.
 **/
[[nodiscard]]
inline constexpr bool is_response(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event carries the EventNotify bit.
 **/
[[nodiscard]]
inline constexpr bool is_notify(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event is a connect or disconnect lifecycle event.
 **/
[[nodiscard]]
inline constexpr bool is_connect(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event type is developer custom, false if system predefined.
 **/
[[nodiscard]]
inline constexpr bool is_custom(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event is an in-process client notification (EventNotifyClient).
 **/
[[nodiscard]]
inline constexpr bool is_notify_client(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event is a properly formed service request call to a provider.
 **/
[[nodiscard]]
inline constexpr bool is_request_call(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event is a properly formed service response call to a consumer.
 **/
[[nodiscard]]
inline constexpr bool is_response_call(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event is a service notify request: a subscription or its removal.
 **/
[[nodiscard]]
inline constexpr bool is_notify_request_call(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event is a broadcast sent to a consumer.
 **/
[[nodiscard]]
inline constexpr bool is_broadcast_call(areg::EventType eventType, areg::CallType callType) noexcept;

/**
 * \brief   Returns true if the event is an attribute update sent to a consumer.
 **/
[[nodiscard]]
inline constexpr bool is_attribute_call(areg::EventType eventType, areg::CallType callType) noexcept;

/**
 * \brief   Returns true if the event reports the failure of a request to its consumer.
 **/
[[nodiscard]]
inline constexpr bool is_request_failure(areg::EventType eventType, areg::CallType callType) noexcept;

/**
 * \brief   Returns true if the event subscribes to a response or removes the subscription.
 **/
[[nodiscard]]
inline constexpr bool is_notify_response(areg::EventType eventType, areg::CallType callType) noexcept;

/**
 * \brief   Returns true if the event subscribes to a broadcast or removes the subscription.
 **/
[[nodiscard]]
inline constexpr bool is_notify_broadcast(areg::EventType eventType, areg::CallType callType) noexcept;

/**
 * \brief   Returns true if the event subscribes to an attribute or removes the subscription.
 **/
[[nodiscard]]
inline constexpr bool is_notify_attribute(areg::EventType eventType, areg::CallType callType) noexcept;

/**
 * \brief   Returns true if the event is a provider connect or disconnect lifecycle event.
 **/
[[nodiscard]]
inline constexpr bool is_provider_connect(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event is a consumer connect or disconnect lifecycle event.
 **/
[[nodiscard]]
inline constexpr bool is_consumer_connect(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event is a known (non-unknown) event type.
 **/
[[nodiscard]]
inline constexpr bool is_valid(areg::EventType eventType) noexcept;

AREG_IMPLEMENT_STREAMABLE(areg::CallType)
AREG_IMPLEMENT_STREAMABLE(areg::EventType)

}   // namespace areg

inline constexpr bool areg::is_matching_any(const uint32_t checkEvent, areg::EventType eventType) noexcept
{
    return ((checkEvent & static_cast<uint32_t>(eventType)) != 0);
}

inline constexpr bool areg::is_matching_exact(const uint32_t checkEvent, uint32_t eventType) noexcept
{
    return ((checkEvent & eventType) == eventType);
}

inline constexpr bool areg::is_internal(areg::EventType eventType) noexcept
{
    return areg::is_matching_any(static_cast<uint32_t>(eventType), areg::EventType::EventInternalMask);
}

inline constexpr bool areg::is_external(areg::EventType eventType) noexcept
{
    return areg::is_matching_any(static_cast<uint32_t>(eventType), areg::EventType::EventExternalMask);
}

inline constexpr bool areg::is_local(areg::EventType eventType) noexcept
{
    return areg::is_matching_any(static_cast<uint32_t>(eventType), areg::EventType::EventLocalMask);
}

inline constexpr bool areg::is_remote(areg::EventType eventType) noexcept
{
    return areg::is_matching_any(static_cast<uint32_t>(eventType), areg::EventType::EventRemoteMask);
}

inline constexpr bool areg::is_custom(areg::EventType eventType) noexcept
{
    return areg::is_matching_any(static_cast<uint32_t>(eventType), areg::EventType::EventCustomMask);
}

inline constexpr bool areg::is_notify_client(areg::EventType eventType) noexcept
{
    return areg::is_matching_exact(static_cast<uint32_t>(eventType), static_cast<uint32_t>(areg::EventType::EventNotifyClient));
}

inline constexpr bool areg::is_notify(areg::EventType eventType) noexcept
{
    return areg::is_matching_any(static_cast<uint32_t>(eventType), areg::EventType::EventNotifyMask);
}

inline constexpr bool areg::is_to_provider(areg::EventType eventType) noexcept
{
    return areg::is_matching_any(static_cast<uint32_t>(eventType), areg::EventType::EventToProviderMask);
}

inline constexpr bool areg::is_to_consumer(areg::EventType eventType) noexcept
{
    return areg::is_matching_any(static_cast<uint32_t>(eventType), areg::EventType::EventToConsumerMask);
}

inline constexpr bool areg::is_connect(areg::EventType eventType) noexcept
{
    return areg::is_matching_any(static_cast<uint32_t>(eventType), areg::EventType::EventConnectMask);
}

inline constexpr bool areg::is_to_service(areg::EventType eventType) noexcept
{
    return areg::is_matching_any(static_cast<uint32_t>(eventType), areg::EventType::EventToServiceMask);
}

inline constexpr bool areg::is_request(areg::EventType eventType) noexcept
{
    return areg::is_matching_any(static_cast<uint32_t>(eventType), areg::EventType::EventRequestMask);
}

inline constexpr bool areg::is_response(areg::EventType eventType) noexcept
{
    return areg::is_matching_any(static_cast<uint32_t>(eventType), areg::EventType::EventResponseMask);
}

inline constexpr bool areg::is_request_call(areg::EventType eventType) noexcept
{
    constexpr uint32_t _bits{ static_cast<uint32_t>(areg::EventType::EventLocalRequest) & ~static_cast<uint32_t>(areg::EventType::EventLocalMask) };
    return areg::is_matching_exact(static_cast<uint32_t>(eventType), _bits);
}

inline constexpr bool areg::is_response_call(areg::EventType eventType) noexcept
{
    constexpr uint32_t _bits{ static_cast<uint32_t>(areg::EventType::EventLocalResponse) & ~static_cast<uint32_t>(areg::EventType::EventLocalMask) };
    return areg::is_matching_exact(static_cast<uint32_t>(eventType), _bits);
}

inline constexpr bool areg::is_notify_request_call(areg::EventType eventType) noexcept
{
    constexpr uint32_t _bits{ static_cast<uint32_t>(areg::EventType::EventLocalNotifyRequest) & ~static_cast<uint32_t>(areg::EventType::EventLocalMask) };
    return areg::is_matching_exact(static_cast<uint32_t>(eventType), _bits);
}

inline constexpr bool areg::is_broadcast_call(areg::EventType eventType, areg::CallType callType) noexcept
{
    return (callType == areg::CallType::Broadcast) && areg::is_response_call(eventType);
}

inline constexpr bool areg::is_attribute_call(areg::EventType eventType, areg::CallType callType) noexcept
{
    return (callType == areg::CallType::Attribute) && areg::is_response_call(eventType);
}

inline constexpr bool areg::is_request_failure(areg::EventType eventType, areg::CallType callType) noexcept
{
    return (callType == areg::CallType::Request) && areg::is_response_call(eventType);
}

inline constexpr bool areg::is_notify_response(areg::EventType eventType, areg::CallType callType) noexcept
{
    return (callType == areg::CallType::Response) && areg::is_notify_request_call(eventType);
}

inline constexpr bool areg::is_notify_broadcast(areg::EventType eventType, areg::CallType callType) noexcept
{
    return (callType == areg::CallType::Broadcast) && areg::is_notify_request_call(eventType);
}

inline constexpr bool areg::is_notify_attribute(areg::EventType eventType, areg::CallType callType) noexcept
{
    return (callType == areg::CallType::Attribute) && areg::is_notify_request_call(eventType);
}

inline constexpr bool areg::is_provider_connect(areg::EventType eventType) noexcept
{
    constexpr uint32_t _bits{ static_cast<uint32_t>(areg::EventType::EventLocalProviderConnect) & ~static_cast<uint32_t>(areg::EventType::EventLocalMask) };
    return areg::is_matching_exact(static_cast<uint32_t>(eventType), _bits);
}

inline constexpr bool areg::is_consumer_connect(areg::EventType eventType) noexcept
{
    constexpr uint32_t _bits{ static_cast<uint32_t>(areg::EventType::EventLocalConsumerConnect) & ~static_cast<uint32_t>(areg::EventType::EventLocalMask) };
    return areg::is_matching_exact(static_cast<uint32_t>(eventType), _bits);
}

inline constexpr bool areg::is_valid(areg::EventType eventType) noexcept
{
    return eventType != areg::EventType::EventUnknown;
}

inline constexpr const char* areg::as_string(areg::EventType eventType) noexcept
{
    switch (eventType)
    {
    case areg::EventType::EventUnknown:
        return "areg::EventType::EventUnknown";
    case areg::EventType::EventInternalMask:
        return "areg::EventType::EventInternalMask";
    case areg::EventType::EventExternalMask:
        return "areg::EventType::EventExternalMask";
    case areg::EventType::EventLocalMask:
        return "areg::EventType::EventLocalMask";
    case areg::EventType::EventRemoteMask:
        return "areg::EventType::EventRemoteMask";
    case areg::EventType::EventToProviderMask:
        return "areg::EventType::EventToProviderMask";
    case areg::EventType::EventToConsumerMask:
        return "areg::EventType::EventToConsumerMask";
    case areg::EventType::EventToServiceMask:
        return "areg::EventType::EventToServiceMask";
    case areg::EventType::EventRequestMask:
        return "areg::EventType::EventRequestMask";
    case areg::EventType::EventResponseMask:
        return "areg::EventType::EventResponseMask";
    case areg::EventType::EventNotifyMask:
        return "areg::EventType::EventNotifyMask";
    case areg::EventType::EventConnectMask:
        return "areg::EventType::EventConnectMask";
    case areg::EventType::EventCustomMask:
        return "areg::EventType::EventCustomMask";
    case areg::EventType::EventNotifyClient:
        return "areg::EventType::EventNotifyClient";
    case areg::EventType::EventLocalRequest:
        return "areg::EventType::EventLocalRequest";
    case areg::EventType::EventRemoteRequest:
        return "areg::EventType::EventRemoteRequest";
    case areg::EventType::EventLocalNotifyRequest:
        return "areg::EventType::EventLocalNotifyRequest";
    case areg::EventType::EventRemoteNotifyRequest:
        return "areg::EventType::EventRemoteNotifyRequest";
    case areg::EventType::EventLocalResponse:
        return "areg::EventType::EventLocalResponse";
    case areg::EventType::EventRemoteResponse:
        return "areg::EventType::EventRemoteResponse";
    case areg::EventType::EventLocalProviderConnect:
        return "areg::EventType::EventLocalProviderConnect";
    case areg::EventType::EventRemoteProviderConnect:
        return "areg::EventType::EventRemoteProviderConnect";
    case areg::EventType::EventLocalConsumerConnect:
        return "areg::EventType::EventLocalConsumerConnect";
    case areg::EventType::EventRemoteConsumerConnect:
        return "areg::EventType::EventRemoteConsumerConnect";
    case areg::EventType::EventRemoteConnection:
        return "areg::EventType::EventRemoteConnection";
    case areg::EventType::EventCustomInternal:
        return "areg::EventType::EventCustomInternal";
    case areg::EventType::EventCustomExternal:
        return "areg::EventType::EventCustomExternal";
    default:
        return "ERR: Undefined areg::EventType value!";
    }
}

inline constexpr const char* areg::as_string(areg::EventPriority eventPrio) noexcept
{
    switch (eventPrio)
    {
    case areg::EventPriority::NormalPrio:
        return "areg::EventPriority::NormalPrio";
    case areg::EventPriority::HighPrio:
        return "areg::EventPriority::HighPrio";
    case areg::EventPriority::CriticalPrio:
        return "areg::EventPriority::CriticalPrio";
    case areg::EventPriority::ExitPrio:
        return "areg::EventPriority::ExitPrio";
    default:
        return "ERR: Undefined areg::EventPriority value!";
    }
}

inline constexpr bool areg::is_interface_call(uint8_t callType) noexcept
{
    return ((callType & areg::CALL_INTERFACE_MASK) != 0u);
}

inline constexpr bool areg::is_interface_call(areg::CallType callType) noexcept
{
    return areg::is_interface_call(static_cast<uint8_t>(callType));
}

inline constexpr bool areg::is_interface_kind(areg::CallType callType) noexcept
{
    return (callType == areg::CallType::Request)   || (callType == areg::CallType::Response)
        || (callType == areg::CallType::Broadcast) || (callType == areg::CallType::Attribute);
}

inline constexpr const char* areg::as_string(areg::CallType callType) noexcept
{
    switch (callType)
    {
    case areg::CallType::Undefined:
        return "areg::CallType::Undefined";
    case areg::CallType::RegisterConsumer:
        return "areg::CallType::RegisterConsumer";
    case areg::CallType::UnregisterConsumer:
        return "areg::CallType::UnregisterConsumer";
    case areg::CallType::RegisterProvider:
        return "areg::CallType::RegisterProvider";
    case areg::CallType::UnregisterProvider:
        return "areg::CallType::UnregisterProvider";
    case areg::CallType::Interface:
        return "areg::CallType::Interface";
    case areg::CallType::Request:
        return "areg::CallType::Request";
    case areg::CallType::Response:
        return "areg::CallType::Response";
    case areg::CallType::Broadcast:
        return "areg::CallType::Broadcast";
    case areg::CallType::Attribute:
        return "areg::CallType::Attribute";
    default:
        return "ERR: Undefined areg::CallType value!";
    }
}

#endif  // AREG_COMPONENT_EVENTDEFS_HPP
