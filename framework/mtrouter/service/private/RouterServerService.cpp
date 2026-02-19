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
    : ServiceCommunicationBase   ( NEService::COOKIE_ROUTER, NERemoteService::RemoteServiceKind::Router, static_cast<uint32_t>(NERemoteService::ConnectionType::Tcpip), NEConnection::SERVER_DISPATCH_MESSAGE_THREAD, ServiceCommunicationBase::eConnectionBehavior::DefaultAccept )
    , RegistrationConsumer ( )
    , RegistrationProvider ( )

    , mServiceRegistry          ( )
{
}

bool RouterServerService::registerServiceProvider(const StubAddress & /* stubService */)
{
    LOG_SCOPE(mtrouter_service_RouterServerService_registerServiceProvider);
    LOG_ERR("Method is not implemented, this should not be called");
    return false;
}

void RouterServerService::unregisterServiceProvider(const StubAddress & /* stubService */, const NEService::DisconnectReason /*reason*/ )
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

void RouterServerService::unregisterServiceConsumer(const ProxyAddress & /* proxyService */, const NEService::DisconnectReason /*reason*/ )
{
    LOG_SCOPE(mtrouter_service_RouterServerService_unregisterServiceConsumer);
    LOG_ERR("Method is not implemented, this should not be called");
}

void RouterServerService::onServiceMessageReceived(const RemoteMessage &msgReceived)
{
    LOG_SCOPE(mtrouter_service_RouterServerService_onServiceMessageReceived);

    ASSERT( msgReceived.isValid() );
    NEService::FuncIdRange msgId { static_cast<NEService::FuncIdRange>(msgReceived.getMessageId()) };
    const ITEM_ID & source{ msgReceived.getSource() };

    LOG_DBG("Processing received valid message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                    , NEService::getString(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(source)
                    , static_cast<uint32_t>(msgReceived.getTarget()));

    switch( msgId )
    {
    case NEService::FuncIdRange::SystemServiceRequestRegister:
        {
            NEService::RegistrationAction reqType;
            msgReceived >> reqType;
            LOG_DBG("Routing service received registration request message [ %s ]", NEService::getString(reqType));
            switch ( reqType )
            {
            case NEService::RegistrationAction::RegisterStub:
                {
                    StubAddress stubService(msgReceived);
                    stubService.setSource(source);
                    registeredRemoteServiceProvider(stubService);
                }
                break;

            case NEService::RegistrationAction::RegisterClient:
                {
                    ProxyAddress proxyService(msgReceived);
                    proxyService.setSource(source);
                    registeredRemoteServiceConsumer(proxyService);
                }
                break;

            case NEService::RegistrationAction::UnregisterStub:
                {
                    StubAddress stubService(msgReceived);
                    NEService::DisconnectReason reason{NEService::DisconnectReason::UndefinedReason};
                    msgReceived >> reason;
                    stubService.setSource(source);
                    unregisteredRemoteServiceProvider(stubService, reason, stubService.getCookie());
                }
                break;

            case NEService::RegistrationAction::UnregisterClient:
                {
                    ProxyAddress proxyService(msgReceived);
                    NEService::DisconnectReason reason { NEService::DisconnectReason::UndefinedReason };
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

    case NEService::FuncIdRange::SystemServiceDisconnect:
        {
            ITEM_ID cookie = NEService::COOKIE_UNKNOWN;
            msgReceived >> cookie;
            removeInstance(cookie);
            mServerConnection.closeConnection(cookie);

            ArrayList<StubAddress>  listStubs;
            ArrayList<ProxyAddress> listProxies;
            mServiceRegistry.getServiceSources(cookie, listStubs, listProxies);

            LOG_DBG("Routing service received disconnect message from cookie [ %u ], [ %d ] stubs and [ %d ] proxies are going to be disconnected"
                        , static_cast<unsigned int>(cookie)
                        , listStubs.getSize()
                        , listProxies.getSize());

            for (uint32_t i = 0; i < listProxies.getSize(); ++i)
            {
                unregisteredRemoteServiceConsumer(listProxies[i], NEService::DisconnectReason::ConsumerDisconnected, cookie);
            }

            for (uint32_t i = 0; i < listStubs.getSize(); ++i)
            {
                unregisteredRemoteServiceProvider(listStubs[i], NEService::DisconnectReason::ProviderDisconnected, cookie);
            }
        }
        break;

    case NEService::FuncIdRange::ServiceLastId:                    // fall through
    case NEService::FuncIdRange::SystemServiceQueryInstances:      // fall through
    case NEService::FuncIdRange::SystemServiceConnect:             // fall through
    case NEService::FuncIdRange::RequestServiceProviderConnection: // fall through
    case NEService::FuncIdRange::ResponseServiceProviderVersion:   // fall through
    case NEService::FuncIdRange::RequestServiceProviderVersion:    // fall through
    case NEService::FuncIdRange::RequestRegisterService:           // fall through
    case NEService::FuncIdRange::ComponentCleanup:                 // fall through
    case NEService::FuncIdRange::SystemServiceNotifyRegister:      // fall through
    case NEService::FuncIdRange::SystemServiceNotifyConnection:    // fall through
    case NEService::FuncIdRange::SystemServiceNotifyInstances:     // fall through
    case NEService::FuncIdRange::ServiceLogRegisterScopes:         // fall through
    case NEService::FuncIdRange::ServiceLogUpdateScopes:           // fall through
    case NEService::FuncIdRange::ServiceLogQueryScopes:            // fall through
    case NEService::FuncIdRange::ServiceLogScopesUpdated:          // fall through
    case NEService::FuncIdRange::ServiceSaveLogConfiguration:      // fall through
    case NEService::FuncIdRange::ServiceLogConfigurationSaved:     // fall through
    case NEService::FuncIdRange::ServiceLogMessage:                // fall through
        break;

    case NEService::FuncIdRange::ResponseServiceProviderConnection:// fall through
    case NEService::FuncIdRange::AttributeLastId:                  // fall through
    case NEService::FuncIdRange::AttributeFirstId:                 // fall through
    case NEService::FuncIdRange::ResponseLastId:                   // fall through
    case NEService::FuncIdRange::ResponseFirstId:                  // fall through
    case NEService::FuncIdRange::RequestLastId:                    // fall through
    case NEService::FuncIdRange::RequestFirstId:                   // fall through
    case NEService::FuncIdRange::EmptyFunctionId:                  // fall through
    default:
        if ( NEService::isExecutableId(static_cast<uint32_t>(msgId)) || NEService::isConnectNotifyId( static_cast<uint32_t>(msgId)) )
        {
            LOG_DBG("Message [ %u ] is executable, going to forward to target [ %u ], received from source [ %u ]"
                       , static_cast<uint32_t>(msgId)
                       , msgReceived.getTarget()
                       , source);

            if ( msgReceived.getTarget() != NEService::TARGET_UNKNOWN )
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

void RouterServerService::onServiceMessageSend(const RemoteMessage &msgSend)
{
    LOG_SCOPE(mtrouter_service_RouterServerService_onServiceMessageSend);

    NEService::FuncIdRange msgId = static_cast<NEService::FuncIdRange>( msgSend.getMessageId() );
    LOG_DBG("Sending message [ %s ] of id [ 0x%X ] is going to send to target [ %u ] from source [ %u ]"
                    , NEService::getString(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(msgSend.getTarget())
                    , static_cast<uint32_t>(msgSend.getSource()));

    if ( NEService::isExecutableId( static_cast<uint32_t>(msgId)) )
    {
        if ( msgSend.getTarget( ) != NEService::TARGET_UNKNOWN )
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

    ArrayList<StubAddress>  stubList;
    ArrayList<ProxyAddress> proxyList;
    extractRemoteServiceAddresses(NEService::COOKIE_ANY, stubList, proxyList);

    for ( uint32_t i = 0; i < stubList.getSize(); ++ i )
    {
        unregisteredRemoteServiceProvider( stubList[i], NEService::DisconnectReason::ServiceDisconnected, NEService::COOKIE_ANY );
    }

    for ( uint32_t i = 0; i < proxyList.getSize(); ++ i )
    {
        unregisteredRemoteServiceConsumer( proxyList[i], NEService::DisconnectReason::ServiceDisconnected, NEService::COOKIE_ANY );
    }

    mServiceRegistry.clear( );
}

void RouterServerService::extractRemoteServiceAddresses( const ITEM_ID & cookie, ArrayList<StubAddress> & out_listStubs, ArrayList<ProxyAddress> & out_lisProxies ) const
{
    mServiceRegistry.getServiceList(cookie, out_listStubs, out_lisProxies);
}

void RouterServerService::registeredRemoteServiceProvider(const StubAddress & stub)
{
    LOG_SCOPE(mtrouter_service_RouterServerService_registeredRemoteServiceProvider);
    ASSERT(stub.isServicePublic());

    LOG_DBG("Going to register remote stub [ %s ]", StubAddress::convAddressToPath(stub).getString());
    if ( mServiceRegistry.getServiceStatus(stub) != NEService::ServiceConnectionState::Connected )
    {
        ListServiceProxies listProxies;
        const ServiceStub & stubService = mServiceRegistry.registerServiceStub(stub, listProxies);
        if ( stubService.getServiceStatus() == NEService::ServiceConnectionState::Connected && listProxies.isEmpty() == false )
        {
            LOG_DBG("Stub [ %s ] is connected, sending notification messages to [ %d ] waiting proxies"
                        , StubAddress::convAddressToPath(stubService.getServiceAddress()).getString()
                        , listProxies.getSize());

            ArrayList<ITEM_ID> sendList;
            for (ListServiceProxiesBase::LISTPOS pos = listProxies.firstPosition(); listProxies.isValidPosition(pos); pos = listProxies.nextPosition(pos) )
            {
                const ServiceProxy & proxyService = listProxies.valueAtPosition(pos);
                const ProxyAddress & addrProxy    = proxyService.getServiceAddress();
                if ( (proxyService.getServiceStatus() == NEService::ServiceConnectionState::Connected) && (addrProxy.getSource() != stub.getSource()) )
                {
                    RemoteMessage msgRegisterProxy = NERemoteService::createServiceClientRegisteredNotification(addrProxy, mServerConnection.getChannelId(), stub.getSource());
                    sendMessage(msgRegisterProxy);

                    LOG_DBG("Send to stub [ %s ] the proxy [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                                , stub.convToString().getString()
                                , addrProxy.convToString().getString()
                                , NEService::getString( static_cast<NEService::FuncIdRange>(msgRegisterProxy.getMessageId()))
                                , static_cast<uint32_t>(msgRegisterProxy.getMessageId())
                                , static_cast<uint32_t>(msgRegisterProxy.getSource())
                                , static_cast<uint32_t>(msgRegisterProxy.getTarget()));

                    if ( sendList.addIfUnique(addrProxy.getSource()) )
                    {
                        RemoteMessage msgRegisterStub  = NERemoteService::createServiceRegisteredNotification(stub, mServerConnection.getChannelId(), addrProxy.getSource());
                        sendMessage(msgRegisterStub);

                        LOG_DBG("Send to proxy [ %s ] the stub [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                                    , addrProxy.convToString().getString()
                                    , stub.convToString().getString()
                                    , NEService::getString( static_cast<NEService::FuncIdRange>(msgRegisterStub.getMessageId()))
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
                                    , StubAddress::convAddressToPath(stub).getString()
                                    , ProxyAddress::convAddressToPath(addrProxy).getString());
                }
            }
        }
        else
        {
            // ignore, stub is not connected or list is empty
            LOG_DBG("Ignoring sending stub [ %s ] connected notification, the proxy list is empty [ %s ]"
                            , StubAddress::convAddressToPath(stub).getString()
                            , listProxies.isEmpty() ? "YES" : "NO");
        }
    }
    else
    {
        // ignore, stub is already connected
        LOG_DBG("Stub [ %s ] is already marked as connected, ignoring registration", StubAddress::convAddressToPath(stub).getString());
    }
}

void RouterServerService::registeredRemoteServiceConsumer(const ProxyAddress & proxy)
{
    LOG_SCOPE(mtrouter_service_RouterServerService_registeredRemoteServiceConsumer);
    if ( mServiceRegistry.getServiceStatus(proxy) != NEService::ServiceConnectionState::Connected )
    {
        ServiceProxy proxyService;
        const ServiceStub & stubService   = mServiceRegistry.registerServiceProxy(proxy, proxyService);
        const StubAddress & addrStub      = stubService.getServiceAddress();

        LOG_DBG("Registered proxy [ %s ], for connection with stub [ %s ], connection status is [ %s ]"
                    , ProxyAddress::convAddressToPath(proxy).getString()
                    , StubAddress::convAddressToPath(addrStub).getString()
                    , NEService::getString( proxyService.getServiceStatus()));

        if ( (proxyService.getServiceStatus() == NEService::ServiceConnectionState::Connected) && (proxy.getSource() != addrStub.getSource()) )
        {
            RemoteMessage msgRegisterProxy = NERemoteService::createServiceClientRegisteredNotification(proxy, mServerConnection.getChannelId(), addrStub.getSource());
            sendMessage(msgRegisterProxy);

            LOG_DBG("Send to stub [ %s ] the proxy [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                        , addrStub.convToString().getString()
                        , proxy.convToString().getString()
                        , NEService::getString( static_cast<NEService::FuncIdRange>(msgRegisterProxy.getMessageId()))
                        , static_cast<uint32_t>(msgRegisterProxy.getMessageId())
                        , static_cast<uint32_t>(msgRegisterProxy.getSource())
                        , static_cast<uint32_t>(msgRegisterProxy.getTarget()));

            RemoteMessage msgRegisterStub  = NERemoteService::createServiceRegisteredNotification(addrStub, mServerConnection.getChannelId(), proxy.getSource());
            sendMessage(msgRegisterStub);

            LOG_DBG("Send to proxy [ %s ] the stub [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                        , proxy.convToString().getString()
                        , addrStub.convToString().getString()
                        , NEService::getString( static_cast<NEService::FuncIdRange>(msgRegisterStub.getMessageId()))
                        , static_cast<uint32_t>(msgRegisterStub.getMessageId())
                        , static_cast<uint32_t>(msgRegisterStub.getSource())
                        , static_cast<uint32_t>(msgRegisterStub.getTarget()));
        }
        else
        {
            // ignore, it is done locally
            LOG_DBG("Ignore send stub registration message, Stub [ %s ] and Proxy [ %s ] have same origin."
                            , StubAddress::convAddressToPath(addrStub).getString()
                            , ProxyAddress::convAddressToPath(proxy).getString());
        }
    }
    else
    {
        LOG_DBG("Proxy [ %s ] is already having connected status, ignoring registration", ProxyAddress::convAddressToPath(proxy).getString());
    }
}

void RouterServerService::unregisteredRemoteServiceProvider(const StubAddress & stub, NEService::DisconnectReason reason, const ITEM_ID & cookie /*= NEService::COOKIE_ANY*/ )
{
    LOG_SCOPE(mtrouter_service_RouterServerService_unregisteredRemoteServiceProvider);
    if ( mServiceRegistry.getServiceStatus(stub) == NEService::ServiceConnectionState::Connected )
    {
        ListServiceProxies listProxies;
        mServiceRegistry.unregisterServiceStub(stub, listProxies);
        LOG_DBG("Unregistered stub [ %s ], [ %d ] proxies are going to be notified"
                        , stub.convToString().getString()
                        , listProxies.getSize());

        LOG_DBG("Filter sources [ %u ] of proxy list", static_cast<unsigned int>(cookie));

        ArrayList<ITEM_ID> sendList;
        for (ListServiceProxiesBase::LISTPOS pos = listProxies.firstPosition(); listProxies.isValidPosition(pos); pos = listProxies.nextPosition(pos) )
        {
            const ServiceProxy & proxyService = listProxies.valueAtPosition(pos);
            const ProxyAddress & addrProxy    = proxyService.getServiceAddress();

            if ( (cookie == NEService::COOKIE_ANY) || (addrProxy.getSource() != cookie) )
            {
                // no need to send message to unregistered stub, only to proxy side
                if (sendList.addIfUnique(addrProxy.getSource()) )
                {
                    sendMessage(NERemoteService::createServiceUnregisteredNotification( stub, reason, mServerConnection.getChannelId(), addrProxy.getSource( ) ) );

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
                LOG_DBG("Proxy [ %s ] is marked as ignored by source [ %u ], remove and skip", addrProxy.convToString().getString(), static_cast<unsigned int>(cookie));
            }
        }
    }
    else
    {
        // ignore, stub is already disconnected
        LOG_DBG("Ignore unregistering stub [ %s ], it is already unregistered", stub.convToString().getString());
    }
}

void RouterServerService::unregisteredRemoteServiceConsumer(const ProxyAddress & proxy, NEService::DisconnectReason reason, const ITEM_ID & cookie /*= NEService::COOKIE_ANY*/ )
{
    LOG_SCOPE(mtrouter_service_RouterServerService_unregisteredRemoteServiceConsumer);
    LOG_DBG("Unregistering services of proxy [ %s ] related to cookie [ %u ]"
                    , ProxyAddress::convAddressToPath(proxy).getString()
                    , static_cast<unsigned int>(cookie));

    RemoteMessage msgRegisterProxy;
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
    const StubAddress & addrStub    = svcStub->getServiceAddress();

    if ((svcStub->getServiceStatus() == NEService::ServiceConnectionState::Connected) && (proxy.getSource() != addrStub.getSource()))
    {
        sendMessage(NERemoteService::createServiceClientUnregisteredNotification( proxy, reason, mServerConnection.getChannelId(), addrStub.getSource( ) ) );

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

void RouterServerService::connectedRemoteServiceChannel(const Channel & /* channel */)
{

}

void RouterServerService::disconnectedRemoteServiceChannel(const Channel & /* channel */)
{

}

void RouterServerService::lostRemoteServiceChannel(const Channel & /* channel */)
{
}

void RouterServerService::failedProcessMessage(const RemoteMessage & /* msgUnprocessed */)
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
