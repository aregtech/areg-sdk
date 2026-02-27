#ifndef AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTPROCESSOR_HPP
#define AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTPROCESSOR_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceManagerEventProcessor.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The Service Manager processing helper class.
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
namespace areg { class ConnectionProvider; }
namespace areg { class RegistrationProvider; }
namespace areg { class ServiceManager; }
namespace areg { class ProxyAddress; }
namespace areg { class StubAddress; }
namespace areg { class InStream; }

namespace areg
{
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
        ServiceManagerEventProcessor( areg::ServiceManager & serviceManager );
        ~ServiceManagerEventProcessor() = default;

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
        void processServiceEvent( areg::ServiceManagerEventData::ServiceManagerCommand cmdService
                                , const areg::InStream & stream
                                , areg::ConnectionProvider& connectProvider
                                , areg::RegistrationProvider & registerProvider );

        /**
         * \brief   Returns the list of registered service providers that contain the list of service consumers.
         **/
        inline const ServerList& getRegisteredServiceList() const;

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
        void _registerServer( const areg::StubAddress & whichServer, areg::RegistrationProvider& registerProvider );

        /**
          * \brief   The function is called when it is requested to unregister the service provider.
          *          For example, when service shuts down or connection is lost.
          *          The function generates event to send the service connection status notification.
         * \param   whichServer         The address of service provider that is registering in the system.
         * \param   reason              The reason to unregister or disconnect the service provider.
         * \param   registerProvider    The service connection object to notify remote application
          *                             about public service connection status.
         **/
        void _unregisterServer( const areg::StubAddress & whichServer, const areg::DisconnectReason reason, areg::RegistrationProvider& registerProvider);

         /**
          * \brief   The function is called when it is requested to register the service consumer.
          *          For example, when the service consumer is created and need to be registered.
          *          The function generates event to send the service connection status notification.
          * \param   whichClient        The address of service consumer that is registering in the system.
          * \param   registerProvider   The service connection object to notify remote application
          *                             about public service connection status.
          **/
        void _registerClient( const areg::ProxyAddress & whichClient, areg::RegistrationProvider& registerProvider);

        /**
         * \brief   The function is called when it is requested to unregister the service consumer.
         *          For example, when the service consumer is created and need to be registered.
         *          The function generates event to send the service connection status notification.
         * \param   whichClient         The address of service consumer that is registering in the system.
         * \param   reason              The reason to unregister or disconnect the service consumer.
         * \param   registerProvider    The service connection object to notify remote application
         *                              about public service connection status.
         **/
        void _unregisterClient( const areg::ProxyAddress & whichClient, const areg::DisconnectReason reason, areg::RegistrationProvider& registerProvider);

        /**
         * \brief   Creates and sends predefined service consumer connected notification.
         * \param   client      The address of service consumer to send the event.
         * \param   server      The address of service provider that connected.
         **/
        void _sendClientConnectedEvent( const areg::ProxyAddress & client, const areg::StubAddress & server ) const;

        /**
         * \brief   Creates and sends predefined service consumer disconnected notification.
         * \param   client      The address of service consumer to send the event.
         * \param   server      The address of service provider that disconnected.
         * \param   status      The service connection status.
         **/
        void _sendClientDisconnectEvent( const areg::ProxyAddress & client, const areg::StubAddress & server, const areg::ServiceConnectionState status ) const;

        /**
         * \brief   Terminates the component thread. No guarantee that all resources are cleanup.
         *          After processing this method the thread is not operable anymore.
         * \param   threadName  The name of component thread to terminate.
         **/
        bool _terminateComponentThread( const areg::String & threadName );

        /**
         * \brief   Creates new instance of the component thread after it was terminated.
         *          All components, services providers and consumers, and worker threads related with the
         *          component thread are restarted again.
         * \param   threadName  The name of the thread to re-start.
         */
        void _startComponentThread( const areg::String & threadName );

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   The instance of service manager.
         **/
        areg::ServiceManager &        mServiceManager;
        /**
         * \brief   The Map of Server Info object as a Key and Client Info List as Values
         **/
        ServerList              mServerList;

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        ServiceManagerEventProcessor() = delete;
        AREG_NOCOPY_NOMOVE( ServiceManagerEventProcessor );
    };

    //////////////////////////////////////////////////////////////////////////
    // ServiceManagerEventProcessor inline methods
    //////////////////////////////////////////////////////////////////////////

    inline const ServerList & ServiceManagerEventProcessor::getRegisteredServiceList() const
    {
        return mServerList;
    }

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTPROCESSOR_HPP
