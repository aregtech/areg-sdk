#ifndef AREG_SERVICE_CONNECTIVITY_SERVICECOMMUNICATONBASE_HPP
#define AREG_SERVICE_CONNECTIVITY_SERVICECOMMUNICATONBASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        service/connectivity/ServiceCommunicatonBase.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service communication base class
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/IETimerConsumer.hpp"
#include "areg/ipc/IERemoteMessageHandler.hpp"
#include "areg/ipc/IEServiceConnectionConsumer.hpp"
#include "areg/ipc/IEServiceConnectionProvider.hpp"
#include "areg/ipc/ServiceEventConsumerBase.hpp"
#include "service/connectivity/IEServiceConnectionHandler.hpp"

#include "service/connectivity/ServerReceiveThread.hpp"
#include "service/connectivity/ServerSendThread.hpp"
#include "service/connectivity/ServerConnection.hpp"
#include "areg/ipc/NERemoteService.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/base/TEArrayList.hpp"
#include "areg/base/Containers.hpp"
#include "areg/component/Timer.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IERemoteServiceConsumer;
class IEServiceMessageDispatcher;

//////////////////////////////////////////////////////////////////////////
// ServiceCommunicatonBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The server side connection service. Used by message router to
 *          accept service connections.
 **/
class ServiceCommunicatonBase   : protected IERemoteMessageHandler
                                , protected IEServiceConnectionConsumer
                                , protected IEServiceConnectionProvider
                                , private   DispatcherThread
                                , private   ServiceEventConsumerBase
                                , private   IEServiceConnectionHandler
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The default connection type of server service.
     **/
    static constexpr NERemoteService::eServiceConnection    CONNECT_TYPE    { NERemoteService::eServiceConnection::ConnectionTcpip };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor to initialize default values.
     **/
    ServiceCommunicatonBase( ITEM_ID serviceId, IEServiceMessageDispatcher& msgDispatcher);
    /**
     * \brief   Destructor
     **/
    virtual ~ServiceCommunicatonBase( void )= default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Checks whether the specified host address is in the white-list.
     * \param   addrClient  The object, which contains client host address to check.
     * \return  Returns true if specified host address is in the white-list.
     **/
    inline bool isAddressInWhiteList( const NESocket::SocketAddress & addrClient ) const;

    /**
     * \brief   Checks whether the specified host address is in the black-list.
     * \param   addrClient  The object, which contains client host address to check.
     * \return  Returns true if specified host address is in the black-list.
     **/
    inline bool isAddressInBlackList( const NESocket::SocketAddress & addrClient ) const;

    /**
     * \brief   Adds specified host address to the white-list.
     * \param   addrClient  The host address of client to add to list.
     **/
    inline void addWhiteList( const NESocket::SocketAddress & addrClient );

    /**
     * \brief   Adds specified host address to black-list.
     * \param   addrClient  The host address of client to add to list.
     **/
    void addBlackList( const NESocket::SocketAddress & addrClient );

    /**
     * \brief   Removes specified host address from white-list.
     * \param   addrClient  The host address of client to remove from list.
     **/
    inline void removeWhiteList( const NESocket::SocketAddress & addrClient );

    /**
     * \brief   Removes specified host address from black-list.
     * \param   addrClient  The host address of client to remove from list.
     **/
    inline void removeBlackList( const NESocket::SocketAddress & addrClient );

    /**
     * \brief   Each time querying the bytes sent via network connection returns
     *          the value after last query.
     **/
    inline uint32_t queryBytesSent(void);

    /**
     * \brief   Each time querying the bytes received via network connection returns
     *          the value after last query.
     **/
    inline uint32_t queryBytesReceived(void);

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

/************************************************************************/
// IERemoteServiceConnection interface overrides
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
// IEServerConnectionHandler interface overrides
/************************************************************************/

    /**
     * \brief   Call to check whether new client connection should be accepted
     *          or rejected. Once client is accepted, server will start to receive
     *          messages from client. Otherwise, connection with client is immediately
     *          closed and communication is stopped.
     * \param   clientSocket    Accepted client socket object to check.
     * \return  Returns true if client connection can be accepted. To reject and close
     *          connection with client, the method should return false.
     **/
    virtual bool canAcceptConnection( const SocketAccepted & clientSocket ) override;

    /**
     * \brief   Triggered, when lost connection with client.
     *          Passed clientSocket parameter specifies client socket, which lost connection.
     * \param   clientSocket    Client socket object, which lost connection.
     **/
    virtual void connectionLost( SocketAccepted & clientSocket ) override;

    /**
     * \brief   Triggered, when there is a connection failure. Normally, this should restart the connection.
     **/
    virtual void connectionFailure( void ) override;

/************************************************************************/
// IEServiceConnectionConsumer
/************************************************************************/

    /**
     * \brief   Triggered when remote service connection and communication channel is established.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void connectedRemoteServiceChannel( const Channel & channel ) override;

    /**
     * \brief   Triggered when disconnected remote service connection and communication channel.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void disconnectedRemoteServiceChannel( const Channel & channel ) override;

    /**
     * \brief   Triggered when remote service connection and communication channel is lost.
     *          The connection is considered lost if it not possible to read or
     *          receive data, and it was not stopped by API call.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void lostRemoteServiceChannel( const Channel & channel ) override;

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
     * \brief	Triggered when dispatcher starts running. 
     *          In this function runs main dispatching loop.
     *          Events are picked and dispatched here.
     *          Override if logic should be changed.
     * \return	Returns true if Exit Event is signaled.
     **/
    virtual bool runDispatcher( void ) override;

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
// IEEventRouter interface overrides
/************************************************************************/

    /**
     * \brief   Called when need to start the network server connection service. 
     **/
    virtual void onServiceStart( void ) override;

    /**
     * \brief   Called when need to stop the network server connection service. 
     **/
    virtual void onServiceStop( void ) override;

    /**
     * \brief   Called when need to restart the network server connection service. 
     **/
    virtual void onServiceRestart( void ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Checks whether can accept connection of specified host and if so, returns true.
     *          The client connection is rejected if the host entry exists in black-list.
     *          The client connection is rejected if white-list is not empty and the
     *          client is not listed. In all other cases the client connection can be
     *          accepted, i.e. if it is not listed in black-list, if white-list is empty
     *          or exists in white-list.
     * \param   addrClient  The client host to check connection acceptance possibility
     * \return  Returns true if the client connection can be accepted.
     **/
    bool canAcceptConnection( const NESocket::SocketAddress & addrConnect ) const;

    /**
     * \brief   Called to start connection procedure to accept client connections.
     * \return  Returns true if could start connection.
     **/
    bool startConnection( void );

    /**
     * \brief   Call to restart the connection. Returns true if succeeded to reconnect.
     **/
    bool restartConnection( void );

    /**
     * \brief   Called to stop connection. All clients are automatically disconnected.
     **/
    void stopConnection( void );

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Triggered when Timer is expired. 
     **/
    void onTimerExpired( void );

    /**
     * \brief   Call when the dispatcher should start listening the events. 
     **/
    inline void startEventListener( void );
    
    /**
     * \brief   Call when the dispatcher should stop listening the events. 
     **/
    inline void stopEventListener( void );

    /**
     * \brief   Call to send the event to process.
     * \param   cmd         The command to send and process.
     * \param   eventPrio   The priority of the event. By default, it is normal.
     * \return  Returns true if succeeded to send the command.
     **/
    inline bool sendCommand(ServiceEventData::eServiceEventCommands cmd, Event::eEventPriority eventPrio = Event::eEventPriority::EventPriorityNormal);

    /**
     * \brief   Call to send the event to process.
     * \param   cmd     The command to send and process.
     * \param   msg     The message to forward.
     * \return  Returns true if succeeded to send the command.
     **/
    inline bool sendCommunicationMessage(ServiceEventData::eServiceEventCommands cmd, const RemoteMessage & msg, Event::eEventPriority eventPrio = Event::eEventPriority::EventPriorityNormal );

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
     * \brief   Returns instance of object. For internal use only.
     **/
    inline ServiceCommunicatonBase & self( void );

//////////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////////
protected:
    ServerConnection            mServerConnection;  //!< The instance of server connection object.
    Timer                       mTimerConnect;      //!< The timer object to trigger in case if failed to create server socket.
    ServerSendThread            mThreadSend;        //!< The thread to send messages to clients
    ServerReceiveThread         mThreadReceive;     //!< The thread to receive messages from clients
    bool                        mIsServiceEnabled;  //!< The flag indicating whether the server servicing is enabled or not.
    String                      mConfigFile;        //!< The full path of connection configuration file.
    StringArray                 mWhiteList;         //!< The list of enabled fixed client hosts.
    StringArray                 mBlackList;         //!< The list of disabled fixes client hosts.
    ServerServiceEventConsumer  mEventConsumer;     //!< The custom event consumer object
    TimerConsumer               mTimerConsumer;     //!< The timer consumer object.
    SynchEvent                  mEventSendStop;     //!< The event set when cannot send and receive data anymore.
    mutable ResourceLock        mLock;              //!< The synchronization object to be accessed from different threads.

//////////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( ServiceCommunicatonBase );
};

//////////////////////////////////////////////////////////////////////////////
// ServiceCommunicatonBase class inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline ServiceCommunicatonBase & ServiceCommunicatonBase::self( void )
{
    return (*this);
}

inline void ServiceCommunicatonBase::startEventListener(void)
{
    ServerServiceEvent::addListener( static_cast<IEServerServiceEventConsumer &>(mEventConsumer), static_cast<DispatcherThread &>(self()) );
}

inline void ServiceCommunicatonBase::stopEventListener(void)
{
    ServerServiceEvent::removeListener( static_cast<IEServerServiceEventConsumer &>(mEventConsumer), static_cast<DispatcherThread &>(self()) );
}

inline bool ServiceCommunicatonBase::sendCommand(ServiceEventData::eServiceEventCommands cmd, Event::eEventPriority eventPrio /*= Event::eEventPriority::EventPriorityNormal*/ )
{
    return ServerServiceEvent::sendEvent( ServerServiceEventData(cmd)
                                        , static_cast<IEServerServiceEventConsumer &>(mEventConsumer)
                                        , static_cast<DispatcherThread &>(self())
                                        , eventPrio);
}

inline bool ServiceCommunicatonBase::sendCommunicationMessage( ServiceEventData::eServiceEventCommands cmd
                                                   , const RemoteMessage & msg
                                                   , Event::eEventPriority eventPrio /*= Event::eEventPriority::EventPriorityNormal*/ )
{
    return ServerServiceEvent::sendEvent( ServerServiceEventData(cmd, msg)
                                        , static_cast<IEServerServiceEventConsumer &>(mEventConsumer)
                                        , static_cast<DispatcherThread &>(self())
                                        , eventPrio );
}

inline bool ServiceCommunicatonBase::isAddressInWhiteList(const NESocket::SocketAddress & addrClient) const
{
    return (mWhiteList.isEmpty() == false ? mWhiteList.find(addrClient.getHostAddress()) != -1 : false);
}

inline bool ServiceCommunicatonBase::isAddressInBlackList(const NESocket::SocketAddress & addrClient) const
{
    return mBlackList.isEmpty() == false ? mBlackList.find(addrClient.getHostAddress()) != -1 : false;
}

inline void ServiceCommunicatonBase::addWhiteList(const NESocket::SocketAddress & addrClient)
{
    if ( mWhiteList.find(addrClient.getHostAddress()) == -1 )
    {
        mWhiteList.add(addrClient.getHostAddress());
    }
}

inline void ServiceCommunicatonBase::addBlackList(const NESocket::SocketAddress & addrClient)
{
    if ( mBlackList.find(addrClient.getHostAddress()) == -1 )
    {
        mBlackList.add(addrClient.getHostAddress());
    }
}

inline void ServiceCommunicatonBase::removeWhiteList(const NESocket::SocketAddress & addrClient)
{
    mWhiteList.removeElem( addrClient.getHostAddress(), 0);
}

inline void ServiceCommunicatonBase::removeBlackList(const NESocket::SocketAddress & addrClient)
{
    mBlackList.removeElem( addrClient.getHostAddress(), 0);
}

inline uint32_t ServiceCommunicatonBase::queryBytesSent(void)
{
    return mThreadSend.extractDataSend();
}

inline uint32_t ServiceCommunicatonBase::queryBytesReceived(void)
{
    return mThreadReceive.extractDataReceive();
}

#endif  // AREG_SERVICE_CONNECTIVITY_SERVICECOMMUNICATONBASE_HPP
