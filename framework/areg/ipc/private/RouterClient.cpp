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

//////////////////////////////////////////////////////////////////////////
// RouterClient class implementation
//////////////////////////////////////////////////////////////////////////

RouterClient::RouterClient(ConnectionConsumer& connectionConsumer, RegistrationConsumer& registerConsumer)
    : ServiceClientConnectionBase   ( areg::COOKIE_ROUTER
                                    , areg::RemoteServiceKind::Router
                                    , static_cast<uint32_t>(areg::ConnectionType::Tcpip)
                                    , areg::MessageSource::SourceClient
                                    , connectionConsumer
                                    , static_cast<RemoteMessageHandler &>(self())
                                    , static_cast<areg::DispatcherThread &>(self())
                                    , RouterClient::PREFIX_THREAD)
    , RegistrationProvider     ( )
    , areg::DispatcherThread              (areg::String(RouterClient::PREFIX_THREAD) + areg::CLIENT_DISPATCH_MESSAGE_THREAD, areg::STACK_SIZE_DEFAULT, areg::QUEUE_SIZE_MAXIMUM)
    , areg::RemoteEventConsumer         ( )

    , mRegisterConsumer (registerConsumer)
{
}

bool RouterClient::connectServiceHost()
{
    bool result{ true };
    if (isRunning() == false)
    {
        if (createThread(areg::WAIT_INFINITE) && waitForDispatcherStart(areg::WAIT_INFINITE))
        {
            result = ServiceClientConnectionBase::connectServiceHost();
        }
        else
        {
            shutdownThread(areg::WAIT_INFINITE);
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
        completionWait(areg::WAIT_INFINITE);
        shutdownThread(areg::DO_NOT_WAIT);
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

bool RouterClient::registerServiceProvider( const areg::StubAddress & stubService )
{
    LOG_SCOPE(areg_ipc_private_RouterClient_registerServiceProvider);
    Lock lock( mLock );
    bool result{ false };
    if ( isConnectionStarted() )
    {
        LOG_DBG("Queuing to send register [ %s ] service message by connection [ %d ]"
                   , areg::StubAddress::convAddressToPath(stubService).getString()
                   , mClientConnection.getCookie());

        result = sendMessage(areg::createRouterRegisterService(stubService, mClientConnection.getCookie(), areg::COOKIE_ROUTER), areg::Event::EventPriority::HighPrio );
    }

    return result;
}

void RouterClient::unregisterServiceProvider(const areg::StubAddress & stubService, const areg::DisconnectReason reason )
{
    LOG_SCOPE(areg_ipc_private_RouterClient_unregisterServiceProvider);

    Lock lock( mLock );
    if ( isConnectionStarted() )
    {
        LOG_DBG("Queuing to send unregister [ %s ] service message by connection [ %d ]"
                   , areg::StubAddress::convAddressToPath(stubService).getString()
                   , mClientConnection.getCookie());

        sendMessage(areg::createRouterUnregisterService(stubService, reason, mClientConnection.getCookie(), areg::COOKIE_ROUTER) );
    }
}

bool RouterClient::registerServiceConsumer(const areg::ProxyAddress & proxyService)
{
    LOG_SCOPE(areg_ipc_private_RouterClient_registerServiceConsumer );
    Lock lock( mLock );
    bool result { false };
    if ( isConnectionStarted() )
    {
        LOG_DBG("Queuing to send register [ %s ] service client message by connection [ %d ]"
                   , areg::ProxyAddress::convAddressToPath(proxyService).getString()
                   , mClientConnection.getCookie());

        result = sendMessage(areg::createRouterRegisterClient(proxyService, mClientConnection.getCookie(), areg::COOKIE_ROUTER), areg::Event::EventPriority::HighPrio);
    }

    return result;
}

void RouterClient::unregisterServiceConsumer(const areg::ProxyAddress & proxyService, const areg::DisconnectReason reason )
{
    LOG_SCOPE(areg_ipc_private_RouterClient_unregisterServiceConsumer);

    Lock lock( mLock );
    if ( isConnectionStarted() )
    {
        LOG_DBG("Queuing to send unregister [ %s ] service client message by connection [ %d ]"
                   , areg::ProxyAddress::convAddressToPath(proxyService).getString()
                   , mClientConnection.getCookie());

        sendMessage(areg::createRouterUnregisterClient(proxyService, reason, mClientConnection.getCookie(), areg::COOKIE_ROUTER) );
    }
}

void RouterClient::failedSendMessage(const areg::RemoteMessage & msgFailed, areg::Socket & whichTarget )
{
    LOG_SCOPE(areg_ipc_private_RouterClient_failedSendMessage);

    if (Application::isServicingReady())
    {
        uint32_t msgId{ msgFailed.getMessageId() };
        if ( areg::isExecutableId(msgId) || areg::isConnectNotifyId(msgId) )
        {
            LOG_WARN("Failed to send message [ %u ] to target [ %llu ], source is [ %llu ], the target socket [ %u ] is [ %s : %s ]"
                       , msgId
                       , msgFailed.getTarget()
                       , msgFailed.getSource()
                       , static_cast<uint32_t>(whichTarget.getHandle())
                       , whichTarget.isValid() ? "VALID" : "INVALID"
                       , whichTarget.isAlive() ? "ALIVE" : "DEAD");

            msgFailed.moveToBegin();
            areg::StreamableEvent * eventError = RemoteEventFactory::createRequestFailedEvent(msgFailed, mChannel);
            if ( eventError != nullptr )
            {
                LOG_DBG("Replying with failure event [ %s ]", eventError->getRuntimeClassName().getString());
                eventError->deliverEvent();
            }

            if ( whichTarget.isValid() && (whichTarget.isAlive() == false))
            {
                LOG_DBG("Trying to reconnect");
                cancelConnection( );
                sendCommand( ServiceEventData::ServiceCommand::CMD_ServiceLost, areg::Event::EventPriority::NormalPrio );
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

void RouterClient::failedReceiveMessage( areg::Socket & whichSource )
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
            sendCommand(ServiceEventData::ServiceCommand::CMD_ServiceLost, areg::Event::EventPriority::NormalPrio);
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

void RouterClient::failedProcessMessage( const areg::RemoteMessage & msgUnprocessed )
{
    LOG_SCOPE(areg_ipc_private_RouterClient_failedProcessMessage);

    if (Application::isServicingReady())
    {
        uint32_t msgId{ msgUnprocessed.getMessageId() };
        if ( areg::isExecutableId(msgId) )
        {
            LOG_DBG("The message [ %u ] for target [ %llu ] and from source [ %llu ] is unprocessed, replying with failed message"
                      , msgId
                      , msgUnprocessed.getTarget()
                      , msgUnprocessed.getSource());

            msgUnprocessed.moveToBegin();
            areg::StreamableEvent * eventError = RemoteEventFactory::createRequestFailedEvent(msgUnprocessed, mChannel);
            if ( eventError != nullptr )
            {
                areg::RemoteMessage data;
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

void RouterClient::processReceivedMessage( const areg::RemoteMessage & msgReceived, areg::Socket & whichSource )
{
    LOG_SCOPE(areg_ipc_private_RouterClient_processReceivedMessage);
    if ( msgReceived.isValid() && whichSource.isValid() )
    {
        areg::FuncIdRange msgId{ static_cast<areg::FuncIdRange>(msgReceived.getMessageId()) };
        areg::MessageResult result{ static_cast<areg::MessageResult>(msgReceived.getResult()) };
        LOG_DBG("Processing received valid message [ %u ], result [ %s ]", msgId, areg::getString(result));

        switch ( msgId )
        {
        case areg::FuncIdRange::SystemServiceNotifyConnection:
            serviceConnectionEvent(msgReceived);
            break;

        case areg::FuncIdRange::SystemServiceNotifyRegister:
            {
                ASSERT( mClientConnection.getCookie() == msgReceived.getTarget() );
                areg::RegistrationAction reqType;
                msgReceived >> reqType;
                LOG_DBG("Remote routing service registration notification of type [ %s ]", areg::getString(reqType));

                switch ( reqType )
                {
                case areg::RegistrationAction::RegisterClient:
                    {
                        areg::ProxyAddress proxy(msgReceived);
                        areg::DisconnectReason reason { areg::DisconnectReason::UndefinedReason };
                        msgReceived >> reason;
                        proxy.setSource( mChannel.getSource() );
                        if ( result == areg::MessageResult::Succeed )
                        {
                            mRegisterConsumer.registeredRemoteServiceConsumer(proxy);
                        }
                        else
                        {
                            mRegisterConsumer.unregisteredRemoteServiceConsumer(proxy, reason, areg::COOKIE_ANY);
                        }
                    }
                    break;

                case areg::RegistrationAction::RegisterStub:
                    {
                        areg::StubAddress stub(msgReceived);
                        stub.setSource( mChannel.getSource() );
                        if ( result == areg::MessageResult::Succeed )
                        {
                            mRegisterConsumer.registeredRemoteServiceProvider( stub );
                        }
                        else
                        {
                            mRegisterConsumer.unregisteredRemoteServiceProvider( stub, areg::DisconnectReason::UndefinedReason, areg::COOKIE_ANY );
                        }
                    }
                    break;

                case areg::RegistrationAction::UnregisterClient:
                    {
                        areg::ProxyAddress proxy(msgReceived);
                        areg::DisconnectReason reason { areg::DisconnectReason::UndefinedReason };
                        msgReceived >> reason;
                        proxy.setSource( mChannel.getSource() );
                        mRegisterConsumer.unregisteredRemoteServiceConsumer(proxy, reason, areg::COOKIE_ANY);
                    }
                    break;

                case areg::RegistrationAction::UnregisterStub:
                    {
                        areg::StubAddress stub(msgReceived);
                        areg::DisconnectReason reason{areg::DisconnectReason::UndefinedReason};
                        msgReceived >> reason;
                        stub.setSource( mChannel.getSource() );
                        mRegisterConsumer.unregisteredRemoteServiceProvider(stub, reason, areg::COOKIE_ANY);
                    }
                    break;

                default:
                    ASSERT(false);
                    break;
                }
            }
            break;

        case areg::FuncIdRange::ServiceLastId:                    // fall through
        case areg::FuncIdRange::SystemServiceQueryInstances:      // fall through
        case areg::FuncIdRange::SystemServiceRequestRegister:     // fall through
        case areg::FuncIdRange::SystemServiceDisconnect:          // fall through
        case areg::FuncIdRange::SystemServiceConnect:             // fall through
        case areg::FuncIdRange::ResponseServiceProviderConnection:// fall through
        case areg::FuncIdRange::RequestServiceProviderConnection: // fall through
        case areg::FuncIdRange::ResponseServiceProviderVersion:   // fall through
        case areg::FuncIdRange::RequestServiceProviderVersion:    // fall through
        case areg::FuncIdRange::RequestRegisterService:           // fall through
        case areg::FuncIdRange::ComponentCleanup:                 // fall through
        case areg::FuncIdRange::SystemServiceNotifyInstances:     // fall through
        case areg::FuncIdRange::ServiceLogRegisterScopes:         // fall through
        case areg::FuncIdRange::ServiceLogUpdateScopes:           // fall through
        case areg::FuncIdRange::ServiceLogQueryScopes:            // fall through
        case areg::FuncIdRange::ServiceLogScopesUpdated:          // fall through
        case areg::FuncIdRange::ServiceSaveLogConfiguration:      // fall through
        case areg::FuncIdRange::ServiceLogConfigurationSaved:     // fall through
        case areg::FuncIdRange::ServiceLogMessage:                // fall through
            break;

        case areg::FuncIdRange::AttributeLastId:          // fall through
        case areg::FuncIdRange::AttributeFirstId:         // fall through
        case areg::FuncIdRange::ResponseLastId:           // fall through
        case areg::FuncIdRange::ResponseFirstId:          // fall through
        case areg::FuncIdRange::RequestLastId:            // fall through
        case areg::FuncIdRange::RequestFirstId:           // fall through
        case areg::FuncIdRange::EmptyFunctionId:          // fall through
        default:
            {
                if ( areg::isExecutableId(static_cast<uint32_t>(msgId)) )
                {
                    areg::StreamableEvent * eventRemote = RemoteEventFactory::createEventFromStream(msgReceived, mChannel);
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
        areg::RemoteMessage data;
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
        areg::RemoteMessage data;
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


void RouterClient::processRemoteResponseEvent(areg::RemoteResponseEvent & responseEvent)
{
    LOG_SCOPE(areg_ipc_private_RouterClient_processRemoteResponseEvent);

    if ( responseEvent.isRemote() )
    {
        areg::RemoteMessage data;
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

bool RouterClient::postEvent(areg::Event & eventElem)
{
    if ( eventElem.isRemote() )
    {
        eventElem.setEventConsumer( static_cast<areg::RemoteEventConsumer *>(this) );
    }

    return areg::EventDispatcher::postEvent(eventElem);
}

void RouterClient::readyForEvents(bool isReady)
{
    if (isReady)
    {
        registerForServiceClientCommands();
        areg::DispatcherThread::readyForEvents(true);
        setConnectionState(ServiceClientConnectionBase::ConnectionPhase::DisconnectState);
    }
    else
    {
        areg::DispatcherThread::readyForEvents(false);
        setConnectionState(ServiceClientConnectionBase::ConnectionPhase::ConnectionStopped);
        unregisterForServiceClientCommands();
    }
}

