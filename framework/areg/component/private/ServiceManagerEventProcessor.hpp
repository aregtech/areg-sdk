#ifndef AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTPROCESSOR_HPP
#define AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTPROCESSOR_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceManagerEventProcessor.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, The Service Manager processing helper class.
 *
 ************************************************************************/

 /************************************************************************
  * Include files
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/component/private/ServerList.hpp"
#include "areg/component/private/ServiceManagerEvents.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IERemoteServiceConnection;
class ServiceManager;
class ProxyAddress;
class StubAddress;
class IEInStream;

//////////////////////////////////////////////////////////////////////////
// ServiceManagerEventProcessor class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Service Manager event processor helper class, which receives the event
 *          passed to Service Manager and processes it.
 **/
class ServiceManagerEventProcessor
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the object, receives the instance of Service Manager.
     **/
    ServiceManagerEventProcessor( ServiceManager & serviceManager );
    ~ServiceManagerEventProcessor( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the list of registered service providers that contain the list of service consumers.
     **/
    inline const ServerList & getRegisteredServiceList( void ) const;

    /**
     * \brief   Called to process Service Manager event.
     * \param   cmdService          The command of Service Manager event to process.
     * \param   stream              The streaming object to de-serialize data of objects.
     * \param   serviceConnection   The instance of service connection object to make connection operations or forward data.
     **/
    void processServiceEvent( ServiceManagerEventData::eServiceManagerCommands cmdService, const IEInStream & sream, IERemoteServiceConnection & serviceConnection );

//////////////////////////////////////////////////////////////////////////
// Hidden calls
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
    void _registerServer( const StubAddress & whichServer, IERemoteServiceConnection & serviceConnection );

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
     * \param   reason          The reason to unregister or disconnect the service provider.
     **/
    void _unregisterServer( const StubAddress & whichServer, const NEService::eDisconnectReason reason, IERemoteServiceConnection & serviceConnection );

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
    void _registerClient( const ProxyAddress & whichClient, IERemoteServiceConnection & serviceConnection );

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
     * \param   reason          The reason to unregister or disconnect the service consumer.
     **/
    void _unregisterClient( const ProxyAddress & whichClient, const NEService::eDisconnectReason reason, IERemoteServiceConnection & serviceConnection );

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
     * \param   status  The service connection status of a service consumer.
     **/
    void _sendClientDisconnectEvent( const ClientInfo & client, const StubAddress & server, const NEService::eServiceConnection status ) const;

    /**
     * \brief   Terminates the component thread. No guarantee that all resources are cleanup.
     *          After processing this method the thread is not operable anymore.
     * \param   threadName  The name of component thread to terminate.
     **/
    bool _terminateComponentThread( const String & threadName );

    /**
     * \brief   Creates new instance of the component thread after it was terminated.
     *          All components, services, proxies and worker threads related with the
     *          component thread are restarted again.
     * \param   threadName  The name of the thread to re-start.
     */
    void _startComponentThread( const String & threadName );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of service manager.
     **/
    ServiceManager &        mServiceManager;
    /**
     * \brief   The Map of Server Info object as a Key and Client Info List as Values
     **/
    ServerList              mServerList;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceManagerEventProcessor( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServiceManagerEventProcessor );
};

//////////////////////////////////////////////////////////////////////////
// ServiceManagerEventProcessor inline methods
//////////////////////////////////////////////////////////////////////////

inline const ServerList & ServiceManagerEventProcessor::getRegisteredServiceList( void ) const
{
    return mServerList;
}

#endif  // AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTPROCESSOR_HPP
