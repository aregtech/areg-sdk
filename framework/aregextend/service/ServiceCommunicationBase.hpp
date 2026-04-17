#ifndef AREG_AREGEXTEND_SERVICE_SERVICECOMMUNICATIONBASE_HPP
#define AREG_AREGEXTEND_SERVICE_SERVICECOMMUNICATIONBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/ServiceCommunicationBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service communication base class
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/DispatcherThread.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/ipc/ConnectionConsumer.hpp"
#include "areg/ipc/ConnectionProvider.hpp"
#include "areg/ipc/ServiceEventConsumer.hpp"
#include "aregextend/service/DataRateHelper.hpp"
#include "aregextend/service/ConnectionHandler.hpp"

#include "areg/base/HashMap.hpp"
#include "areg/base/OrderedMap.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/component/Timer.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "aregextend/service/ServerConnection.hpp"
#include "aregextend/service/private/ClientConnectionPair.hpp"
#include "aregextend/service/private/ServerReceiveThread.hpp"
#include "aregextend/service/private/ServerSendThread.hpp"

#include <atomic>
#include <memory>
#include <vector>

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// ServiceCommunicationBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The server side base connection service. Used by message router to
 *          accept service connections.
 *
 *          When numPairs > 0 (pool mode), accepted client sockets are routed to
 *          one of numPairs pre-started ClientConnectionPair instances via
 *          cookie % numPairs.  Each pair owns one send thread and one receive
 *          thread that serve multiple clients simultaneously.  This eliminates
 *          contention on the shared mThreadSend / mThreadReceive for high-throughput
 *          scenarios (mtrouter).
 *
 *          When numPairs == 0 (legacy mode), all I/O goes through the single shared
 *          mThreadSend / mThreadReceive path.  Used by logcollector which handles
 *          low data volumes and gains nothing from the pool overhead.
 **/
class ServiceCommunicationBase  : public    RemoteMessageHandler
                                , public    ConnectionConsumer
                                , public    ConnectionProvider
                                , protected DispatcherThread
                                , protected ServiceEventConsumer
                                , protected ConnectionHandler
{
//////////////////////////////////////////////////////////////////////////
// The internal types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The default connection behavior.
     *          The default behavior is either accept or reject the connection.
     *              -   If the default behavior is to accept the connection,
     *                  the connection is rejected if IP-address is black-listed.
     *              -   If the default behavior is to rejected the connection,
     *                  the connection is accepted if it IP-address white-listed.
     **/
    enum class ConnectionPolicy : uint8_t
    {
          Accept    //!< The default behavior is to accept the connection.
        , Reject    //!< The default behavior is to reject the connection.
    };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes service communication with the specified service ID, connection types,
     *          dispatcher thread, and connection policy.
     *
     * \param   serviceId       The unique identifier of the service in the system.
     * \param   service         The remote service kind.
     * \param   connectTypes    The bitwise set of connection types.
     * \param   stackSizeKb     Stack size of the dispatcher thread in kilobytes.
     * \param   dispatcher      The name of the message dispatcher thread.
     * \param   behavior        Connection default behavior.  By default all connections are accepted.
     * \param   numPairs        Number of dedicated pool thread pairs to create.  Pass 0 to keep
     *                          the legacy shared send/receive path (default).
     **/
    ServiceCommunicationBase( const ITEM_ID & serviceId
                            , areg::RemoteServiceKind service
                            , uint32_t connectTypes
                            , uint32_t stackSizeKb
                            , const String & dispatcher
                            , ServiceCommunicationBase::ConnectionPolicy behavior = ServiceCommunicationBase::ConnectionPolicy::Accept
                            , uint32_t numPairs = 0u );
    virtual ~ServiceCommunicationBase() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Checks whether the specified host address is in the white-list.
     *
     * \param   addrClient      The object, which contains client host address to check.
     * \return  Returns true if specified host address is in the white-list.
     **/
    [[nodiscard]]
    inline bool is_address_in_white_list( const areg::SocketAddress & addrClient ) const;

    /**
     * \brief   Checks whether the specified host address is in the black-list.
     *
     * \param   addrClient      The object, which contains client host address to check.
     * \return  Returns true if specified host address is in the black-list.
     **/
    [[nodiscard]]
    inline bool is_address_in_black_list( const areg::SocketAddress & addrClient ) const;

    /**
     * \brief   Adds specified host address to the white-list.
     *
     * \param   addrClient      The host address of client to add to list.
     **/
    inline void add_white_list( const areg::SocketAddress & addrClient );

    /**
     * \brief   Adds specified host address to black-list.
     *
     * \param   addrClient      The host address of client to add to list.
     **/
    inline void add_black_list( const areg::SocketAddress & addrClient );

    /**
     * \brief   Removes specified host address from white-list.
     *
     * \param   addrClient      The host address of client to remove from list.
     **/
    inline void remove_white_list( const areg::SocketAddress & addrClient );

    /**
     * \brief   Removes specified host address from black-list.
     *
     * \param   addrClient      The host address of client to remove from list.
     **/
    inline void remove_black_list( const areg::SocketAddress & addrClient );

    /**
     * \brief   Returns the list of connected instances.
     **/
    inline const areg::MapInstances & instances() const;

    /**
     * \brief   Returns the number of pool thread pairs that were created.
     *          Returns 0 when running in legacy shared-thread mode.
     **/
    [[nodiscard]]
    inline uint32_t active_client_pair_count() const noexcept;

    /**
     * \brief   Call to wait the service communication thread to complete the job. The method should
     *          be called when exit the process.
     **/
    inline void wait_to_complete( );

    /**
     * \brief   Queues the message for sending.
     *          In pool mode, routes the message to the pool pair that owns
     *          the target client via target % numPairs.
     *
     * \param   data            The data of the message.
     * \param   eventPrio       The priority of the message to set.
     **/
    bool send_message(const RemoteMessage & data, areg::EventPriority eventPrio = areg::EventPriority::NormalPrio );

    /**
     * \brief   Queues the message for sending (move variant).
     *          Transfers ownership of the message payload to the send thread
     *          without incrementing the shared buffer reference count.
     *          In pool mode, routes the message to the pool pair that owns
     *          the target client via target % numPairs.
     *
     * \param   data            The data of the message (moved from).
     * \param   eventPrio       The priority of the message to set.
     **/
    bool send_message(RemoteMessage && data, areg::EventPriority eventPrio = areg::EventPriority::NormalPrio );

    /**
     * \brief   Returns the instance of data rate helper object to use when computing data rate.
     **/
    inline DataRateHelper& data_rate_helper() const noexcept;

    /**
     * \brief   Each time querying the bytes sent via network connection returns the value after
     *          last query.
     **/
    [[nodiscard]]
    inline uint64_t query_bytes_sent() const noexcept;

    /**
     * \brief   Each time querying the bytes received via network connection returns the value after
     *          last query.
     **/
    [[nodiscard]]
    inline uint64_t query_bytes_received() const noexcept;

    inline uint32_t query_msg_sent() const noexcept;

    inline uint32_t query_msg_received() const noexcept;

    /**
     * \brief   Enable or disable the data rate calculation. Also propagates the flag to
     *          all currently active pool thread pairs.
     *
     * \param   enable      If true, the data rate calculation is enabled. Otherwise, it is
     *                      disabled.
     **/
    void enable_data_rate(bool enable) noexcept;

    /**
     * \brief   Returns enable or disable the data rate calculation flag.
     **/
    [[nodiscard]]
    inline bool is_data_rate_enabled() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// ServiceCommunicationBase overrides
/************************************************************************/
    /**
     * \brief   Adds an entry into the list of connected instances.
     *
     * \param   cookie      The cookie of connected instance.
     * \param   instance    The name of the connected instance.
     **/
    virtual void add_instance(const ITEM_ID & cookie, const areg::ConnectedInstance & instance );

    /**
     * \brief   Removes the connected instance identified by the specified cookie.
     *
     * \param   cookie      The cookie of connected instance.
     **/
    virtual void remove_instance(const ITEM_ID & cookie );

    /**
     * \brief   Removes all connected instances from the map.
     **/
    virtual void remove_all_instances();

/************************************************************************/
// RemoteMessageHandler interface overrides
/************************************************************************/

    /**
     * \brief   Triggered, when failed to send message.
     *
     * \param   msgFailed       The message, which failed to send.
     * \param   whichTarget     The target socket to send message.
     **/
    void failed_send_message( const RemoteMessage & msgFailed, Socket & whichTarget ) override;

    /**
     * \brief   Triggered, when failed to receive message.
     *
     * \param   whichSource     Indicates the failed source socket to receive message.
     **/
    void failed_receive_message( Socket & whichSource ) override;

    /**
     * \brief   Triggered, when need to process received message.
     *
     * \param   msgReceived     Received message to process.
     * \param   whichSource     The source socket, which received message.
     **/
    void process_received_message( RemoteMessage & msgReceived, Socket & whichSource ) override;

/************************************************************************/
// ConnectionConsumer
/************************************************************************/

    /**
     * \brief   Triggered when remote service connection and communication channel is established.
     *
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void on_service_channel_connected(const Channel& channel) override = 0;

    /**
     * \brief   Triggered when disconnected remote service connection and communication channel.
     *
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void on_service_channel_disconnected(const Channel& channel) override = 0;

    /**
     * \brief   Triggered when remote service connection and communication channel is lost. The
     *          connection is considered lost if it not possible to read or receive data, and it was
     *          not stopped by API call.
     *
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void on_service_channel_lost(const Channel& channel) override = 0;

/************************************************************************/
// ConnectionProvider interface overrides
/************************************************************************/
    /**
     * \brief   Call to configure remote service. The passed file name can be either absolute or
     *          relative path. The function will read configuration file and initialize settings. If
     *          file path is nullptr or empty, Remote Service will have default configuration
     *          settings.
     *
     * \param   service         The type of service connection to setup.
     * \param   connectTypes    The bitwise set of connections.
     * \return  Returns true if system could configure. Otherwise, it returns false.
     **/
    bool setup_connection_data(areg::RemoteServiceKind service, uint32_t connectTypes) override;

    /**
     * \brief   Call manually to set router service host name and port number. Note, if remote
     *          service is already started, this call will change data, but will not restart
     *          service.
     *
     * \param   hostName    IP-address or host name of routing service to connect.
     * \param   portNr      Port number of routing service to connect.
     **/
    void apply_connection_data( const String & hostName, uint16_t portNr ) override;

    /**
     * \brief   Call to start remote service. The host name and port number should be already set.
     *
     * \return  Returns true if start service is triggered.
     **/
    bool connect_service_host() override;

    /**
     * \brief   Call to restart remove service. The host name and the port number should be already
     *          set. If the service had connection, it will be lost and re-connected again. If there
     *          was no connection, it starts new connection.
     *
     * \return  Returns true if succeeded to restart service.
     **/
    bool reconnect_service_host() override;

    /**
     * \brief   Call to stop service. No more remote communication should be possible.
     **/
    void disconnect_service_host() override;

    /**
     * \brief   Returns true, if remote service is started and ready to operate.
     **/
    [[nodiscard]]
    bool is_host_connected() const override;

    /**
     * \brief   Returns true, if remote service connection is triggered, not connected yet and in
     *          pending state.
     **/
    [[nodiscard]]
    bool is_host_pending() const override;

    /**
     * \brief   Returns true if service is configured and ready to start.
     **/
    [[nodiscard]]
    bool is_host_setup() const override;

    /**
     * \brief   Creates the service connect request message, sets the message target and the source.
     *
     * \param   source          The ID of the source that sends connection message request.
     * \param   target          The ID of the target to send the connection message request.
     * \param   msgSource       The message source type of the connected client.
     * \return  Returns the created message for remote communication.
     **/
    [[nodiscard]]
    RemoteMessage connect_message( const ITEM_ID & source, const ITEM_ID & target, areg::MessageSource msgSource) const override;

    /**
     * \brief   Creates the service disconnect request message, sets the message target and the
     *          source.
     *
     * \param   source      The ID of the source that sends the disconnect message request.
     * \param   target      The ID of the target to send the disconnection message request.
     * \return  Returns the created message for remote communication.
     **/
    [[nodiscard]]
    RemoteMessage disconnect_message( const ITEM_ID & source, const ITEM_ID & target ) const override;

/************************************************************************/
// ServiceEventConsumer overrides
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired.
     **/
    void on_reconnect_timer() override;

    /**
     * \brief   Called when need to start the network server connection service.
     **/
    void on_service_start() override;

    /**
     * \brief   Called when need to stop the network server connection service.
     **/
    void on_service_stop() override;

    /**
     * \brief   Called when need to restart the network server connection service.
     **/
    void on_service_restart() override;

    /**
     * \brief   Triggered when need to quit the service.
     **/
    void on_service_exit() override;

    /**
     * \brief   Called when need to inform the channel connection.
     *
     * \param   cookie      The channel connection cookie.
     **/
    void on_channel_connected(const ITEM_ID & cookie) override;

/************************************************************************/
// ConnectionHandler interface overrides
/************************************************************************/

    /**
     * \brief   Call to check whether new client connection should be accepted or rejected. Once
     *          client is accepted, server will start to receive messages from client. Otherwise,
     *          connection with client is immediately closed and communication is stopped.
     *
     * \param   clientSocket    Accepted client socket object to check.
     * \return  Returns true if client connection can be accepted. To reject and close connection
     *          with client, the method should return false.
     **/
    [[nodiscard]]
    bool can_accept_connection( const SocketAccepted & clientSocket ) override;

    /**
     * \brief   Triggered, when lost connection with client. Passed clientSocket parameter specifies
     *          client socket, which lost connection.
     *
     * \param   clientSocket    Client socket object, which lost connection.
     **/
    void connection_lost( SocketAccepted & clientSocket ) override;

    /**
     * \brief   Triggered, when there is a connection failure. Normally, this should restart the
     *          connection.
     **/
    void connection_failure() override;

    /**
     * \brief   Called when need to disconnect and unregister all service providers and service
     *          consumers.
     **/
    void disconnect_services() override;

    /**
     * \brief   Called after a new client socket has been physically accepted.
     *          In pool mode (numPairs > 0): routes the socket to the appropriate pool pair
     *          via cookie % numPairs, unregisters from the global multiplexer, and returns true.
     *          In legacy mode (numPairs == 0): returns false — shared send/receive path active.
     *
     * \param   clientSocket    The newly accepted client socket (cookie already assigned).
     * \return  Returns true if the socket was handed off to a pool pair (pool mode).
     *          Returns false for the legacy shared-thread path.
     **/
    bool on_client_accepted( areg::SocketAccepted & clientSocket ) override;

/************************************************************************/
// ServiceCommunicationBase
/************************************************************************/

    /**
     * \brief   Called to start connection procedure to accept client connections.
     *
     * \return  Returns true if could start connection.
     **/
    bool start_connection();

    /**
     * \brief   Call to restart the connection. Returns true if succeeded to reconnect.
     **/
    bool restart_connection();

    /**
     * \brief   Called to stop connection. All clients are automatically disconnected.
     **/
    void stop_connection();

    /**
     * \brief   Starts the message sending thread and returns true if succeeded.
     **/
    bool start_send_thread();

    /**
     * \brief   Starts the message receiving thread and returns true if succeeded.
     **/
    bool start_receive_thread();

    /**
     * \brief   Call to send the event to process.
     *
     * \param   cmd             The command to send and process.
     * \param   eventPrio       The priority of the event. By default, it is normal.
     * \return  Returns true if succeeded to send the command.
     **/
    inline bool send_command(ServiceEventData::ServiceCommand cmd, areg::EventPriority eventPrio = areg::EventPriority::NormalPrio);

    /**
     * \brief   Call to send the event to process.
     *
     * \param   cmd     The command to send and process.
     * \param   msg     The message to forward.
     * \return  Returns true if succeeded to send the command.
     **/
    inline bool send_communication_message(ServiceEventData::ServiceCommand cmd, const RemoteMessage & msg, areg::EventPriority eventPrio = areg::EventPriority::NormalPrio );

    /**
     * \brief   Call to send the disconnect event to the global send thread.
     *          Pool send threads are stopped explicitly by stop_connection().
     *
     * \param   eventPrio       The priority of set to the event.
     **/
    inline void disconnect_service( areg::EventPriority eventPrio );

/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Call to enable or disable event dispatching threads to receive events. Override if
     *          need to make event dispatching preparation job.
     *
     * \param   is_ready    The flag to indicate whether the dispatcher is ready for events.
     **/
    void ready_for_events( bool is_ready ) override;

/************************************************************************/
// EventRouter interface overrides
/************************************************************************/

    /**
     * \brief   Posts event and delivers to its target. Since the Dispatcher Thread is a Base object
     *          for Worker and Component threads, it does nothing and only destroys event object
     *          without processing. Override this method or use Worker / Component thread.
     *
     * \param   eventElem       Event object to post
     * \return  In this class it always returns true.
     **/
    [[nodiscard]]
    bool post_event( Event & eventElem ) override;

//////////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Returns instance of object. For internal use only.
     **/
    inline ServiceCommunicationBase & self();

//////////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////////
protected:
    const ConnectionPolicy          mConnectBehavior;   //!< The default connection behavior.
    const areg::RemoteServiceKind   mService;           //!< The remote service type.
    const uint32_t                  mConnectTypes;      //!< The bitwise flags of remote service connections.
    const uint32_t                  mNumPairs;          //!< Pool size: 0 = legacy shared-thread mode; N > 0 = pool mode.
    ServerConnection                mServerConnection;  //!< The instance of server connection object.
    Timer                           mTimerConnect;      //!< The timer object to trigger in case if failed to create server socket.
    ServerSendThread                mThreadSend;        //!< The thread to send messages to clients
    ServerReceiveThread             mThreadReceive;     //!< The thread to receive messages from clients

    // JUSTIFICATION: unique_ptr is used because ServiceCommunicationBase has exclusive ownership
    //                of each pool pair.  The pair is created once at server start and destroyed
    //                at server stop.  Async paths (connection_lost, failed_receive_message) only
    //                remove a single socket from the pair's multiplexer — they never stop or
    //                destroy the pair itself.
    // NOTE: mClientPairs MUST be declared before mDataRateHelper — the DataRateHelper constructor
    //       calls enable_data_rate() which iterates mClientPairs.  C++ initializes members in
    //       declaration order; declaring mClientPairs first guarantees it is a valid empty vector
    //       before DataRateHelper's constructor body runs.
    using ClientPairList = std::vector<std::unique_ptr<ClientConnectionPair>>;
    ClientPairList                  mClientPairs;       //!< Pool thread pairs; size == mNumPairs when running.
    std::atomic_bool                mShuttingDown;      //!< True during stop_connection() — suppresses spurious disconnect callbacks.
    DataRateHelper                  mDataRateHelper;    //!< The helper object to query information of sent and receive bytes.
    StringArray                     mWhiteList;         //!< The list of enabled fixed client hosts.
    StringArray                     mBlackList;         //!< The list of disabled fixes client hosts.
    ServiceServerConsumer           mEventConsumer;     //!< The custom event consumer object
    ReconnectTimerConsumer          mTimerConsumer;     //!< The timer consumer object.
    areg::MapInstances              mInstanceMap;       //!< The map of connected instance.
    SyncEvent                       mEventSendStop;     //!< The event set when cannot send and receive data anymore.
//    mutable NolockSyncObject        mLock;              //!< The synchronization object to be accessed from different threads.
    mutable ResourceLock            mLock;              //!< The synchronization object to be accessed from different threads.

//////////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////////
private:
    ServiceCommunicationBase() = delete;
    AREG_NOCOPY_NOMOVE( ServiceCommunicationBase );
};

//////////////////////////////////////////////////////////////////////////////
// ServiceCommunicationBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline ServiceCommunicationBase & ServiceCommunicationBase::self()
{
    return (*this);
}

inline bool ServiceCommunicationBase::is_address_in_white_list(const areg::SocketAddress & addrClient) const
{
    return mWhiteList.contains(addrClient.host_address());
}

inline bool ServiceCommunicationBase::is_address_in_black_list(const areg::SocketAddress & addrClient) const
{
    return mBlackList.contains(addrClient.host_address());
}

inline void ServiceCommunicationBase::add_white_list(const areg::SocketAddress & addrClient)
{
    mBlackList.remove_elem( addrClient.host_address( ) );
    mWhiteList.add_if_unique( addrClient.host_address( ) );
}

inline void ServiceCommunicationBase::add_black_list(const areg::SocketAddress & addrClient)
{
    mWhiteList.remove_elem( addrClient.host_address( ) );
    mBlackList.add_if_unique( addrClient.host_address( ) );
}

inline void ServiceCommunicationBase::remove_white_list(const areg::SocketAddress & addrClient)
{
    mWhiteList.remove_elem( addrClient.host_address(), 0);
}

inline void ServiceCommunicationBase::remove_black_list(const areg::SocketAddress & addrClient)
{
    mBlackList.remove_elem( addrClient.host_address(), 0);
}

inline const areg::MapInstances & ServiceCommunicationBase::instances() const
{
    return mInstanceMap;
}

inline uint32_t ServiceCommunicationBase::active_client_pair_count() const noexcept
{
    return static_cast<uint32_t>(mClientPairs.size());
}

inline void ServiceCommunicationBase::wait_to_complete( )
{
    wait_completion( areg::WAIT_INFINITE );
    shutdown( areg::DO_NOT_WAIT );
}

inline bool ServiceCommunicationBase::send_command( ServiceEventData::ServiceCommand cmd, areg::EventPriority eventPrio /*= areg::EventPriority::NormalPrio*/ )
{
    return ServiceServerEvent::send_event( ServiceEventData( cmd )
                                          , static_cast<ServiceServerEventConsumer &>(mEventConsumer)
                                          , static_cast<DispatcherThread &>(self( ))
                                          , eventPrio );
}

inline bool ServiceCommunicationBase::send_communication_message( ServiceEventData::ServiceCommand cmd
                                                                , const RemoteMessage & msg
                                                                , areg::EventPriority eventPrio /*= areg::EventPriority::NormalPrio*/ )
{
    return ServiceServerEvent::send_event( ServiceEventData( cmd, msg )
                                          , static_cast<ServiceServerEventConsumer &>(mEventConsumer)
                                          , static_cast<DispatcherThread &>(self( ))
                                          , eventPrio );
}

inline DataRateHelper& ServiceCommunicationBase::data_rate_helper() const noexcept
{
    return const_cast<DataRateHelper &>(mDataRateHelper);
}

inline uint64_t ServiceCommunicationBase::query_bytes_sent() const noexcept
{
    return mThreadSend.extract_data_send();
}

inline uint64_t ServiceCommunicationBase::query_bytes_received() const noexcept
{
    return mThreadReceive.extract_data_receive();
}

inline uint32_t ServiceCommunicationBase::query_msg_sent() const noexcept
{
#if 0
    uint32_t result{ mThreadSend.extract_msgs_sent()};
    for (const auto& elem : mClientPairs)
    {
        if (!elem)
            continue;
    }

    return result;
#else
    return mThreadSend.extract_msgs_sent();
#endif
}

inline uint32_t ServiceCommunicationBase::query_msg_received() const noexcept
{
#if 0
    uint32_t result{ mThreadReceive.extract_msgs_received() };
    for (const auto& elem : mClientPairs)
    {
        if (!elem)
            continue;
    }

    return result;
#else
    return mThreadReceive.extract_msgs_received();
#endif
}

inline bool ServiceCommunicationBase::is_data_rate_enabled() const noexcept
{
    return mThreadSend.is_data_rate_enabled() && mThreadReceive.is_data_rate_enabled();
}

inline void ServiceCommunicationBase::disconnect_service( areg::EventPriority eventPrio )
{
    SendMessageEvent::send_event( SendMessageEventData( )
                                 , static_cast<SendMessageEventConsumer &>(mThreadSend)
                                 , static_cast<DispatcherThread &>(mThreadSend)
                                 , eventPrio );
}

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_SERVICECOMMUNICATIONBASE_HPP
