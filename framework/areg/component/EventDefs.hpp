#ifndef AREG_COMPONENT_EVENTDEFS_HPP
#define AREG_COMPONENT_EVENTDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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
#include "areg/base/IOStream.hpp"
#include "areg/base/MemoryDefs.hpp"

#include <type_traits>

namespace areg {

    constexpr uint32_t  QUEUE_MIN_RING_CAPACITY     {                 32u };    //!< Smallest ring size.
    constexpr uint32_t  QUEUE_MAX_RING_CAPACITY     {           16777216u };    //! explicit-size upper bound (keeps round-up safe)
    constexpr uint32_t  QUEUE_DEFAULT_RING_CAPACITY {               1024u };    //!< Default Ring size to use.
    constexpr uint32_t  QUEUE_DEFAULT_FULL_WAIT_MS  { areg::WAIT_1_SECOND };    //!< Default lossless block timeout.
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
 *   Bit 7  (0x0080)  ToSystem     directed to system handler
 *   Bit 8  (0x0100)  Request      service request event
 *   Bit 9  (0x0200)  Response     service response event
 *   Bit 10 (0x0400)  Broadcast    service broadcast event
 *   Bit 11 (0x0800)  Attribute    service attribute event
 *   Bit 12 (0x1000)  Notify       notification or subscription management
 *   Bit 13 (0x2000)  Connect      lifecycle connect or disconnect
 *   Bit 14 (0x4000)  Custom       developer-defined event marker
 *   Bit 15 (0x8000)  Failure      failure indicator
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
    , EventToSystemMask         =   128 /*0x0080*/  //!< System event.                  Bit set: 0000 0000 1000 0000

    , EventRequestMask          =   256 /*0x0100*/  //!< Service request event.         Bit set: 0000 0001 0000 0000
    , EventResponseMask         =   512 /*0x0200*/  //!< Service response event.        Bit set: 0000 0010 0000 0000
    , EventBroadcastMask        =  1024 /*0x0400*/  //!< Service broadcast event.       Bit set: 0000 0100 0000 0000
    , EventAttributeMask        =  2048 /*0x0800*/  //!< Service attribute event.       Bit set: 0000 1000 0000 0000

    , EventNotifyMask           =  4096 /*0x1000*/  //!< Notification event.            Bit set: 0001 0000 0000 0000
    , EventConnectMask          =  8192 /*0x2000*/  //!< Connection event.              Bit set: 0010 0000 0000 0000
    , EventCustomMask           = 16384 /*0x4000*/  //!< Custom event.                  Bit set: 0100 0000 0000 0000
    , EventFailureMask          = 32768 /*0x8000*/  //!< Connection event.              Bit set: 1000 0000 0000 0000

    , EventNotifyClient         =  4101 /*0x1005*/  //!< Client Notification.           Bit set: 0001 0000 0000 0101 -> (EventInternalMask | EventLocalMask | EventNotifyMask)

    , EventLocalRequest         =   278 /*0x0116*/  //!< Local Service Request.         Bit set: 0000 0001 0001 0110 -> (EventExternalMask | EventLocalMask  | EventToProviderMask | EventRequestMask)
    , EventRemoteRequest        =   282 /*0x011A*/  //!< Remote Service Request.        Bit set: 0000 0001 0001 1010 -> (EventExternalMask | EventRemoteMask | EventToProviderMask | EventRequestMask)

    , EventLocalNotifyRequest   =  4374 /*0x1116*/  //!< Local Request notify.          Bit set: 0001 0001 0001 0110 -> (EventExternalMask | EventLocalMask  | EventToProviderMask | EventRequestMask | EventNotifyMask)
    , EventRemoteNotifyRequest  =  4378 /*0x111A*/  //!< Remote Request notify.         Bit set: 0001 0001 0001 1010 -> (EventExternalMask | EventRemoteMask | EventToProviderMask | EventRequestMask | EventNotifyMask)
    , EventLocalNotifyResponse  =  4886 /*0x1316*/  //!< Local notify response.         Bit set: 0001 0011 0001 0110 -> (EventExternalMask | EventLocalMask  | EventToProviderMask | EventRequestMask | EventResponseMask  | EventNotifyMask)
    , EventRemoteNotifyResponse =  4890 /*0x131A*/  //!< Remote notify response.        Bit set: 0001 0011 0001 1010 -> (EventExternalMask | EventRemoteMask | EventToProviderMask | EventRequestMask | EventResponseMask  | EventNotifyMask)
    , EventLocalNotifyBroadcast =  5398 /*0x1516*/  //!< Local notify broadcast.        Bit set: 0001 0101 0001 0110 -> (EventExternalMask | EventLocalMask  | EventToProviderMask | EventRequestMask | EventBroadcastMask | EventNotifyMask)
    , EventRemoteNotifyBroadcast=  5402 /*0x151A*/  //!< Remote notify broadcast.       Bit set: 0001 0101 0001 1010 -> (EventExternalMask | EventRemoteMask | EventToProviderMask | EventRequestMask | EventBroadcastMask | EventNotifyMask)
    , EventLocalNotifyAttribute =  6422 /*0x1916*/  //!< Local notify attribute.        Bit set: 0001 1001 0001 0110 -> (EventExternalMask | EventLocalMask  | EventToProviderMask | EventRequestMask | EventAttributeMask | EventNotifyMask)
    , EventRemoteNotifyAttribute=  6426 /*0x191A*/  //!< Remote notify attribute.       Bit set: 0001 1001 0001 1010 -> (EventExternalMask | EventRemoteMask | EventToProviderMask | EventRequestMask | EventAttributeMask | EventNotifyMask)

    , EventLocalResponse        =   550 /*0x0226*/  //!< Local Service Response.        Bit set: 0000 0010 0010 0110 -> (EventExternalMask | EventLocalMask  | EventToConsumerMask | EventResponseMask)
    , EventRemoteResponse       =   554 /*0x022A*/  //!< Remote Service Response.       Bit set: 0000 0010 0010 1010 -> (EventExternalMask | EventRemoteMask | EventToConsumerMask | EventResponseMask)
    , EventLocalBroadcast       =  1062 /*0x0426*/  //!< Local Service Response.        Bit set: 0000 0100 0010 0110 -> (EventExternalMask | EventLocalMask  | EventToConsumerMask | EventBroadcastMask)
    , EventRemoteBroadcast      =  1066 /*0x042A*/  //!< Remote Service Response.       Bit set: 0000 0100 0010 1010 -> (EventExternalMask | EventRemoteMask | EventToConsumerMask | EventBroadcastMask)
    , EventLocalAttribute       =  2086 /*0x0826*/  //!< Local Service Response.        Bit set: 0000 1000 0010 0110 -> (EventExternalMask | EventLocalMask  | EventToConsumerMask | EventAttributeMask)
    , EventRemoteAttribute      =  2090 /*0x082A*/  //!< Remote Service Response.       Bit set: 0000 1000 0010 1010 -> (EventExternalMask | EventRemoteMask | EventToConsumerMask | EventAttributeMask)

    , EventLocalProviderConnect = 12310 /*0x3016*/  //!< Local Provider Connection.     Bit set: 0011 0000 0001 0110 -> (EventExternalMask | EventLocalMask  | EventToProviderMask | EventNotifyMask | EventConnectMask)
    , EventRemoteProviderConnect= 12314 /*0x301A*/  //!< Remote Provider Connection.    Bit set: 0011 0000 0001 1010 -> (EventExternalMask | EventRemoteMask | EventToProviderMask | EventNotifyMask | EventConnectMask)

    , EventLocalConsumerConnect = 12326 /*0x3026*/  //!< Local Proxy Connection.        Bit set: 0011 0000 0010 0110 -> (EventExternalMask | EventLocalMask  | EventToConsumerMask | EventNotifyMask | EventConnectMask)
    , EventRemoteConsumerConnect= 12330 /*0x302A*/  //!< Remote Proxy Connection.       Bit set: 0011 0000 0010 1010 -> (EventExternalMask | EventRemoteMask | EventToConsumerMask | EventNotifyMask | EventConnectMask)

    , EventLocalConnection      = 12406 /*0x3076*/  //!< Local Proxy Connection.        Bit set: 0011 0000 0111 0110 -> (EventExternalMask | EventLocalMask  | EventToConsumerMask | EventToProviderMask | EventToServiceMask | EventNotifyMask | EventConnectMask)
    , EventRemoteConnection     = 12410 /*0x307A*/  //!< Remote Proxy Connection.       Bit set: 0011 0000 0111 1010 -> (EventExternalMask | EventRemoteMask | EventToConsumerMask | EventToProviderMask | EventToServiceMask | EventNotifyMask | EventConnectMask)

    , EventCustomInternal       = 16389 /*0x4005*/  //!< Custom Notification.           Bit set: 0100 0000 0000 0101 -> (EventInternalMask | EventLocalMask  | EventCustomMask)
    , EventCustomExternal       = 16390 /*0x4006*/  //!< Custom Thread event.           Bit set: 0100 0000 0000 0110 -> (EventExternalMask | EventLocalMask  | EventCustomMask)

    , EventLocalRequestFailed   = 33062 /*0x8126*/  //!< Local Service Request Failed.  Bit set: 1000 0001 0010 0110 -> (EventExternalMask | EventLocalMask  | EventToConsumerMask | EventRequestMask | EventFailureMask)
    , EventRemoteRequestFailed  = 33066 /*0x812A*/  //!< Remote Service Request Failed. Bit set: 1000 0001 0010 1010 -> (EventExternalMask | EventRemoteMask | EventToConsumerMask | EventRequestMask | EventFailureMask)
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
 * areg::EventCallType
 * Dispatch category for the event runnable path. Bitfield design:
 *   bit 0 (0x01)  IsLocal    same-process inter-thread
 *   bit 1 (0x02)  IsRemote   IPC via mtrouter
 *   bit 2 (0x04)  IsCustom   developer-defined (EventCustomBit set)
 *   bit 3 (0x08)  IsService  framework service-interface call
 ************************************************************************/
enum class EventCallType : uint8_t
{
      Undefined         = 0x00u   //!< Unclassified; default state.
    , CallLocal         = 0x01u   //!< Local: same-process inter-thread.
    , CallRemote        = 0x02u   //!< Remote: IPC via mtrouter.
    , CallCustom        = 0x05u   //!< Custom local: developer-defined event.
    , CallService       = 0x08u   //!< Service call base (use Local or Remote variant).
    , CallServiceLocal  = 0x09u   //!< Service call, local delivery.
    , CallServiceRemote = 0x0Au   //!< Service call, remote delivery.
};

[[nodiscard]]
static inline constexpr const char* as_string(areg::EventCallType callType) noexcept;

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
 * \brief   Returns true if the event is directed to the system handler.
 **/
[[nodiscard]]
inline constexpr bool is_to_system(areg::EventType eventType) noexcept;

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
 * \brief   Returns true if the event carries the service broadcast bit.
 **/
[[nodiscard]]
inline constexpr bool is_broadcast(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event carries the service attribute bit.
 **/
[[nodiscard]]
inline constexpr bool is_attribute(areg::EventType eventType) noexcept;

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
 * \brief   Returns true if the event type is developer custom, false if system predefined.
 **/
[[nodiscard]]
inline constexpr bool is_failure(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event is an in-process client notification (EventNotifyClient).
 **/
[[nodiscard]]
inline constexpr bool is_notify_client(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event matches the local notify-response delivery pattern.
 **/
[[nodiscard]]
inline constexpr bool is_notify_response(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event matches the local notify-broadcast delivery pattern.
 **/
[[nodiscard]]
inline constexpr bool is_notify_broadcast(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event matches the local notify-attribute delivery pattern.
 **/
[[nodiscard]]
inline constexpr bool is_notify_attribute(areg::EventType eventType) noexcept;

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
 * \brief   Returns true if the event is a properly formed service broadcast call to a consumer.
 **/
[[nodiscard]]
inline constexpr bool is_broadcast_call(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event is a properly formed attribute update call to a consumer.
 **/
[[nodiscard]]
inline constexpr bool is_attribute_call(areg::EventType eventType) noexcept;

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

/**
 * \brief   Returns true if the event is a request-failure notification (local or remote).
 **/
[[nodiscard]]
inline constexpr bool is_request_failure(areg::EventType eventType) noexcept;


AREG_IMPLEMENT_STREAMABLE(areg::EventCallType)
AREG_IMPLEMENT_STREAMABLE(areg::EventType)

// Failure events are notifications sent TO the consumer proxy, not the provider stub.
static_assert(
       (static_cast<uint32_t>(areg::EventType::EventLocalRequestFailed)  & static_cast<uint32_t>(areg::EventType::EventToConsumerMask)) != 0
    && (static_cast<uint32_t>(areg::EventType::EventRemoteRequestFailed) & static_cast<uint32_t>(areg::EventType::EventToConsumerMask)) != 0,
    "Request-failure events must carry EventToConsumerMask"
);

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

inline constexpr bool areg::is_failure(areg::EventType eventType) noexcept
{
    return areg::is_matching_any(static_cast<uint32_t>(eventType), areg::EventType::EventFailureMask);
}

inline constexpr bool areg::is_notify_client(areg::EventType eventType) noexcept
{
    return areg::is_matching_exact(static_cast<uint32_t>(eventType), static_cast<uint32_t>(areg::EventType::EventNotifyClient));
}

inline constexpr bool areg::is_notify_response(areg::EventType eventType) noexcept
{
    constexpr uint32_t _bits{ static_cast<uint32_t>(areg::EventType::EventLocalNotifyResponse) & ~static_cast<uint32_t>(areg::EventType::EventLocalMask) };
    return areg::is_matching_exact(static_cast<uint32_t>(eventType), _bits);
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

inline constexpr bool areg::is_to_system(areg::EventType eventType) noexcept
{
    return areg::is_matching_any(static_cast<uint32_t>(eventType), areg::EventType::EventToSystemMask);
}

inline constexpr bool areg::is_request(areg::EventType eventType) noexcept
{
    return areg::is_matching_any(static_cast<uint32_t>(eventType), areg::EventType::EventRequestMask);
}

inline constexpr bool areg::is_response(areg::EventType eventType) noexcept
{
    return areg::is_matching_any(static_cast<uint32_t>(eventType), areg::EventType::EventResponseMask);
}

inline constexpr bool areg::is_broadcast(areg::EventType eventType) noexcept
{
    return areg::is_matching_any(static_cast<uint32_t>(eventType), areg::EventType::EventBroadcastMask);
}

inline constexpr bool areg::is_attribute(areg::EventType eventType) noexcept
{
    return areg::is_matching_any(static_cast<uint32_t>(eventType), areg::EventType::EventAttributeMask);
}

inline constexpr bool areg::is_notify_broadcast(areg::EventType eventType) noexcept
{
    constexpr uint32_t _bits{ static_cast<uint32_t>(areg::EventType::EventLocalNotifyBroadcast) & ~static_cast<uint32_t>(areg::EventType::EventLocalMask) };
    return areg::is_matching_exact(static_cast<uint32_t>(eventType), _bits);
}

inline constexpr bool areg::is_notify_attribute(areg::EventType eventType) noexcept
{
    constexpr uint32_t _bits{ static_cast<uint32_t>(areg::EventType::EventLocalNotifyAttribute) & ~static_cast<uint32_t>(areg::EventType::EventLocalMask) };
    return areg::is_matching_exact(static_cast<uint32_t>(eventType), _bits);
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

inline constexpr bool areg::is_broadcast_call(areg::EventType eventType) noexcept
{
    constexpr uint32_t _bits{ static_cast<uint32_t>(areg::EventType::EventLocalBroadcast) & ~static_cast<uint32_t>(areg::EventType::EventLocalMask) };
    return areg::is_matching_exact(static_cast<uint32_t>(eventType), _bits);
}

inline constexpr bool areg::is_attribute_call(areg::EventType eventType) noexcept
{
    constexpr uint32_t _bits{ static_cast<uint32_t>(areg::EventType::EventLocalAttribute) & ~static_cast<uint32_t>(areg::EventType::EventLocalMask) };
    return areg::is_matching_exact(static_cast<uint32_t>(eventType), _bits);
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

inline constexpr bool areg::is_request_failure(areg::EventType eventType) noexcept
{
    constexpr uint32_t _bits
    {
        static_cast<uint32_t>(areg::EventType::EventLocalRequestFailed)
        & ~(  static_cast<uint32_t>(areg::EventType::EventLocalMask)
            | static_cast<uint32_t>(areg::EventType::EventRemoteMask))
    };
    return areg::is_matching_exact(static_cast<uint32_t>(eventType), _bits);
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
    case areg::EventType::EventToSystemMask:
        return "areg::EventType::EventToSystemMask";
    case areg::EventType::EventRequestMask:
        return "areg::EventType::EventRequestMask";
    case areg::EventType::EventResponseMask:
        return "areg::EventType::EventResponseMask";
    case areg::EventType::EventBroadcastMask:
        return "areg::EventType::EventBroadcastMask";
    case areg::EventType::EventAttributeMask:
        return "areg::EventType::EventAttributeMask";
    case areg::EventType::EventNotifyMask:
        return "areg::EventType::EventNotifyMask";
    case areg::EventType::EventConnectMask:
        return "areg::EventType::EventConnectMask";
    case areg::EventType::EventCustomMask:
        return "areg::EventType::EventCustomMask";
    case areg::EventType::EventFailureMask:
        return "areg::EventType::EventFailureMask";
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
    case areg::EventType::EventLocalNotifyResponse:
        return "areg::EventType::EventLocalNotifyResponse";
    case areg::EventType::EventRemoteNotifyResponse:
        return "areg::EventType::EventRemoteNotifyResponse";
    case areg::EventType::EventLocalNotifyBroadcast:
        return "areg::EventType::EventLocalNotifyBroadcast";
    case areg::EventType::EventRemoteNotifyBroadcast:
        return "areg::EventType::EventRemoteNotifyBroadcast";
    case areg::EventType::EventLocalNotifyAttribute:
        return "areg::EventType::EventLocalNotifyAttribute";
    case areg::EventType::EventRemoteNotifyAttribute:
        return "areg::EventType::EventRemoteNotifyAttribute";
    case areg::EventType::EventLocalResponse:
        return "areg::EventType::EventLocalResponse";
    case areg::EventType::EventRemoteResponse:
        return "areg::EventType::EventRemoteResponse";
    case areg::EventType::EventLocalBroadcast:
        return "areg::EventType::EventLocalBroadcast";
    case areg::EventType::EventRemoteBroadcast:
        return "areg::EventType::EventRemoteBroadcast";
    case areg::EventType::EventLocalAttribute:
        return "areg::EventType::EventLocalAttribute";
    case areg::EventType::EventRemoteAttribute:
        return "areg::EventType::EventRemoteAttribute";
    case areg::EventType::EventLocalProviderConnect:
        return "areg::EventType::EventLocalProviderConnect";
    case areg::EventType::EventRemoteProviderConnect:
        return "areg::EventType::EventRemoteProviderConnect";
    case areg::EventType::EventLocalConsumerConnect:
        return "areg::EventType::EventLocalConsumerConnect";
    case areg::EventType::EventRemoteConsumerConnect:
        return "areg::EventType::EventRemoteConsumerConnect";
    case areg::EventType::EventCustomInternal:
        return "areg::EventType::EventCustomInternal";
    case areg::EventType::EventCustomExternal:
        return "areg::EventType::EventCustomExternal";
    case areg::EventType::EventLocalRequestFailed:
        return "areg::EventType::EventLocalRequestFailed";
    case areg::EventType::EventRemoteRequestFailed:
        return "areg::EventType::EventRemoteRequestFailed";
    default:
        ASSERT(false);
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
        ASSERT(false);
        return "ERR: Undefined areg::EventPriority value!";
    }
}

inline constexpr const char* areg::as_string(areg::EventCallType callType) noexcept
{
    switch (callType)
    {
    case areg::EventCallType::Undefined:
        return "areg::EventCallType::Undefined";
    case areg::EventCallType::CallLocal:
        return "areg::EventCallType::CallLocal";
    case areg::EventCallType::CallRemote:
        return "areg::EventCallType::CallRemote";
    case areg::EventCallType::CallCustom:
        return "areg::EventCallType::CallCustom";
    case areg::EventCallType::CallService:
        return "areg::EventCallType::CallService";
    case areg::EventCallType::CallServiceLocal:
        return "areg::EventCallType::CallServiceLocal";
    case areg::EventCallType::CallServiceRemote:
        return "areg::EventCallType::CallServiceRemote";
    default:
        ASSERT(false);
        return "ERR: Undefined areg::EventCallType value!";
    }
}

#endif  // AREG_COMPONENT_EVENTDEFS_HPP
