#ifndef AREG_COMPONENT_PRIVATE_CESERVICEMANAGER_HPP
#define AREG_COMPONENT_PRIVATE_CESERVICEMANAGER_HPP
/************************************************************************
 * \file        areg/component/private/CEServiceManager.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Service Manager declaration.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/component/CEDispatcherThread.hpp"
#include "areg/component/private/CEServiceManagerEvents.hpp"

#include "areg/base/ESynchObjects.hpp"
#include "areg/component/private/CEServerList.hpp"
#include "areg/ipc/CEClientService.hpp"
#include "areg/ipc/IERemoteServiceConsumer.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEStubAddress;
class CEProxyAddress;
class CEServiceRequestEvent;
class CEServiceResponseEvent;

//////////////////////////////////////////////////////////////////////////
// CEServiceManager class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Service Manager is a singleton module instantiated
 *          on System Startup and shutting down on System Shutdown.
 *          The Service Manager is running in separate Dispatcher Thread,
 *          which is processing Servicing Events. The Service Manager
 *          Events are sent by Stub Server or by Proxy client objects
 *          every time when Stub and/or Proxy are connected and/or disconnected.
 *          The Service Manager is a module responsible to provide
 *          handshake between Server and connected Client components.
 *          Every Proxy client object and every Stub server of Service Interface 
 *          on startup are requesting registration at Service Manager.
 *          When the requested Stub of Server Interface is available in
 *          the System, Service Manager is sending connection Events
 *          to both objects, providing connection address information.
 *          Once Proxy client received connection available message from
 *          Service Manager, Proxy will know the target Stub address and
 *          can send request to be processed on Stub server instance.
 *          When Stub Server is shutting down, Service Manager receives
 *          appropriate information from Server side and sends all relevant
 *          Proxy clients server disconnected message to stop sending requests.
 *          Similar notification messages are received by Stub server object
 *          every time when Proxy client is connecting or disconnecting
 *          to the Service.
 *          Service Manager as well is starting the Timer Service, which provides
 *          possibility to start and stop timers, and the Service Manager shuts down
 *          the Timer Service when the application is shutting down the Service Manager.
 *          The Service Manager start and stop are managed from
 *          Component Loader module, when the system is requested
 *          to load and run, or shutdown and stop registered components.
 *          Service Manager is starting before first Component Thread
 *          is started and shutdown, when last Component Thread is stopped.
 **/
class CEServiceManager  : private   CEDispatcherThread
                        , private   IEServiceManagerEventConsumer
                        , private   IERemoteServiceConsumer
{
    friend class CEApplication;
//////////////////////////////////////////////////////////////////////////
// Declare Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME(CEServiceManager)

//////////////////////////////////////////////////////////////////////////
// Predefined constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   CEServiceManager::SERVICE_MANAGER_THREAD_NAME
     *          Predefined Service Manager Thread name.
     **/
    static const char* const    SERVICE_MANAGER_THREAD_NAME /*= "_AREG_SERVICE_MANAGER_THREAD_"*/;

//////////////////////////////////////////////////////////////////////////
// Static public methods to access globally
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// Service Manager Server and Client registration functions
/************************************************************************/

    /**
     * \brief   Static method to be called globally.
     *          The function is called when Stub Server is starting up
     *          and requests registration at Service Manager module
     *          by specifying the address of available Stub of Service Interface.
     *          The function will generate appropriate Event and send 
     *          registration request to Service Manager. If connection and 
     *          registration succeeded, the Stub server will receive notification
     *          and all Proxy clients will receive connection available messages
     *          to start sending requests and assign for Attribute Update Notifications.
     * \param   whichServer     The address of Stub Server object, which has been
     *                          started and requesting registration at Service Manager Module.
     **/
    static void RequestRegisterServer( const CEStubAddress & whichServer );

    /**
     * \brief   Static method to be called globally.
     *          The function is called when Stub Server is shutting down
     *          and requests to unregister at Service Manager module
     *          by specifying the address of Stub of Service Interface.
     *          The function will generate appropriate Event and send 
     *          unregister request to Service Manager. All Proxy clients 
     *          will receive appropriate disconnect messages to stop
     *          communication with Stub Service Interface.
     * \param   whichServer     The address of Stub Server object, which has been
     **/
    static void RequestUnregisterServer( const CEStubAddress & whichServer );

    /**
     * \brief   Static method to be called globally.
     *          The function is called when new Proxy client is start up,
     *          and requests registration at Service Manager module
     *          by specifying the address of Proxy of Service Interface.
     *          The function will generate appropriate Event and send 
     *          registration request to Service Manager. If connection and 
     *          registration succeeded, and the Stub server of implemented
     *          Service Interface is available in the system, the Proxy will 
     *          receive connection available notification message, containing
     *          available Stub address of implemented Service Interface.
     * \param   whichClient     The address of Proxy client object, which has been
     *                          started and requesting registration at Service Manager Module.
     **/
    static void RequestRegisterClient( const CEProxyAddress & whichClient );

    /**
     * \brief   Static method to be called globally.
     *          The function is called when Proxy is shutting down,
     *          and requests to unregister at Service Manager module
     *          by specifying the address of Proxy of Service Interface.
     *          The function will generate appropriate Event and send 
     *          unregister request to Service Manager. After Proxy client
     *          is unregistered, the Stub server will get notification
     *          of disconnected client.
     * \param   whichClient     The address of Proxy client object, which is
     *                          unregistering at Service Manager Module.
     **/
    static void RequestUnregisterClient( const CEProxyAddress & whichClient );

    /**
     * \brief   The function returns true, if Service Manager is running and ready to
     *          process Service Manager Events. Otherwise, it returns false.
     **/
    static bool IsServiceManagerStarted( void );

/************************************************************************/
// Service Manager start / stop functions
/************************************************************************/
private:
    /**
     * \brief   This function called, when Service Manager should be started.
     *          If Service Manager already started, the function will be ignored.
     *          The function is called from Component Loader module before
     *          it starts first Component Thread.
     *          The function returns true, if Service Manager thread started with
     *          success and the Service Manager is ready to receive Events.
     * \return  Returns true, if Service Manager started or already running and
     *          is ready to receive Events.
     **/
    static bool StartServiceManager( void );

    /**
     * \brief   This function called, when Service Manager should stop activities.
     *          The function is called from application manager when all
     *          Component Threads are stopped and completed completed.
     **/
    static void StopServiceManager( void );

/************************************************************************/
// Broker client start / stop functions
/************************************************************************/
private:

    /**
     * \brief   Call to configure broker client. If passed NULL, it will use
     *          default broker configuration file. This call will not start client
     *          automatically. To start broker client, call BrokerClientStart()
     *          manually.
     * \param   configFile  The configuration file of broker client.
     *                      If NULL, it will load default configuration file.
     * \return  Returns true if succeeded to load configuration file.
     *          Otherwise, it returns false.
     **/
    static bool RoutingServiceClientConfigure( const char * configFile = NULL );

    /**
     * \brief   Call to start the client part of remove Routing Service.
     * \param   configFile  If not NULL, the broker will be first configured.
     *                      If NULL and broker was not configured, it will use
     *                      default configuration file.
     *                      If NULL and broker was configured, it will ignore configuration.
     * \return  Returns true if succeeded to start broker client.
     **/
    static bool RoutingServiceClientStart( const char * configFile = NULL );


    /**
     * \brief   Call to start connection to remote Routing Service. If called, it overwrites IP-Address and
     *          port number set via configuration file.
     * \param   ipAddress   Should be valid IP-Address of remote Routing Service.
     * \param   portNr      Should be valid Port Number of remote Routing Service.
     * \return  Returns true if Broker successfully started.
     **/
    static bool RoutingServiceClientStart( const char * ipAddress, unsigned short portNr );

    /**
     * \brief   Call to stop broker client.
     **/
    static void RoutingServiceClientStop( void );

    /**
     * \brief   Call to enable or disable remote Routing Service client.
     *          If client is already running and it is requested to disable, it will be stopped first.
     * \param   enable  Flag, which is indicating whether the remote Routing Service client should be enabled or disabled.
     *                  If true, it is enabled. Otherwise, it is disabled.
     **/
    static void RoutingServiceClientEnable( bool enable );

    /**
     * \brief   Returns true if Routing Service client is started and ready to operate.
     **/
    static bool IsRoutingServiceClientStarted( void );

    /**
     * \brief   Returns true if Routing Service client is configured and ready to start.
     **/
    static bool IsRoutingServiceClientConfigured( void );

    /**
     * \brief   Returns true if remote Routing Service is enabled.
     **/
    static bool IsRoutingServiceEnabled( void );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
private:

/************************************************************************/
// IEBrokerServerEventConsumer overrides
/************************************************************************/

    /**
     * \brief   Automatically triggered when event is dispatched by registered
     *          worker / component thread.
     * \param   data    The data object passed in event. It should have at least
     *                  default constructor and assigning operator.
     *                  This object is not used for IPC.
     **/
    virtual void ProcessEvent( const CEServiceManagerEventData & data );

/************************************************************************/
// IEEventRouter Interface overrides.
/************************************************************************/

    /**
     * \brief	Posts event and delivers to its target thread / process.
     * \param	eventElem	Event object to post.
     * \return	Returns true if target was found and the event
     *          delivered with success. Otherwise it returns false.
     **/
    virtual bool PostEvent( CEEvent & eventElem );

    /**
     * \brief	Triggered when dispatcher starts running. 
     *          In this function runs main dispatching loop.
     *          Events are picked and dispatched here.
     *          Override if logic should be changed.
     * \return	Returns true if Exit Event is signaled.
     **/
    virtual bool RunDispatcher( void );

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
     * \param   channel     The connection channel of remote Routing Service.
     **/
    virtual void RemoteServiceStarted( const CEChannel & channel );

    /**
     * \brief   Triggered when connection with remote service has been stopped.
     * \param   channel     The connection channel of remote Routing Service.
     **/
    virtual void RemoteServiceStopped( const CEChannel & channel );

    /**
     * \brief   Triggered when connection with remote Routing Service is lost.
     *          The connection is considered lost if it not possible to read or
     *          receive data, and there was not stop connection triggered.
     * \param   channel     The connection channel of remote Routing Service.
     **/
    virtual void RemoveServiceLostConnection( const CEChannel & channel );

//////////////////////////////////////////////////////////////////////////
// Private static methods
//////////////////////////////////////////////////////////////////////////
private:
    static CEServiceManager & GetServiceManager( void );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default Constructor.
     *          Protected and cannot be accessed globally.
     **/
    CEServiceManager( void );

    /**
     * \brief   Destructor
     **/
    ~CEServiceManager( void );

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The function is called when Stub Server is starting up
     *          and requests registration at Service Manager module
     *          by specifying the address of available Stub of Service Interface.
     *          The function will generate appropriate Event and send 
     *          registration request to Service Manager. If connection and 
     *          registration succeeded, the Stub server will receive notification
     *          and all Proxy clients will receive connection available messages
     *          to start sending requests and assign for Attribute Update Notifications.
     * \param   whichServer     The address of Stub Server object, which has been
     *                          started and requesting registration at Service Manager Module.
     **/
    void RegisterServer( const CEStubAddress & whichServer );

    /**
     * \brief   The function is called when Stub Server is shutting down
     *          and requests to unregister at Service Manager module
     *          by specifying the address of Stub of Service Interface.
     *          The function will generate appropriate Event and send 
     *          unregister request to Service Manager. All Proxy clients 
     *          will receive appropriate disconnect messages to stop
     *          communication with Stub Service Interface.
     * \param   whichServer     The address of Stub Server object, which has been
     *                          started and requesting registration at Service Manager Module.
     **/
    void UnregisterServer( const CEStubAddress & whichServer );

    /**
     * \brief   The function is called when new Proxy client is start up,
     *          and requests registration at Service Manager module
     *          by specifying the address of Proxy of Service Interface.
     *          The function will generate appropriate Event and send 
     *          registration request to Service Manager. If connection and 
     *          registration succeeded, and the Stub server of implemented
     *          Service Interface is available in the system, the Proxy will 
     *          receive connection available notification message, containing
     *          available Stub address of implemented Service Interface.
     * \param   whichClient     The address of Proxy client object, which has been
     *                          started and requesting registration at Service Manager Module.
     **/
    void RegisterClient( const CEProxyAddress & whichClient );

    /**
     * \brief   The function is called when Proxy is shutting down,
     *          and requests to unregister at Service Manager module
     *          by specifying the address of Proxy of Service Interface.
     *          The function will generate appropriate Event and send 
     *          unregister request to Service Manager. After Proxy client
     *          is unregistered, the Stub server will get notification
     *          of disconnected client.
     * \param   whichClient     The address of Proxy client object, which is
     *                          unregistering at Service Manager Module.
     **/
    void UnregisterClient( const CEProxyAddress & whichClient );

    /**
     * \brief   Sends predefined Service Manager Event, notifying Proxy client 
     *          that connection with Stub server is available and established.
     *          When Proxy receives Event, it can start sending requests to 
     *          the Stub of implemented Service Interface.
     * \param   client      The Client Info object containing all required information
     *                      to send Event. The target of Event is a Proxy address,
     *                      specified in the Client Info object.
     **/
    void SendClientConnectedEvent( const CEClientInfo & client, const CEStubAddress & server ) const;

    /**
     * \brief   Sends predefined Service Manager event to notify that Proxy client 
     *          disconnected service.
     *          When Proxy receives Event, it should stop sending requests to 
     *          the Stub of implemented Service Interface.
     * \param   client  The Client Info object containing all required information
     *                  to send Event. The target of Event is a Proxy address,
     *                  specified in the Client Info object.
     **/
    void SendClientDisconnectedEvent(const CEClientInfo & client, const CEStubAddress & server) const;

    /**
     * \brief   Starts Service Manager Thread. If Thread is started, the Timer Server
     *          will automatically start as well.
     * \return  Returns true if Service Manager Thread started and ready to receive Events.
     **/
    bool StartServiceManagerThread( void );

    /**
     * \brief   Starts Service Manager Thread. This call will stop Timer Server, all
     *          pending server and client Service Interface will receive disconnect
     *          notification, and the Service Manager Thread will stop and complete job.
     **/
    void StopServiceManagerThread( void );

    /**
     * \brief   Returns reference to CEServiceManager object
     **/
    inline CEServiceManager & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The Map of Server Info object as a Key and Client Info List as Values
     **/
    CEServerList        mServerList;

    /**
     * \brief           The connection service.
     **/
    CEClientService     mConnectService;

    /**
     * \brief   Synchronization object, for multi-threading access.
     **/
    mutable CEResourceLock   mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    CEServiceManager( const CEServiceManager & /*src*/ );
    const CEServiceManager & operator = ( const CEServiceManager & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEServiceManager class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CEServiceManager & CEServiceManager::self( void )
{   return (*this);     }

#endif  // AREG_COMPONENT_PRIVATE_CESERVICEMANAGER_HPP
