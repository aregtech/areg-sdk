#ifndef AREG_MCROUTER_SERVICE_ROUTERSERVERSERVICE_HPP
#define AREG_MCROUTER_SERVICE_ROUTERSERVERSERVICE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        mcrouter/service/RouterServerService.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Message router connection service.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/ipc/IEServiceRegisterConsumer.hpp"
#include "areg/ipc/IEServiceRegisterProvider.hpp"
#include "extend/service/ServiceCommunicatonBase.hpp"

#include "mcrouter/service/private/ServiceRegistry.hpp"

//////////////////////////////////////////////////////////////////////////
// RouterServerService class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The server side connection service. Used by message router to
 *          accept service connections.
 **/
class RouterServerService   : public    ServiceCommunicatonBase
                            , private   IEServiceRegisterConsumer
                            , private   IEServiceRegisterProvider
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor to initialize default values.
     **/
    RouterServerService( void );
    /**
     * \brief   Destructor
     **/
    virtual ~RouterServerService( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

/************************************************************************/
// IEServiceRegisterProvider interface overrides
/************************************************************************/

    /**
     * \brief   Call to register the remote service provider in the system and connect with service consumers.
     *          When service provider is registered, the service provider and all waiting service consumers
     *          receive appropriate connection notifications.
     * \param   stubService     The address of service provider to register in the system.
     * \return  Returns true if succeeded registration.
     **/
    virtual bool registerServiceProvider( const StubAddress & stubService ) override;

    /**
     * \brief   Call to unregister the service provider from the system and disconnect service consumers.
     *          All connected service consumers automatically receive disconnect notifications.
     * \param   stubService     The address of service provider to unregister in the system.
     * \param   reason          The reason to unregister and disconnect the service provider.
     **/
    virtual void unregisterServiceProvider( const StubAddress & stubService, const NEService::eDisconnectReason reason ) override;

    /**
     * \brief   Call to register the remote service consumer in the system and connect to service provider.
     *          If the service provider is already available, the service consumer and the service provider
     *          receive a connection notification.
     * \param   proxyService    The address of the service consumer to register in system.
     * \return  Returns true if registration process started with success. Otherwise, it returns false.
     **/
    virtual bool registerServiceConsumer( const ProxyAddress & proxyService ) override;

    /**
     * \brief   Call to unregister the service consumer from the system and disconnect service provider.
     *          Both, the service provider and the service consumer receive appropriate disconnect notification.
     * \param   proxyService    The address of the service consumer to unregister from the system.
     * \param   reason          The reason to unregister and disconnect the service consumer.
     **/
    virtual void unregisterServiceConsumer( const ProxyAddress & proxyService, const NEService::eDisconnectReason reason ) override;

/************************************************************************/
// IEServiceRegisterConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Call to extract the list of addresses of registered and valid remote service providers and consumers of specified cookie.
     *          If cookie value is 'NEService::COOKIE_ANY' it retrieves the list of all remote service providers and consumers.
     *          On output out_listStubs and out_lisProxies contain the list of remote services.
     * \param   cookie          The cookie to filter. Pass NEService::COOKIE_ANY to ignore filtering.
     * \param   out_listStubs   On output this contains the list of address of the remote service providers of specified cookie.
     * \param   out_lisProxies  On output this contains the list of address of the remote service consumers of specified cookie.
     **/
    virtual void extractRemoteServiceAddresses(const ITEM_ID & cookie, TEArrayList<StubAddress> & OUT out_listStubs, TEArrayList<ProxyAddress> & OUT out_lisProxies ) const override;

    /**
     * \brief   Triggered when a remote service provider is registered in the system.
     * \param   stub    The address of remote service provider that has been registered.
     **/
    virtual void registeredRemoteServiceProvider( const StubAddress & stub ) override;

    /**
     * \brief   Triggered when a remote service consumer is registered in the system.
     * \param   proxy   The address of remote service consumer that has been registered.
     **/
    virtual void registeredRemoteServiceConsumer( const ProxyAddress & proxy ) override;

    /**
     * \brief   Triggered when a remote service provider is unregistered from the system.
     * \param   stub    The address of the remote service provider that has been unregistered.
     * \param   reason  The reason that remote service provider is unregistered.
     * \param   cookie  The cookie of source that has initiated to unregister provider.
     *                  The parameter is ignored if 'NEService::COOKIE_ANY'.
     **/
    virtual void unregisteredRemoteServiceProvider( const StubAddress & stub, NEService::eDisconnectReason reason, const ITEM_ID & cookie /*= NEService::COOKIE_ANY*/ ) override;

    /**
     * \brief   Triggered when a remote service consumer is unregistered from the system.
     * \param   proxy   The address of the remote service consumer that has been unregistered.
     * \param   reason  The reason that remote service consumer is unregistered.
     * \param   cookie  The cookie of source that has initiated to unregister consumer.
     *                  The parameter is ignored if 'NEService::COOKIE_ANY'.
     **/
    virtual void unregisteredRemoteServiceConsumer( const ProxyAddress & proxy, NEService::eDisconnectReason reason, const ITEM_ID & cookie /*= NEService::COOKIE_ANY*/ ) override;

    /**
     * \brief   Triggered when remote service connection and communication channel is established.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void connectedRemoteServiceChannel( const Channel & channel ) override;

    /**
     * \brief   Triggered when disconnected remote service connection and communication channel.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void disconnectedRemoteServiceChannel( const Channel & channel ) override;

    /**
     * \brief   Triggered when remote service connection and communication channel is lost.
     *          The connection is considered lost if it not possible to read or
     *          receive data, and it was not stopped by API call.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void lostRemoteServiceChannel( const Channel & channel ) override;

/************************************************************************/
// IERemoteMessageHandler interface overrides
/************************************************************************/

    /**
     * \brief   Triggered, when failed to process message, i.e. the target for message processing was not found.
     *          In case of request message processing, the source should receive error notification.
     * \param   msgUnprocessed  Unprocessed message data.
     **/
    virtual void failedProcessMessage( const RemoteMessage & msgUnprocessed ) override;

/************************************************************************/
// IEServiceEventConsumerBase overrides
/************************************************************************/

    /**
     * \brief   Called when receive event the client connection is started.
     **/
    virtual void onServiceConnectionStarted( void ) override;

    /**
     * \brief   Called when receive event the client connection is stopped.
     **/
    virtual void onServiceConnectionStopped( void ) override;

    /**
     * \brief   Called when receive event the client connection is lost.
     **/
    virtual void onServiceConnectionLost( void ) override;

    /**
     * \brief   Called when received a communication message to dispatch and process.
     * \param   msgReceived     The received the communication message. 
     **/
    virtual void onServiceMessageReceived(const RemoteMessage & msgReceived) override;

    /**
     * \brief   Called when need to send a communication message.
     * \param   msgSend The communication message to send. 
     **/
    virtual void onServiceMessageSend(const RemoteMessage & msgSend) override;

/************************************************************************/
// IEServiceConnectionHandler overrides
/************************************************************************/

    /**
     * \brief   Called when need to disconnect and unregister all service providers and service consumers.
     **/
    virtual void disconnectServices( void ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns instance of object. For internal use only.
     **/
    inline RouterServerService & self( void );

//////////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////////
private:
    ServiceRegistry mServiceRegistry;   //!< The service registry map to track stub-proxy connections

//////////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( RouterServerService );
};

//////////////////////////////////////////////////////////////////////////////
// RouterServerService class inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline RouterServerService & RouterServerService::self( void )
{
    return (*this);
}

#endif  // AREG_MCROUTER_SERVICE_ROUTERSERVERSERVICE_HPP
