#ifndef AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTPROCESSOR_HPP
#define AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTPROCESSOR_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceManagerEventProcessor.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
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
class IEServiceConnectionProvider;
class IEServiceRegisterProvider;
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
     * \brief   Called to process Service Manager event.
     * \param   cmdService          The command of Service Manager event to process.
     * \param   stream              The streaming object to de-serialize data of objects.
     * \param   connectProvider     The instance of service connection provider to forward connection requests.
     * \param   registerProvider    The instance of service register provider to forward register requests
     **/
    void processServiceEvent( ServiceManagerEventData::eServiceManagerCommands cmdService
                            , const IEInStream & stream
                            , IEServiceConnectionProvider& connectProvider
                            , IEServiceRegisterProvider & registerProvider );

    /**
     * \brief   Returns the list of registered service providers that contain the list of service consumers.
     **/
    inline const ServerList& getRegisteredServiceList(void) const;

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   The function is called when it is requested to register the service provider.
     *          For example, when the service is created and need to be registered.
     *          The function generates event to send the service connection status notification.
     * \param   whichServer         The address of service provider that is registering in the system.
     * \param   registerProvider    The service connection object to notify remote application
     *                              about public service connection status.
     **/
    void _registerServer( const StubAddress & whichServer, IEServiceRegisterProvider& registerProvider );

    /**
      * \brief   The function is called when it is requested to unregister the service provider.
      *          For example, when service shuts down or connection is lost.
      *          The function generates event to send the service connection status notification.
     * \param   whichServer         The address of service provider that is registering in the system.
     * \param   reason              The reason to unregister or disconnect the service provider.
     * \param   registerProvider    The service connection object to notify remote application
      *                             about public service connection status.
     **/
    void _unregisterServer( const StubAddress & whichServer, const NEService::eDisconnectReason reason, IEServiceRegisterProvider& registerProvider);

     /**
      * \brief   The function is called when it is requested to register the service consumer.
      *          For example, when the service consumer is created and need to be registered.
      *          The function generates event to send the service connection status notification.
      * \param   whichClient        The address of service consumer that is registering in the system.
      * \param   registerProvider   The service connection object to notify remote application
      *                             about public service connection status.
      **/
    void _registerClient( const ProxyAddress & whichClient, IEServiceRegisterProvider& registerProvider);

    /**
     * \brief   The function is called when it is requested to unregister the service consumer.
     *          For example, when the service consumer is created and need to be registered.
     *          The function generates event to send the service connection status notification.
     * \param   whichClient         The address of service consumer that is registering in the system.
     * \param   reason              The reason to unregister or disconnect the service consumer.
     * \param   registerProvider    The service connection object to notify remote application
     *                              about public service connection status.
     **/
    void _unregisterClient( const ProxyAddress & whichClient, const NEService::eDisconnectReason reason, IEServiceRegisterProvider& registerProvider);

    /**
     * \brief   Creates and sends predefined service consumer connected notification.
     * \param   client      The address of service consumer to send the event.
     * \param   server      The address of service provider that connected.
     **/
    void _sendClientConnectedEvent( const ProxyAddress & client, const StubAddress & server ) const;

    /**
     * \brief   Creates and sends predefined service consumer disconnected notification.
     * \param   client      The address of service consumer to send the event.
     * \param   server      The address of service provider that disconnected.
     * \param   status      The service connection status.
     **/
    void _sendClientDisconnectEvent( const ProxyAddress & client, const StubAddress & server, const NEService::eServiceConnection status ) const;

    /**
     * \brief   Terminates the component thread. No guarantee that all resources are cleanup.
     *          After processing this method the thread is not operable anymore.
     * \param   threadName  The name of component thread to terminate.
     **/
    bool _terminateComponentThread( const String & threadName );

    /**
     * \brief   Creates new instance of the component thread after it was terminated.
     *          All components, services providers and consumers, and worker threads related with the
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
