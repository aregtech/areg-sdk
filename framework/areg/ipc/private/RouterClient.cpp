/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/RouterClient.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service Connection client implementation
 ************************************************************************/
#include "areg/ipc/private/RouterClient.hpp"

#include "areg/ipc/RegistrationConsumer.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"

#include "areg/component/RemoteEventFactory.hpp"
#include "areg/component/StreamableEvent.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/Process.hpp"
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE(areg_ipc_private_RouterClient_failedSendMessage);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient_failedReceiveMessage);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient_failedProcessMessage);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient_processReceivedMessage);

DEF_LOG_SCOPE(areg_ipc_private_RouterClient_processRemoteRequestEvent);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient_processRemoteResponseEvent);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient_processRemoteNotifyRequestEvent);

DEF_LOG_SCOPE(areg_ipc_private_RouterClient_registerServiceProvider);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient_unregisterServiceProvider);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient_registerServiceConsumer);
DEF_LOG_SCOPE(areg_ipc_private_RouterClient_unregisterServiceConsumer);

namespace areg
{

    //////////////////////////////////////////////////////////////////////////
    // RouterClient class implementation
    //////////////////////////////////////////////////////////////////////////

    RouterClient::RouterClient(ConnectionConsumer& connectionConsumer, RegistrationConsumer& registerConsumer)
        : ServiceClientConnectionBase   ( COOKIE_ROUTER
                                        , RemoteServiceKind::Router
                                        , static_cast<uint32_t>(ConnectionType::Tcpip)
                                        , MessageSource::SourceClient
                                        , connectionConsumer
                                        , static_cast<RemoteMessageHandler &>(self())
                                        , static_cast<DispatcherThread &>(self())
                                        , RouterClient::PREFIX_THREAD)
        , RegistrationProvider     ( )
        , DispatcherThread              (String(RouterClient::PREFIX_THREAD) + CLIENT_DISPATCH_MESSAGE_THREAD, STACK_SIZE_DEFAULT, QUEUE_SIZE_MAXIMUM)
        , RemoteEventConsumer         ( )

        , mRegisterConsumer (registerConsumer)
    {
    }

    bool RouterClient::connectServiceHost()
    {
        bool result{ true };
        if (isRunning() == false)
        {
            if (createThread(WAIT_INFINITE) && waitForDispatcherStart(WAIT_INFINITE))
            {
                result = ServiceClientConnectionBase::connectServiceHost();
            }
            else
            {
                shutdownThread(WAIT_INFINITE);
            }
        }
        else if (mClientConnection.isValid() == false)
        {
            result = ServiceClientConnectionBase::connectServiceHost();
        }
        else
        {
            result = false;
        }

        return result;
    }

    void RouterClient::disconnectServiceHost()
    {
        if (isRunning())
        {
            ServiceClientConnectionBase::disconnectServiceHost();
            completionWait(WAIT_INFINITE);
            shutdownThread(DO_NOT_WAIT);
        }
    }

    void RouterClient::onServiceExit()
    {
        ServiceClientConnectionBase::onServiceExit();
        triggerExit();
    }

    bool RouterClient::isServiceHostPending() const
    {
        Lock lock(mLock);
        return (isRunning() && ((mClientConnection.isValid() == false) || (getConnectionState() == ServiceClientConnectionBase::ConnectionPhase::ConnectionStarting)));
    }

    bool RouterClient::registerServiceProvider( const StubAddress & stubService )
    {
        LOG_SCOPE(areg_ipc_private_RouterClient_registerServiceProvider);
        Lock lock( mLock );
        bool result{ false };
        if ( isConnectionStarted() )
        {
            LOG_DBG("Queuing to send register [ %s ] service message by connection [ %d ]"
                       , StubAddress::convAddressToPath(stubService).getString()
                       , mClientConnection.getCookie());

            result = sendMessage(createRouterRegisterService(stubService, mClientConnection.getCookie(), COOKIE_ROUTER), Event::EventPriority::HighPrio );
        }

        return result;
    }

    void RouterClient::unregisterServiceProvider(const StubAddress & stubService, const DisconnectReason reason )
    {
        LOG_SCOPE(areg_ipc_private_RouterClient_unregisterServiceProvider);

        Lock lock( mLock );
        if ( isConnectionStarted() )
        {
            LOG_DBG("Queuing to send unregister [ %s ] service message by connection [ %d ]"
                       , StubAddress::convAddressToPath(stubService).getString()
                       , mClientConnection.getCookie());

            sendMessage(createRouterUnregisterService(stubService, reason, mClientConnection.getCookie(), COOKIE_ROUTER) );
        }
    }

    bool RouterClient::registerServiceConsumer(const ProxyAddress & proxyService)
    {
        LOG_SCOPE(areg_ipc_private_RouterClient_registerServiceConsumer );
        Lock lock( mLock );
        bool result { false };
        if ( isConnectionStarted() )
        {
            LOG_DBG("Queuing to send register [ %s ] service client message by connection [ %d ]"
                       , ProxyAddress::convAddressToPath(proxyService).getString()
                       , mClientConnection.getCookie());

            result = sendMessage(createRouterRegisterClient(proxyService, mClientConnection.getCookie(), COOKIE_ROUTER), Event::EventPriority::HighPrio);
        }

        return result;
    }

    void RouterClient::unregisterServiceConsumer(const ProxyAddress & proxyService, const DisconnectReason reason )
    {
        LOG_SCOPE(areg_ipc_private_RouterClient_unregisterServiceConsumer);

        Lock lock( mLock );
        if ( isConnectionStarted() )
        {
            LOG_DBG("Queuing to send unregister [ %s ] service client message by connection [ %d ]"
                       , ProxyAddress::convAddressToPath(proxyService).getString()
                       , mClientConnection.getCookie());

            sendMessage(createRouterUnregisterClient(proxyService, reason, mClientConnection.getCookie(), COOKIE_ROUTER) );
        }
    }

    void RouterClient::failedSendMessage(const RemoteMessage & msgFailed, Socket & whichTarget )
    {
        LOG_SCOPE(areg_ipc_private_RouterClient_failedSendMessage);

        if (Application::isServicingReady())
        {
            uint32_t msgId{ msgFailed.getMessageId() };
            if ( isExecutableId(msgId) || isConnectNotifyId(msgId) )
            {
                LOG_WARN("Failed to send message [ %u ] to target [ %llu ], source is [ %llu ], the target socket [ %u ] is [ %s : %s ]"
                           , msgId
                           , msgFailed.getTarget()
                           , msgFailed.getSource()
                           , static_cast<uint32_t>(whichTarget.getHandle())
                           , whichTarget.isValid() ? "VALID" : "INVALID"
                           , whichTarget.isAlive() ? "ALIVE" : "DEAD");

                msgFailed.moveToBegin();
                StreamableEvent * eventError = RemoteEventFactory::createRequestFailedEvent(msgFailed, mChannel);
                if ( eventError != nullptr )
                {
                    LOG_DBG("Replying with failure event [ %s ]", eventError->getRuntimeClassName().getString());
                    eventError->deliverEvent();
                }

                if ( whichTarget.isValid() && (whichTarget.isAlive() == false))
                {
                    LOG_DBG("Trying to reconnect");
                    cancelConnection( );
                    sendCommand( ServiceEventData::ServiceCommand::CMD_ServiceLost, Event::EventPriority::NormalPrio );
                }
            }
            else
            {
                LOG_WARN("The failed message, it is neither executable, nor connection notification. Ignoring to generate request failed event.");
            }
        }
        else
        {
            LOG_WARN("Ignore send message failure, the application is closing");
        }
    }

    void RouterClient::failedReceiveMessage( Socket & whichSource )
    {
        LOG_SCOPE(areg_ipc_private_RouterClient_failedReceiveMessage);

        if (Application::isServicingReady())
        {
            if (whichSource.isValid())
            {
                LOG_WARN("Failed to receive message from socket [ %lu ], which [ %s : %s ], going to reconnect"
                           , static_cast<uint32_t>(whichSource.getHandle())
                           , whichSource.isValid() ? "VALID" : "INVALID"
                           , whichSource.isAlive() ? "ALIVE" : "DEAD");
                cancelConnection();
                sendCommand(ServiceEventData::ServiceCommand::CMD_ServiceLost, Event::EventPriority::NormalPrio);
            }
            else
            {
                LOG_WARN("Ignoring sending reconnect event, the socket is invalid");
            }
        }
        else
        {
            LOG_WARN("Ignore receive message failure, the application is closing");
        }
    }

    void RouterClient::failedProcessMessage( const RemoteMessage & msgUnprocessed )
    {
        LOG_SCOPE(areg_ipc_private_RouterClient_failedProcessMessage);

        if (Application::isServicingReady())
        {
            uint32_t msgId{ msgUnprocessed.getMessageId() };
            if ( isExecutableId(msgId) )
            {
                LOG_DBG("The message [ %u ] for target [ %llu ] and from source [ %llu ] is unprocessed, replying with failed message"
                          , msgId
                          , msgUnprocessed.getTarget()
                          , msgUnprocessed.getSource());

                msgUnprocessed.moveToBegin();
                StreamableEvent * eventError = RemoteEventFactory::createRequestFailedEvent(msgUnprocessed, mChannel);
                if ( eventError != nullptr )
                {
                    RemoteMessage data;
                    if ( RemoteEventFactory::createStreamFromEvent( data, *eventError, mChannel) )
                    {
                        sendMessage(data);
                    }
                }
            }
            else
            {
                LOG_WARN("The unprocessed message is neither executable, nor connection notification. Ignoring to generate request failed event.");
            }
        }
        else
        {
            LOG_WARN("Ignore processing failure message, the application is closing");
        }
    }

    void RouterClient::processReceivedMessage( const RemoteMessage & msgReceived, Socket & whichSource )
    {
        LOG_SCOPE(areg_ipc_private_RouterClient_processReceivedMessage);
        if ( msgReceived.isValid() && whichSource.isValid() )
        {
            FuncIdRange msgId{ static_cast<FuncIdRange>(msgReceived.getMessageId()) };
            MessageResult result{ static_cast<MessageResult>(msgReceived.getResult()) };
            LOG_DBG("Processing received valid message [ %u ], result [ %s ]", msgId, areg::getString(result));

            switch ( msgId )
            {
            case FuncIdRange::SystemServiceNotifyConnection:
                serviceConnectionEvent(msgReceived);
                break;

            case FuncIdRange::SystemServiceNotifyRegister:
                {
                    ASSERT( mClientConnection.getCookie() == msgReceived.getTarget() );
                    RegistrationAction reqType;
                    msgReceived >> reqType;
                    LOG_DBG("Remote routing service registration notification of type [ %s ]", areg::getString(reqType));

                    switch ( reqType )
                    {
                    case RegistrationAction::RegisterClient:
                        {
                            ProxyAddress proxy(msgReceived);
                            DisconnectReason reason { DisconnectReason::UndefinedReason };
                            msgReceived >> reason;
                            proxy.setSource( mChannel.getSource() );
                            if ( result == MessageResult::Succeed )
                            {
                                mRegisterConsumer.registeredRemoteServiceConsumer(proxy);
                            }
                            else
                            {
                                mRegisterConsumer.unregisteredRemoteServiceConsumer(proxy, reason, COOKIE_ANY);
                            }
                        }
                        break;

                    case RegistrationAction::RegisterStub:
                        {
                            StubAddress stub(msgReceived);
                            stub.setSource( mChannel.getSource() );
                            if ( result == MessageResult::Succeed )
                            {
                                mRegisterConsumer.registeredRemoteServiceProvider( stub );
                            }
                            else
                            {
                                mRegisterConsumer.unregisteredRemoteServiceProvider( stub, DisconnectReason::UndefinedReason, COOKIE_ANY );
                            }
                        }
                        break;

                    case RegistrationAction::UnregisterClient:
                        {
                            ProxyAddress proxy(msgReceived);
                            DisconnectReason reason { DisconnectReason::UndefinedReason };
                            msgReceived >> reason;
                            proxy.setSource( mChannel.getSource() );
                            mRegisterConsumer.unregisteredRemoteServiceConsumer(proxy, reason, COOKIE_ANY);
                        }
                        break;

                    case RegistrationAction::UnregisterStub:
                        {
                            StubAddress stub(msgReceived);
                            DisconnectReason reason{DisconnectReason::UndefinedReason};
                            msgReceived >> reason;
                            stub.setSource( mChannel.getSource() );
                            mRegisterConsumer.unregisteredRemoteServiceProvider(stub, reason, COOKIE_ANY);
                        }
                        break;

                    default:
                        ASSERT(false);
                        break;
                    }
                }
                break;

            case FuncIdRange::ServiceLastId:                    // fall through
            case FuncIdRange::SystemServiceQueryInstances:      // fall through
            case FuncIdRange::SystemServiceRequestRegister:     // fall through
            case FuncIdRange::SystemServiceDisconnect:          // fall through
            case FuncIdRange::SystemServiceConnect:             // fall through
            case FuncIdRange::ResponseServiceProviderConnection:// fall through
            case FuncIdRange::RequestServiceProviderConnection: // fall through
            case FuncIdRange::ResponseServiceProviderVersion:   // fall through
            case FuncIdRange::RequestServiceProviderVersion:    // fall through
            case FuncIdRange::RequestRegisterService:           // fall through
            case FuncIdRange::ComponentCleanup:                 // fall through
            case FuncIdRange::SystemServiceNotifyInstances:     // fall through
            case FuncIdRange::ServiceLogRegisterScopes:         // fall through
            case FuncIdRange::ServiceLogUpdateScopes:           // fall through
            case FuncIdRange::ServiceLogQueryScopes:            // fall through
            case FuncIdRange::ServiceLogScopesUpdated:          // fall through
            case FuncIdRange::ServiceSaveLogConfiguration:      // fall through
            case FuncIdRange::ServiceLogConfigurationSaved:     // fall through
            case FuncIdRange::ServiceLogMessage:                // fall through
                break;

            case FuncIdRange::AttributeLastId:          // fall through
            case FuncIdRange::AttributeFirstId:         // fall through
            case FuncIdRange::ResponseLastId:           // fall through
            case FuncIdRange::ResponseFirstId:          // fall through
            case FuncIdRange::RequestLastId:            // fall through
            case FuncIdRange::RequestFirstId:           // fall through
            case FuncIdRange::EmptyFunctionId:          // fall through
            default:
                {
                    if ( isExecutableId(static_cast<uint32_t>(msgId)) )
                    {
                        StreamableEvent * eventRemote = RemoteEventFactory::createEventFromStream(msgReceived, mChannel);
                        if ( eventRemote != nullptr )
                        {
                            eventRemote->deliverEvent();
                        }
                        else
                        {
                            failedProcessMessage(msgReceived);
                        }
                    }
                    else
                    {
                        LOG_WARN("The message [ %u ] was not processed on client service side", msgId);
                    }
                }
                break;
            }
        }
        else
        {
            LOG_WARN("Invalid message from host [ %s : %u ], ignore processing"
                        , whichSource.getAddress().getHostAddress().getString()
                        , whichSource.getAddress().getHostPort( ) );
        }
    }


    void RouterClient::processRemoteRequestEvent( RemoteRequestEvent & requestEvent)
    {
        LOG_SCOPE(areg_ipc_private_RouterClient_processRemoteRequestEvent);

        if ( requestEvent.isRemote() )
        {
            RemoteMessage data;
            if ( RemoteEventFactory::createStreamFromEvent( data, requestEvent, mChannel) )
            {
                LOG_DBG("Sending [ %s ] event: remote message [ %u ] from source [ %llu ] to target [ %llu ]"
                          , requestEvent.getRuntimeClassName().getString()
                          , data.getMessageId()
                          , data.getSource()
                          , data.getTarget());

                sendMessage(data);
            }
            else
            {
                LOG_ERR("Failed to create remote request data with message [ %u ]", requestEvent.getRequestId() );
            }
        }
        else
        {
            LOG_WARN("Request event with message [ %u ] is not remote, ignoring sending event", requestEvent.getRequestId());
        }
    }

    void RouterClient::processRemoteNotifyRequestEvent( RemoteNotifyRequestEvent & requestNotifyEvent )
    {
        LOG_SCOPE(areg_ipc_private_RouterClient_processRemoteNotifyRequestEvent);

        if ( requestNotifyEvent.isRemote() )
        {
            RemoteMessage data;
            if ( RemoteEventFactory::createStreamFromEvent( data, requestNotifyEvent, mChannel) )
            {
                LOG_DBG("Send [ %s ] event: remote message [ %u ] from source [ %llu ] to target [ %llu ]"
                          , requestNotifyEvent.getRuntimeClassName().getString()
                          , data.getMessageId()
                          , data.getSource()
                          , data.getTarget());

                sendMessage(data);
            }
            else
            {
                LOG_ERR("Failed to create remote notify request message [ %u ]", requestNotifyEvent.getRequestId() );
            }
        }
        else
        {
            LOG_WARN("Notify request [ %u ] is not remote, ignoring sending event", requestNotifyEvent.getRequestId());
        }
    }


    void RouterClient::processRemoteResponseEvent(RemoteResponseEvent & responseEvent)
    {
        LOG_SCOPE(areg_ipc_private_RouterClient_processRemoteResponseEvent);

        if ( responseEvent.isRemote() )
        {
            RemoteMessage data;
            if ( RemoteEventFactory::createStreamFromEvent( data, responseEvent, mChannel) )
            {
                LOG_DBG("Forwarding [ %s ] message [ %u ] from source [ %llu ] to target [ %llu ]"
                          , responseEvent.getRuntimeClassName().getString()
                          , data.getMessageId()
                          , data.getSource()
                          , data.getTarget());

                sendMessage(data);
            }
            else
            {
                LOG_ERR("Failed to create remote response message [ %u ]", responseEvent.getResponseId() );
            }
        }
        else
        {
            LOG_WARN("Response event with message [ %u ] is not remote, ignoring", responseEvent.getResponseId());
        }
    }

    bool RouterClient::postEvent(Event & eventElem)
    {
        if ( eventElem.isRemote() )
        {
            eventElem.setEventConsumer( static_cast<RemoteEventConsumer *>(this) );
        }

        return EventDispatcher::postEvent(eventElem);
    }

    void RouterClient::readyForEvents(bool isReady)
    {
        if (isReady)
        {
            registerForServiceClientCommands();
            DispatcherThread::readyForEvents(true);
            setConnectionState(ServiceClientConnectionBase::ConnectionPhase::DisconnectState);
        }
        else
        {
            DispatcherThread::readyForEvents(false);
            setConnectionState(ServiceClientConnectionBase::ConnectionPhase::ConnectionStopped);
            unregisterForServiceClientCommands();
        }
    }
} // namespace areg

