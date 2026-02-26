/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        mtrouter/service/private/RouterServerService.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Message router connection service.
 ************************************************************************/
#include "mtrouter/service/RouterServerService.hpp"

#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE(mtrouter_service_RouterServerService_registerServiceProvider);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService_unregisterServiceProvider);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService_registerServiceConsumer);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService_unregisterServiceConsumer);

DEF_LOG_SCOPE(mtrouter_service_RouterServerService_registeredRemoteServiceProvider);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService_registeredRemoteServiceConsumer);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService_unregisteredRemoteServiceProvider);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService_unregisteredRemoteServiceConsumer);

DEF_LOG_SCOPE(mtrouter_service_RouterServerService_onServiceMessageReceived);
DEF_LOG_SCOPE(mtrouter_service_RouterServerService_onServiceMessageSend);

//////////////////////////////////////////////////////////////////////////
// RouterServerService class implementation
//////////////////////////////////////////////////////////////////////////

RouterServerService::RouterServerService()
    : ServiceCommunicationBase   ( areg::COOKIE_ROUTER, areg::RemoteServiceKind::Router, static_cast<uint32_t>(areg::ConnectionType::Tcpip), areg::SERVER_DISPATCH_MESSAGE_THREAD, ServiceCommunicationBase::ConnectionPolicy::Accept )
    , RegistrationConsumer ( )
    , RegistrationProvider ( )

    , mServiceRegistry          ( )
{
}

bool RouterServerService::registerServiceProvider(const areg::StubAddress & /* stubService */)
{
    LOG_SCOPE(mtrouter_service_RouterServerService_registerServiceProvider);
    LOG_ERR("Method is not implemented, this should not be called");
    return false;
}

void RouterServerService::unregisterServiceProvider(const areg::StubAddress & /* stubService */, const areg::DisconnectReason /*reason*/ )
{
    LOG_SCOPE(mtrouter_service_RouterServerService_unregisterServiceProvider);
    LOG_ERR("Method is not implemented, this should not be called");
}

bool RouterServerService::registerServiceConsumer(const ProxyAddress & /* proxyService */)
{
    LOG_SCOPE(mtrouter_service_RouterServerService_registerServiceConsumer);
    LOG_ERR("Method is not implemented, this should not be called");
    return false;
}

void RouterServerService::unregisterServiceConsumer(const ProxyAddress & /* proxyService */, const areg::DisconnectReason /*reason*/ )
{
    LOG_SCOPE(mtrouter_service_RouterServerService_unregisterServiceConsumer);
    LOG_ERR("Method is not implemented, this should not be called");
}

void RouterServerService::onServiceMessageReceived(const areg::RemoteMessage &msgReceived)
{
    LOG_SCOPE(mtrouter_service_RouterServerService_onServiceMessageReceived);

    ASSERT( msgReceived.isValid() );
    areg::FuncIdRange msgId { static_cast<areg::FuncIdRange>(msgReceived.getMessageId()) };
    const ITEM_ID & source{ msgReceived.getSource() };

    LOG_DBG("Processing received valid message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                    , areg::getString(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(source)
                    , static_cast<uint32_t>(msgReceived.getTarget()));

    switch( msgId )
    {
    case areg::FuncIdRange::SystemServiceRequestRegister:
        {
            areg::RegistrationAction reqType;
            msgReceived >> reqType;
            LOG_DBG("Routing service received registration request message [ %s ]", areg::getString(reqType));
            switch ( reqType )
            {
            case areg::RegistrationAction::RegisterStub:
                {
                    areg::StubAddress stubService(msgReceived);
                    stubService.setSource(source);
                    registeredRemoteServiceProvider(stubService);
                }
                break;

            case areg::RegistrationAction::RegisterClient:
                {
                    ProxyAddress proxyService(msgReceived);
                    proxyService.setSource(source);
                    registeredRemoteServiceConsumer(proxyService);
                }
                break;

            case areg::RegistrationAction::UnregisterStub:
                {
                    areg::StubAddress stubService(msgReceived);
                    areg::DisconnectReason reason{areg::DisconnectReason::UndefinedReason};
                    msgReceived >> reason;
                    stubService.setSource(source);
                    unregisteredRemoteServiceProvider(stubService, reason, stubService.getCookie());
                }
                break;

            case areg::RegistrationAction::UnregisterClient:
                {
                    ProxyAddress proxyService(msgReceived);
                    areg::DisconnectReason reason { areg::DisconnectReason::UndefinedReason };
                    msgReceived >> reason;
                    proxyService.setSource(source);
                    unregisteredRemoteServiceConsumer(proxyService, reason, proxyService.getCookie());
                }
                break;

            default:
                ASSERT(false);
                break;  // do nothing
            }
        }
        break;

    case areg::FuncIdRange::SystemServiceDisconnect:
        {
            ITEM_ID cookie = areg::COOKIE_UNKNOWN;
            msgReceived >> cookie;
            removeInstance(cookie);
            mServerConnection.closeConnection(cookie);

            areg::ArrayList<areg::StubAddress>  listStubs;
            areg::ArrayList<ProxyAddress> listProxies;
            mServiceRegistry.getServiceSources(cookie, listStubs, listProxies);

            LOG_DBG("Routing service received disconnect message from cookie [ %u ], [ %d ] stubs and [ %d ] proxies are going to be disconnected"
                        , static_cast<uint32_t>(cookie)
                        , listStubs.getSize()
                        , listProxies.getSize());

            for (uint32_t i = 0; i < listProxies.getSize(); ++i)
            {
                unregisteredRemoteServiceConsumer(listProxies[i], areg::DisconnectReason::ConsumerDisconnected, cookie);
            }

            for (uint32_t i = 0; i < listStubs.getSize(); ++i)
            {
                unregisteredRemoteServiceProvider(listStubs[i], areg::DisconnectReason::ProviderDisconnected, cookie);
            }
        }
        break;

    case areg::FuncIdRange::ServiceLastId:                    // fall through
    case areg::FuncIdRange::SystemServiceQueryInstances:      // fall through
    case areg::FuncIdRange::SystemServiceConnect:             // fall through
    case areg::FuncIdRange::RequestServiceProviderConnection: // fall through
    case areg::FuncIdRange::ResponseServiceProviderVersion:   // fall through
    case areg::FuncIdRange::RequestServiceProviderVersion:    // fall through
    case areg::FuncIdRange::RequestRegisterService:           // fall through
    case areg::FuncIdRange::ComponentCleanup:                 // fall through
    case areg::FuncIdRange::SystemServiceNotifyRegister:      // fall through
    case areg::FuncIdRange::SystemServiceNotifyConnection:    // fall through
    case areg::FuncIdRange::SystemServiceNotifyInstances:     // fall through
    case areg::FuncIdRange::ServiceLogRegisterScopes:         // fall through
    case areg::FuncIdRange::ServiceLogUpdateScopes:           // fall through
    case areg::FuncIdRange::ServiceLogQueryScopes:            // fall through
    case areg::FuncIdRange::ServiceLogScopesUpdated:          // fall through
    case areg::FuncIdRange::ServiceSaveLogConfiguration:      // fall through
    case areg::FuncIdRange::ServiceLogConfigurationSaved:     // fall through
    case areg::FuncIdRange::ServiceLogMessage:                // fall through
        break;

    case areg::FuncIdRange::ResponseServiceProviderConnection:// fall through
    case areg::FuncIdRange::AttributeLastId:                  // fall through
    case areg::FuncIdRange::AttributeFirstId:                 // fall through
    case areg::FuncIdRange::ResponseLastId:                   // fall through
    case areg::FuncIdRange::ResponseFirstId:                  // fall through
    case areg::FuncIdRange::RequestLastId:                    // fall through
    case areg::FuncIdRange::RequestFirstId:                   // fall through
    case areg::FuncIdRange::EmptyFunctionId:                  // fall through
    default:
        if ( areg::isExecutableId(static_cast<uint32_t>(msgId)) || areg::isConnectNotifyId( static_cast<uint32_t>(msgId)) )
        {
            LOG_DBG("Message [ %u ] is executable, going to forward to target [ %u ], received from source [ %u ]"
                       , static_cast<uint32_t>(msgId)
                       , msgReceived.getTarget()
                       , source);

            if ( msgReceived.getTarget() != areg::TARGET_UNKNOWN )
            {
                sendMessage( msgReceived );
            }
        }
        else
        {
            LOG_ERR("Message [ %u ] is not executable, ignoring to forward to target [ %u ] from source [ %u ]"
                            , static_cast<uint32_t>(msgId)
                            , static_cast<uint32_t>(msgReceived.getTarget())
                            , static_cast<uint32_t>(source));
            ASSERT(false);
        }
        break;
    }
}

void RouterServerService::onServiceMessageSend(const areg::RemoteMessage &msgSend)
{
    LOG_SCOPE(mtrouter_service_RouterServerService_onServiceMessageSend);

    areg::FuncIdRange msgId = static_cast<areg::FuncIdRange>( msgSend.getMessageId() );
    LOG_DBG("Sending message [ %s ] of id [ 0x%X ] is going to send to target [ %u ] from source [ %u ]"
                    , areg::getString(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(msgSend.getTarget())
                    , static_cast<uint32_t>(msgSend.getSource()));

    if ( areg::isExecutableId( static_cast<uint32_t>(msgId)) )
    {
        if ( msgSend.getTarget( ) != areg::TARGET_UNKNOWN )
        {
            sendMessage( msgSend );
        }
    }
    else
    {
        LOG_ERR("The message [ %u ] is neither executable, nor router notification. Ignoring sending message", static_cast<uint32_t>(msgId));
        ASSERT(false);
    }
}

void RouterServerService::disconnectServices()
{
    ServiceCommunicationBase::disconnectServices( );

    areg::ArrayList<areg::StubAddress>  stubList;
    areg::ArrayList<ProxyAddress> proxyList;
    extractRemoteServiceAddresses(areg::COOKIE_ANY, stubList, proxyList);

    for ( uint32_t i = 0; i < stubList.getSize(); ++ i )
    {
        unregisteredRemoteServiceProvider( stubList[i], areg::DisconnectReason::ServiceDisconnected, areg::COOKIE_ANY );
    }

    for ( uint32_t i = 0; i < proxyList.getSize(); ++ i )
    {
        unregisteredRemoteServiceConsumer( proxyList[i], areg::DisconnectReason::ServiceDisconnected, areg::COOKIE_ANY );
    }

    mServiceRegistry.clear( );
}

void RouterServerService::extractRemoteServiceAddresses( const ITEM_ID & cookie, areg::ArrayList<areg::StubAddress> & out_listStubs, areg::ArrayList<ProxyAddress> & out_lisProxies ) const
{
    mServiceRegistry.getServiceList(cookie, out_listStubs, out_lisProxies);
}

void RouterServerService::registeredRemoteServiceProvider(const areg::StubAddress & stub)
{
    LOG_SCOPE(mtrouter_service_RouterServerService_registeredRemoteServiceProvider);
    ASSERT(stub.isServicePublic());

    LOG_DBG("Going to register remote stub [ %s ]", areg::StubAddress::convAddressToPath(stub).getString());
    if ( mServiceRegistry.getServiceStatus(stub) != areg::ServiceConnectionState::Connected )
    {
        ListServiceProxies listProxies;
        const ServiceStub & stubService = mServiceRegistry.registerServiceStub(stub, listProxies);
        if ( stubService.getServiceStatus() == areg::ServiceConnectionState::Connected && listProxies.isEmpty() == false )
        {
            LOG_DBG("Stub [ %s ] is connected, sending notification messages to [ %d ] waiting proxies"
                        , areg::StubAddress::convAddressToPath(stubService.getServiceAddress()).getString()
                        , listProxies.getSize());

            areg::ArrayList<ITEM_ID> sendList;
            for (ListServiceProxiesBase::LISTPOS pos = listProxies.firstPosition(); listProxies.isValidPosition(pos); pos = listProxies.nextPosition(pos) )
            {
                const ServiceProxy & proxyService = listProxies.valueAtPosition(pos);
                const ProxyAddress & addrProxy    = proxyService.getServiceAddress();
                if ( (proxyService.getServiceStatus() == areg::ServiceConnectionState::Connected) && (addrProxy.getSource() != stub.getSource()) )
                {
                    areg::RemoteMessage msgRegisterProxy = areg::createServiceClientRegisteredNotification(addrProxy, mServerConnection.getChannelId(), stub.getSource());
                    sendMessage(msgRegisterProxy);

                    LOG_DBG("Send to stub [ %s ] the proxy [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                                , stub.convToString().getString()
                                , addrProxy.convToString().getString()
                                , areg::getString( static_cast<areg::FuncIdRange>(msgRegisterProxy.getMessageId()))
                                , static_cast<uint32_t>(msgRegisterProxy.getMessageId())
                                , static_cast<uint32_t>(msgRegisterProxy.getSource())
                                , static_cast<uint32_t>(msgRegisterProxy.getTarget()));

                    if ( sendList.addIfUnique(addrProxy.getSource()) )
                    {
                        areg::RemoteMessage msgRegisterStub  = areg::createServiceRegisteredNotification(stub, mServerConnection.getChannelId(), addrProxy.getSource());
                        sendMessage(msgRegisterStub);

                        LOG_DBG("Send to proxy [ %s ] the stub [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                                    , addrProxy.convToString().getString()
                                    , stub.convToString().getString()
                                    , areg::getString( static_cast<areg::FuncIdRange>(msgRegisterStub.getMessageId()))
                                    , static_cast<uint32_t>(msgRegisterStub.getMessageId())
                                    , static_cast<uint32_t>(msgRegisterStub.getSource())
                                    , static_cast<uint32_t>(msgRegisterStub.getTarget()));
                    }
                    else
                    {
                        // ignore, it already has registered stub
                        LOG_DBG("Ignoring sending stub registration message to target [ %u ], target already notified", static_cast<uint32_t>(addrProxy.getSource()));
                    }
                }
                else
                {
                     // ignore, it already has registered stub locally or proxy is not connected
                    LOG_DBG("ignoring sending stub registration message, Stub [ %s ] and Proxy [ %s ] have same origin."
                                    , areg::StubAddress::convAddressToPath(stub).getString()
                                    , ProxyAddress::convAddressToPath(addrProxy).getString());
                }
            }
        }
        else
        {
            // ignore, stub is not connected or list is empty
            LOG_DBG("Ignoring sending stub [ %s ] connected notification, the proxy list is empty [ %s ]"
                            , areg::StubAddress::convAddressToPath(stub).getString()
                            , listProxies.isEmpty() ? "YES" : "NO");
        }
    }
    else
    {
        // ignore, stub is already connected
        LOG_DBG("Stub [ %s ] is already marked as connected, ignoring registration", areg::StubAddress::convAddressToPath(stub).getString());
    }
}

void RouterServerService::registeredRemoteServiceConsumer(const ProxyAddress & proxy)
{
    LOG_SCOPE(mtrouter_service_RouterServerService_registeredRemoteServiceConsumer);
    if ( mServiceRegistry.getServiceStatus(proxy) != areg::ServiceConnectionState::Connected )
    {
        ServiceProxy proxyService;
        const ServiceStub & stubService   = mServiceRegistry.registerServiceProxy(proxy, proxyService);
        const areg::StubAddress & addrStub      = stubService.getServiceAddress();

        LOG_DBG("Registered proxy [ %s ], for connection with stub [ %s ], connection status is [ %s ]"
                    , ProxyAddress::convAddressToPath(proxy).getString()
                    , areg::StubAddress::convAddressToPath(addrStub).getString()
                    , areg::getString( proxyService.getServiceStatus()));

        if ( (proxyService.getServiceStatus() == areg::ServiceConnectionState::Connected) && (proxy.getSource() != addrStub.getSource()) )
        {
            areg::RemoteMessage msgRegisterProxy = areg::createServiceClientRegisteredNotification(proxy, mServerConnection.getChannelId(), addrStub.getSource());
            sendMessage(msgRegisterProxy);

            LOG_DBG("Send to stub [ %s ] the proxy [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                        , addrStub.convToString().getString()
                        , proxy.convToString().getString()
                        , areg::getString( static_cast<areg::FuncIdRange>(msgRegisterProxy.getMessageId()))
                        , static_cast<uint32_t>(msgRegisterProxy.getMessageId())
                        , static_cast<uint32_t>(msgRegisterProxy.getSource())
                        , static_cast<uint32_t>(msgRegisterProxy.getTarget()));

            areg::RemoteMessage msgRegisterStub  = areg::createServiceRegisteredNotification(addrStub, mServerConnection.getChannelId(), proxy.getSource());
            sendMessage(msgRegisterStub);

            LOG_DBG("Send to proxy [ %s ] the stub [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                        , proxy.convToString().getString()
                        , addrStub.convToString().getString()
                        , areg::getString( static_cast<areg::FuncIdRange>(msgRegisterStub.getMessageId()))
                        , static_cast<uint32_t>(msgRegisterStub.getMessageId())
                        , static_cast<uint32_t>(msgRegisterStub.getSource())
                        , static_cast<uint32_t>(msgRegisterStub.getTarget()));
        }
        else
        {
            // ignore, it is done locally
            LOG_DBG("Ignore send stub registration message, Stub [ %s ] and Proxy [ %s ] have same origin."
                            , areg::StubAddress::convAddressToPath(addrStub).getString()
                            , ProxyAddress::convAddressToPath(proxy).getString());
        }
    }
    else
    {
        LOG_DBG("Proxy [ %s ] is already having connected status, ignoring registration", ProxyAddress::convAddressToPath(proxy).getString());
    }
}

void RouterServerService::unregisteredRemoteServiceProvider(const areg::StubAddress & stub, areg::DisconnectReason reason, const ITEM_ID & cookie /*= areg::COOKIE_ANY*/ )
{
    LOG_SCOPE(mtrouter_service_RouterServerService_unregisteredRemoteServiceProvider);
    if ( mServiceRegistry.getServiceStatus(stub) == areg::ServiceConnectionState::Connected )
    {
        ListServiceProxies listProxies;
        mServiceRegistry.unregisterServiceStub(stub, listProxies);
        LOG_DBG("Unregistered stub [ %s ], [ %d ] proxies are going to be notified"
                        , stub.convToString().getString()
                        , listProxies.getSize());

        LOG_DBG("Filter sources [ %u ] of proxy list", static_cast<uint32_t>(cookie));

        areg::ArrayList<ITEM_ID> sendList;
        for (ListServiceProxiesBase::LISTPOS pos = listProxies.firstPosition(); listProxies.isValidPosition(pos); pos = listProxies.nextPosition(pos) )
        {
            const ServiceProxy & proxyService = listProxies.valueAtPosition(pos);
            const ProxyAddress & addrProxy    = proxyService.getServiceAddress();

            if ( (cookie == areg::COOKIE_ANY) || (addrProxy.getSource() != cookie) )
            {
                // no need to send message to unregistered stub, only to proxy side
                if (sendList.addIfUnique(addrProxy.getSource()) )
                {
                    sendMessage(areg::createServiceUnregisteredNotification( stub, reason, mServerConnection.getChannelId(), addrProxy.getSource( ) ) );

                    LOG_INFO("Send stub [ %s ] disconnect message to proxy [ %s ]"
                                    , stub.convToString().getString()
                                    , addrProxy.convToString().getString());
                }
                else
                {
                    // ignore, it already has unregistered stub
                    LOG_DBG("Ignore unregistered stub message for proxy [ %s ], it was already notified", addrProxy.convToString().getString());
                }
            }
            else
            {
                // ignore, it already has unregistered stub locally or proxy status did not changed
                ServiceProxy dummy;
                mServiceRegistry.unregisterServiceProxy(addrProxy, dummy);
                LOG_DBG("Proxy [ %s ] is marked as ignored by source [ %u ], remove and skip", addrProxy.convToString().getString(), static_cast<uint32_t>(cookie));
            }
        }
    }
    else
    {
        // ignore, stub is already disconnected
        LOG_DBG("Ignore unregistering stub [ %s ], it is already unregistered", stub.convToString().getString());
    }
}

void RouterServerService::unregisteredRemoteServiceConsumer(const ProxyAddress & proxy, areg::DisconnectReason reason, const ITEM_ID & cookie /*= areg::COOKIE_ANY*/ )
{
    LOG_SCOPE(mtrouter_service_RouterServerService_unregisteredRemoteServiceConsumer);
    LOG_DBG("Unregistering services of proxy [ %s ] related to cookie [ %u ]"
                    , ProxyAddress::convAddressToPath(proxy).getString()
                    , static_cast<uint32_t>(cookie));

    areg::RemoteMessage msgRegisterProxy;
    ServiceProxy svcProxy;
    const ServiceStub * svcStub     = nullptr;

    if (proxy.getSource() == cookie)
    {
        svcStub = &mServiceRegistry.unregisterServiceProxy(proxy, svcProxy);
    }
    else
    {
        svcStub = &mServiceRegistry.disconnectProxy(proxy);
    }

    ASSERT(svcStub != nullptr);
    const areg::StubAddress & addrStub    = svcStub->getServiceAddress();

    if ((svcStub->getServiceStatus() == areg::ServiceConnectionState::Connected) && (proxy.getSource() != addrStub.getSource()))
    {
        sendMessage(areg::createServiceClientUnregisteredNotification( proxy, reason, mServerConnection.getChannelId(), addrStub.getSource( ) ) );

        LOG_INFO("Send proxy [ %s ] disconnect message to stub [ %s ]"
                        , proxy.convToString().getString()
                        , addrStub.convToString().getString());
    }
    else
    {
        LOG_DBG("Ignore notifying proxy [ %s ] disconnect"
                        , proxy.convToString().getString());
    }
}

void RouterServerService::connectedRemoteServiceChannel(const areg::Channel & /* channel */)
{

}

void RouterServerService::disconnectedRemoteServiceChannel(const areg::Channel & /* channel */)
{

}

void RouterServerService::lostRemoteServiceChannel(const areg::Channel & /* channel */)
{
}

void RouterServerService::failedProcessMessage(const areg::RemoteMessage & /* msgUnprocessed */)
{
}

void RouterServerService::onServiceConnectionStarted()
{
}

void RouterServerService::onServiceConnectionStopped()
{
}

void RouterServerService::onServiceConnectionLost()
{
}
