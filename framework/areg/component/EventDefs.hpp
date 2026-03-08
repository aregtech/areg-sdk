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
 * \brief       Areg Platform, areg namespace contains
 *              collection of constants for events.
 *
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/IOStream.hpp"

namespace areg {
/**
 * \brief   Event types.
 **/
enum class EventType    : uint32_t
{
        EventUnknown              =    0  /*0x0000*/  //!< Unknown event type.      Bit set: 0000 0000 0000 0000

    , EventInternal             =    1  /*0x0001*/  //!< Internal event.          Bit set: 0000 0000 0000 0001
    , EventExternal             =    2  /*0x0002*/  //!< External event.          Bit set: 0000 0000 0000 0010

    , EventLocal                =   16  /*0x0010*/  //!< Local event.             Bit set: 0000 0000 0001 0000
    , EventRemote               =   32  /*0x0020*/  //!< Remote event.            Bit set: 0000 0000 0010 0000

    , EventNotify               =  256  /*0x0100*/  //!< Notification event.      Bit set: 0000 0001 0000 0000
    , EventToStub               =  512  /*0x0200*/  //!< Stub event.              Bit set: 0000 0010 0000 0000
    , EventToProxy              = 1024  /*0x0400*/  //!< Proxy event.             Bit set: 0000 0100 0000 0000
    , EventConnect              = 2048  /*0x0800*/  //!< Connection event.        Bit set: 0000 1000 0000 0000

    , EventNotifyClient         =  273  /*0x0111*/  //!< Client Notification.     Bit set: 0000 0001 0001 0001

    , EventLocalServiceRequest  =  530  /*0x0212*/  //!< Local Service Request.   Bit set: 0000 0010 0001 0010
    , EventRemoteServiceRequest =  546  /*0x0222*/  //!< Remote Service Request.  Bit set: 0000 0010 0010 0010

    , EventLocalNotifyRequest   =  786  /*0x0312*/  //!< Local Request notify.    Bit set: 0000 0011 0001 0010
    , EventRemoteNotifyRequest  =  802  /*0x0322*/  //!< Remote  Request notify.  Bit set: 0000 0011 0010 0010

    , EventLocalServiceResponse = 1042  /*0x0412*/  //!< Local Service Response.  Bit set: 0000 0100 0001 0010
    , EventRemoteServiceResponse= 1058  /*0x0422*/  //!< Remote Service Response. Bit set: 0000 0100 0010 0010

    , EventLocalStubConnect     = 2834  /*0x0B12*/  //!< Local Stub Connection.   Bit set: 0000 1011 0001 0010
    , EventRemoteStubConnect    = 2850  /*0x0B22*/  //!< Remote Stub Connection.  Bit set: 0000 1011 0010 0010

    , EventLocalProxyConnect    = 3346  /*0x0D12*/  //!< Local Proxy Connection.  Bit set: 0000 1101 0001 0010
    , EventRemoteProxyConnect   = 3362  /*0x0D22*/  //!< Remote Proxy Connection. Bit set: 0000 1101 0010 0010

    , EventCustom               =32784  /*0x8010*/  //!< Custom event.            Bit set: 1000 0000 0001 0000
    , EventCustomInternal       =32785  /*0x8011*/  //!< Custom Notification.     Bit set: 1000 0000 0001 0001
    , EventCustomExternal       =32786  /*0x8012*/  //!< Custom Thread event.     Bit set: 1000 0000 0001 0010
};

/**
 * \brief   Converts an event type to its string representation.
 **/
[[nodiscard]]
static inline const char* as_string(EventType eventType) noexcept;

/**
 * \brief   The priorities of the events.
 **/
enum class EventPriority : uint32_t
{
      UndefinedPrio    //!< Undefined priority, should not be used.
    , LowPrio          //!< The priority of the event is low, should be processed only if there is no event in the queue.
    , NormalPrio       //!< The priority of the event is normal, should be processed in FIFO principle.
    , HighPrio         //!< The priority of the event is high, should be processed before any other events.
    , CriticalPrio     //!< The priority of the event is critical, should be processed nearly immediately.
    , IgnorePrio       //!< The priority of event should be ignored. Should be set in event.
    , ExitPrio         //!< The highest priority of the event. Should be processed as soon as possible and should not be removed from the stack.
};

/**
 * \brief   Converts an event priority to its string representation.
 **/
[[nodiscard]]
static inline const char* as_string(EventPriority eventPrio) noexcept;

/**
 * \brief   The default priority of the events.
 **/
static constexpr EventPriority DefaultPriority{ EventPriority::NormalPrio };

/**
 * \brief   Returns true if the event type is internal.
 **/
[[nodiscard]]
inline static bool is_internal(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event type is external.
 **/
[[nodiscard]]
inline static bool is_external(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event type is local.
 **/
[[nodiscard]]
inline static bool is_local(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event type is remote.
 **/
[[nodiscard]]
inline static bool is_remote(areg::EventType eventType) noexcept;

/**
 * \brief   Returns true if the event type is developer custom, false if system predefined.
 **/
[[nodiscard]]
inline static bool is_custom(areg::EventType eventType) noexcept;

AREG_IMPLEMENT_STREAMABLE(areg::EventType)

}

inline bool areg::is_internal(areg::EventType eventType) noexcept
{
    return (static_cast<uint32_t>(eventType) & static_cast<uint32_t>(areg::EventType::EventInternal)) != 0;
}

inline bool areg::is_external(areg::EventType eventType) noexcept
{
    return (static_cast<uint32_t>(eventType) & static_cast<uint32_t>(areg::EventType::EventExternal)) != 0;
}

inline bool areg::is_local(areg::EventType eventType) noexcept
{
    return (static_cast<uint32_t>(eventType) & static_cast<uint32_t>(areg::EventType::EventLocal)) != 0;
}

inline bool areg::is_remote(areg::EventType eventType) noexcept
{
    return (static_cast<uint32_t>(eventType) & static_cast<uint32_t>(areg::EventType::EventRemote)) != 0;
}

inline bool areg::is_custom(areg::EventType eventType) noexcept
{
    return (static_cast<uint32_t>(eventType) & static_cast<uint32_t>(areg::EventType::EventCustom)) != 0;
}

inline const char* areg::as_string(areg::EventType eventType) noexcept
{
    switch (eventType)
    {
    case areg::EventType::EventUnknown:
        return "areg::EventType::EventUnknown";

    case areg::EventType::EventInternal:
        return "areg::EventType::EventInternal";
    case areg::EventType::EventExternal:
        return "areg::EventType::EventExternal";

    case areg::EventType::EventLocal:
        return "areg::EventType::EventLocal";
    case areg::EventType::EventRemote:
        return "areg::EventType::EventRemote";

    case areg::EventType::EventNotify:
        return "areg::EventType::EventNotify";
    case areg::EventType::EventToStub:
        return "areg::EventType::EventToStub";
    case areg::EventType::EventToProxy:
        return "areg::EventType::EventToProxy";
    case areg::EventType::EventConnect:
        return "areg::EventType::EventConnect";

    case areg::EventType::EventNotifyClient:
        return "areg::EventType::EventNotifyClient";

    case areg::EventType::EventLocalServiceRequest:
        return "areg::EventType::EventLocalServiceRequest";
    case areg::EventType::EventRemoteServiceRequest:
        return "areg::EventType::EventRemoteServiceRequest";

    case areg::EventType::EventLocalNotifyRequest:
        return "areg::EventType::EventLocalNotifyRequest";
    case areg::EventType::EventRemoteNotifyRequest:
        return "areg::EventType::EventRemoteNotifyRequest";

    case areg::EventType::EventLocalServiceResponse:
        return "areg::EventType::EventLocalServiceResponse";
    case areg::EventType::EventRemoteServiceResponse:
        return "areg::EventType::EventRemoteServiceResponse";

    case areg::EventType::EventLocalStubConnect:
        return "areg::EventType::EventLocalStubConnect";
    case areg::EventType::EventRemoteStubConnect:
        return "areg::EventType::EventRemoteStubConnect";

    case areg::EventType::EventLocalProxyConnect:
        return "areg::EventType::EventLocalProxyConnect";
    case areg::EventType::EventRemoteProxyConnect:
        return "areg::EventType::EventRemoteProxyConnect";

    case areg::EventType::EventCustom:
        return "areg::EventType::EventCustom";
    case areg::EventType::EventCustomInternal:
        return "areg::EventType::EventCustomInternal";
    case areg::EventType::EventCustomExternal:
        return "areg::EventType::EventCustomExternal";

    default:
        return "ERR: Undefined areg::EventType value!";
    }
}

inline const char* areg::as_string(areg::EventPriority eventPrio) noexcept
{
    switch (eventPrio)
    {
    case areg::EventPriority::UndefinedPrio:
        return "areg::EventPriority::UndefinedPrio";
    case areg::EventPriority::LowPrio:
        return "areg::EventPriority::LowPrio";
    case areg::EventPriority::NormalPrio:
        return "areg::EventPriority::NormalPrio";
    case areg::EventPriority::HighPrio:
        return "areg::EventPriority::HighPrio";
    case areg::EventPriority::CriticalPrio:
        return "areg::EventPriority::CriticalPrio";
    case areg::EventPriority::IgnorePrio:
        return "areg::EventPriority::IgnorePrio";
    case areg::EventPriority::ExitPrio:
        return "areg::EventPriority::ExitPrio";
    default:
        return "ERR: Undefined areg::EventPriority value!";
    }
}

#endif  // AREG_COMPONENT_EVENTDEFS_HPP
