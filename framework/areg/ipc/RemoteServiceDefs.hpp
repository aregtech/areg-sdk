#ifndef AREG_IPC_REMOTESERVICEDEFS_HPP
#define AREG_IPC_REMOTESERVICEDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/ipc/RemoteServiceDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Remote service namespace
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/component/EventDefs.hpp"

#include <string_view>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class StubAddress;
    class ProxyAddress;
    class Channel;
} // namespace areg

//////////////////////////////////////////////////////////////////////////
// Remote Service namespace declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The remote servicing namespace with shared objects and helper methods
 *          to create fixed communication messages.
 **/
namespace areg {
/**
 * \brief   areg::ConnectionType
 *          Remote services connection types.
 **/
enum class ConnectionType   : uint32_t
{
      Undefined     = 0 //!< Undefined connection
    , Tcpip         = 1 //!< Service connection via TCP/IP
    , Udp           = 2 //!< Service connection via UDP, not implemented at the moment
    , Web           = 4 //!< Service connection via Web socket
    , SharedMemory  = 8 //!< Service connection via Shared Memory
};

/**
 * \brief   Returns a string representation of the ConnectionType.
 **/
[[nodiscard]]
static inline constexpr const char* as_string(ConnectionType type) noexcept;

/**
 * \brief   areg::RemoteServiceKind
 *          Remote services
 **/
enum class RemoteServiceKind : uint32_t
{
      Unknown = 0 //!< Undefined service.
    , Router  = 1 //!< Message routing service.
    , Logger  = 2 //!< Message logging service.
};

/**
 * \brief   Returns a string representation of the RemoteServiceKind.
 **/
[[nodiscard]]
static inline constexpr const char* as_string(RemoteServiceKind kind) noexcept;

/**
 * \brief   Indicates the connect or disconnect of the remote instance.
 **/
enum class RemoteConnectionState : uint32_t
{
      Disconnected  = 0 //!< Remote instance is disconnected.
    , Connected     = 1 //!< Remote instance is connected.
};

/**
 * \brief   areg::DEFAULT_REMOTE_SERVICE_ENABLED
 *          Message router enable / disable default flag. If true, by default it is enabled.
 *          The default values are used if failed to read and parse router configuration file.
 **/
constexpr bool              DEFAULT_REMOTE_SERVICE_ENABLED  { true };

/**
 * \brief   Returns EventHeader template for the server connect (hello) message.
 **/
[[nodiscard]]
constexpr areg::EventHeader message_hello_server() noexcept;

/**
 * \brief   Returns EventHeader template for the server disconnect (bye) message.
 **/
[[nodiscard]]
constexpr areg::EventHeader message_bye_server() noexcept;

/**
 * \brief   Returns EventHeader template for the notify-client-connection message.
 **/
[[nodiscard]]
constexpr areg::EventHeader notify_client_connection() noexcept;

/**
 * \brief   Returns EventHeader template for the service registration request message.
 **/
[[nodiscard]]
constexpr areg::EventHeader message_register_service() noexcept;

/**
 * \brief   Returns EventHeader template for the query-instances message.
 **/
[[nodiscard]]
constexpr areg::EventHeader message_query_instances() noexcept;

/**
 * \brief   Returns EventHeader template for the notify-instances message.
 **/
[[nodiscard]]
constexpr areg::EventHeader message_notify_instances() noexcept;

/**
 * \brief   Returns EventHeader template for the register-notify message.
 **/
[[nodiscard]]
constexpr areg::EventHeader message_register_notify() noexcept;

/**
 * \brief   Creates a connection request message with specified source, target, and message
 *          source type.
 *
 * \param   source          The ID of the source that generates and sends the message.
 * \param   target          The ID of the target to send the connect message request.
 * \param   msgSource       The message source of the application to connect to service.
 * \return  Returns initialized connection request message.
 **/
[[nodiscard]]
AREG_API MessageEnvelope create_connect_request(const ITEM_ID & source, const ITEM_ID & target, areg::MessageSource msgSource);

/**
 * \brief   Creates a disconnection request message with specified source and target.
 *
 * \param   source      The ID of the source that sends the disconnect message request.
 * \param   target      The ID of the target to send the disconnect message request.
 * \return  Returns initialized disconnection request message.
 **/
[[nodiscard]]
AREG_API MessageEnvelope create_disconnect_request( const ITEM_ID & source, const ITEM_ID & target );

/**
 * \brief   Creates a connection available notification message.
 *
 * \param   source      The ID of the source that sends the connect notification message.
 * \param   target      The ID of the target to send the connect notification message.
 * \return  Returns initialized connection available notification message.
 **/
[[nodiscard]]
AREG_API MessageEnvelope create_connect_notify( const ITEM_ID & source, const ITEM_ID & target );

/**
 * \brief   Creates a connection rejection notification message.
 *
 * \param   source      The ID of the source that sends the connection rejected message.
 * \param   target      The ID of the target to send the connection rejected message.
 * \return  Returns initialized connection rejection notification message.
 **/
[[nodiscard]]
AREG_API MessageEnvelope create_reject_notify( const ITEM_ID & source, const ITEM_ID & target );

/**
 * \brief   Creates a disconnection notification message sent by server to clients.
 *
 * \param   source      The ID of the source that sends the disconnect notification message.
 * \param   target      The ID of the target to send the disconnect notification message.
 * \return  Returns initialized disconnection notification message.
 **/
[[nodiscard]]
AREG_API MessageEnvelope create_disconnect_notify( const ITEM_ID & source, const ITEM_ID & target );

/**
 * \brief   Creates a message to register a Stub at the router.
 *
 * \param   stub        The address of remote Stub service, which is registering.
 * \param   source      The ID of the source that sends the request message to register service provider.
 * \param   target      The ID of the target to send the request message to register service provider.
 * \return  Returns initialized Stub registration message.
 **/
[[nodiscard]]
AREG_API MessageEnvelope router_register_service( const StubAddress & stub, const ITEM_ID & source, const ITEM_ID & target);

/**
 * \brief   Creates a message to unregister a Stub from the router.
 *
 * \param   stub        The address of remote Stub service, which is unregistering.
 * \param   reason      The unregister / disconnect reason.
 * \param   source      The ID of the source that sends the request message to unregister service provider.
 * \param   target      The ID of the target to send the request message to unregister service provider.
 * \return  Returns initialized Stub unregistration message.
 **/
[[nodiscard]]
AREG_API MessageEnvelope router_unregister_service( const StubAddress & stub, areg::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target);

/**
 * \brief   Creates a message to register a Proxy at the router.
 *
 * \param   proxy       The address of remote Proxy, which is registering.
 * \param   source      The ID of the source that sends the request message to register service consumer.
 * \param   target      The ID of the target to send the request message to register service consumer.
 * \return  Returns initialized Proxy registration message.
 **/
[[nodiscard]]
AREG_API MessageEnvelope router_register_consumer( const ProxyAddress & proxy, const ITEM_ID & source, const ITEM_ID & target );

/**
 * \brief   Creates a message to unregister a Proxy from the router.
 *
 * \param   proxy       The address of remote Proxy, which is unregistering.
 * \param   reason      The unregister / disconnect reason.
 * \param   source      The ID of the source that sends the request message to unregister service consumer.
 * \param   target      The ID of the target to send the request message to unregister service consumer.
 * \return  Returns initialized Proxy unregistration message.
 **/
[[nodiscard]]
AREG_API MessageEnvelope router_unregister_consumer( const ProxyAddress & proxy, areg::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target);

/**
 * \brief   Creates a Stub availability notification message to broadcast.
 *
 * \param   stub        The address of remote Stub to notify registering.
 * \param   source      The ID of the source that sends the service provider is registered notification message.
 * \param   target      The ID of the target to send the service provider is registered notification message.
 * \return  Returns initialized Stub availability notification message.
 **/
[[nodiscard]]
AREG_API MessageEnvelope service_registered_event( const StubAddress & stub, const ITEM_ID & source, const ITEM_ID & target );

/**
 * \brief   Creates a Stub unavailability notification message to broadcast.
 *
 * \param   stub        The address of remote Stub to notify unregistering.
 * \param   reason      The unregister / disconnect reason.
 * \param   source      The ID of the source that sends the service provider is unregistered notification message.
 * \param   target      The ID of the target to send the service provider is unregistered notification message.
 * \return  Returns initialized Stub unavailability notification message.
 **/
[[nodiscard]]
AREG_API MessageEnvelope service_unregistered_event( const StubAddress & stub, areg::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target);

/**
 * \brief   Creates a Proxy availability notification message to broadcast.
 *
 * \param   proxy       The address of remote Proxy to notify registering.
 * \param   source      The ID of the source that sends the service consumer is registered notification message.
 * \param   target      The ID of the target to send the service consumer is registered notification message.
 * \return  Returns initialized Proxy availability notification message.
 **/
[[nodiscard]]
AREG_API MessageEnvelope client_registered_event( const ProxyAddress & proxy, const ITEM_ID & source, const ITEM_ID & target);

/**
 * \brief   Creates a Proxy unavailability notification message to broadcast.
 *
 * \param   proxy       The address of remote Proxy to notify unregistering.
 * \param   reason      The unregister / disconnect reason.
 * \param   source      The ID of the source that sends the service consumer is unregistered notification message.
 * \param   target      The ID of the target to send the service consumer is unregistered notification message.
 * \return  Returns initialized Proxy unavailability notification message.
 **/
[[nodiscard]]
AREG_API MessageEnvelope client_unregistered_event( const ProxyAddress & proxy, areg::DisconnectReason reason, const ITEM_ID & source, const ITEM_ID & target);

/**
 * \brief   Checks whether specified message is a connect request.
 *
 * \param   msg     The received envelope to check.
 * \return  Returns true if the message is a connect request.
 **/
[[nodiscard]]
AREG_API bool is_server_hello( const MessageEnvelope & msg );

/**
 * \brief   Checks whether specified message is a disconnect request.
 *
 * \param   msg     The received envelope to check.
 * \return  Returns true if the message is a disconnect request.
 **/
[[nodiscard]]
AREG_API bool is_server_bye( const MessageEnvelope & msg );

/**
 * \brief   Checks whether specified message is a client notification request.
 *
 * \param   msg     The received envelope to check.
 * \return  Returns true if the message is a client notification request.
 **/
[[nodiscard]]
AREG_API bool is_notify_client( const MessageEnvelope & msg );

/**
 * \brief   Checks whether specified message is a service registration request.
 *
 * \param   msg     The received envelope to check.
 * \return  Returns true if the message is a service registration request.
 **/
[[nodiscard]]
AREG_API bool is_register_message( const MessageEnvelope & msg );

//////////////////////////////////////////////////////////////////////////
// Make RemoteServiceKind and ConnectionType streamable
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_STREAMABLE(areg::RemoteServiceKind);
AREG_IMPLEMENT_STREAMABLE(areg::ConnectionType);
AREG_IMPLEMENT_STREAMABLE(areg::RemoteConnectionState);

} // namespace areg

constexpr areg::EventHeader areg::message_hello_server() noexcept
{
    areg::EventHeader hdr{};
    hdr.checksum  = areg::CHECKSUM_INVALID;
    hdr.target    = static_cast<uint32_t>(areg::COOKIE_ROUTER);
    hdr.messageId = static_cast<uint32_t>(areg::FuncIdRange::SystemServiceConnect);
    hdr.eventType = static_cast<uint16_t>(areg::EventType::EventRemoteConnection);
    hdr.result    = areg::MESSAGE_SUCCESS;
    hdr.sequenceNr= areg::SEQUENCE_NUMBER_NOTIFY;
    return hdr;
}

constexpr areg::EventHeader areg::message_bye_server() noexcept
{
    areg::EventHeader hdr{};
    hdr.checksum  = areg::CHECKSUM_INVALID;
    hdr.target    = static_cast<uint32_t>(areg::COOKIE_ROUTER);
    hdr.messageId = static_cast<uint32_t>(areg::FuncIdRange::SystemServiceDisconnect);
    hdr.eventType = static_cast<uint16_t>(areg::EventType::EventRemoteConnection);
    hdr.result    = areg::MESSAGE_SUCCESS;
    hdr.sequenceNr = areg::SEQUENCE_NUMBER_ANY;
    return hdr;
}

constexpr areg::EventHeader areg::notify_client_connection() noexcept
{
    areg::EventHeader hdr{};
    hdr.checksum  = areg::CHECKSUM_INVALID;
    hdr.source    = static_cast<uint32_t>(areg::COOKIE_ROUTER);
    hdr.messageId = static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyConnection);
    hdr.eventType = static_cast<uint16_t>(areg::EventType::EventRemoteConnection);
    hdr.result    = areg::MESSAGE_SUCCESS;
    hdr.sequenceNr = areg::SEQUENCE_NUMBER_NOTIFY;
    return hdr;
}

constexpr areg::EventHeader areg::message_register_service() noexcept
{
    areg::EventHeader hdr{};
    hdr.checksum  = areg::CHECKSUM_INVALID;
    hdr.target    = static_cast<uint32_t>(areg::COOKIE_ROUTER);
    hdr.messageId = static_cast<uint32_t>(areg::FuncIdRange::SystemServiceRequestRegister);
    hdr.eventType = static_cast<uint16_t>(areg::EventType::EventRemoteConnection);
    hdr.result    = areg::MESSAGE_SUCCESS;
    hdr.sequenceNr = areg::SEQUENCE_NUMBER_NOTIFY;
    return hdr;
}

constexpr areg::EventHeader areg::message_query_instances() noexcept
{
    areg::EventHeader hdr{};
    hdr.checksum  = areg::CHECKSUM_INVALID;
    hdr.target = static_cast<uint32_t>(areg::COOKIE_ROUTER);
    hdr.messageId = static_cast<uint32_t>(areg::FuncIdRange::SystemServiceQueryInstances);
    hdr.eventType = static_cast<uint16_t>(areg::EventType::EventRemoteConnection);
    hdr.result    = areg::MESSAGE_SUCCESS;
    hdr.sequenceNr = areg::SEQUENCE_NUMBER_NOTIFY;
    return hdr;
}

constexpr areg::EventHeader areg::message_notify_instances() noexcept
{
    areg::EventHeader hdr{};
    hdr.checksum  = areg::CHECKSUM_INVALID;
    hdr.target    = static_cast<uint32_t>(areg::COOKIE_ROUTER);
    hdr.messageId = static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyInstances);
    hdr.eventType = static_cast<uint16_t>(areg::EventType::EventRemoteConnection);
    hdr.result    = areg::MESSAGE_SUCCESS;
    hdr.sequenceNr = areg::SEQUENCE_NUMBER_NOTIFY;
    return hdr;
}

constexpr areg::EventHeader areg::message_register_notify() noexcept
{
    areg::EventHeader hdr{};
    hdr.checksum  = areg::CHECKSUM_INVALID;
    hdr.source    = static_cast<uint32_t>(areg::COOKIE_ROUTER);
    hdr.messageId = static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyRegister);
    hdr.eventType = static_cast<uint16_t>(areg::EventType::EventRemoteConnection);
    hdr.result    = areg::MESSAGE_SUCCESS;
    hdr.sequenceNr = areg::SEQUENCE_NUMBER_NOTIFY;
    return hdr;
}

inline constexpr const char* areg::as_string(areg::ConnectionType type) noexcept
{
    switch (type)
    {
    case ConnectionType::Undefined:
        return "Undefined";
    case ConnectionType::Tcpip:
        return "TCP/IP";
    case ConnectionType::Udp:
        return "UDP";
    case ConnectionType::Web:
        return "Web Socket";
    case ConnectionType::SharedMemory:
        return "Shared Memory";
    default:
        return "Unknown";
    }
}

constexpr const char* areg::as_string(areg::RemoteServiceKind kind) noexcept
{
    switch (kind)
    {
    case areg::RemoteServiceKind::Unknown:
        return "areg::RemoteServiceKindUnknown";
    case areg::RemoteServiceKind::Router:
        return "areg::RemoteServiceKind::Router";
    case areg::RemoteServiceKind::Logger:
        return "areg::RemoteServiceKindLogger";
    default:
        return "ERR: Unknown areg::RemoteServiceKind type!";
    }
}

#endif  // AREG_IPC_REMOTESERVICEDEFS_HPP
