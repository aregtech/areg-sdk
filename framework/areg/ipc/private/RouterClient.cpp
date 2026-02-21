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
                                    , areg::eRemoteServices::ServiceRouter
                                    , static_cast<uint32_t>(areg::eConnectionTypes::ConnectTcpip)
                                    , areg::eMessageSource::MessageSourceClient
                                    , connectionConsumer
                                    , static_cast<RemoteMessageHandler &>(self())
                                    , static_cast<DispatcherThread &>(self())
                                    , RouterClient::PREFIX_THREAD)
    , RegistrationProvider     ( )
    , DispatcherThread              (String(RouterClient::PREFIX_THREAD) + areg::CLIENT_DISPATCH_MESSAGE_THREAD, areg::STACK_SIZE_DEFAULT, areg::QUEUE_SIZE_MAXIMUM)
    , RemoteEventConsumer         ( )

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
    return (isRunning() && ((mClientConnection.isValid() == false) || (getConnectionState() == ServiceClientConnectionBase::eConnectionState::ConnectionStarting)));
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

        result = sendMessage(areg::createRouterRegisterService(stubService, mClientConnection.getCookie(), areg::COOKIE_ROUTER), Event::eEventPriority::EventPriorityHigh );
    }

    return result;
}

void RouterClient::unregisterServiceProvider(const StubAddress & stubService, const areg::eDisconnectReason reason )
{
    LOG_SCOPE(areg_ipc_private_RouterClient_unregisterServiceProvider);

    Lock lock( mLock );
    if ( isConnectionStarted() )
    {
        LOG_DBG("Queuing to send unregister [ %s ] service message by connection [ %d ]"
                   , StubAddress::convAddressToPath(stubService).getString()
                   , mClientConnection.getCookie());

        sendMessage(areg::createRouterUnregisterService(stubService, reason, mClientConnection.getCookie(), areg::COOKIE_ROUTER) );
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

        result = sendMessage(areg::createRouterRegisterClient(proxyService, mClientConnection.getCookie(), areg::COOKIE_ROUTER), Event::eEventPriority::EventPriorityHigh);
    }

    return result;
}

void RouterClient::unregisterServiceConsumer(const ProxyAddress & proxyService, const areg::eDisconnectReason reason )
{
    LOG_SCOPE(areg_ipc_private_RouterClient_unregisterServiceConsumer);

    Lock lock( mLock );
    if ( isConnectionStarted() )
    {
        LOG_DBG("Queuing to send unregister [ %s ] service client message by connection [ %d ]"
                   , ProxyAddress::convAddressToPath(proxyService).getString()
                   , mClientConnection.getCookie());

        sendMessage(areg::createRouterUnregisterClient(proxyService, reason, mClientConnection.getCookie(), areg::COOKIE_ROUTER) );
    }
}

void RouterClient::failedSendMessage(const RemoteMessage & msgFailed, Socket & whichTarget )
{
    LOG_SCOPE(areg_ipc_private_RouterClient_failedSendMessage);

    if (Application::isServicingReady())
    {
        unsigned int msgId{ msgFailed.getMessageId() };
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
                sendCommand( ServiceEventData::eServiceEventCommands::CMD_ServiceLost, Event::eEventPriority::EventPriorityNormal );
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
            sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceLost, Event::eEventPriority::EventPriorityNormal);
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
        unsigned int msgId{ msgUnprocessed.getMessageId() };
        if ( areg::isExecutableId(msgId) )
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
        areg::eFuncIdRange msgId{ static_cast<areg::eFuncIdRange>(msgReceived.getMessageId()) };
        areg::eMessageResult result{ static_cast<areg::eMessageResult>(msgReceived.getResult()) };
        LOG_DBG("Processing received valid message [ %u ], result [ %s ]", msgId, areg::getString(result));

        switch ( msgId )
        {
        case areg::eFuncIdRange::SystemServiceNotifyConnection:
            serviceConnectionEvent(msgReceived);
            break;

        case areg::eFuncIdRange::SystemServiceNotifyRegister:
            {
                ASSERT( mClientConnection.getCookie() == msgReceived.getTarget() );
                areg::eServiceRequestType reqType;
                msgReceived >> reqType;
                LOG_DBG("Remote routing service registration notification of type [ %s ]", areg::getString(reqType));

                switch ( reqType )
                {
                case areg::eServiceRequestType::RegisterClient:
                    {
                        ProxyAddress proxy(msgReceived);
                        areg::eDisconnectReason reason { areg::eDisconnectReason::ReasonUndefined };
                        msgReceived >> reason;
                        proxy.setSource( mChannel.getSource() );
                        if ( result == areg::eMessageResult::ResultSucceed )
                        {
                            mRegisterConsumer.registeredRemoteServiceConsumer(proxy);
                        }
                        else
                        {
                            mRegisterConsumer.unregisteredRemoteServiceConsumer(proxy, reason, areg::COOKIE_ANY);
                        }
                    }
                    break;

                case areg::eServiceRequestType::RegisterStub:
                    {
                        StubAddress stub(msgReceived);
                        stub.setSource( mChannel.getSource() );
                        if ( result == areg::eMessageResult::ResultSucceed )
                        {
                            mRegisterConsumer.registeredRemoteServiceProvider( stub );
                        }
                        else
                        {
                            mRegisterConsumer.unregisteredRemoteServiceProvider( stub, areg::eDisconnectReason::ReasonUndefined, areg::COOKIE_ANY );
                        }
                    }
                    break;

                case areg::eServiceRequestType::UnregisterClient:
                    {
                        ProxyAddress proxy(msgReceived);
                        areg::eDisconnectReason reason { areg::eDisconnectReason::ReasonUndefined };
                        msgReceived >> reason;
                        proxy.setSource( mChannel.getSource() );
                        mRegisterConsumer.unregisteredRemoteServiceConsumer(proxy, reason, areg::COOKIE_ANY);
                    }
                    break;

                case areg::eServiceRequestType::UnregisterStub:
                    {
                        StubAddress stub(msgReceived);
                        areg::eDisconnectReason reason{areg::eDisconnectReason::ReasonUndefined};
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

        case areg::eFuncIdRange::ServiceLastId:                    // fall through
        case areg::eFuncIdRange::SystemServiceQueryInstances:      // fall through
        case areg::eFuncIdRange::SystemServiceRequestRegister:     // fall through
        case areg::eFuncIdRange::SystemServiceDisconnect:          // fall through
        case areg::eFuncIdRange::SystemServiceConnect:             // fall through
        case areg::eFuncIdRange::ResponseServiceProviderConnection:// fall through
        case areg::eFuncIdRange::RequestServiceProviderConnection: // fall through
        case areg::eFuncIdRange::ResponseServiceProviderVersion:   // fall through
        case areg::eFuncIdRange::RequestServiceProviderVersion:    // fall through
        case areg::eFuncIdRange::RequestRegisterService:           // fall through
        case areg::eFuncIdRange::ComponentCleanup:                 // fall through
        case areg::eFuncIdRange::SystemServiceNotifyInstances:     // fall through
        case areg::eFuncIdRange::ServiceLogRegisterScopes:         // fall through
        case areg::eFuncIdRange::ServiceLogUpdateScopes:           // fall through
        case areg::eFuncIdRange::ServiceLogQueryScopes:            // fall through
        case areg::eFuncIdRange::ServiceLogScopesUpdated:          // fall through
        case areg::eFuncIdRange::ServiceSaveLogConfiguration:      // fall through
        case areg::eFuncIdRange::ServiceLogConfigurationSaved:     // fall through
        case areg::eFuncIdRange::ServiceLogMessage:                // fall through
            break;

        case areg::eFuncIdRange::AttributeLastId:          // fall through
        case areg::eFuncIdRange::AttributeFirstId:         // fall through
        case areg::eFuncIdRange::ResponseLastId:           // fall through
        case areg::eFuncIdRange::ResponseFirstId:          // fall through
        case areg::eFuncIdRange::RequestLastId:            // fall through
        case areg::eFuncIdRange::RequestFirstId:           // fall through
        case areg::eFuncIdRange::EmptyFunctionId:          // fall through
        default:
            {
                if ( areg::isExecutableId(static_cast<unsigned int>(msgId)) )
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
        setConnectionState(ServiceClientConnectionBase::eConnectionState::DisconnectState);
    }
    else
    {
        DispatcherThread::readyForEvents(false);
        setConnectionState(ServiceClientConnectionBase::eConnectionState::ConnectionStopped);
        unregisterForServiceClientCommands();
    }
}

