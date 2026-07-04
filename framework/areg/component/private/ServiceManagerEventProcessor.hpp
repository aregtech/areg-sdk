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
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/ServiceManagerEventProcessor.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The Service Manager processing helper class.
 *
 ************************************************************************/

 /************************************************************************
  * Include files
  ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/component/private/ServerList.hpp"
#include "areg/component/private/ServiceManagerEvents.hpp"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class ConnectionProvider;
class RegistrationProvider;
class ServiceManager;
class ProxyAddress;
class StubAddress;
class InStream;

//////////////////////////////////////////////////////////////////////////
// ServiceManagerEventProcessor class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Processes service manager events including server/client registration, connection
 *          status, and thread management.
 **/
class ServiceManagerEventProcessor
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes with a service manager instance.
     *
     * \param   serviceManager      The service manager instance.
     **/
    ServiceManagerEventProcessor( ServiceManager & serviceManager );
    ~ServiceManagerEventProcessor() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Processes a service manager event.
     *
     * \param   cmdService          The service manager command to process.
     * \param   stream              The stream to deserialize data from.
     * \param   connectProvider     The connection provider to forward connection requests.
     * \param   registerProvider    The registration provider to forward register requests.
     **/
    void process_service_event( ServiceManagerEventData::ServiceManagerCommand cmdService
                            , const InStream & stream
                            , ConnectionProvider& connectProvider
                            , RegistrationProvider & registerProvider );

    /**
     * \brief   Returns the list of registered service providers.
     **/
    inline const ServerList& registered_service_list() const;

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Registers a service provider and sends connection notification.
     *
     * \param   whichServer         The server address to register.
     * \param   registerProvider    The provider to send notification.
     **/
    void _register_provider( const StubAddress & whichServer, RegistrationProvider& registerProvider );

    /**
     * \brief   Unregisters a service provider and sends disconnection notification.
     *
     * \param   whichServer         The server address to unregister.
     * \param   reason              The disconnect reason.
     * \param   registerProvider    The provider to send notification.
     **/
    void _unregister_provider( const StubAddress & whichServer, const areg::DisconnectReason reason, RegistrationProvider& registerProvider);

    /**
     * \brief   Registers a service consumer and sends connection notification.
     *
     * \param   whichClient         The client address to register.
     * \param   registerProvider    The provider to send notification.
     **/
    void _register_consumer( const ProxyAddress & whichClient, RegistrationProvider& registerProvider);

    /**
     * \brief   Unregisters a service consumer and sends disconnection notification.
     *
     * \param   whichClient         The client address to unregister.
     * \param   reason              The disconnect reason.
     * \param   registerProvider    The provider to send notification.
     **/
    void _unregister_consumer( const ProxyAddress & whichClient, const areg::DisconnectReason reason, RegistrationProvider& registerProvider);

    /**
     * \brief   Sends client connected notification event.
     *
     * \param   client      The client address.
     * \param   server      The server address.
     **/
    void _send_connected( const ProxyAddress & client, const StubAddress & server ) const;

    /**
     * \brief   Sends client disconnected notification event.
     *
     * \param   client      The client address.
     * \param   server      The server address.
     * \param   status      The service connection status.
     **/
    void _send_disconnected( const ProxyAddress & client, const StubAddress & server, const areg::ServiceConnectionState status ) const;

    /**
     * \brief   Terminates the component thread. Returns true if successful.
     *
     * \param   threadName      The thread name to terminate.
     * \return  Returns true if thread was terminated.
     **/
    bool _terminate_component_thread( const String & threadName );

    /**
     * \brief   Restarts component thread and all related components and workers.
     *
     * \param   threadName      The thread name to restart.
     **/
    void _start_component_thread( const String & threadName );

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
    /**
     * \brief   Deleted default constructor.
     **/
    ServiceManagerEventProcessor() = delete;
    AREG_NOCOPY_NOMOVE( ServiceManagerEventProcessor );
};

//////////////////////////////////////////////////////////////////////////
// ServiceManagerEventProcessor inline methods
//////////////////////////////////////////////////////////////////////////

inline const ServerList & ServiceManagerEventProcessor::registered_service_list() const
{
    return mServerList;
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_SERVICEMANAGEREVENTPROCESSOR_HPP
