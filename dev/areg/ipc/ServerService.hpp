#ifndef AREG_IPC_SERVERSERVICE_HPP
#define AREG_IPC_SERVERSERVICE_HPP

/************************************************************************
 * \file        areg/ipc/ServerService.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Connection server declaration
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/ipc/IERemoteService.hpp"
#include "areg/component/IETimerConsumer.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/ipc/IERemoteServiceConsumer.hpp"
#include "areg/ipc/private/ServerServiceEvent.hpp"
#include "areg/ipc/private/IERemoteServiceHandler.hpp"
#include "areg/ipc/private/IEServerConnectionHandler.hpp"

#include "areg/ipc/private/ServerReceiveThread.hpp"
#include "areg/ipc/private/ServerConnection.hpp"
#include "areg/ipc/private/ServerSendThread.hpp"
#include "areg/ipc/private/ServiceRegistry.hpp"
#include "areg/ipc/NERemoteService.hpp"
#include "areg/base/ESynchObjects.hpp"
#include "areg/base/TEArrayList.hpp"
#include "areg/base/EContainers.hpp"
#include "areg/component/Timer.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IERemoteServiceConsumer;

//////////////////////////////////////////////////////////////////////////
// ServerService class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The server side connection service. Used by message broker to
 *          accept application connections.
 **/
class AREG_API ServerService    : public    IERemoteService
                                , private   DispatcherThread
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
     * \brief   The default connection type of server service.
     **/
    static const NERemoteService::eServiceConnection   CONNECT_TYPE /*= NERemoteService::ConnectionTcpip*/;

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
    virtual ~ServerService( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Checks whether the specified host address is in the white-list.
     * \param   addrClient  The object, which contains client host address to check.
     * \return  Returns true if specified host address is in the white-list.
     **/
    inline bool isAddressInWhiteList( const NESocket::InterlockedValue & addrClient ) const;

    /**
     * \brief   Checks whether the specified host address is in the black-list.
     * \param   addrClient  The object, which contains client host address to check.
     * \return  Returns true if specified host address is in the black-list.
     **/
    inline bool isAddressInBlackList( const NESocket::InterlockedValue & addrClient ) const;

    /**
     * \brief   Adds specified host address to the white-list.
     * \param   addrClient  The host address of client to add to list.
     **/
    inline void addWhiteList( const NESocket::InterlockedValue & addrClient );

    /**
     * \brief   Adds specified host address to black-list.
     * \param   addrClient  The host address of client to add to list.
     **/
    void addBlackList( const NESocket::InterlockedValue & addrClient );

    /**
     * \brief   Removes specified host address from white-list.
     * \param   addrClient  The host address of client to remove from list.
     **/
    inline void removeWhiteList( const NESocket::InterlockedValue & addrClient );

    /**
     * \brief   Removes specified host address from black-list.
     * \param   addrClient  The host address of client to remove from list.
     **/
    inline void removeBlackList( const NESocket::InterlockedValue & addrClient );

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
     * \brief   Call manually to set service broker host name and port number.
     *          Note, if remote service is already started, this call will change
     *          data, but will not restart service.
     * \param   hostName    IP-address or name of service broker host.
     * \param   portNr      Port number of service broker service.
     **/
    virtual void setRemoteServiceAddress( const char * hostName, unsigned short portNr );

    /**
     * \brief   Call to start remote service. The host name and port number should be already set.
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

protected:
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
    virtual bool canAcceptConnection( const SocketAccepted & clientSocket );

    /**
     * \brief   Triggered, when lost connection with client.
     *          Passed clientSocket parameter specifies client socket, which lost connection.
     * \param   clientSocket    Client socket object, which lost connection.
     **/
    virtual void connectionLost( SocketAccepted & clientSocket );

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
    virtual void processEvent( const ServerServiceEventData & data );

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
    virtual void getRemoteServiceList( TEArrayList<StubAddress, const StubAddress &> & out_listStubs) const;

    /**
     * \brief   Call to receive list of registered remote proxy services, created in current module. 
     *          Called when establishing connection with broker and the system needs 
     *          to send registration messages. On output out_lisProxies
     *          contains list of remote proxy client services in module.
     * \param   out_lisProxies      On output, this contains list of remote proxies, created in current module.
     * \return  Returns size of remote client proxy services in the list.
     **/
    virtual void getRemoteServiceList( TEArrayList<ProxyAddress, const ProxyAddress &> & out_lisProxies) const;

    /**
     * \brief   Call to receive list of registered remote stub service, which connection cookie is equal to 
     *          specified cookie value. In output out_listStubs will contain list of remote stub addresses.
     * \param   cookie          The cookie value to check
     * \param   out_listStubs   On output this will contain list of remote stub addresses, which cookie is
     *                          equal to specified cookie value.
     **/
    virtual void getServiceList( ITEM_ID cookie, TEArrayList<StubAddress, const StubAddress &> out_listStubs ) const;

    /**
     * \brief   Call to receive list of registered remote proxy service, which connection cookie is equal to 
     *          specified cookie value. In output out_lisProxies will contain list of remote proxy addresses.
     * \param   cookie          The cookie value to check
     * \param   out_lisProxies  On output this will contain list of remote proxy addresses, which cookie is
     *                          equal to specified cookie value.
     **/
    virtual void getServiceList( ITEM_ID cookie, TEArrayList<ProxyAddress, const ProxyAddress &> out_lisProxies ) const;

    /**
     * \brief   Registers remote stub in the current process.
     * \param   stub    The address of remote stub server to register
     **/
    virtual void registerRemoteStub( const StubAddress & stub );

    /**
     * \brief   Registers remote proxy in the current process.
     * \param   proxy   The address of remote proxy client to register
     **/
    virtual void registerRemoteProxy( const ProxyAddress & proxy );

    /**
     * \brief   Unregisters remote stub in the current process.
     * \param   stub    The address of remote stub server to unregister
     **/
    virtual void unregisterRemoteStub( const StubAddress & stub );

    /**
     * \brief   Unregisters remote proxy in the current process.
     * \param   proxy   The address of remote proxy client to unregister
     **/
    virtual void unregisterRemoteProxy( const ProxyAddress & proxy );

    /**
     * \brief   Triggered when remote service has been started and there is a
     *          connection established with service.
     * \param   channel     The connection channel of remote service broker.
     **/
    virtual void remoteServiceStarted( const Channel & channel );

    /**
     * \brief   Triggered when connection with remote service has been stopped.
     * \param   channel     The connection channel of remote service broker.
     **/
    virtual void remoteServiceStopped( const Channel & channel );

    /**
     * \brief   Triggered when connection with remote service broker is lost.
     *          The connection is considered lost if it not possible to read or
     *          receive data, and there was not stop connection triggered.
     * \param   channel     The connection channel of remote service broker.
     **/
    virtual void removeServiceLostConnection( const Channel & channel );

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
    bool canAcceptConnection( const NESocket::InterlockedValue & addrConnect ) const;

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
    inline ServerService & self( void );

//////////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////////
private:
    ServerConnection            mServerConnection;      //!< The instance of server connection object.
    Timer                       mTimerConnect;          //!< The timer object to trigger in case if failed to create server socket.
    ServerSendThread            mThreadSend;            //!< The thread to send messages to clients
    ServerReceiveThread         mThreadReceive;         //!< The thread to receive messages from clients
    ServiceRegistry             mServiceRegistry;       //!< The service registry map to track stub-proxy connections
    bool                        mIsServiceEnabled;      //!< The flag indicating whether the server servicing is enabled or not.
    String                      mConfigFile;            //!< The full path of connection configuration file.
    StringArray                 mWhiteList;             //!< The list of enabled fixed client hosts.
    StringArray                 mBlackList;             //!< The list of disabled fixes client hosts.
    mutable CriticalSection     mLock;                  //!< The synchronization object to be accessed from different threads.

//////////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////////
private:
    ServerService( const ServerService & /*src*/ );
    const ServerService & operator = ( const ServerService & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////////
// ServerService class inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline ServerService & ServerService::self( void )
{
    return (*this);
}

inline bool ServerService::isAddressInWhiteList(const NESocket::InterlockedValue & addrClient) const
{
    return mWhiteList.find(addrClient.getHostAddress()) != -1;
}

inline bool ServerService::isAddressInBlackList(const NESocket::InterlockedValue & addrClient) const
{
    return mBlackList.find(addrClient.getHostAddress()) != -1;
}

inline void ServerService::addWhiteList(const NESocket::InterlockedValue & addrClient)
{
    if ( mWhiteList.find(addrClient.getHostAddress()) == -1 )
    {
        mWhiteList.add(addrClient.getHostAddress());
    }
}

inline void ServerService::addBlackList(const NESocket::InterlockedValue & addrClient)
{
    if ( mBlackList.find(addrClient.getHostAddress()) == -1 )
    {
        mBlackList.add(addrClient.getHostAddress());
    }
}

inline void ServerService::removeWhiteList(const NESocket::InterlockedValue & addrClient)
{
    mWhiteList.remove( addrClient.getHostAddress(), 0);
}

inline void ServerService::removeBlackList(const NESocket::InterlockedValue & addrClient)
{
    mBlackList.remove( addrClient.getHostAddress(), 0);
}

#endif  // AREG_IPC_SERVERSERVICE_HPP
