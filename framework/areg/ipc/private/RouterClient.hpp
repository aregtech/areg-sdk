#ifndef AREG_IPC_PRIVATE_ROUTERCLIENT_HPP
#define AREG_IPC_PRIVATE_ROUTERCLIENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/RouterClient.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service client connection 
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/component/RemoteEventConsumer.hpp"
#include "areg/ipc/RegistrationProvider.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/ipc/ServiceClientConnectionBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class RegistrationConsumer;

//////////////////////////////////////////////////////////////////////////
// RouterClient class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The connected client servicing object to handle connections,
 *          to read and send message, to dispatch messages and
 *          communicate with service manager.
 **/
class RouterClient  : public    ServiceClientConnectionBase
                    , public    RegistrationProvider
                    , protected DispatcherThread
                    , private   RemoteMessageHandler
                    , private   RemoteEventConsumer
{
private:
    //! The prefix to add to the send and receive message threads.
    static constexpr std::string_view   PREFIX_THREAD{ "router_" };
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes client servicing object, sets remote service consumer object.
     * \param   connectionConsumer  The instance of remote service connection consumer object to handle service connection notifications.
     * \param   registerConsumer    The instance of remote service registration consumer to handle service register notification.
     **/
    RouterClient(ConnectionConsumer& connectionConsumer, RegistrationConsumer & registerConsumer);
    /**
     * \brief   Destructor
     **/
    virtual ~RouterClient() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

/************************************************************************/
// ServiceClientConnectionBase overrides
/************************************************************************/

    /**
     * \brief   Call to start remote service. The host name and port number should be already set.
     * \return  Returns true if start service is triggered.
     **/
    bool connectServiceHost() override;

    /**
     * \brief   Call to stop service. No more remote communication should be possible.
     **/
    void disconnectServiceHost() override;

    /**
     * \brief   Triggered when need to quit the service.
     **/
    void onServiceExit() override;

    /**
     * \brief   Returns true, if remote service connection is triggered, not connected yet and in pending state.
     **/
    bool isServiceHostPending() const override;

/************************************************************************/
// RemoteMessageHandler interface overrides
/************************************************************************/

    /**
     * \brief   Triggered, when failed to send message.
     * \param   msgFailed   The message, which failed to send.
     * \param   whichTarget The target socket to send message.
     **/
    void failedSendMessage( const areg::RemoteMessage & msgFailed, areg::Socket & whichTarget ) override;

    /**
     * \brief   Triggered, when failed to receive message.
     * \param   whichSource Indicates the failed source socket to receive message.
     **/
    void failedReceiveMessage( areg::Socket & whichSource ) override;

    /**
     * \brief   Triggered, when failed to process message, i.e. the target for message processing was not found.
     *          In case of request message processing, the source should receive error notification.
     * \param   msgUnprocessed  Unprocessed message data.
     **/
    void failedProcessMessage( const areg::RemoteMessage & msgUnprocessed ) override;

    /**
     * \brief   Triggered, when need to process received message.
     * \param   msgReceived Received message to process.
     * \param   whichSource The source socket, which received message.
     **/
    void processReceivedMessage( const areg::RemoteMessage & msgReceived, areg::Socket & whichSource ) override;

/************************************************************************/
// RegistrationProvider interface overrides
/************************************************************************/

    /**
     * \brief   Call to register the remote service provider in the system and connect with service consumers.
     *          When service provider is registered, the service provider and all waiting service consumers
     *          receive appropriate connection notifications.
     * \param   stubService     The address of service provider to register in the system.
     * \return  Returns true if succeeded registration.
     **/
    bool registerServiceProvider( const areg::StubAddress & stubService ) override;

    /**
     * \brief   Call to unregister the service provider from the system and disconnect service consumers.
     *          All connected service consumers automatically receive disconnect notifications.
     * \param   stubService     The address of service provider to unregister in the system.
     * \param   reason          The reason to unregister and disconnect the service provider.
     **/
    void unregisterServiceProvider( const areg::StubAddress & stubService, const areg::DisconnectReason reason ) override;

    /**
     * \brief   Call to register the remote service consumer in the system and connect to service provider.
     *          If the service provider is already available, the service consumer and the service provider
     *          receive a connection notification.
     * \param   proxyService    The address of the service consumer to register in system.
     * \return  Returns true if registration process started with success. Otherwise, it returns false.
     **/
    bool registerServiceConsumer( const ProxyAddress & proxyService ) override;

    /**
     * \brief   Call to unregister the service consumer from the system and disconnect service provider.
     *          Both, the service provider and the service consumer receive appropriate disconnect notification.
     * \param   proxyService    The address of the service consumer to unregister from the system.
     * \param   reason          The reason to unregister and disconnect the service consumer.
     **/
    void unregisterServiceConsumer( const ProxyAddress & proxyService, const areg::DisconnectReason reason ) override;

/************************************************************************/
// EventRouter interface overrides
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
    bool postEvent( Event & eventElem ) override;

/************************************************************************/
// RemoteEventConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Triggered when the Stub receives remote request event to process.
     * \param   requestEvent        The remote request event to be processed.
     **/
    void processRemoteRequestEvent( RemoteRequestEvent & requestEvent ) override;

    /**
     * \brief   Triggered when the Stub receives remote notification request event to process.
     *          For example, send by Proxy and processed by Stub when need to start or stop
     *          sending attribute update notifications.
     * \param   requestNotifyEvent  The remote notification request event to be processed.
     **/
    void processRemoteNotifyRequestEvent( RemoteNotifyRequestEvent & requestNotifyEvent ) override;

    /**
     * \brief   Triggered when the Stub receives remote response request event to process.
     *          For example, send by Proxy and processed by Stub when need to start or stop
     *          to subscribe on information or response sent by Stub.
     * \param   responseEvent   The remote response event sent on processed request.
     **/
    void processRemoteResponseEvent( RemoteResponseEvent & responseEvent ) override;

/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Call to enable or disable event dispatching threads to receive events.
     *          Override if need to make event dispatching preparation job.
     * \param   isReady     The flag to indicate whether the dispatcher is ready for events.
     **/
    void readyForEvents( bool isReady ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden operations and attributes
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Returns instance of client servicing object.
     **/
    inline RouterClient & self();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of service register consumer.
     **/
    RegistrationConsumer &     mRegisterConsumer;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    RouterClient() = delete;
    AREG_NOCOPY_NOMOVE( RouterClient );
};

//////////////////////////////////////////////////////////////////////////
// RouterClient class inline methods implementation
//////////////////////////////////////////////////////////////////////////

inline RouterClient & RouterClient::self()
{
    return (*this);
}

#endif  // AREG_IPC_PRIVATE_ROUTERCLIENT_HPP
