#ifndef AREG_IPC_SERVICECLIENTCONNECTIONBASE_HPP
#define AREG_IPC_SERVICECLIENTCONNECTIONBASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/ServiceClientConnectionBase.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service client connection 
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/ipc/IEServiceConnectionProvider.hpp"
#include "areg/ipc/ServiceEventConsumerBase.hpp"

#include "areg/ipc/ClientConnection.hpp"
#include "areg/ipc/private/ClientReceiveThread.hpp"
#include "areg/ipc/private/ClientSendThread.hpp"
#include "areg/component/Channel.hpp"
#include "areg/component/Timer.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/base/String.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEServiceConnectionConsumer;
class IERemoteMessageHandler;
//////////////////////////////////////////////////////////////////////////
// ServiceClientConnectionBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The connected client servicing object to handle connections,
 *          to read and send message, to dispatch messages and
 *          communicate with service manager.
 **/
class AREG_API ServiceClientConnectionBase  : public    IEServiceConnectionProvider
                                            , public    IEServiceEventConsumerBase
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   ServiceClientConnectionBase::eConnectionState
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
     * \brief   Returns the string value of ServiceClientConnectionBase::eConnectionState type
     **/
    static inline const char * getString(ServiceClientConnectionBase::eConnectionState val);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes client servicing object, sets remote service consumer object.
     * \param   target              The ID of remote target service to send messages.
     * \param   service             The type of remote service.
     * \param   connectTypes        The bitwise set of connection types.
     * \param   msgSource           The type of message source application that sends messages.
     * \param   connectionConsumer  The instance of remote service connection consumer object to handle service connection notifications.
     * \param   messageHandler      The instance of message handler.
     * \param   messageDispatcher   The instance of message dispatcher.
     * \param   prefixName          The prefix to add to the names of message receive and send threads.
     **/
    ServiceClientConnectionBase(  const ITEM_ID & target
                                , NERemoteService::eRemoteServices service
                                , unsigned int connectTypes
                                , NEService::eMessageSource msgSource
                                , IEServiceConnectionConsumer& connectionConsumer
                                , IERemoteMessageHandler & messageHandler
                                , DispatcherThread & messageDispatcher
                                , const String & prefixName);
    /**
     * \brief   Destructor
     **/
    virtual ~ServiceClientConnectionBase( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attribute
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the value of cookie set in connection.
     **/
    inline const ITEM_ID & getConnectionCookie( void ) const;

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
     * \brief   Enable or disable the data rate calculation.
     * \param   enable  If true, the data rate calculation is enabled.
     *                  Otherwise, it is disabled.
     **/
    inline void enableCalculateDataRate(bool enable);

    /**
     * \brief   Returns enable or disable the data rate calculation flag.
     **/
    inline bool isCalculateDataRateEnabled(void) const;

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

    /**
     * \brief   Registers the client socket connection thread to receive service commands.
     **/
    inline void registerForServiceClientCommands(void);

    /**
     * \brief   Unregisters the client socket connection thread to receive service commands.
     **/
    inline void unregisterForServiceClientCommands(void);

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

/************************************************************************/
// ServiceClientConnectionBase interface overrides
/************************************************************************/

    /**
     * \brief   Triggered when get service notifies the client the connection
     *          status like connection accepted, disconnected or rejected.
     *          Override the method if need custom connection reaction.
     * \param   msgReceived     The message sent by service to the client.
     **/
    virtual void serviceConnectionEvent(const RemoteMessage& msgReceived);

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
    virtual bool setupServiceConnectionData(NERemoteService::eRemoteServices service, uint32_t connectTypes) override;

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
     * \brief   Creates the service connect request message, sets the message target and the source.
     * \param   source      The ID of the source that sends connection message request.
     * \param   target      The ID of the target to send the connection message request.
     * \param   msgSource   The message source type of the connected client.
     * \return  Returns the created message for remote communication.
     **/
    virtual RemoteMessage createServiceConnectMessage( const ITEM_ID & source, const ITEM_ID & target, NEService::eMessageSource msgSource) const override;

    /**
     * \brief   Creates the service disconnect request message, sets the message target and the source.
     * \param   source  The ID of the source that sends the disconnect message request.
     * \param   target  The ID of the target to send the disconnection message request.
     * \return  Returns the created message for remote communication.
     **/
    virtual RemoteMessage createServiceDisconnectMessage( const ITEM_ID & source, const ITEM_ID & target ) const override;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
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

    /**
     * \brief   Called when need to inform the channel connection.
     * \param   cookie  The channel connection cookie.
     **/
    virtual void onChannelConnected(const ITEM_ID & cookie) override;

//////////////////////////////////////////////////////////////////////////
// Protected operations and attributes
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Returns true if client socket connection is started.
     **/
    inline bool isConnectionStarted(void) const;

    /**
     * \brief   Call to send an event with the command to process.
     * \param   cmd         The command to send and process.
     * \param   eventPrio   The priority of the event. By default, the priority is normal.
     */
    inline void sendCommand(ServiceEventData::eServiceEventCommands cmd, Event::eEventPriority eventPrio = Event::eEventPriority::EventPriorityNormal );

    /**
     * \brief   Queues the message for sending
     * \param   data        The data of the message.
     * \param   eventPrio   The priority of the message to set.
     **/
    inline bool sendMessage(const RemoteMessage & data, Event::eEventPriority eventPrio = Event::eEventPriority::EventPriorityNormal );

    /**
     * \brief   Called to start client socket connection. Returns true if connected.
     **/
    bool startConnection( void );

    /**
     * \brief   Called when connection is lost and should be immediately canceled.
     **/
    void cancelConnection( void );

    /**
     * \brief   Sets client socket connection state.
     * \param   newState    The connection state to set.
     **/
    inline void setConnectionState( ServiceClientConnectionBase::eConnectionState newState );

    /**
     * \brief   Returns current client socket connection state.
     **/
    inline ServiceClientConnectionBase::eConnectionState getConnectionState( void ) const;

    /**
     * \brief   Call to send the disconnect event. It disconnects the socket  and exits the thread.
     * \param   eventPrio   The priority of set to the event.
     **/
    inline void disconnectService( Event::eEventPriority eventPrio );

//////////////////////////////////////////////////////////////////////////
// Hidden operations and attributes
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Returns instance of client servicing object.
     **/
    inline ServiceClientConnectionBase & self( void );

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
    const NERemoteService::eRemoteServices  mService;

    /**
     * \brief   The bitwise set of connection types supported by remote service.
     **/
    const unsigned int                      mConnectTypes;

    /**
     * \brief   The type of messaging source application.
     **/
    const NEService::eMessageSource         mMessageSource;

    /**
     * \brief   Client connection object
     **/
    ClientConnection                        mClientConnection;
    /**
     * \brief   Instance of remote servicing consumer to handle message.
     **/
    IEServiceConnectionConsumer &           mConnectionConsumer;

    /**
     * \brief   The thread that makes message dispatching.
     **/
    DispatcherThread &                      mMessageDispatcher;

    /**
     * \brief   The connection channel.
     **/
    Channel                                 mChannel;

    /**
     * \brief   The sate of connection
     **/
    eConnectionState                        mConnectionState;

    /**
     * \brief   The Client Service event consumer
     **/
    ServiceClientEventConsumer              mEventConsumer;

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
    ServiceClientConnectionBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServiceClientConnectionBase );
};

//////////////////////////////////////////////////////////////////////////
// ServiceClientConnectionBase class inline methods implementation
//////////////////////////////////////////////////////////////////////////

inline ServiceClientConnectionBase & ServiceClientConnectionBase::self( void )
{
    return (*this);
}

inline const ITEM_ID & ServiceClientConnectionBase::getConnectionCookie( void ) const
{
    return mClientConnection.getCookie();
}

inline uint32_t ServiceClientConnectionBase::queryBytesSent( void )
{
    return mThreadSend.extractDataSend();
}

inline uint32_t ServiceClientConnectionBase::queryBytesReceived( void )
{
    return mThreadReceive.extractDataReceive();
}

inline void ServiceClientConnectionBase::enableCalculateDataRate(bool enable)
{
    mThreadReceive.setEnableCalculateData(enable);
    mThreadSend.setEnableCalculateData(enable);
}

inline bool ServiceClientConnectionBase::isCalculateDataRateEnabled(void) const
{
    return mThreadReceive.isCalculateDataEnabled() && mThreadSend.isCalculateDataEnabled();
}

inline bool ServiceClientConnectionBase::isConnectState( void ) const
{
    return (static_cast<uint16_t>(mConnectionState) & static_cast<uint16_t>(ServiceClientConnectionBase::eConnectionState::ConnectState));
}

inline bool ServiceClientConnectionBase::isConnectedState( void ) const
{
    return (mConnectionState == ServiceClientConnectionBase::eConnectionState::ConnectionStarted);
}

inline bool ServiceClientConnectionBase::isDisconnectState( void ) const
{
    return (static_cast<uint16_t>(mConnectionState) & static_cast<uint16_t>(ServiceClientConnectionBase::eConnectionState::DisconnectState));
}

inline void ServiceClientConnectionBase::registerForServiceClientCommands(void)
{
    ServiceClientEvent::addListener(static_cast<IEServiceClientEventConsumer&>(mEventConsumer), mMessageDispatcher);
}

inline void ServiceClientConnectionBase::unregisterForServiceClientCommands(void)
{
    ServiceClientEvent::removeListener(static_cast<IEServiceClientEventConsumer&>(mEventConsumer), mMessageDispatcher);
}

inline const char * ServiceClientConnectionBase::getString(ServiceClientConnectionBase::eConnectionState val)
{
    switch (val)
    {
    case ServiceClientConnectionBase::eConnectionState::ConnectionStopped:
        return "ServiceClientConnectionBase::ConnectionStopped";
    case ServiceClientConnectionBase::eConnectionState::ConnectionStarting:
        return "ServiceClientConnectionBase::ConnectionStarting";
    case ServiceClientConnectionBase::eConnectionState::ConnectionStarted:
        return "ServiceClientConnectionBase::ConnectionStarted";
    case ServiceClientConnectionBase::eConnectionState::ConnectionStopping:
        return "ServiceClientConnectionBase::ConnectionStopping";
    default:
        return "ERR: Invalid value of ServiceClientConnectionBase::eConnectionState type";
    }
}

inline bool ServiceClientConnectionBase::isConnectionStarted(void) const
{
    const ITEM_ID & cookie = mClientConnection.getCookie();
    return (mClientConnection.isValid() && (cookie != NEService::COOKIE_LOCAL) && (cookie != NEService::COOKIE_UNKNOWN));
}

inline void ServiceClientConnectionBase::setConnectionState(const ServiceClientConnectionBase::eConnectionState newState)
{
    mConnectionState = newState;
}

inline ServiceClientConnectionBase::eConnectionState ServiceClientConnectionBase::getConnectionState(void) const
{
    return mConnectionState;
}

inline void ServiceClientConnectionBase::sendCommand( ServiceEventData::eServiceEventCommands cmd
                                                    , Event::eEventPriority eventPrio /*= Event::eEventPriority::EventPriorityNormal*/ )
{
    ServiceClientEvent::sendEvent( ServiceEventData( cmd )
                                 , static_cast<IEServiceClientEventConsumer &>(mEventConsumer)
                                 , mMessageDispatcher
                                 , eventPrio );
}

inline bool ServiceClientConnectionBase::sendMessage(const RemoteMessage & data, Event::eEventPriority eventPrio /*= Event::eEventPriority::EventPriorityNormal*/ )
{
    return SendMessageEvent::sendEvent( SendMessageEventData(data)
                                      , static_cast<IESendMessageEventConsumer &>(mThreadSend)
                                      , static_cast<DispatcherThread &>(mThreadSend)
                                      , eventPrio);
}

inline void ServiceClientConnectionBase::disconnectService( Event::eEventPriority eventPrio )
{
    SendMessageEvent::sendEvent( SendMessageEventData()
                               , static_cast<IESendMessageEventConsumer &>(mThreadSend)
                               , static_cast<DispatcherThread &>(mThreadSend)
                               , eventPrio );
}

#endif  // AREG_IPC_SERVICECLIENTCONNECTIONBASE_HPP
