#ifndef AREG_IPC_PRIVATE_ROUTERCLIENT_HPP
#define AREG_IPC_PRIVATE_ROUTERCLIENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/RouterClient.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service client connection 
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/component/RemoteEventConsumer.hpp"
#include "areg/ipc/RegistrationProvider.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/ipc/ServiceClientConnectionBase.hpp"
#include "areg/component/RemoteEventFactory.hpp"

namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class RegistrationConsumer;

//////////////////////////////////////////////////////////////////////////
// RouterClient class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Handles router client connection management, message processing, service
 *          provider/consumer registration, and event dispatching.
 **/
class RouterClient final    : public    ServiceClientConnectionBase
                            , public    RegistrationProvider
                            , protected DispatcherThread
                            , private   RemoteMessageHandler
                            , private   RemoteEventConsumer
{
private:
    //! The prefix to add to the send and receive message threads.
    static constexpr std::string_view   PREFIX_THREAD{ "router_" };
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes router client with connection and registration consumer handlers.
     *
     * \param   connectionConsumer      The instance of remote service connection consumer object to
     *                                  handle service connection notifications.
     * \param   registerConsumer        The instance of remote service registration consumer to
     *                                  handle service register notification.
     **/
    RouterClient(ConnectionConsumer& connectionConsumer, RegistrationConsumer & registerConsumer);
    ~RouterClient() override = default;

//////////////////////////////////////////////////////////////////////////
// Raw-message fast-path helpers
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Sends a pre-serialized MessageEnvelope directly to the send thread,
     *          bypassing all event creation and serialization overhead.
     *
     * \param   msg     The pre-built envelope to send.
     * \return  Returns true if the message was accepted by the send thread.
     **/
    inline bool send_raw_message(const MessageEnvelope& msg);

    /**
     * \brief   Returns the active IPC connection channel.
     *          The channel carries the router cookie stamped in every outgoing message
     *          Valid only after the connection handshake completes
     *          (i.e. when is_connection_started() returns true).
     *
     * \return  Const reference to the active Channel object.
     **/
    inline const areg::Channel& connection_channel() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

/************************************************************************/
// ServiceClientConnectionBase overrides
/************************************************************************/

    /**
     * \brief   Initiates connection to remote service. Host name and port must be set beforehand.
     *
     * \return  Returns true if connection is triggered.
     **/
    bool connect_service_host() final;

    /**
     * \brief   Stops service connection and disables remote communication.
     **/
    void disconnect_service_host() final;

    /**
     * \brief   Called when service needs to shut down.
     **/
    void on_service_exit() final;

    /**
     * \brief   Returns true if remote service connection is pending (triggered but not yet connected).
     **/
    bool is_host_pending() const final;

/************************************************************************/
// RemoteMessageHandler interface overrides
/************************************************************************/

    /**
     * \brief   Called when message sending fails.
     *
     * \param   msgFailed       The message, which failed to send.
     * \param   whichTarget     The target socket to send message.
     **/
    void failed_send_message( const MessageEnvelope & msgFailed, Socket & whichTarget ) final;

    /**
     * \brief   Called when message receiving fails.
     *
     * \param   whichSource     Indicates the failed source socket to receive message.
     **/
    void failed_receive_message( Socket & whichSource ) final;

    /**
     * \brief   Called when message processing fails (target not found). For requests, error
     *          notification is sent to source.
     *
     * \param   msgUnprocessed      Unprocessed message data.
     **/
    void failed_process_message( const MessageEnvelope & msgUnprocessed ) final;

    /**
     * \brief   Called to process a received message from specified source socket.
     *
     * \param   msgReceived     Received message to process.
     * \param   whichSource     The source socket, which received message.
     **/
    void process_received_message( MessageEnvelope & msgReceived, Socket & whichSource ) final;

/************************************************************************/
// RegistrationProvider interface overrides
/************************************************************************/

    /**
     * \brief   Registers a service provider (Stub) in the system. Connected consumers receive
     *          connection notifications.
     *
     * \param   stubService     The address of service provider to register in the system.
     * \return  Returns true if registration succeeded.
     **/
    bool register_service_provider( const StubAddress & stubService ) final;

    /**
     * \brief   Unregisters a service provider (Stub) from the system. Connected consumers receive
     *          disconnect notifications.
     *
     * \param   stubService     The address of service provider to unregister in the system.
     * \param   reason          The reason to unregister and disconnect the service provider.
     **/
    void unregister_service_provider( const StubAddress & stubService, const areg::DisconnectReason reason ) final;

    /**
     * \brief   Registers a service consumer (Proxy) in the system. If provider is available, both
     *          receive connection notification.
     *
     * \param   proxyService    The address of the service consumer to register in system.
     * \return  Returns true if registration process started successfully.
     **/
    bool register_service_consumer( const ProxyAddress & proxyService ) final;

    /**
     * \brief   Unregisters a service consumer (Proxy) from the system. Provider receives disconnect
     *          notification.
     *
     * \param   proxyService    The address of the service consumer to unregister from the system.
     * \param   reason          The reason to unregister and disconnect the service consumer.
     **/
    void unregister_service_consumer( const ProxyAddress & proxyService, const areg::DisconnectReason reason ) final;

/************************************************************************/
// EventRouter interface overrides
/************************************************************************/

    /**
     * \brief   Posts an event for delivery to its target. Base implementation destroys event
     *          without processing.
     *
     * \param   eventElem       Event object to post.
     * \return  Always returns true in this base class.
     **/
    [[nodiscard]]
    bool post_event( Event & eventElem ) final;

/************************************************************************/
// RemoteEventConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Called when Stub receives a remote request event to process.
     *
     * \param   reqEvent    The remote request event to be processed.
     **/
    void process_request_event( ServiceRequestEvent & reqEvent ) final;

    /**
     * \brief   Called when Stub receives a remote notification subscription request (start/stop attribute updates).
     *
     * \param   reqNotifyEvent      The remote notification request event to be processed.
     **/
    void process_notify_request(ServiceRequestEvent& reqNotifyEvent ) final;

    /**
     * \brief   Called when Stub receives a remote response subscription request.
     *
     * \param   respEvent       The remote response event sent on processed request.
     **/
    void process_response_event(ServiceResponseEvent& respEvent ) final;

/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Enables or disables event dispatching thread to receive events. Override for custom preparation.
     *
     * \param   is_ready     The flag to indicate whether the dispatcher is ready for events.
     **/
    void ready_for_events( bool is_ready ) final;

/************************************************************************/
// ServiceEventConsumer interface overrides
/************************************************************************/
    /**
     * \brief   Called to process and dispatch a received communication message.
     *
     * \param   msgReceived     The received communication message.
     **/
    void on_message_received(const MessageEnvelope& msgReceived) final;

//////////////////////////////////////////////////////////////////////////
// Hidden operations and attributes
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Returns reference to this router client object.
     **/
    [[nodiscard]]
    inline RouterClient & self() noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of service register consumer.
     **/
    RegistrationConsumer &     mRegisterConsumer;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    RouterClient() = delete;
    AREG_NOCOPY_NOMOVE( RouterClient );
};

//////////////////////////////////////////////////////////////////////////
// RouterClient class inline methods implementation
//////////////////////////////////////////////////////////////////////////

inline RouterClient & RouterClient::self() noexcept
{
    return (*this);
}

inline bool RouterClient::send_raw_message(const MessageEnvelope& msg)
{
    return send_message(msg);
}

inline const areg::Channel& RouterClient::connection_channel() const noexcept
{
    return mChannel;
}

inline void RouterClient::on_message_received(const MessageEnvelope& msgReceived)
{
    ASSERT(areg::is_executable_id(static_cast<uint32_t>(msgReceived.message_id())));
    MessageEnvelope routable{ msgReceived };
    if (!RemoteEventFactory::route_incoming_message(routable, mChannel))
    {
        failed_process_message(msgReceived);
    }
}

} // namespace areg
#endif  // AREG_IPC_PRIVATE_ROUTERCLIENT_HPP
