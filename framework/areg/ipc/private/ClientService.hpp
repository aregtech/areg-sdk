#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ClientService.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service Connection client declaration
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/ipc/IERemoteService.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/ipc/IERemoteServiceHandler.hpp"
#include "areg/component/IERemoteEventConsumer.hpp"
#include "areg/component/IETimerConsumer.hpp"

#include "areg/ipc/private/ClientReceiveThread.hpp"
#include "areg/ipc/private/ClientServiceEvent.hpp"
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
class IERemoteServiceConsumer;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// ClientService class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The connected client servicing object to handle connections,
 *          to read and send message, to dispatch messages and
 *          communicate with service manager.
 **/
class ClientService : public    IERemoteService
                    , private   DispatcherThread
                    , private   IERemoteServiceHandler
                    , private   IEClientServiceEventConsumer
                    , private   IERemoteEventConsumer
                    , private   IETimerConsumer
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
     * \brief   ClientService::eConnectionState
     *          Defines connection state values
     **/
    typedef enum class E_ConnectionState
    {
          ConnectionStopped     //!< The connection is stopped, i.e. not connected.
        , ConnectionStarting    //!< The connection is initiated, but the status is not known.
        , ConnectionStarted     //!< The connection is established and accepted by server.
        , ConnectionStopping    //!< The connection is stopping, because of manual request.
    } eConnectionState;

    /**
     * \brief   Returns the string value of ClientService::eConnectionState type
     **/
    static inline const char * getString(ClientService::eConnectionState val);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes client servicing object, sets remote service consumer object.
     * \param   serviceConsumer The instance of remote servicing consumer object to handles notifications.
     **/
    ClientService( IERemoteServiceConsumer & serviceConsumer );
    /**
     * \brief   Destructor
     **/
    virtual ~ClientService( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attribute
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the value of cookie set in connection.
     **/
    ITEM_ID getConnectionCookie( void ) const;


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
    virtual bool configureRemoteServicing( const char * configFile ) override;

    /**
     * \brief   Call manually to set router service host name and port number.
     *          Note, if remote service is already started, this call will change
     *          data, but will not restart service.
     * \param   hostName    IP-address or host name of routing service to connect.
     * \param   portNr      Port number of routing service to connect.
     **/
    virtual void setRemoteServiceAddress( const char * hostName, unsigned short portNr ) override;

    /**
     * \brief   Call to start thread for remote servicing. The host name and port number should be already set.
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

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

/************************************************************************/
// IETimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired.
     *          The passed Timer parameter is indicating object, which has been expired.
     *          Overwrite method to receive messages.
     * \param   timer   The timer object that is expired.
     **/
    virtual void processTimer( Timer & timer ) override;

/************************************************************************/
// TEEventConsumer<DATA_CLASS, DATA_CLASS_TYPE> interface overrides.
/************************************************************************/
    /**
     * \brief   Automatically triggered when event is dispatched by registered
     *          worker / component thread.
     * \param   data    The data object passed in event. It should have at least
     *                  default constructor and assigning operator.
     *                  This object is not used for IPC.
     **/
    virtual void processEvent( const ClientServiceEventData & data ) override;

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

private:
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
    inline bool startConnection( void );
    /**
     * \brief   Called to stop client socket connection.
     **/
    inline void stopConnection( void );
    /**
     * \brief   Called when connection is lost and should be immediately canceled.
     **/
    inline void cancelConnection( void );
    /**
     * \brief   Returns true if client socket connection is started.
     **/
    inline bool isStarted( void ) const;
    /**
     * \brief   Sets client socket connection state.
     * \param   newState    The connection state to set.
     **/
    inline void setConnectionState( ClientService::eConnectionState newState );
    /**
     * \brief   Returns current client socket connection state.
     **/
    inline ClientService::eConnectionState getConnectionState( void ) const;
    /**
     * \brief   Queues the message for sending
     **/
    inline bool queueSendMessage(const RemoteMessage & data);
    /**
     * \brief   Returns instance of client servicing object.
     **/
    inline ClientService & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Client connection object
     **/
    ClientConnection            mClientConnection;
    /**
     * \brief   Instance of remote servicing consumer to handle message.
     **/
    IERemoteServiceConsumer &   mServiceConsumer;
    /**
     * \brief   Connection retry timer object.
     **/
    Timer                       mTimerConnect;
    /**
     * \brief   Message receiver thread
     **/
    ClientReceiveThread         mThreadReceive;
    /**
     * \brief   Message sender thread
     **/
    ClientSendThread            mThreadSend;
    /**
     * \brief   Flag, indicates whether the remote servicing is enabled or not.
     **/
    bool                        mIsServiceEnabled;
    /**
     * \brief   The path of configuration file name.
     **/
    String                      mConfigFile;
    /**
     * \brief   The connection channel.
     **/
    Channel                     mChannel;
    /**
     * \brief   The sate of connection
     **/
    eConnectionState            mConnectionState;
    /**
     * \brief   Data access synchronization object
     **/
    mutable ResourceLock        mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ClientService( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ClientService );
};

//////////////////////////////////////////////////////////////////////////
// ClientService class inline methods implementation
//////////////////////////////////////////////////////////////////////////

inline ClientService & ClientService::self( void )
{
    return (*this);
}

inline ITEM_ID ClientService::getConnectionCookie( void ) const
{
    return mClientConnection.getCookie();
}


inline const char * ClientService::getString(ClientService::eConnectionState val)
{
    switch (val)
    {
    case ClientService::eConnectionState::ConnectionStopped:
        return "ClientService::ConnectionStopped";
    case ClientService::eConnectionState::ConnectionStarting:
        return "ClientService::ConnectionStarting";
    case ClientService::eConnectionState::ConnectionStarted:
        return "ClientService::ConnectionStarted";
    case ClientService::eConnectionState::ConnectionStopping:
        return "ClientService::ConnectionStopping";
    default:
        return "ERR: Invalid value of ClientService::eConnectionState type";
    }
}
