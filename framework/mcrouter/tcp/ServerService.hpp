#ifndef AREG_MCROUTER_TCP_SERVERSERVICE_HPP
#define AREG_MCROUTER_TCP_SERVERSERVICE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        mcrouter/tcp/ServerService.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service Connection server declaration
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/IETimerConsumer.hpp"
#include "areg/ipc/IERemoteService.hpp"
#include "areg/ipc/IERemoteServiceConsumer.hpp"
#include "areg/ipc/IERemoteServiceHandler.hpp"
#include "mcrouter/tcp/private/IEServerConnectionHandler.hpp"

#include "mcrouter/tcp/private/ServerReceiveThread.hpp"
#include "mcrouter/tcp/private/ServerServiceEvent.hpp"
#include "mcrouter/tcp/private/ServerSendThread.hpp"
#include "mcrouter/tcp/private/ServerConnection.hpp"
#include "mcrouter/tcp/private/ServiceRegistry.hpp"
#include "areg/ipc/NERemoteService.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/base/TEArrayList.hpp"
#include "areg/base/Containers.hpp"
#include "areg/component/Timer.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IERemoteServiceConsumer;

//////////////////////////////////////////////////////////////////////////
// ServerService class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The server side connection service. Used by message router to
 *          accept service connections.
 **/
class ServerService : public    IERemoteService
                    , private   DispatcherThread
                    , private   IEServerConnectionHandler
                    , private   IERemoteServiceConsumer
                    , private   IERemoteServiceHandler
{
    friend class ServerServiceEventConsumer;
    friend class TimerConsumer;
//////////////////////////////////////////////////////////////////////////
// Internal objects
//////////////////////////////////////////////////////////////////////////
    
//////////////////////////////////////////////////////////////////////////
// ServerService::ServerServiceEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////
    class ServerServiceEventConsumer : public  IEServerServiceEventConsumer
    {
    public:
        //!< Initializes the server service consumer object
        ServerServiceEventConsumer( ServerService & service );

        virtual ~ServerServiceEventConsumer( void ) = default;

    //////////////////////////////////////////////////////////////////////////
    // Hidden methods
    //////////////////////////////////////////////////////////////////////////
    private:

    /************************************************************************/
    // IEServerServiceEventConsumer interface overrides.
    /************************************************************************/
        /**
         * \brief   Automatically triggered when event is dispatched by registered
         *          worker / component thread.
         * \param   data    The data object passed in event. It should have at least
         *                  default constructor and assigning operator.
         *                  This object is not used for IPC.
         **/
        virtual void processEvent( const ServerServiceEventData & data ) override;

    //////////////////////////////////////////////////////////////////////////
    // Hidden variables
    //////////////////////////////////////////////////////////////////////////
    private:
        ServerService & mService;   //!< The instance of server service object.

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        ServerServiceEventConsumer( void ) = delete;
        DECLARE_NOCOPY_NOMOVE( ServerServiceEventConsumer );
    };

//////////////////////////////////////////////////////////////////////////
// ServerService::TimerConsumer class declaration
//////////////////////////////////////////////////////////////////////////
    //!< The timer consumer object used by server service
    class TimerConsumer : public IETimerConsumer
    {
    public:
        TimerConsumer( ServerService & service );

        virtual ~TimerConsumer( void ) = default;

    //////////////////////////////////////////////////////////////////////////
    // Hidden methods
    //////////////////////////////////////////////////////////////////////////
    private:
    /************************************************************************/
    // IETimerConsumer interface overrides.
    /************************************************************************/
        /**
         * \brief   Triggered when Timer is expired. 
         * \param   timer   The timer object that is expired.
         **/
        virtual void processTimer( Timer & timer ) override;

    //////////////////////////////////////////////////////////////////////////
    // Hidden variables
    //////////////////////////////////////////////////////////////////////////
    private:
        ServerService & mService;   //!< The instance of server service object

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        TimerConsumer( void ) = delete;
        DECLARE_NOCOPY_NOMOVE( TimerConsumer );
    };

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
    ServerService( void );
    /**
     * \brief   Destructor
     **/
    virtual ~ServerService( void )= default;

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

/************************************************************************/
// IERemoteService interface overrides
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
    virtual bool configureRemoteServicing( const String & configFile ) override;

    /**
     * \brief   Call manually to set routing service host name and port number.
     *          Note, if remote service is already started, this call will change
     *          data, but will not restart service.
     * \param   hostName    IP-address or host name of routing service to connect.
     * \param   portNr      Port number of routing service to connect.
     **/
    virtual void setRemoteServiceAddress( const String & hostName, unsigned short portNr ) override;

    /**
     * \brief   Call to start remote service. The host name and port number should be already set.
     * \return  Returns true if start service is triggered.
     **/
    virtual bool startRemoteServicing( void ) override;

    /**
     * \brief   Call to restart remove service. The host name and the port number should be already set.
     *          If the service had connection, it will be lost and re-connected again. If there was no
     *          connection, it starts new connection.
     * \return  Returns true if succeeded to restart service.
     **/
    virtual bool restartRemoteServicing(void) override;

    /**
     * \brief   Call to stop service. No more remote communication should be possible.
     **/
    virtual void stopRemoteServicing( void ) override;

    /**
     * \brief   Returns true, if remote service is started and ready to operate.
     **/
    virtual bool isRemoteServicingStarted( void ) const override;

    /**
     * \brief   Returns true if service is configured and ready to start
     **/
    virtual bool isRemoteServicingConfigured( void ) const override;

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

protected:
    /**
     * \brief   Call to register remote service server stub object.
     *          All clients waiting for service should be connected notifications.
     * \param   stubService     The address of server stub service to register in system
     *                          The address contains service name and role name of service.
     * \return  Returns true if succeeded to start registration.
     **/
    virtual bool registerService( const StubAddress & stubService ) override;

    /**
     * \brief   Call to unregister previously registered server stub interface.
     * \param   stubService     The address of server stub service to unregister in system.
     **/
    virtual void unregisterService( const StubAddress & stubService ) override;

    /**
     * \brief   Call to register client proxy of service. If system already has registered
     *          service server stub, the client will receive connected notification.
     *          Otherwise, the client will be in disconnected state as long, until server
     *          service is not registered in system.
     * \param   proxyService    The address of client proxy to register in system.
     * \return  Returns true if registration process started with success. Otherwise, it returns false.
     **/
    virtual bool registerServiceClient( const ProxyAddress & proxyService ) override;

    /**
     * \brief   Call to unregister previously registered client prosy service.
     * \param   proxyService    The address of client proxy to unregister from system.
     **/
    virtual void unregisterServiceClient( const ProxyAddress & proxyService ) override;

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
// IERemoteServiceConsumer
/************************************************************************/

    /**
     * \brief   Call to receive list of registered remote stub and proxy services, which connection cookie is equal to 
     *          specified value. In output out_listStubs and out_lisProxies contain list of remote stub and proxy addresses.
     * \param   cookie          The cookie to filter. Pass NEService::COOKIE_ANY to ignore filtering
     * \param   out_listStubs   On output this will contain list of remote stub addresses connected with specified cookie value.
     * \param   out_lisProxies  On output this will contain list of remote proxy addresses connected with specified cookie value.
     **/
    virtual void getServiceList( ITEM_ID IN cookie, TEArrayList<StubAddress> & OUT out_listStubs, TEArrayList<ProxyAddress> & OUT out_lisProxies ) const override;

    /**
     * \brief   Registers remote stub in the current process.
     * \param   stub    The address of remote stub server to register
     **/
    virtual void registerRemoteStub( const StubAddress & stub ) override;

    /**
     * \brief   Registers remote proxy in the current process.
     * \param   proxy   The address of remote proxy client to register
     **/
    virtual void registerRemoteProxy( const ProxyAddress & proxy ) override;

    /**
     * \brief   Unregisters remote stub in the current process.
     * \param   stub    The address of remote stub server to unregister
     * \param   cookie  The cookie that has initiated unregister message.
     *                  The parameter is ignored if 'NEService::COOKIE_ANY'.
     **/
    virtual void unregisterRemoteStub( const StubAddress & stub, ITEM_ID cookie /*= NEService::COOKIE_ANY*/ ) override;

    /**
     * \brief   Unregisters remote proxy in the current process.
     * \param   proxy   The address of remote proxy client to unregister
     * \param   cookie  The cookie that has initiated unregister message.
     *                  The parameter is ignored if 'NEService::COOKIE_ANY'.
     **/
    virtual void unregisterRemoteProxy( const ProxyAddress & proxy, ITEM_ID cookie /*= NEService::COOKIE_ANY*/ ) override;

    /**
     * \brief   Triggered when remote service has been started and there is a
     *          connection established with service.
     * \param   channel     The connection channel of remote routing service.
     **/
    virtual void remoteServiceStarted( const Channel & channel ) override;

    /**
     * \brief   Triggered when connection with remote service has been stopped.
     * \param   channel     The connection channel of remote routing service.
     **/
    virtual void remoteServiceStopped( const Channel & channel ) override;

    /**
     * \brief   Triggered when connection with remote routing service is lost.
     *          The connection is considered lost if it not possible to read or
     *          receive data, and there was not stop connection triggered.
     * \param   channel     The connection channel of remote routing service.
     **/
    virtual void remoteServiceConnectionLost( const Channel & channel ) override;

/************************************************************************/
// IERemoteServiceHandler interface overrides
/************************************************************************/

    /**
     * \brief   Triggered, when failed to send message.
     * \param   msgFailed   The message, which failed to send.
     **/
    virtual void failedSendMessage( const RemoteMessage & msgFailed ) override;

    /**
     * \brief   Triggered, when failed to receive message.
     * \param   whichSource Indicated the source, which failed.
     **/
    virtual void failedReceiveMessage( SOCKETHANDLE whichSource ) override;

    /**
     * \brief   Triggered, when failed to process message, i.e. the target for message processing was not found.
     *          In case of request message processing, the source should receive error notification.
     * \param   msgUnprocessed  Unprocessed message data.
     **/
    virtual void failedProcessMessage( const RemoteMessage & msgUnprocessed ) override;

    /**
     * \brief   Triggered, when need to process received message.
     * \param   msgReceived Received message to process.
     * \param   addrHost    The address of remote host, which sent message.
     * \param   whichSource The socket handle, which received message.
     **/
    virtual void processReceivedMessage( const RemoteMessage & msgReceived, const NESocket::SocketAddress & addrHost, SOCKETHANDLE whichSource ) override;

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

    /**
     * \brief   Called when need to start the network server connection service. 
     **/
    void onServiceStart( void );

    /**
     * \brief   Called when need to stop the network server connection service. 
     **/
    void onServiceStop( void );

    /**
     * \brief   Called when need to restart the network server connection service. 
     **/
    void onServiceRestart( void );

    /**
     * \brief   Called when received a communication message to dispatch and process.
     * \param   msgReceived     The received the communication message. 
     **/
    void onServiceMessageReceived(const RemoteMessage & msgReceived);

    /**
     * \brief   Called when need to send a communication message.
     * \param   msgReceived     The received the communication message. 
     **/
    void onServiceMessageSend(const RemoteMessage & msgSend);

    /**
     * \brief   Triggered when Timer is expired. 
     **/
    void onTimerExpired( void );

    /**
     * \brief   Returns instance of object. For internal use only.
     **/
    inline ServerService & self( void );

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
     * \param   cmd     The command to send and process.
     * \return  Returns true if succeeded to send the command.
     **/
    inline bool sendCommand(ServerServiceEventData::eServerServiceCommands cmd);

    /**
     * \brief   Call to send the event to process.
     * \param   cmd     The command to send and process.
     * \param   msg     The message to forward.
     * \return  Returns true if succeeded to send the command.
     **/
    inline bool sendCommunicationMessage(ServerServiceEventData::eServerServiceCommands cmd, const RemoteMessage & msg);

//////////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////////
private:
    ServerConnection            mServerConnection;  //!< The instance of server connection object.
    Timer                       mTimerConnect;      //!< The timer object to trigger in case if failed to create server socket.
    ServerSendThread            mThreadSend;        //!< The thread to send messages to clients
    ServerReceiveThread         mThreadReceive;     //!< The thread to receive messages from clients
    ServiceRegistry             mServiceRegistry;   //!< The service registry map to track stub-proxy connections
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
    DECLARE_NOCOPY_NOMOVE( ServerService );
};

//////////////////////////////////////////////////////////////////////////////
// ServerService class inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline ServerService & ServerService::self( void )
{
    return (*this);
}

inline void ServerService::startEventListener(void)
{
    ServerServiceEvent::addListener( static_cast<IEServerServiceEventConsumer &>(mEventConsumer), static_cast<DispatcherThread &>(self()) );
}

inline void ServerService::stopEventListener(void)
{
    ServerServiceEvent::removeListener( static_cast<IEServerServiceEventConsumer &>(mEventConsumer), static_cast<DispatcherThread &>(self()) );
}

inline bool ServerService::sendCommand(ServerServiceEventData::eServerServiceCommands cmd)
{
    return ServerServiceEvent::sendEvent( ServerServiceEventData(cmd)
                                        , static_cast<IEServerServiceEventConsumer &>(mEventConsumer)
                                        , static_cast<DispatcherThread &>(self())
                                        , Event::eEventPriority::EventPriorityHigh);
}

inline bool ServerService::sendCommunicationMessage(ServerServiceEventData::eServerServiceCommands cmd, const RemoteMessage & msg)
{
    return ServerServiceEvent::sendEvent( ServerServiceEventData(cmd, msg)
                                        , static_cast<IEServerServiceEventConsumer &>(mEventConsumer)
                                        , static_cast<DispatcherThread &>(self())
                                        , Event::eEventPriority::EventPriorityHigh);
}

inline bool ServerService::isAddressInWhiteList(const NESocket::SocketAddress & addrClient) const
{
    return (mWhiteList.isEmpty() == false ? mWhiteList.find(addrClient.getHostAddress()) != -1 : false);
}

inline bool ServerService::isAddressInBlackList(const NESocket::SocketAddress & addrClient) const
{
    return mBlackList.isEmpty() == false ? mBlackList.find(addrClient.getHostAddress()) != -1 : false;
}

inline void ServerService::addWhiteList(const NESocket::SocketAddress & addrClient)
{
    if ( mWhiteList.find(addrClient.getHostAddress()) == -1 )
    {
        mWhiteList.add(addrClient.getHostAddress());
    }
}

inline void ServerService::addBlackList(const NESocket::SocketAddress & addrClient)
{
    if ( mBlackList.find(addrClient.getHostAddress()) == -1 )
    {
        mBlackList.add(addrClient.getHostAddress());
    }
}

inline void ServerService::removeWhiteList(const NESocket::SocketAddress & addrClient)
{
    mWhiteList.removeElem( addrClient.getHostAddress(), 0);
}

inline void ServerService::removeBlackList(const NESocket::SocketAddress & addrClient)
{
    mBlackList.removeElem( addrClient.getHostAddress(), 0);
}

inline uint32_t ServerService::queryBytesSent(void)
{
    return mThreadSend.extractDataSend();
}

inline uint32_t ServerService::queryBytesReceived(void)
{
    return mThreadReceive.extractDataReceive();
}

#endif  // AREG_MCROUTER_TCP_SERVERSERVICE_HPP
