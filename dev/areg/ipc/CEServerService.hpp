#ifndef AREG_IPC_CESERVERSERVICE_HPP
#define AREG_IPC_CESERVERSERVICE_HPP

/************************************************************************
 * \file        areg/ipc/CEServerService.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Connection server declaration
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/ipc/IERemoteService.hpp"
#include "areg/component/CETimerEvent.hpp"
#include "areg/component/CEDispatcherThread.hpp"
#include "areg/ipc/IERemoteServiceConsumer.hpp"
#include "areg/ipc/private/CEServerServiceEvent.hpp"
#include "areg/ipc/private/IERemoteServiceHandler.hpp"
#include "areg/ipc/private/IEServerConnectionHandler.hpp"

#include "areg/ipc/private/CEServerReceiveThread.hpp"
#include "areg/ipc/private/CEServerConnection.hpp"
#include "areg/ipc/private/CEServerSendThread.hpp"
#include "areg/ipc/private/CEServiceRegistry.hpp"
#include "areg/ipc/NERemoteService.hpp"
#include "areg/base/ESynchObjects.hpp"
#include "areg/base/TEArrayList.hpp"
#include "areg/base/EContainers.hpp"
#include "areg/component/CETimer.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IERemoteServiceConsumer;

//////////////////////////////////////////////////////////////////////////
// CEServerService class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The server side connection service. Used by message broker to
 *          accept application connections.
 **/
class AREG_API CEServerService  : public    IERemoteService
                                , private   CEDispatcherThread
                                , private   IEServerConnectionHandler
                                , private   IERemoteServiceConsumer
                                , private   IERemoteServiceHandler
                                , private   IEServerServiceEventConsumer
                                , private   IETimerConsumer
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The defaul connection type of server service.
     **/
    static const NERemoteService::eServiceConnection   CONNECT_TYPE /*= NERemoteService::ConnectionTcpip*/;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor to initialize default values.
     **/
    CEServerService( void );
    /**
     * \brief   Destructor
     **/
    virtual ~CEServerService( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Checks whether the specified host address is in the white-list.
     * \param   addrClient  The object, which contains client host address to check.
     * \return  Returns true if specified host address is in the white-list.
     **/
    inline bool IsAddressInWhiteList( const NESocket::CEInterlockedValue & addrClient ) const;

    /**
     * \brief   Checks whether the specified host address is in the black-list.
     * \param   addrClient  The object, which contains client host address to check.
     * \return  Returns true if specified host address is in the black-list.
     **/
    inline bool IsAddressInBlackList( const NESocket::CEInterlockedValue & addrClient ) const;

    /**
     * \brief   Adds specified host address to the white-list.
     * \param   addrClient  The host address of client to add to list.
     **/
    inline void AddWhiteList( const NESocket::CEInterlockedValue & addrClient );

    /**
     * \brief   Adds specified host address to black-list.
     * \param   addrClient  The host address of client to add to list.
     **/
    void AddBlackList( const NESocket::CEInterlockedValue & addrClient );

    /**
     * \brief   Removes specified host address from white-list.
     * \param   addrClient  The host address of client to remove from list.
     **/
    inline void RemoveWhiteList( const NESocket::CEInterlockedValue & addrClient );

    /**
     * \brief   Removes specified host address from black-list.
     * \param   addrClient  The host address of client to remove from list.
     **/
    inline void RemoveBlackList( const NESocket::CEInterlockedValue & addrClient );

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
     * \brief   Call manually to set service broker host name and port number.
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

protected:
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
    virtual bool CanAcceptConnection( const CESocketAccepted & clientSocket );

    /**
     * \brief   Triggered, when lost connection with client.
     *          Passed clientSocket parameter specifies client socket, which lost connection.
     * \param   clientSocket    Client socket object, which lost connection.
     **/
    virtual void ConnectionLost( CESocketAccepted & clientSocket );

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
    virtual void ProcessEvent( const CEServerServiceEventData & data );

/************************************************************************/
// IERemoteServiceConsumer
/************************************************************************/

    /**
     * \brief   Call to receive list of registered remote stub server services, created in current module. 
     *          Called when establishing connection with broker and the system needs 
     *          to send registration messages. On output out_listStubs
     *          contains list of remote stub server services in module.
     * \param   out_listStubs       On output, this contains list of remote stubs, created in current module.
     * \return  Returns size of remote stub server services in the list.
     **/
    virtual void GetRemoteServiceList( TEArrayList<CEStubAddress, const CEStubAddress &> & out_listStubs) const;

    /**
     * \brief   Call to receive list of registered remote proxy services, created in current module. 
     *          Called when establishing connection with broker and the system needs 
     *          to send registration messages. On output out_lisProxies
     *          contains list of remote proxy client services in module.
     * \param   out_lisProxies      On output, this contains list of remote proxies, created in current module.
     * \return  Returns size of remote client proxy services in the list.
     **/
    virtual void GetRemoteServiceList( TEArrayList<CEProxyAddress, const CEProxyAddress &> & out_lisProxies) const;

    /**
     * \brief   Call to receive list of registered remote stub service, which connection cookie is equal to 
     *          specified cookie value. In output out_listStubs will contain list of remote stub addresses.
     * \param   cookie          The cookie value to check
     * \param   out_listStubs   On output this will contain list of remote stub addresses, which cookie is
     *                          equal to specified cookie value.
     **/
    virtual void GetServiceList( ITEM_ID cookie, TEArrayList<CEStubAddress, const CEStubAddress &> out_listStubs ) const;

    /**
     * \brief   Call to receive list of registered remote proxy service, which connection cookie is equal to 
     *          specified cookie value. In output out_lisProxies will contain list of remote proxy addresses.
     * \param   cookie          The cookie value to check
     * \param   out_lisProxies  On output this will contain list of remote proxy addresses, which cookie is
     *                          equal to specified cookie value.
     **/
    virtual void GetServiceList( ITEM_ID cookie, TEArrayList<CEProxyAddress, const CEProxyAddress &> out_lisProxies ) const;

    /**
     * \brief   Registers remote stub in the current process.
     * \param   stub    The address of remote stub server to register
     **/
    virtual void RegisterRemoteStub( const CEStubAddress & stub );

    /**
     * \brief   Registers remote proxy in the current process.
     * \param   proxy   The address of remote proxy client to register
     **/
    virtual void RegisterRemoteProxy( const CEProxyAddress & proxy );

    /**
     * \brief   Unregisters remote stub in the current process.
     * \param   stub    The address of remote stub server to unregister
     **/
    virtual void UnregisterRemoteStub( const CEStubAddress & stub );

    /**
     * \brief   Unregisters remote proxy in the current process.
     * \param   proxy   The address of remote proxy client to unregister
     **/
    virtual void UnregisterRemoteProxy( const CEProxyAddress & proxy );

    /**
     * \brief   Triggered when remote service has been started and there is a
     *          connection established with service.
     * \param   channel     The connection channel of remote service broker.
     **/
    virtual void RemoteServiceStarted( const CEChannel & channel );

    /**
     * \brief   Triggered when connection with remote service has been stopped.
     * \param   channel     The connection channel of remote service broker.
     **/
    virtual void RemoteServiceStopped( const CEChannel & channel );

    /**
     * \brief   Triggered when connection with remote service broker is lost.
     *          The connection is considered lost if it not possible to read or
     *          receive data, and there was not stop connection triggered.
     * \param   channel     The connection channel of remote service broker.
     **/
    virtual void RemoveServiceLostConnection( const CEChannel & channel );

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
    bool canAcceptConnection( const NESocket::CEInterlockedValue & addrConnect ) const;

    /**
     * \brief   Called to start connection procedure to accept client connections.
     * \return  Returns true if could start connection.
     **/
    bool startConnection( void );

    /**
     * \brief   Called to stop connection. All clients are automatically disconnected.
     **/
    void stopConnection( void );

    /**
     * \brief   Returns instance of object. For internal use only.
     **/
    inline CEServerService & self( void );

//////////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////////
private:
    CEServerConnection          mServerConnection;      //!< The instance of server connection object.
    CETimer                     mTimerConnect;          //!< The timer object to trigger in case if failed to create server socket.
    CEServerSendThread          mThreadSend;            //!< The thread to send messages to clients
    CEServerReceiveThread       mThreadReceive;         //!< The thread to receive messages from clients
    CEServiceRegistry           mServiceRegistry;       //!< The service registry map to track stub-proxy connections
    bool                        mIsServiceEnabled;      //!< The flag indicating whether the server servicing is enabled or not.
    CEString                    mConfigFile;            //!< The full path of connection configuration file.
    CEStringArray               mWhiteList;             //!< The list of enabled fixed client hosts.
    CEStringArray               mBlackList;             //!< The list of disabled fixes client hosts.
    mutable CECriticalSection   mLock;                  //!< The synchronization object to be accessed from different threads.

//////////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////////
private:
    CEServerService( const CEServerService & /*src*/ );
    const CEServerService & operator = ( const CEServerService & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////////
// CEServerService class inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline CEServerService & CEServerService::self( void )
{   return (*this);         }

inline bool CEServerService::IsAddressInWhiteList(const NESocket::CEInterlockedValue & addrClient) const
{
    return mWhiteList.FindElement(addrClient.GetHostAddress()) != -1;
}

inline bool CEServerService::IsAddressInBlackList(const NESocket::CEInterlockedValue & addrClient) const
{
    return mBlackList.FindElement(addrClient.GetHostAddress()) != -1;
}

inline void CEServerService::AddWhiteList(const NESocket::CEInterlockedValue & addrClient)
{
    if ( mWhiteList.FindElement(addrClient.GetHostAddress()) == -1 )
        mWhiteList.Add(addrClient.GetHostAddress());
}

inline void CEServerService::AddBlackList(const NESocket::CEInterlockedValue & addrClient)
{
    if ( mBlackList.FindElement(addrClient.GetHostAddress()) == -1 )
        mBlackList.Add(addrClient.GetHostAddress());
}

inline void CEServerService::RemoveWhiteList(const NESocket::CEInterlockedValue & addrClient)
{
    mWhiteList.RemoveElement( addrClient.GetHostAddress(), 0);
}

inline void CEServerService::RemoveBlackList(const NESocket::CEInterlockedValue & addrClient)
{
    mBlackList.RemoveElement( addrClient.GetHostAddress(), 0);
}

#endif  // AREG_IPC_CESERVERSERVICE_HPP
