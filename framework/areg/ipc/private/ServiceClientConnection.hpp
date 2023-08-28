#ifndef AREG_IPC_PRIVATE_SERVICECLIENTCONNECTION_HPP
#define AREG_IPC_PRIVATE_SERVICECLIENTCONNECTION_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ServiceClientConnection.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service client connection 
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/IERemoteEventConsumer.hpp"
#include "areg/ipc/IEServiceConnectionProvider.hpp"
#include "areg/ipc/IEServiceRegisterProvider.hpp"
#include "areg/ipc/IERemoteMessageHandler.hpp"
#include "areg/ipc/ServiceEventConsumerBase.hpp"

#include "areg/ipc/private/ClientReceiveThread.hpp"
#include "areg/ipc/private/ClientSendThread.hpp"
#include "areg/ipc/private/ClientConnection.hpp"
#include "areg/ipc/NERemoteService.hpp"
#include "areg/component/Channel.hpp"
#include "areg/component/Timer.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/base/String.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEServiceConnectionConsumer;
class IEServiceRegisterConsumer;

//////////////////////////////////////////////////////////////////////////
// ServiceClientConnection class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The connected client servicing object to handle connections,
 *          to read and send message, to dispatch messages and
 *          communicate with service manager.
 **/
class ServiceClientConnection   : public    IEServiceConnectionProvider
                                , public    IEServiceRegisterProvider
                                , private   DispatcherThread
                                , private   IERemoteMessageHandler
                                , private   IERemoteEventConsumer
                                , private   IEServiceEventConsumerBase
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Supported connection type. It is TCP/IP connection
     **/
    static constexpr    NERemoteService::eServiceConnection   CONNECT_TYPE  = NERemoteService::eServiceConnection::ConnectionTcpip;

    /**
     * \brief   ServiceClientConnection::eConnectionState
     *          Defines connection state values
     **/
    enum class eConnectionState : unsigned short
    {
          DisconnectState       = 1     //!< 0000 0001, The disconnect state
        , ConnectionStopped     = 3     //!< 0000 0011, The connection is stopped, i.e. not connected.
        , ConnectionStopping    = 5     //!< 0000 0101, The connection is stopping, because of manual request.
        , ConnectState          = 16    //!< 0001 0000, the connect state
        , ConnectionStarting    = 16    //!< 0011 0000, The connection is initiated, but the status is not known.
        , ConnectionStarted     = 80    //!< 0101 0000, The connection is established and accepted by server.
    };

    /**
     * \brief   Returns the string value of ServiceClientConnection::eConnectionState type
     **/
    static inline const char * getString(ServiceClientConnection::eConnectionState val);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes client servicing object, sets remote service consumer object.
     * \param   connectionConsumer  The instance of remote service connection consumer object to handle service connection notifications.
     * \param   registerConsumer    The instance of remote service registration consumer to handle service register notification.
     **/
    ServiceClientConnection(IEServiceConnectionConsumer& connectionConsumer, IEServiceRegisterConsumer & registerConsumer);
    /**
     * \brief   Destructor
     **/
    virtual ~ServiceClientConnection( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attribute
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the value of cookie set in connection.
     **/
    ITEM_ID getConnectionCookie( void ) const;

    /**
     * \brief   Each time querying the bytes sent via network connection returns
     *          the value after last query.
     **/
    inline uint32_t queryBytesSent( void );

    /**
     * \brief   Each time querying the bytes received via network connection returns
     *          the value after last query.
     **/
    inline uint32_t queryBytesReceived( void );

    /**
     * \brief   Returns true if the connection status is either connecting or connected.
     **/
    inline bool isConnectState( void ) const;

    /**
     * \brief   Returns true if the connection state is connected.
     **/
    inline bool isConnectedState( void ) const;

    /**
     * \brief   Returns true if the connection status is either disconnecting or disconnected.
     **/
    inline bool isDisconnectState( void ) const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

/************************************************************************/
// IEServiceConnectionProvider interface overrides
/************************************************************************/

    /**
     * \brief   Call to configure remote service. The passed file name
     *          can be either absolute or relative path.
     *          The function will read configuration file and initialize settings.
     *          If file path is nullptr or empty, Remote Service will have default
     *          configuration settings.
     * \param   configFile  Relative or absolute path of remote service configuration file.
     *                      If nullptr or empty, it will use default settings.
     * \return  Returns true if system could configure. Otherwise, it returns false.
     **/
    virtual bool setupServiceConnectionHost( const String & configFile ) override;

    /**
     * \brief   Call manually to set router service host name and port number.
     *          Note, if remote service is already started, this call will change
     *          data, but will not restart service.
     * \param   hostName    IP-address or host name of routing service to connect.
     * \param   portNr      Port number of routing service to connect.
     **/
    virtual void applyServiceConnectionData( const String & hostName, unsigned short portNr ) override;

    /**
     * \brief   Call to start remote service. The host name and port number should be already set.
     * \return  Returns true if start service is triggered.
     **/
    virtual bool connectServiceHost( void ) override;

    /**
     * \brief   Call to restart remove service. The host name and the port number should be already set.
     *          If the service had connection, it will be lost and re-connected again. If there was no
     *          connection, it starts new connection.
     * \return  Returns true if succeeded to restart service.
     **/
    virtual bool reconnectServiceHost( void ) override;

    /**
     * \brief   Call to stop service. No more remote communication should be possible.
     **/
    virtual void disconnectServiceHost( void ) override;

    /**
     * \brief   Returns true, if remote service is started and ready to operate.
     **/
    virtual bool isServiceHostConnected( void ) const override;

    /**
     * \brief   Returns true if service is configured and ready to start
     **/
    virtual bool isServiceHostSetup( void ) const override;

    /**
     * \brief   Returns true if remote service is enabled.
     **/
    virtual bool isRemoteServicingEnabled( void ) const override;

    /**
     * \brief   Enables or disables remote service.
     *          The method should be implemented to set business logic of enabling and disabling
     *          remote service in case if it is already started.
     * \param   enable  If true, the service is enabled. Otherwise, it is disabled.
     **/
    virtual void enableRemoteServicing( bool enable ) override;

/************************************************************************/
// IEServiceRegisterProvider interface overrides
/************************************************************************/

    /**
     * \brief   Call to register the remote service provider in the system and connect with service consumers.
     *          When service provider is registered, the service provider and all waiting service consumers
     *          receive appropriate connection notifications.
     * \param   stubService     The address of service provider to register in the system.
     * \return  Returns true if succeeded registration.
     **/
    virtual bool registerServiceProvider( const StubAddress & stubService ) override;

    /**
     * \brief   Call to unregister the service provider from the system and disconnect service consumers.
     *          All connected service consumers automatically receive disconnect notifications.
     * \param   stubService     The address of service provider to unregister in the system.
     * \param   reason          The reason to unregister and disconnect the service provider.
     **/
    virtual void unregisterServiceProvider( const StubAddress & stubService, const NEService::eDisconnectReason reason ) override;

    /**
     * \brief   Call to register the remote service consumer in the system and connect to service provider.
     *          If the service provider is already available, the service consumer and the service provider
     *          receive a connection notification.
     * \param   proxyService    The address of the service consumer to register in system.
     * \return  Returns true if registration process started with success. Otherwise, it returns false.
     **/
    virtual bool registerServiceConsumer( const ProxyAddress & proxyService ) override;

    /**
     * \brief   Call to unregister the service consumer from the system and disconnect service provider.
     *          Both, the service provider and the service consumer receive appropriate disconnect notification.
     * \param   proxyService    The address of the service consumer to unregister from the system.
     * \param   reason          The reason to unregister and disconnect the service consumer.
     **/
    virtual void unregisterServiceConsumer( const ProxyAddress & proxyService, const NEService::eDisconnectReason reason ) override;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// IEServiceEventConsumerBase interface overrides
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired.
     **/
    virtual void onServiceReconnectTimerExpired(void) override;

    /**
     * \brief   Called when receive event to start service and connection.
     **/
    virtual void onServiceStart(void) override;

    /**
     * \brief   Called when receive event to stop service and connection.
     **/
    virtual void onServiceStop(void) override;

    /**
     * \brief   Called when receive event to restart service and connection.
     **/
    virtual void onServiceRestart( void ) override;

    /**
     * \brief   Called when receive event the client connection is started.
     **/
    virtual void onServiceConnectionStarted(void) override;

    /**
     * \brief   Called when receive event the client connection is stopped.
     **/
    virtual void onServiceConnectionStopped(void) override;

    /**
     * \brief   Called when receive event the client connection is lost.
     **/
    virtual void onServiceConnectionLost(void) override;

    /**
     * \brief   Triggered when need to quit the service.
     **/
    virtual void onServiceExit(void) override;

    /**
     * \brief   Called when received a communication message to dispatch and process.
     * \param   msgReceived     The received the communication message.
     **/
    virtual void onServiceMessageReceived(const RemoteMessage& msgReceived) override;

    /**
     * \brief   Called when need to send a communication message.
     * \param   msgReceived     The received the communication message.
     **/
    virtual void onServiceMessageSend(const RemoteMessage& msgSend) override;

/************************************************************************/
// IERemoteMessageHandler interface overrides
/************************************************************************/

    /**
     * \brief   Triggered, when failed to send message.
     * \param   msgFailed   The message, which failed to send.
     * \param   whichTarget The target socket to send message.
     **/
    virtual void failedSendMessage( const RemoteMessage & msgFailed, Socket & whichTarget ) override;

    /**
     * \brief   Triggered, when failed to receive message.
     * \param   whichSource Indicates the failed source socket to receive message.
     **/
    virtual void failedReceiveMessage( Socket & whichSource ) override;

    /**
     * \brief   Triggered, when failed to process message, i.e. the target for message processing was not found.
     *          In case of request message processing, the source should receive error notification.
     * \param   msgUnprocessed  Unprocessed message data.
     **/
    virtual void failedProcessMessage( const RemoteMessage & msgUnprocessed ) override;

    /**
     * \brief   Triggered, when need to process received message.
     * \param   msgReceived Received message to process.
     * \param   whichSource The source socket, which received message.
     **/
    virtual void processReceivedMessage( const RemoteMessage & msgReceived, Socket & whichSource ) override;

/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Call to enable or disable event dispatching threads to receive events.
     *          Override if need to make event dispatching preparation job.
     * \param   isReady     The flag to indicate whether the dispatcher is ready for events.
     **/
    virtual void readyForEvents( bool isReady ) override;

/************************************************************************/
// IEEventRouter interface overrides
/************************************************************************/

    /**
     * \brief	Posts event and delivers to its target.
     *          Since the Dispatcher Thread is a Base object for
     *          Worker and Component threads, it does nothing
     *          and only destroys event object without processing.
     *          Override this method or use Worker / Component thread.
     * \param	eventElem	Event object to post
     * \return	In this class it always returns true.
     **/
    virtual bool postEvent( Event & eventElem ) override;

/************************************************************************/
// IERemoteEventConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Triggered when the Stub receives remote request event to process.
     * \param   requestEvent        The remote request event to be processed.
     **/
    virtual void processRemoteRequestEvent( RemoteRequestEvent & requestEvent ) override;

    /**
     * \brief   Triggered when the Stub receives remote notification request event to process.
     *          For example, send by Proxy and processed by Stub when need to start or stop
     *          sending attribute update notifications.
     * \param   requestNotifyEvent  The remote notification request event to be processed.
     **/
    virtual void processRemoteNotifyRequestEvent( RemoteNotifyRequestEvent & requestNotifyEvent ) override;

    /**
     * \brief   Triggered when the Stub receives remote response request event to process.
     *          For example, send by Proxy and processed by Stub when need to start or stop
     *          to subscribe on information or response sent by Stub.
     * \param   requestNotifyEvent  The remote response request event to be processed.
     **/
    virtual void processRemoteResponseEvent( RemoteResponseEvent & responseEvent ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden operations and attributes
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Called to start client socket connection. Returns true if connected.
     **/
    inline bool _startConnection( void );
    /**
     * \brief   Called when connection is lost and should be immediately canceled.
     **/
    inline void _cancelConnection( void );
    /**
     * \brief   Returns true if client socket connection is started.
     **/
    inline bool _isStarted( void ) const;
    /**
     * \brief   Sets client socket connection state.
     * \param   newState    The connection state to set.
     **/
    inline void _setConnectionState( ServiceClientConnection::eConnectionState newState );
    /**
     * \brief   Returns current client socket connection state.
     **/
    inline ServiceClientConnection::eConnectionState _getConnectionState( void ) const;
    /**
     * \brief   Call to send an event with the command to process.
     * \param   cmd         The command to send and process.
     * \param   eventPrio   The priority of the event. By default, the priority is normal.
     */
    inline void _sendCommand(ServiceEventData::eServiceEventCommands cmd, Event::eEventPriority eventPrio = Event::eEventPriority::EventPriorityNormal );
    /**
     * \brief   Queues the message for sending
     * \param   data        The data of the message.
     * \param   eventPrio   The priority of the message to set.
     **/
    inline bool _sendMessage(const RemoteMessage & data, Event::eEventPriority eventPrio = Event::eEventPriority::EventPriorityNormal );
    /**
     * \brief   Call to send the disconnect event. It disconnects the socket  and exits the thread.
     * \param   eventPrio   The priority of set to the event.
     **/
    inline void _disconnectService( Event::eEventPriority eventPrio );

    /**
     * \brief   Returns instance of client servicing object.
     **/
    inline ServiceClientConnection & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Client connection object
     **/
    ClientConnection                mClientConnection;
    /**
     * \brief   Instance of remote servicing consumer to handle message.
     **/
    IEServiceConnectionConsumer &   mConnectionConsumer;
    /**
     * \brief   The instance of service register consumer.
     **/
    IEServiceRegisterConsumer &     mRegisterConsumer;
    /**
     * \brief   Connection retry timer object.
     **/
    Timer                           mTimerConnect;
    /**
     * \brief   Message receiver thread
     **/
    ClientReceiveThread             mThreadReceive;
    /**
     * \brief   Message sender thread
     **/
    ClientSendThread                mThreadSend;
    /**
     * \brief   Flag, indicates whether the remote servicing is enabled or not.
     **/
    bool                            mIsServiceEnabled;
    /**
     * \brief   The path of configuration file name.
     **/
    String                          mConfigFile;
    /**
     * \brief   The connection channel.
     **/
    Channel                         mChannel;
    /**
     * \brief   The sate of connection
     **/
    eConnectionState                mConnectionState;
    /**
     * \brief   The Client Service event consumer
     **/
    ServiceClientEventConsumer      mEventConsumer;
    /**
     * \brief   The Client Service event consumer
     **/
    ReconnectTimerConsumer          mTimerConsumer;
    /**
     * \brief   Data access synchronization object
     **/
    mutable ResourceLock            mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceClientConnection( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServiceClientConnection );
};

//////////////////////////////////////////////////////////////////////////
// ServiceClientConnection class inline methods implementation
//////////////////////////////////////////////////////////////////////////

inline ServiceClientConnection & ServiceClientConnection::self( void )
{
    return (*this);
}

inline ITEM_ID ServiceClientConnection::getConnectionCookie( void ) const
{
    return mClientConnection.getCookie();
}

inline uint32_t ServiceClientConnection::queryBytesSent( void )
{
    return mThreadSend.extractDataSend();
}

inline uint32_t ServiceClientConnection::queryBytesReceived( void )
{
    return mThreadReceive.extractDataReceive();
}

inline bool ServiceClientConnection::isConnectState( void ) const
{
    return (static_cast<uint16_t>(mConnectionState) & static_cast<uint16_t>(ServiceClientConnection::eConnectionState::ConnectState));
}

inline bool ServiceClientConnection::isConnectedState( void ) const
{
    return (mConnectionState == ServiceClientConnection::eConnectionState::ConnectionStarted);
}

inline bool ServiceClientConnection::isDisconnectState( void ) const
{
    return (static_cast<uint16_t>(mConnectionState) & static_cast<uint16_t>(ServiceClientConnection::eConnectionState::DisconnectState));
}

inline const char * ServiceClientConnection::getString(ServiceClientConnection::eConnectionState val)
{
    switch (val)
    {
    case ServiceClientConnection::eConnectionState::ConnectionStopped:
        return "ServiceClientConnection::ConnectionStopped";
    case ServiceClientConnection::eConnectionState::ConnectionStarting:
        return "ServiceClientConnection::ConnectionStarting";
    case ServiceClientConnection::eConnectionState::ConnectionStarted:
        return "ServiceClientConnection::ConnectionStarted";
    case ServiceClientConnection::eConnectionState::ConnectionStopping:
        return "ServiceClientConnection::ConnectionStopping";
    default:
        return "ERR: Invalid value of ServiceClientConnection::eConnectionState type";
    }
}

#endif  // AREG_IPC_PRIVATE_SERVICECLIENTCONNECTION_HPP
