#ifndef AREG_IPC_CLIENTSERVICE_HPP
#define AREG_IPC_CLIENTSERVICE_HPP

/************************************************************************
 * \file        areg/ipc/ClientService.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Connection client declaration
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/ipc/IERemoteService.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/ipc/private/IERemoteServiceHandler.hpp"
#include "areg/component/IERemoteEventConsumer.hpp"
#include "areg/component/IETimerConsumer.hpp"

#include "areg/ipc/private/ClientReceiveThread.hpp"
#include "areg/ipc/private/ClientServiceEvent.hpp"
#include "areg/ipc/private/ClientSendThread.hpp"
#include "areg/ipc/private/ClientConnection.hpp"
#include "areg/ipc/NERemoteService.hpp"
#include "areg/component/Channel.hpp"
#include "areg/component/Timer.hpp"
#include "areg/base/ESynchObjects.hpp"
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
class AREG_API ClientService    : public    IERemoteService
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
    static const NERemoteService::eServiceConnection   CONNECT_TYPE /*= NERemoteService::ConnectionTcpip*/;

    /**
     * \brief   ClientService::eConnectionState
     *          Defines connection state values
     **/
    typedef enum E_ConnectionState
    {
          ConnectionStopped     //!< The connection is stopped, i.e. not connected.
        , ConnectionStarting    //!< The connection is initiated, but the status is not known.
        , ConnectionStarted     //!< The connection is established and accepted by server.
        , ConnectionStopping    //!< The connection is stopping, because of manual request.
    } eConnectionState;

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
    virtual ~ClientService( void );

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
     *          If file path is NULL or empty, Remote Service will have default 
     *          configuration settings.
     * \param   configFile  Relative or absolute path of remote service configuration file.
     *                      If NULL or empty, it will use default settings.
     * \return  Returns true if system could configure. Otherwise, it returns false.
     **/
    virtual bool configureRemoteServicing( const char * configFile );

    /**
     * \brief   Call manually to set router service host name and port number.
     *          Note, if remote service is already started, this call will change
     *          data, but will not restart service.
     * \param   hostName    IP-address or name of service broker host.
     * \param   portNr      Port number of service broker service.
     **/
    virtual void setRemoteServiceAddress( const char * hostName, unsigned short portNr );

    /**
     * \brief   Call to start thread for remote servicing. The host name and port number should be already set.
     * \return  Returns true if start service is triggered.
     **/
    virtual bool startRemoteServicing( void );

    /**
     * \brief   Call to stop service. No more remote communication should be possible.
     **/
    virtual void stopRemoteServicing( void );

    /**
     * \brief   Returns true, if remote service is started and ready to operate.
     **/
    virtual bool isRemoteServicingStarted( void ) const;

    /**
     * \brief   Returns true if service is configured and ready to start
     **/
    virtual bool isRemoteServicingConfigured( void ) const;

    /**
     * \brief   Returns true if remote service is enabled.
     **/
    virtual bool isRemoteServicingEnabled( void ) const;

    /**
     * \brief   Enables or disables remote service.
     *          The method should be implemented to set business logic of enabling and disabling
     *          remote service in case if it is already started.
     * \param   enable  If true, the service is enabled. Otherwise, it is disabled.
     **/
    virtual void enableRemoteServicing( bool enable );

    /**
     * \brief   Call to register remote service server stub object.
     *          All clients waiting for service should be connected notifications.
     * \param   stubService     The address of server stub service to register in system
     *                          The address contains service name and role name of service.
     * \return  Returns true if succeeded to start registration.
     **/
    virtual bool registerService( const StubAddress & stubService );

    /**
     * \brief   Call to unregister previously registered server stub interface.
     * \param   stubService     The address of server stub service to unregister in system.
     **/
    virtual void unregisterService( const StubAddress & stubService );

    /**
     * \brief   Call to register client proxy of service. If system already has registered
     *          service server stub, the client will receive connected notification.
     *          Otherwise, the client will be in disconnected state as long, until server
     *          service is not registered in system.
     * \param   proxyService    The address of client proxy to register in system.
     * \return  Returns true if registration process started with success. Otherwise, it returns false.
     **/
    virtual bool registerServiceClient( const ProxyAddress & proxyService );

    /**
     * \brief   Call to unregister previously registered client prosy service.
     * \param   proxyService    The address of client proxy to unregister from system.
     **/
    virtual void unregisterServiceClient( const ProxyAddress & proxyService );

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
    virtual void processTimer( Timer & timer );

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
    virtual void processEvent( const ClientServiceEventData & data );

/************************************************************************/
// IERemoteServiceHandler interface overrides
/************************************************************************/

    /**
     * \brief   Triggered, when failed to send message.
     * \param   msgFailed   The message, which failed to send.
     **/
    virtual void failedSendMessage( const RemoteMessage & msgFailed );

    /**
     * \brief   Triggered, when failed to receive message.
     * \param   whichSource Indicated the source, which failed.
     **/
    virtual void failedReceiveMessage( SOCKETHANDLE whichSource );

    /**
     * \brief   Triggered, when failed to process message, i.e. the target for message processing was not found.
     *          In case of request message processing, the source should receive error notification.
     * \param   msgUnprocessed  Unprocessed message data.
     **/
    virtual void failedProcessMessage( const RemoteMessage & msgUnprocessed );

    /**
     * \brief   Triggered, when need to process received message.
     * \param   msgReceived Received message to process.
     * \param   addrHost    The address of remote host, which sent message.
     * \param   whichSource The socket handle, which received message.
     **/
    virtual void processReceivedMessage( const RemoteMessage & msgReceived, const NESocket::InterlockedValue & addrHost, SOCKETHANDLE whichSource );

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
    virtual bool runDispatcher( void );

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
    virtual bool postEvent( Event & eventElem );

private:
/************************************************************************/
// IERemoteEventConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Triggered when the Stub receives remote request event to process.
     * \param   requestEvent        The remote request event to be processed.
     **/
    virtual void processRemoteEvent( RemoteRequestEvent & requestEvent );

    /**
     * \brief   Triggered when the Stub receives remote notification request event to process.
     *          For example, send by Proxy and processed by Stub when need to start or stop
     *          sending attribute update notifications.
     * \param   requestNotifyEvent  The remote notification request event to be processed.
     **/
    virtual void processRemoteEvent( RemoteNotifyRequestEvent & requestNotifyEvent );

    /**
     * \brief   Triggered when the Stub receives remote response request event to process.
     *          For example, send by Proxy and processed by Stub when need to start or stop
     *          to subscribe on information or response sent by Stub.
     * \param   requestNotifyEvent  The remote response request event to be processed.
     **/
    virtual void processRemoteEvent( RemoteResponseEvent & responseEvent );

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
    ClientConnection          mClientConnection;
    /**
     * \brief   Instance of remote servicing consumer to handle message.
     **/
    IERemoteServiceConsumer &   mServiceConsumer;
    /**
     * \brief   Connection retry timer object.
     **/
    Timer                     mTimerConnect;
    /**
     * \brief   Message receiver thread
     **/
    ClientReceiveThread       mThreadReceive;
    /**
     * \brief   Message sender thread
     **/
    ClientSendThread          mThreadSend;
    /**
     * \brief   Flag, indicates whether the remote servicing is enabled or not.
     **/
    bool                        mIsServiceEnabled;
    /**
     * \brief   The path of configuration file name.
     **/
    String                    mConfigFile;
    /**
     * \brief   The connection channel.
     **/
    Channel                   mChannel;
    /**
     * \brief   The sate of connection
     **/
    eConnectionState            mConnectionState;
    /**
     * \brief   Data access synchronization object
     **/
    mutable CriticalSection   mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ClientService( void );
    ClientService( const ClientService & );
    const ClientService & operator = ( const ClientService & );
};

//////////////////////////////////////////////////////////////////////////
// ClientService class inline methods implementation
//////////////////////////////////////////////////////////////////////////

inline ClientService & ClientService::self( void )
{   return (*this);                         }

inline ITEM_ID ClientService::getConnectionCookie( void ) const
{   return mClientConnection.getCookie();   }

#endif  // AREG_IPC_CLIENTSERVICE_HPP
