/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/RouterClient.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service Connection client implementation
 ************************************************************************/
#include "areg/ipc/private/RouterClient.hpp"

#include "areg/ipc/IEServiceRegisterConsumer.hpp"
#include "areg/ipc/private/NEConnection.hpp"

#include "areg/component/RemoteEventFactory.hpp"
#include "areg/component/StreamableEvent.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/NEService.hpp"
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

RouterClient::RouterClient(IEServiceConnectionConsumer& connectionConsumer, IEServiceRegisterConsumer& registerConsumer)
    : ServiceClientConnectionBase   ( NEService::COOKIE_ROUTER
                                    , NERemoteService::eRemoteServices::ServiceRouter
                                    , static_cast<uint32_t>(NERemoteService::eConnectionTypes::ConnectTcpip)
                                    , NEService::eMessageSource::MessageSourceClient
                                    , connectionConsumer
                                    , static_cast<IERemoteMessageHandler &>(self())
                                    , static_cast<DispatcherThread &>(self())
                                    , RouterClient::PREFIX_THREAD)
    , IEServiceRegisterProvider     ( )
    , DispatcherThread              (String(RouterClient::PREFIX_THREAD) + NEConnection::CLIENT_DISPATCH_MESSAGE_THREAD, NECommon::STACK_SIZE_DEFAULT, NECommon::QUEUE_SIZE_MAXIMUM)
    , IERemoteEventConsumer         ( )

    , mRegisterConsumer (registerConsumer)
{
}

bool RouterClient::connectServiceHost(void)
{
    bool result{ true };
    if (isRunning() == false)
    {
        if (createThread(NECommon::WAIT_INFINITE) && waitForDispatcherStart(NECommon::WAIT_INFINITE))
        {
            result = ServiceClientConnectionBase::connectServiceHost();
        }
        else
        {
            shutdownThread(NECommon::WAIT_INFINITE);
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

void RouterClient::disconnectServiceHost(void)
{
    if (isRunning())
    {
        ServiceClientConnectionBase::disconnectServiceHost();
        completionWait(NECommon::WAIT_INFINITE);
        shutdownThread(NECommon::DO_NOT_WAIT);
    }
}

void RouterClient::onServiceExit(void)
{
    ServiceClientConnectionBase::onServiceExit();
    triggerExit();
}

bool RouterClient::isServiceHostPending(void) const
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

        result = sendMessage(NERemoteService::createRouterRegisterService(stubService, mClientConnection.getCookie(), NEService::COOKIE_ROUTER), Event::eEventPriority::EventPriorityHigh );
    }

    return result;
}

void RouterClient::unregisterServiceProvider(const StubAddress & stubService, const NEService::eDisconnectReason reason )
{
    LOG_SCOPE(areg_ipc_private_RouterClient_unregisterServiceProvider);

    Lock lock( mLock );
    if ( isConnectionStarted() )
    {
        LOG_DBG("Queuing to send unregister [ %s ] service message by connection [ %d ]"
                   , StubAddress::convAddressToPath(stubService).getString()
                   , mClientConnection.getCookie());

        sendMessage(NERemoteService::createRouterUnregisterService(stubService, reason, mClientConnection.getCookie(), NEService::COOKIE_ROUTER) );
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

        result = sendMessage(NERemoteService::createRouterRegisterClient(proxyService, mClientConnection.getCookie(), NEService::COOKIE_ROUTER), Event::eEventPriority::EventPriorityHigh);
    }

    return result;
}

void RouterClient::unregisterServiceConsumer(const ProxyAddress & proxyService, const NEService::eDisconnectReason reason )
{
    LOG_SCOPE(areg_ipc_private_RouterClient_unregisterServiceConsumer);

    Lock lock( mLock );
    if ( isConnectionStarted() )
    {
        LOG_DBG("Queuing to send unregister [ %s ] service client message by connection [ %d ]"
                   , ProxyAddress::convAddressToPath(proxyService).getString()
                   , mClientConnection.getCookie());

        sendMessage(NERemoteService::createRouterUnregisterClient(proxyService, reason, mClientConnection.getCookie(), NEService::COOKIE_ROUTER) );
    }
}

void RouterClient::failedSendMessage(const RemoteMessage & msgFailed, Socket & whichTarget )
{
    LOG_SCOPE(areg_ipc_private_RouterClient_failedSendMessage);

    if (Application::isServicingReady())
    {
        unsigned int msgId{ msgFailed.getMessageId() };
        if ( NEService::isExecutableId(msgId) || NEService::isConnectNotifyId(msgId) )
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
        if ( NEService::isExecutableId(msgId) )
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
        NEService::eFuncIdRange msgId{ static_cast<NEService::eFuncIdRange>(msgReceived.getMessageId()) };
        NEMemory::eMessageResult result{ static_cast<NEMemory::eMessageResult>(msgReceived.getResult()) };
        LOG_DBG("Processing received valid message [ %u ], result [ %s ]", msgId, NEMemory::getString(result));

        switch ( msgId )
        {
        case NEService::eFuncIdRange::SystemServiceNotifyConnection:
            serviceConnectionEvent(msgReceived);
            break;

        case NEService::eFuncIdRange::SystemServiceNotifyRegister:
            {
                ASSERT( mClientConnection.getCookie() == msgReceived.getTarget() );
                NEService::eServiceRequestType reqType;
                msgReceived >> reqType;
                LOG_DBG("Remote routing service registration notification of type [ %s ]", NEService::getString(reqType));

                switch ( reqType )
                {
                case NEService::eServiceRequestType::RegisterClient:
                    {
                        ProxyAddress proxy(msgReceived);
                        NEService::eDisconnectReason reason { NEService::eDisconnectReason::ReasonUndefined };
                        msgReceived >> reason;
                        proxy.setSource( mChannel.getSource() );
                        if ( result == NEMemory::eMessageResult::ResultSucceed )
                        {
                            mRegisterConsumer.registeredRemoteServiceConsumer(proxy);
                        }
                        else
                        {
                            mRegisterConsumer.unregisteredRemoteServiceConsumer(proxy, reason, NEService::COOKIE_ANY);
                        }
                    }
                    break;

                case NEService::eServiceRequestType::RegisterStub:
                    {
                        StubAddress stub(msgReceived);
                        stub.setSource( mChannel.getSource() );
                        if ( result == NEMemory::eMessageResult::ResultSucceed )
                        {
                            mRegisterConsumer.registeredRemoteServiceProvider( stub );
                        }
                        else
                        {
                            mRegisterConsumer.unregisteredRemoteServiceProvider( stub, NEService::eDisconnectReason::ReasonUndefined, NEService::COOKIE_ANY );
                        }
                    }
                    break;

                case NEService::eServiceRequestType::UnregisterClient:
                    {
                        ProxyAddress proxy(msgReceived);
                        NEService::eDisconnectReason reason { NEService::eDisconnectReason::ReasonUndefined };
                        msgReceived >> reason;
                        proxy.setSource( mChannel.getSource() );
                        mRegisterConsumer.unregisteredRemoteServiceConsumer(proxy, reason, NEService::COOKIE_ANY);
                    }
                    break;

                case NEService::eServiceRequestType::UnregisterStub:
                    {
                        StubAddress stub(msgReceived);
                        NEService::eDisconnectReason reason{NEService::eDisconnectReason::ReasonUndefined};
                        msgReceived >> reason;
                        stub.setSource( mChannel.getSource() );
                        mRegisterConsumer.unregisteredRemoteServiceProvider(stub, reason, NEService::COOKIE_ANY);
                    }
                    break;

                default:
                    ASSERT(false);
                    break;
                }
            }
            break;

        case NEService::eFuncIdRange::ServiceLastId:                    // fall through
        case NEService::eFuncIdRange::SystemServiceQueryInstances:      // fall through
        case NEService::eFuncIdRange::SystemServiceRequestRegister:     // fall through
        case NEService::eFuncIdRange::SystemServiceDisconnect:          // fall through
        case NEService::eFuncIdRange::SystemServiceConnect:             // fall through
        case NEService::eFuncIdRange::ResponseServiceProviderConnection:// fall through
        case NEService::eFuncIdRange::RequestServiceProviderConnection: // fall through
        case NEService::eFuncIdRange::ResponseServiceProviderVersion:   // fall through
        case NEService::eFuncIdRange::RequestServiceProviderVersion:    // fall through
        case NEService::eFuncIdRange::RequestRegisterService:           // fall through
        case NEService::eFuncIdRange::ComponentCleanup:                 // fall through
        case NEService::eFuncIdRange::SystemServiceNotifyInstances:     // fall through
        case NEService::eFuncIdRange::ServiceLogRegisterScopes:         // fall through
        case NEService::eFuncIdRange::ServiceLogUpdateScopes:           // fall through
        case NEService::eFuncIdRange::ServiceLogQueryScopes:            // fall through
        case NEService::eFuncIdRange::ServiceLogScopesUpdated:          // fall through
        case NEService::eFuncIdRange::ServiceSaveLogConfiguration:      // fall through
        case NEService::eFuncIdRange::ServiceLogConfigurationSaved:     // fall through
        case NEService::eFuncIdRange::ServiceLogMessage:                // fall through
            break;

        case NEService::eFuncIdRange::AttributeLastId:          // fall through
        case NEService::eFuncIdRange::AttributeFirstId:         // fall through
        case NEService::eFuncIdRange::ResponseLastId:           // fall through
        case NEService::eFuncIdRange::ResponseFirstId:          // fall through
        case NEService::eFuncIdRange::RequestLastId:            // fall through
        case NEService::eFuncIdRange::RequestFirstId:           // fall through
        case NEService::eFuncIdRange::EmptyFunctionId:          // fall through
        default:
            {
                if ( NEService::isExecutableId(static_cast<unsigned int>(msgId)) )
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
        eventElem.setEventConsumer( static_cast<IERemoteEventConsumer *>(this) );
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

