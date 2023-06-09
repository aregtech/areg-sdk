#ifndef AREG_COMPONENT_PRIVATE_SERVICEMANAGER_HPP
#define AREG_COMPONENT_PRIVATE_SERVICEMANAGER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceManager.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service Manager declaration.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/component/DispatcherThread.hpp"
#include "areg/component/private/ServiceManagerEvents.hpp"

#include "areg/base/SynchObjects.hpp"
#include "areg/component/private/ServerList.hpp"
#include "areg/ipc/private/ClientService.hpp"
#include "areg/ipc/IERemoteServiceConsumer.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class StubAddress;
class ProxyAddress;
class ServiceRequestEvent;
class ServiceResponseEvent;

//////////////////////////////////////////////////////////////////////////
// ServiceManager class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Service Manager is a singleton module, which should be instantiated
 *          and run on System Startup. The service manager is responsible
 *          for service registration and automated service discovery.
 * 
 *          All system proxies and servers are automatically registered at
 *          Service Manager. As soon as a service server is available, the
 *          Service Manager generates appropriate events and automatically
 *          sends notifications to proxies to notify service availability, 
 *          and sends connection event to notify servers new client connection.
 *          When server or client is disconnected, Service Manager generates
 *          appropriate event to notify disconnect event. The proxies can send
 *          requests and subscribe to data update as long as the server remains
 *          connected. No message is sent to the server if it is not available yet.
 *          So that, each proxy / client first should wait for connected event, then
 *          start triggering tasks on the server side.
 * 
 *          Service Manager as well is starting the Timer Service, which provides
 *          possibility to start and stop timers.
 * 
 *          To start application Service Manager, use Application::initialize() method,
 *          which should be called before any model is loaded.
 **/
class ServiceManager    : private   DispatcherThread
                        , private   IEServiceManagerEventConsumer
                        , private   IERemoteServiceConsumer
{
    friend class Application;
//////////////////////////////////////////////////////////////////////////
// Declare Runtime
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME(ServiceManager)

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
    static void requestRegisterServer( const StubAddress & whichServer );

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
    static void requestUnregisterServer( const StubAddress & whichServer );

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
    static void requestRegisterClient( const ProxyAddress & whichClient );

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
    static void requestUnregisterClient( const ProxyAddress & whichClient );

    /**
     * \brief   Static method to be called globally.
     *          The function is called if the component thread should be terminated and
     *          restarted again. This will terminate all worker threads, components, service,
     *          and the proxies of the component thread. Delete and re-create and restart the
     *          component thread, so that it can restart again.
     * 
     * \param   whichThread     The instance of valid component thread.
     */
    static void requestRecreateThread( const ComponentThread & whichThread );

    /**
     * \brief   The function returns true, if Service Manager is running and ready to
     *          process Service Manager Events. Otherwise, it returns false.
     **/
    static bool isServiceManagerStarted( void );

    /**
     * \brief   Call to query the amount of send and receive data size in bytes.
     *          The call extracts the send and receive sizes, and resets them to zero.
     *          On output 'sizeSend' and 'sizeReceive' parameters contain the size
     *          since the last call of the method.
     *          If need to measure the total amount of data, accumulate calls.
     *          If need to measure the data rate per second, call this method each second.
     * 
     * \param   sizeSend    On output this parameter contains the size of data in bytes
     *                      sent since the last call of the method.
     * \param   sizeReceive On output this parameter contains the size of data in bytes
     *                      received since the last call of the method.
     **/
    static void queryCommunicationData( unsigned int & OUT sizeSend, unsigned int & OUT sizeReceive );

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
    static bool _startServiceManager( void );

    /**
     * \brief   This function called, when Service Manager should stop activities.
     *          The function is called from application manager when all
     *          Component Threads are stopped and completed.
     **/
    static void _stopServiceManager( void );

/************************************************************************/
// Message router client start / stop functions
/************************************************************************/

    /**
     * \brief   Call to configure router client. If passed nullptr, it will use
     *          default router configuration file. This call will not start client
     *          automatically. To start router client, call _routingServiceStart()
     *          manually.
     * \param   configFile  The configuration file of router client.
     *                      If nullptr, it will load default configuration file.
     * \return  Returns true if succeeded to load configuration file.
     *          Otherwise, it returns false.
     * \see     _routingServiceStart
     **/
    static bool _routingServiceConfigure( const String & configFile = String::getEmptyString() );

    /**
     * \brief   Call to start the client part of remove Routing Service.
     * \param   configFile  If not nullptr, the router will be first configured.
     *                      If nullptr and router was not configured, it will use
     *                      default configuration file.
     *                      If nullptr and router was configured, it will ignore configuration.
     * \return  Returns true if succeeded to start router client.
     * \see     _routingServiceConfigure, _routingServiceStop
     **/
    static bool _routingServiceStart( const String & configFile = String::getEmptyString() );


    /**
     * \brief   Call to start connection to remote Routing Service. If called, it overwrites IP-Address and
     *          port number set via configuration file.
     * \param   ipAddress   Should be valid IP-Address of remote Routing Service.
     * \param   portNr      Should be valid Port Number of remote Routing Service.
     * \return  Returns true if router client successfully started.
     * \see     _routingServiceConfigure, _routingServiceStop
     **/
    static bool _routingServiceStart( const String & ipAddress, unsigned short portNr );

    /**
     * \brief   Call to stop router client.
     * \see     _routingServiceStart
     **/
    static void _routingServiceStop( void );

    /**
     * \brief   Call to enable or disable remote Routing Service client.
     *          If client is already running and it is requested to disable, it will be stopped first.
     * \param   enable  Flag, which is indicating whether the remote Routing Service client should be enabled or disabled.
     *                  If true, it is enabled. Otherwise, it is disabled.
     **/
    static void _routingServiceEnable( bool enable );

    /**
     * \brief   Returns true if Routing Service client is started and ready to operate.
     **/
    static bool _isRoutingServiceStarted( void );

    /**
     * \brief   Returns true if Routing Service client is configured and ready to start.
     **/
    static bool _isRoutingServiceConfigured( void );

    /**
     * \brief   Returns true if remote Routing Service is enabled.
     **/
    static bool _isRoutingServiceEnabled( void );

    /**
     * \brief   The function generates an event to create and start component thread.
     *          Only for internal use.
     * 
     * \param   componentThread The name of the thread to create and restart.
     **/
    static void _requestCreateThread( const String & componentThread );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// IEServiceManagerEventConsumer overrides
/************************************************************************/

    /**
     * \brief   Automatically triggered when event is dispatched by registered
     *          worker / component thread.
     * \param   data    The data object passed in event. It should have at least
     *                  default constructor and assigning operator.
     *                  This object is not used for IPC.
     **/
    virtual void processEvent( const ServiceManagerEventData & data ) override;

/************************************************************************/
// IEEventRouter Interface overrides.
/************************************************************************/

    /**
     * \brief	Posts event and delivers to its target thread / process.
     * \param	eventElem	Event object to post.
     * \return	Returns true if target was found and the event
     *          delivered with success. Otherwise it returns false.
     **/
    virtual bool postEvent( Event & eventElem ) override;

    /**
     * \brief	Triggered when dispatcher starts running. 
     *          In this function runs main dispatching loop.
     *          Events are picked and dispatched here.
     *          Override if logic should be changed.
     * \return	Returns true if Exit Event is signaled.
     **/
    virtual bool runDispatcher( void ) override;

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
    virtual void getServiceList( ITEM_ID cookie, TEArrayList<StubAddress> & OUT out_listStubs, TEArrayList<ProxyAddress> & OUT out_lisProxies ) const override;

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
    virtual void unregisterRemoteStub( const StubAddress & stub, ITEM_ID cookie = NEService::COOKIE_ANY ) override;

    /**
     * \brief   Unregisters remote proxy in the current process.
     * \param   proxy   The address of remote proxy client to unregister
     * \param   cookie  The cookie that has initiated unregister message.
     *                  The parameter is ignored if 'NEService::COOKIE_ANY'.
     **/
    virtual void unregisterRemoteProxy( const ProxyAddress & proxy, ITEM_ID cookie = NEService::COOKIE_ANY ) override;

    /**
     * \brief   Triggered when remote service has been started and there is a
     *          connection established with service.
     * \param   channel     The connection channel of remote Routing Service.
     **/
    virtual void remoteServiceStarted( const Channel & channel ) override;

    /**
     * \brief   Triggered when connection with remote service has been stopped.
     * \param   channel     The connection channel of remote Routing Service.
     **/
    virtual void remoteServiceStopped( const Channel & channel ) override;

    /**
     * \brief   Triggered when connection with remote Routing Service is lost.
     *          The connection is considered lost if it not possible to read or
     *          receive data, and there was not stop connection triggered.
     * \param   channel     The connection channel of remote Routing Service.
     **/
    virtual void remoteServiceConnectionLost( const Channel & channel ) override;

//////////////////////////////////////////////////////////////////////////
// Private static methods
//////////////////////////////////////////////////////////////////////////

    static inline ServiceManager & getInstance( void );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default Constructor.
     *          Protected and cannot be accessed globally.
     **/
    ServiceManager( void );

    /**
     * \brief   Destructor
     **/
    virtual ~ServiceManager( void ) = default;

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
    void _registerServer( const StubAddress & whichServer );

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
    void _unregisterServer( const StubAddress & whichServer );

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
    void _registerClient( const ProxyAddress & whichClient );

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
    void _unregisterClient( const ProxyAddress & whichClient );

    /**
     * \brief   Sends predefined Service Manager Event, notifying Proxy client 
     *          that connection with Stub server is available and established.
     *          When Proxy receives Event, it can start sending requests to 
     *          the Stub of implemented Service Interface.
     * \param   client      The Client Info object containing all required information
     *                      to send Event. The target of Event is a Proxy address,
     *                      specified in the Client Info object.
     **/
    void _sendClientConnectedEvent( const ClientInfo & client, const StubAddress & server ) const;

    /**
     * \brief   Sends predefined Service Manager event to notify that Proxy client 
     *          disconnected service.
     *          When Proxy receives Event, it should stop sending requests to 
     *          the Stub of implemented Service Interface.
     * \param   client  The Client Info object containing all required information
     *                  to send Event. The target of Event is a Proxy address,
     *                  specified in the Client Info object.
     **/
    void _sendClientDisconnectedEvent(const ClientInfo & client, const StubAddress & server) const;

    /**
     * \brief   Starts Service Manager Thread. If Thread is started, the Timer Server
     *          will automatically start as well.
     * \return  Returns true if Service Manager Thread started and ready to receive Events.
     **/
    bool _startServiceManagerThread( void );

    /**
     * \brief   Starts Service Manager Thread. This call will stop Timer Server, all
     *          pending server and client Service Interface will receive disconnect
     *          notification, and the Service Manager Thread will stop and complete job.
     **/
    void _stopServiceManagerThread( void );

    /**
     * \brief   Terminates the component thread. No guarantee that all resources are cleanup.
     *          After processing this method the thread is not operable anymore.
     * \param   threadName  The name of component thread to terminate.
     **/
    bool _terminateComponentThread( const String& threadName );
    
    /**
     * \brief   Creates new instance of the component thread after it was terminated.
     *          All components, services, proxies and worker threads related with the
     *          component thread are restarted again.
     * \param   threadName  The name of the thread to re-start.
     */
    void _startComponentThread( const String & threadName );

    /**
     * \brief   Returns reference to ServiceManager object
     **/
    inline ServiceManager & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The Map of Server Info object as a Key and Client Info List as Values
     **/
    ServerList              mServerList;
    /**
     * \brief   The connection service.
     **/
    ClientService           mConnectService;
    /**
     * \brief   Synchronization object, for multi-threading access.
     **/
    mutable ResourceLock    mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( ServiceManager );
};

//////////////////////////////////////////////////////////////////////////
// ServiceManager class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline ServiceManager & ServiceManager::self( void )
{
    return (*this);
}

#endif  // AREG_COMPONENT_PRIVATE_SERVICEMANAGER_HPP
