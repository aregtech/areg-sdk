#ifndef AREG_IPC_CECLIENTSERVICE_HPP
#define AREG_IPC_CECLIENTSERVICE_HPP

/************************************************************************
 * \file        areg/src/ipc/CEClientService.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Connection client declaration
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/ipc/IERemoteService.hpp"
#include "areg/src/component/CEDispatcherThread.hpp"
#include "areg/src/ipc/private/IERemoteServiceHandler.hpp"
#include "areg/src/component/IERemoteEventConsumer.hpp"
#include "areg/src/component/CETimerEvent.hpp"

#include "areg/src/ipc/private/CEClientReceiveThread.hpp"
#include "areg/src/ipc/private/CEClientServiceEvent.hpp"
#include "areg/src/ipc/private/CEClientSendThread.hpp"
#include "areg/src/ipc/private/CEClientConnection.hpp"
#include "areg/src/ipc/NERemoteService.hpp"
#include "areg/src/component/CEChannel.hpp"
#include "areg/src/component/CETimer.hpp"
#include "areg/src/base/ESynchObjects.hpp"
#include "areg/src/base/CEString.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IERemoteServiceConsumer;
class CEDispatcherThread;

//////////////////////////////////////////////////////////////////////////
// CEClientService class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The connected client servicing object to handle connections,
 *          to read and send message, to dispatch messages and 
 *          communicate with service manager.
 **/
class AREG_API CEClientService  : public    IERemoteService
                                , private   CEDispatcherThread
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
     * \brief   CEClientService::eConnectionState
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
    CEClientService( IERemoteServiceConsumer & serviceConsumer );
    /**
     * \brief   Destructor
     **/
    virtual ~CEClientService( void );

//////////////////////////////////////////////////////////////////////////
// Attribute
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the value of cookie set in connection.
     **/
    ITEM_ID GetConnectionCookie( void ) const;


//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
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
    virtual bool ServiceConfigure( const char * configFile );

    /**
     * \brief   Call manually to set router service host name and port number.
     *          Note, if remote service is already started, this call will change
     *          data, but will not restart service.
     * \param   hostName    IP-address or name of service broker host.
     * \param   portNr      Port number of service broker service.
     **/
    virtual void SetRouterServiceAddress( const char * hostName, unsigned short portNr );

    /**
     * \brief   Call to start remote service. The host name and port number should be already set.
     * \return  Returns true if start service is triggered.
     **/
    virtual bool StartRemotingService( void );

    /**
     * \brief   Call to stop service. No more remote communication should be possible.
     **/
    virtual void StopRemotingService( void );

    /**
     * \brief   Returns true, if remote service is started and ready to operate.
     **/
    virtual bool IsServiceStarted( void ) const;

    /**
     * \brief   Returns true if service is configured and ready to start
     **/
    virtual bool IsServiceConfigured( void ) const;

    /**
     * \brief   Returns true if remote service is enabled.
     **/
    virtual bool IsServiceEnabled( void ) const;

    /**
     * \brief   Enables or disables remote service.
     *          The method should be implemented to set business logic of enabling and disabling
     *          remote service in case if it is already started.
     * \param   enable  If true, the service is enabled. Otherwise, it is disabled.
     **/
    virtual void EnableService( bool enable );

    /**
     * \brief   Call to register remote service server stub object.
     *          All clients waiting for service should be connected notifications.
     * \param   stubService     The address of server stub service to register in system
     *                          The address contains service name and role name of service.
     * \return  Returns true if succeeded to start registration.
     **/
    virtual bool RegisterService( const CEStubAddress & stubService );

    /**
     * \brief   Call to unregister previously registered server stub interface.
     * \param   stubService     The address of server stub service to unregister in system.
     **/
    virtual void UnregisterService( const CEStubAddress & stubService );

    /**
     * \brief   Call to register client proxy of service. If system already has registered
     *          service server stub, the client will receive connected notification.
     *          Otherwise, the client will be in disconnected state as long, until server
     *          service is not registered in system.
     * \param   proxyService    The address of client proxy to register in system.
     * \return  Returns true if registration process started with success. Otherwise, it returns false.
     **/
    virtual bool RegisterServiceClient( const CEProxyAddress & proxyService );

    /**
     * \brief   Call to unregister previously registered client prosy service.
     * \param   proxyService    The address of client proxy to unregister from system.
     **/
    virtual void UnregisterServiceClient( const CEProxyAddress & proxyService );

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
    virtual void ProcessTimer( CETimer & timer );

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
    virtual void ProcessEvent( const CEClientServiceEventData & data );

/************************************************************************/
// IERemoteServiceHandler interface overrides
/************************************************************************/

    /**
     * \brief   Triggered, when failed to send message.
     * \param   msgFailed   The message, which failed to send.
     **/
    virtual void FailedSendMessage( const CERemoteMessage & msgFailed );

    /**
     * \brief   Triggered, when failed to receive message.
     * \param   whichSource Indicated the source, which failed.
     **/
    virtual void FailedReceiveMessage( SOCKETHANDLE whichSource );

    /**
     * \brief   Triggered, when failed to process message, i.e. the target for message processing was not found.
     *          In case of request message processing, the source should receive error notification.
     * \param   msgUnprocessed  Unprocessed message data.
     **/
    virtual void FailedProcessMessage( const CERemoteMessage & msgUnprocessed );

    /**
     * \brief   Triggered, when need to process received message.
     * \param   msgReceived Received message to process.
     * \param   addrHost    The address of remote host, which sent message.
     * \param   whichSource The socket handle, which received message.
     **/
    virtual void ProcessReceivedMessage( const CERemoteMessage & msgReceived, const NESocket::CEInterlockedValue & addrHost, SOCKETHANDLE whichSource );

/************************************************************************/
// CEDispatcherThread overrides
/************************************************************************/

    /**
     * \brief	Triggered when dispatcher starts running. 
     *          In this function runs main dispatching loop.
     *          Events are picked and dispatched here.
     *          Override if logic should be changed.
     * \return	Returns true if Exit Event is signaled.
     **/
    virtual bool RunDispatcher( void );

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
    virtual bool PostEvent( CEEvent & eventElem );

private:
/************************************************************************/
// IERemoteEventConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Triggered when the Stub receives remote request event to process.
     * \param   requestEvent        The remote request event to be processed.
     **/
    virtual void ProcessRemoteEvent( CERemoteRequestEvent & requestEvent );

    /**
     * \brief   Triggered when the Stub receives remote notification request event to process.
     *          For example, send by Proxy and processed by Stub when need to start or stop
     *          sending attribute update notifications.
     * \param   requestNotifyEvent  The remote notification request event to be processed.
     **/
    virtual void ProcessRemoteEvent( CERemoteNotifyRequestEvent & requestNotifyEvent );

    /**
     * \brief   Triggered when the Stub receives remote response request event to process.
     *          For example, send by Proxy and processed by Stub when need to start or stop
     *          to subscribe on information or response sent by Stub.
     * \param   requestNotifyEvent  The remote response request event to be processed.
     **/
    virtual void ProcessRemoteEvent( CERemoteResponseEvent & responseEvent );

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
    inline void setConnectionState( CEClientService::eConnectionState newState );
    /**
     * \brief   Returns current client socket connection state.
     **/
    inline CEClientService::eConnectionState getConnectionState( void ) const;
    /**
     * \brief   Returns instance of client servicing object.
     **/
    inline CEClientService & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Client connection object
     **/
    CEClientConnection          mClientConnection;
    /**
     * \brief   Instance of remote servicing consumer to handle message.
     **/
    IERemoteServiceConsumer &   mServiceConsumer;
    /**
     * \brief   Connection retry timer object.
     **/
    CETimer                     mTimerConnect;
    /**
     * \brief   Message receiver thread
     **/
    CEClientReceiveThread       mThreadReceive;
    /**
     * \brief   Message sender thread
     **/
    CEClientSendThread          mThreadSend;
    /**
     * \brief   Flag, indicates whether the remote servicing is enabled or not.
     **/
    bool                        mIsServiceEnabled;
    /**
     * \brief   The path of configuration file name.
     **/
    CEString                    mConfigFile;
    /**
     * \brief   The connection channel.
     **/
    CEChannel                   mChannel;
    /**
     * \brief   The sate of connection
     **/
    eConnectionState            mConnectionState;
    /**
     * \brief   Data access synchronization object
     **/
    mutable CECriticalSection   mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEClientService( void );
    CEClientService( const CEClientService & );
    const CEClientService & operator = ( const CEClientService & );
};

//////////////////////////////////////////////////////////////////////////
// CEClientService class inline methods implementation
//////////////////////////////////////////////////////////////////////////

inline CEClientService & CEClientService::self( void )
{   return (*this);                         }

inline ITEM_ID CEClientService::GetConnectionCookie( void ) const
{   return mClientConnection.GetCookie();   }

#endif  // AREG_IPC_CECLIENTSERVICE_HPP
