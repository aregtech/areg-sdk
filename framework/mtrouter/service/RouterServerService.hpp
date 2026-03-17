#ifndef AREG_mtrouter_SERVICE_ROUTERSERVERSERVICE_HPP
#define AREG_mtrouter_SERVICE_ROUTERSERVERSERVICE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        mtrouter/service/RouterServerService.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Message router connection service.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/ipc/RegistrationConsumer.hpp"
#include "areg/ipc/RegistrationProvider.hpp"
#include "aregextend/service/ServiceCommunicationBase.hpp"

#include "mtrouter/service/private/ServiceRegistry.hpp"

//////////////////////////////////////////////////////////////////////////
// RouterServerService class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Server-side service that accepts message router connections and manages service
 *          registration.
 **/
class RouterServerService   : public    areg::ext::ServiceCommunicationBase
                            , private   areg::RegistrationConsumer
                            , private   areg::RegistrationProvider
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    RouterServerService();
    virtual ~RouterServerService() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

/************************************************************************/
// RegistrationProvider interface overrides
/************************************************************************/

    /**
     * \brief   Registers a remote service provider and notifies all waiting consumers. Returns true
     *          on success.
     *
     * \param   stubService     The address of the service provider to register.
     * \return  Returns true if registration succeeded.
     **/
    bool register_service_provider( const areg::StubAddress & stubService ) override;

    /**
     * \brief   Unregisters a remote service provider and notifies all connected consumers.
     *
     * \param   stubService     The address of the service provider to unregister.
     * \param   reason          The disconnection reason.
     **/
    void unregister_service_provider( const areg::StubAddress & stubService, const areg::DisconnectReason reason ) override;

    /**
     * \brief   Registers a remote service consumer and connects to the provider if available.
     *          Returns true on success.
     *
     * \param   proxyService    The address of the service consumer to register.
     * \return  Returns true if registration started successfully.
     **/
    bool register_service_consumer( const areg::ProxyAddress & proxyService ) override;

    /**
     * \brief   Unregisters a remote service consumer and notifies the provider.
     *
     * \param   proxyService    The address of the service consumer to unregister.
     * \param   reason          The disconnection reason.
     **/
    void unregister_service_consumer( const areg::ProxyAddress & proxyService, const areg::DisconnectReason reason ) override;

/************************************************************************/
// RegistrationConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Extracts lists of remote service providers and consumers for the specified cookie.
     *          If cookie is areg::COOKIE_ANY, retrieves all services.
     *
     * \param   cookie              The cookie to filter by. Pass areg::COOKIE_ANY to retrieve
     *                              all services.
     * \param[out] listProviders       On output, contains the list of remote service provider
     *                                 addresses.
     * \param[out] listConsumers       On output, contains the list of remote service consumer
     *                                 addresses.
     **/
    void extract_service_addresses(const ITEM_ID & cookie, areg::ArrayList<areg::StubAddress> & listProviders, areg::ArrayList<areg::ProxyAddress> & listConsumers ) const override;

    /**
     * \brief   Triggered when a remote service provider is registered.
     *
     * \param   stub    The address of the registered service provider.
     **/
    void on_provider_registered( const areg::StubAddress & stub ) override;

    /**
     * \brief   Triggered when a remote service consumer is registered.
     *
     * \param   proxy       The address of the registered service consumer.
     **/
    void on_consumer_registered( const areg::ProxyAddress & proxy ) override;

    /**
     * \brief   Triggered when a remote service provider is unregistered.
     *
     * \param   stub        The address of the unregistered service provider.
     * \param   reason      The disconnection reason.
     * \param   cookie      The cookie of the source that initiated unregistration. Ignored if
     *                      areg::COOKIE_ANY.
     **/
    void on_provider_unregistered( const areg::StubAddress & stub, areg::DisconnectReason reason, const ITEM_ID & cookie /*= areg::COOKIE_ANY*/ ) override;

    /**
     * \brief   Triggered when a remote service consumer is unregistered.
     *
     * \param   proxy       The address of the unregistered service consumer.
     * \param   reason      The disconnection reason.
     * \param   cookie      The cookie of the source that initiated unregistration. Ignored if
     *                      areg::COOKIE_ANY.
     **/
    void on_consumer_unregistered( const areg::ProxyAddress & proxy, areg::DisconnectReason reason, const ITEM_ID & cookie /*= areg::COOKIE_ANY*/ ) override;

    /**
     * \brief   Triggered when a remote service communication channel is established.
     *
     * \param   channel     The established communication channel.
     **/
    void on_service_channel_connected( const areg::Channel & channel ) override;

    /**
     * \brief   Triggered when a remote service communication channel is disconnected.
     *
     * \param   channel     The disconnected communication channel.
     **/
    void on_service_channel_disconnected( const areg::Channel & channel ) override;

    /**
     * \brief   Triggered when a remote service communication channel is lost due to inability to
     *          read or receive data.
     *
     * \param   channel     The lost communication channel.
     **/
    void on_service_channel_lost( const areg::Channel & channel ) override;

/************************************************************************/
// RemoteMessageHandler interface overrides
/************************************************************************/

    /**
     * \brief   Triggered when message processing fails because the target was not found. Request
     *          sources receive error notification.
     *
     * \param   msgUnprocessed      The unprocessed message.
     **/
    void failed_process_message( const areg::RemoteMessage & msgUnprocessed ) override;

/************************************************************************/
// ServiceEventConsumer overrides
/************************************************************************/

    /**
     * \brief   Triggered when the client connection starts.
     **/
    void on_connection_started() override;

    /**
     * \brief   Triggered when the client connection stops.
     **/
    void on_connection_stopped() override;

    /**
     * \brief   Triggered when the client connection is lost.
     **/
    void on_connection_lost() override;

    /**
     * \brief   Triggered when a message is received for dispatch and processing.
     *
     * \param   msgReceived     The received communication message.
     **/
    void on_message_received(const areg::RemoteMessage & msgReceived) override;

    /**
     * \brief   Triggered when a message needs to be sent.
     *
     * \param   msgSend     The message to send.
     **/
    void on_message_send(const areg::RemoteMessage & msgSend) override;

/************************************************************************/
// ConnectionHandler overrides
/************************************************************************/

    /**
     * \brief   Disconnects and unregisters all service providers and consumers.
     **/
    void disconnect_services() override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns a reference to this instance for internal use.
     **/
    inline RouterServerService & self();

//////////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////////
private:
    ServiceRegistry mServiceRegistry;   //!< The service registry map to track stub-proxy connections

//////////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( RouterServerService );
};

//////////////////////////////////////////////////////////////////////////////
// RouterServerService class inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline RouterServerService & RouterServerService::self()
{
    return (*this);
}

#endif  // AREG_mtrouter_SERVICE_ROUTERSERVERSERVICE_HPP
