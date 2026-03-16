#ifndef AREG_IPC_SERVICECLIENTCONNECTIONBASE_HPP
#define AREG_IPC_SERVICECLIENTCONNECTIONBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/ServiceClientConnectionBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service client connection 
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/ipc/ConnectionProvider.hpp"
#include "areg/ipc/ServiceEventConsumer.hpp"

#include "areg/ipc/ClientConnection.hpp"
#include "areg/ipc/private/ClientReceiveThread.hpp"
#include "areg/ipc/private/ClientSendThread.hpp"
#include "areg/component/Channel.hpp"
#include "areg/component/Timer.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/String.hpp"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class ConnectionConsumer;
class RemoteMessageHandler;
//////////////////////////////////////////////////////////////////////////
// ServiceClientConnectionBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Manages remote service client connections, handling message I/O, connection state
 *          transitions, and communication with the service router.
 **/
class AREG_API ServiceClientConnectionBase  : public    ConnectionProvider
                                            , public    ServiceEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   ServiceClientConnectionBase::ConnectionPhase
     *          Defines connection state values
     **/
    enum class ConnectionPhase : uint16_t
    {
          DisconnectState       = 1     //!< 0000 0001, The disconnect state
        , ConnectionStopped     = 3     //!< 0000 0011, The connection is stopped, i.e. not connected.
        , ConnectionStopping    = 5     //!< 0000 0101, The connection is stopping, because of manual request.
        , ConnectState          = 16    //!< 0001 0000, the connect state
        , ConnectionStarting    = 48    //!< 0011 0000, The connection is initiated, but the status is not known.
        , ConnectionStarted     = 80    //!< 0101 0000, The connection is established and accepted by server.
    };

    /**
     * \brief   Returns the string representation of a ConnectionPhase value.
     **/
    static inline constexpr const char * as_string(ServiceClientConnectionBase::ConnectionPhase val);

    /**
     * \brief   Returns true if the given state has the specified phase bit set.
     *          Used to test the DisconnectState and ConnectState bitmask bits.
     **/
    static constexpr bool has_phase_bit(ConnectionPhase state, ConnectionPhase mask) noexcept
    {
        return (static_cast<uint16_t>(state) & static_cast<uint16_t>(mask)) != 0;
    }

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes client connection with service target, connection types, handlers, and
     *          dispatcher thread.
     *
     * \param   target                  The ID of remote target service to send messages.
     * \param   service                 The type of remote service.
     * \param   connectTypes            The bitwise set of connection types.
     * \param   msgSource               The type of message source application that sends messages.
     * \param   connectionConsumer      The instance of remote service connection consumer object to
     *                                  handle service connection notifications.
     * \param   messageHandler          The instance of message handler.
     * \param   messageDispatcher       The instance of message dispatcher.
     * \param   prefixName              The prefix to add to the names of message receive and send
     *                                  threads.
     **/
    ServiceClientConnectionBase(  const ITEM_ID & target
                                , areg::RemoteServiceKind service
                                , uint32_t connectTypes
                                , areg::MessageSource msgSource
                                , ConnectionConsumer& connectionConsumer
                                , RemoteMessageHandler & messageHandler
                                , DispatcherThread & messageDispatcher
                                , const String & prefixName);
    virtual ~ServiceClientConnectionBase();

//////////////////////////////////////////////////////////////////////////
// Attribute
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the connection cookie identifier.
     **/
    [[nodiscard]]
    inline const ITEM_ID & connection_cookie() const noexcept;

    /**
     * \brief   Returns the number of bytes sent since the last query and resets the counter.
     **/
    [[nodiscard]]
    inline uint32_t query_bytes_sent() noexcept;

    /**
     * \brief   Returns the number of bytes received since the last query and resets the counter.
     **/
    [[nodiscard]]
    inline uint32_t query_bytes_received() noexcept;

    /**
     * \brief   Enable or disable the data rate calculation.
     *
     * \param   enable      If true, the data rate calculation is enabled. Otherwise, it is
     *                      disabled.
     **/
    inline void enable_data_rate(bool enable);

    /**
     * \brief   Returns true if data rate calculation is enabled.
     **/
    [[nodiscard]]
    inline bool is_data_rate_enabled() const noexcept;

    /**
     * \brief   Returns true if the connection status is either connecting or connected.
     **/
    [[nodiscard]]
    inline bool is_connect_state() const noexcept;

    /**
     * \brief   Returns true if the connection state is connected.
     **/
    [[nodiscard]]
    inline bool is_connected_state() const noexcept;

    /**
     * \brief   Returns true if the connection status is either disconnecting or disconnected.
     **/
    [[nodiscard]]
    inline bool is_disconnect_state() const noexcept;

    /**
     * \brief   Registers the client socket connection thread to receive service commands.
     **/
    inline void register_client_commands();

    /**
     * \brief   Unregisters the client socket connection thread to receive service commands.
     **/
    inline void unregister_client_commands();

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

/************************************************************************/
// ServiceClientConnectionBase interface overrides
/************************************************************************/

    /**
     * \brief   Called when service notifies the client of connection status changes (accepted,
     *          disconnected, rejected). Override to implement custom connection handling.
     *
     * \param   msgReceived     The message sent by service to the client.
     **/
    virtual void service_connection_event(const RemoteMessage& msgReceived);

/************************************************************************/
// ConnectionProvider interface overrides
/************************************************************************/

    /**
     * \brief   Configures remote service by reading configuration file and initializing connection
     *          settings.
     *
     * \param   service         The type of remote service to setup.
     * \param   connectTypes    The type of connection to setup.
     * \return  Returns true if configuration succeeded; false otherwise.
     **/
    bool setup_connection_data(areg::RemoteServiceKind service, uint32_t connectTypes) override;

    /**
     * \brief   Sets router service host name and port number. Note: does not restart service if
     *          already started.
     *
     * \param   hostName    IP-address or host name of routing service to connect.
     * \param   portNr      Port number of routing service to connect.
     **/
    void apply_connection_data( const String & hostName, uint16_t portNr ) override;

    /**
     * \brief   Initiates connection to remote service. Host name and port must be set beforehand.
     *
     * \return  Returns true if connection is triggered.
     **/
    bool connect_service_host() override;

    /**
     * \brief   Restarts remote service connection. If connected, the connection is lost and
     *          re-established; otherwise starts a new connection.
     *
     * \return  Returns true if reconnection succeeded.
     **/
    bool reconnect_service_host() override;

    /**
     * \brief   Stops service connection and disables remote communication.
     **/
    void disconnect_service_host() override;

    /**
     * \brief   Returns true if remote service is started and ready to operate.
     **/
    bool is_host_connected() const override;

    /**
     * \brief   Returns true if remote service connection is pending (triggered but not yet
     *          connected).
     **/
    bool is_host_pending() const override;

    /**
     * \brief   Returns true if service is configured and ready to start.
     **/
    bool is_host_setup() const override;

    /**
     * \brief   Creates a service connection request message with specified source, target, and
     *          message source type.
     *
     * \param   source          The ID of the source that sends connection message request.
     * \param   target          The ID of the target to send the connection message request.
     * \param   msgSource       The message source type of the connected client.
     * \return  Returns the created message for remote communication.
     **/
    RemoteMessage connect_message( const ITEM_ID & source, const ITEM_ID & target, areg::MessageSource msgSource) const override;

    /**
     * \brief   Creates a service disconnection request message with specified source and target.
     *
     * \param   source      The ID of the source that sends the disconnect message request.
     * \param   target      The ID of the target to send the disconnection message request.
     * \return  Returns the created message for remote communication.
     **/
    RemoteMessage disconnect_message( const ITEM_ID & source, const ITEM_ID & target ) const override;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// ServiceEventConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Called when the reconnection timer expires.
     **/
    void on_reconnect_timer() override;

    /**
     * \brief   Called when event signals to start service and connection.
     **/
    void on_service_start() override;

    /**
     * \brief   Called when event signals to stop service and connection.
     **/
    void on_service_stop() override;

    /**
     * \brief   Called when event signals to restart service and connection.
     **/
    void on_service_restart() override;

    /**
     * \brief   Called when client connection is started.
     **/
    void on_connection_started() override;

    /**
     * \brief   Called when client connection is stopped.
     **/
    void on_connection_stopped() override;

    /**
     * \brief   Called when client connection is lost.
     **/
    void on_connection_lost() override;

    /**
     * \brief   Called when service needs to shut down.
     **/
    void on_service_exit() override;

    /**
     * \brief   Called to process and dispatch a received communication message.
     *
     * \param   msgReceived     The received communication message.
     **/
    void on_message_received(const RemoteMessage& msgReceived) override;

    /**
     * \brief   Called when communication message needs to be sent.
     *
     * \param   msgSend     The communication message to send.
     **/
    void on_message_send(const RemoteMessage& msgSend) override;

    /**
     * \brief   Called to notify that a channel connection is established.
     *
     * \param   cookie      The channel connection cookie.
     **/
    void on_channel_connected(const ITEM_ID & cookie) override;

//////////////////////////////////////////////////////////////////////////
// Protected operations and attributes
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Returns true if client socket connection is started and ready to operate.
     **/
    [[nodiscard]]
    inline bool is_connection_started() const noexcept;

    /**
     * \brief   Queues a service command event with optional priority.
     *
     * \param   cmd             The command to send and process.
     * \param   eventPrio       The priority of the event. By default, the priority is normal.
     **/
    inline void send_command(ServiceEventData::ServiceCommand cmd, areg::EventPriority eventPrio = areg::EventPriority::NormalPrio );

    /**
     * \brief   Queues a message for sending with optional priority.
     *
     * \param   data            The data of the message.
     * \param   eventPrio       The priority of the message to set.
     **/
    inline bool send_message(const RemoteMessage & data, areg::EventPriority eventPrio = areg::EventPriority::NormalPrio );

    /**
     * \brief   Starts client socket connection.
     *
     * \return  Returns true if connected.
     **/
    bool start_connection();

    /**
     * \brief   Cancels client connection immediately when connection is lost.
     **/
    void cancel_connection();

    /**
     * \brief   Sets the client socket connection state.
     *
     * \param   newState    The connection state to set.
     **/
    inline void set_connection_state( ServiceClientConnectionBase::ConnectionPhase newState );

    /**
     * \brief   Returns the current client socket connection state.
     **/
    [[nodiscard]]
    inline ServiceClientConnectionBase::ConnectionPhase connection_state() const noexcept;

    /**
     * \brief   Queues a disconnect event to close socket and exit thread.
     *
     * \param   eventPrio       The priority of the event.
     **/
    inline void disconnect_service( areg::EventPriority eventPrio );

//////////////////////////////////////////////////////////////////////////
// Hidden operations and attributes
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Returns reference to this client connection object.
     **/
    [[nodiscard]]
    inline ServiceClientConnectionBase & self() noexcept;

//////////////////////////////////////////////////////////////////////////
// Protected member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The ID of the target to send messages.
     **/
    const ITEM_ID                           mTarget;
    
    /**
     * \brief   The remote target service to communicate.
     **/
    const areg::RemoteServiceKind  mService;

    /**
     * \brief   The bitwise set of connection types supported by remote service.
     **/
    const uint32_t                      mConnectTypes;

    /**
     * \brief   The type of messaging source application.
     **/
    const areg::MessageSource         mMessageSource;

    /**
     * \brief   Client connection object
     **/
    ClientConnection                        mClientConnection;
    /**
     * \brief   Instance of remote servicing consumer to handle message.
     **/
    ConnectionConsumer &                    mConnectionConsumer;

    /**
     * \brief   The thread that makes message dispatching.
     **/
    DispatcherThread &                      mMessageDispatcher;

    /**
     * \brief   The connection channel.
     **/
    Channel                                 mChannel;

    /**
     * \brief   The current connection state.
     **/
    ConnectionPhase                        mConnectionState;

    /**
     * \brief   The Client Service event consumer
     **/
    ServiceClientConsumer                   mEventConsumer;

    /**
     * \brief   Data access synchronization object
     **/
    mutable ResourceLock                    mLock;

//////////////////////////////////////////////////////////////////////////
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   Connection retry timer object.
     **/
    Timer                                   mTimerConnect;
    /**
     * \brief   Message receiver thread
     **/
    ClientReceiveThread                     mThreadReceive;
    /**
     * \brief   Message sender thread
     **/
    ClientSendThread                        mThreadSend;
    /**
     * \brief   The Client Service event consumer
     **/
    ReconnectTimerConsumer                  mTimerConsumer;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceClientConnectionBase() = delete;
    AREG_NOCOPY_NOMOVE( ServiceClientConnectionBase );
};

//////////////////////////////////////////////////////////////////////////
// ServiceClientConnectionBase class inline methods implementation
//////////////////////////////////////////////////////////////////////////

inline ServiceClientConnectionBase & ServiceClientConnectionBase::self() noexcept
{
    return (*this);
}

inline const ITEM_ID & ServiceClientConnectionBase::connection_cookie() const noexcept
{
    return mClientConnection.cookie();
}

inline uint32_t ServiceClientConnectionBase::query_bytes_sent() noexcept
{
    return mThreadSend.extract_data_send();
}

inline uint32_t ServiceClientConnectionBase::query_bytes_received() noexcept
{
    return mThreadReceive.extract_data_receive();
}

inline void ServiceClientConnectionBase::enable_data_rate(bool enable)
{
    mThreadReceive.set_data_rate_enabled(enable);
    mThreadSend.set_data_rate_enabled(enable);
}

inline bool ServiceClientConnectionBase::is_data_rate_enabled() const noexcept
{
    return mThreadReceive.is_data_rate_enabled() && mThreadSend.is_data_rate_enabled();
}

inline bool ServiceClientConnectionBase::is_connect_state() const noexcept
{
    return has_phase_bit(mConnectionState, ConnectionPhase::ConnectState);
}

inline bool ServiceClientConnectionBase::is_connected_state() const noexcept
{
    return (mConnectionState == ConnectionPhase::ConnectionStarted);
}

inline bool ServiceClientConnectionBase::is_disconnect_state() const noexcept
{
    return has_phase_bit(mConnectionState, ConnectionPhase::DisconnectState);
}

inline void ServiceClientConnectionBase::register_client_commands()
{
    ServiceClientEvent::add_listener(static_cast<ServiceClientEventConsumer&>(mEventConsumer), mMessageDispatcher);
}

inline void ServiceClientConnectionBase::unregister_client_commands()
{
    ServiceClientEvent::remove_listener(static_cast<ServiceClientEventConsumer&>(mEventConsumer), mMessageDispatcher);
}

inline constexpr const char * ServiceClientConnectionBase::as_string(ServiceClientConnectionBase::ConnectionPhase val)
{
    switch (val)
    {
    case ServiceClientConnectionBase::ConnectionPhase::DisconnectState:
        return "ServiceClientConnectionBase::DisconnectState";
    case ServiceClientConnectionBase::ConnectionPhase::ConnectionStopped:
        return "ServiceClientConnectionBase::ConnectionStopped";
    case ServiceClientConnectionBase::ConnectionPhase::ConnectionStarting:
        return "ServiceClientConnectionBase::ConnectionStarting";
    case ServiceClientConnectionBase::ConnectionPhase::ConnectionStarted:
        return "ServiceClientConnectionBase::ConnectionStarted";
    case ServiceClientConnectionBase::ConnectionPhase::ConnectionStopping:
        return "ServiceClientConnectionBase::ConnectionStopping";
    case ServiceClientConnectionBase::ConnectionPhase::ConnectState:
        return "ServiceClientConnectionBase::ConnectState";
    default:
        return "ERR: Invalid value of ServiceClientConnectionBase::ConnectionPhase type";
    }
}

inline bool ServiceClientConnectionBase::is_connection_started() const noexcept
{
    const ITEM_ID cookie{ mClientConnection.cookie() };
    return (mClientConnection.is_valid() && (cookie != areg::COOKIE_LOCAL) && (cookie != areg::COOKIE_UNKNOWN));
}

inline void ServiceClientConnectionBase::set_connection_state(const ServiceClientConnectionBase::ConnectionPhase newState)
{
    mConnectionState = newState;
}

inline ServiceClientConnectionBase::ConnectionPhase ServiceClientConnectionBase::connection_state() const noexcept
{
    return mConnectionState;
}

inline void ServiceClientConnectionBase::send_command( ServiceEventData::ServiceCommand cmd
                                                    , areg::EventPriority eventPrio /*= areg::EventPriority::NormalPrio*/ )
{
    ServiceClientEvent::send_event( ServiceEventData( cmd )
                                 , static_cast<ServiceClientEventConsumer &>(mEventConsumer)
                                 , mMessageDispatcher
                                 , eventPrio );
}

inline bool ServiceClientConnectionBase::send_message(const RemoteMessage & data, areg::EventPriority eventPrio /*= areg::EventPriority::NormalPrio*/ )
{
    return SendMessageEvent::send_event( SendMessageEventData(data)
                                      , static_cast<SendMessageEventConsumer &>(mThreadSend)
                                      , static_cast<DispatcherThread &>(mThreadSend)
                                      , eventPrio);
}

inline void ServiceClientConnectionBase::disconnect_service( areg::EventPriority eventPrio )
{
    SendMessageEvent::send_event( SendMessageEventData()
                               , static_cast<SendMessageEventConsumer &>(mThreadSend)
                               , static_cast<DispatcherThread &>(mThreadSend)
                               , eventPrio );
}

} // namespace areg
#endif  // AREG_IPC_SERVICECLIENTCONNECTIONBASE_HPP
