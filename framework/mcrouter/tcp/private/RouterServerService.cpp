/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        mcrouter/tcp/private/RouterServerService.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Message router connection service.
 ************************************************************************/
#include "mcrouter/tcp/RouterServerService.hpp"

#include "areg/ipc/private/NEConnection.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/ipc/NERemoteService.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(mcrouter_tcp_RouterServerService_registerServiceProvider);
DEF_TRACE_SCOPE(mcrouter_tcp_RouterServerService_unregisterServiceProvider);
DEF_TRACE_SCOPE(mcrouter_tcp_RouterServerService_registerServiceConsumer);
DEF_TRACE_SCOPE(mcrouter_tcp_RouterServerService_unregisterServiceConsumer);

DEF_TRACE_SCOPE(mcrouter_tcp_RouterServerService_registeredRemoteServiceProvider);
DEF_TRACE_SCOPE(mcrouter_tcp_RouterServerService_registeredRemoteServiceConsumer);
DEF_TRACE_SCOPE(mcrouter_tcp_RouterServerService_unregisteredRemoteServiceProvider);
DEF_TRACE_SCOPE(mcrouter_tcp_RouterServerService_unregisteredRemoteServiceConsumer);

DEF_TRACE_SCOPE(mcrouter_tcp_RouterServerService_onServiceMessageReceived);
DEF_TRACE_SCOPE(mcrouter_tcp_RouterServerService_onServiceMessageSend);

//////////////////////////////////////////////////////////////////////////
// RouterServerService class implementation
//////////////////////////////////////////////////////////////////////////

RouterServerService::RouterServerService( void )
    : ServiceCommunicatonBase   ( NEService::COOKIE_ROUTER, NERemoteService::eRemoteServices::ServiceRouter, static_cast<uint32_t>(NERemoteService::eConnectionTypes::ConnectTcpip), NEConnection::SERVER_DISPATCH_MESSAGE_THREAD, ServiceCommunicatonBase::eConnectionBehavior::DefaultAccept )
    , IEServiceRegisterConsumer ( )
    , IEServiceRegisterProvider ( )

    , mServiceRegistry          ( )
{
}

bool RouterServerService::registerServiceProvider(const StubAddress & /* stubService */)
{
    TRACE_SCOPE(mcrouter_tcp_RouterServerService_registerServiceProvider);
    TRACE_ERR("Method is not implemented, this should not be called");
    return false;
}

void RouterServerService::unregisterServiceProvider(const StubAddress & /* stubService */, const NEService::eDisconnectReason /*reason*/ )
{
    TRACE_SCOPE(mcrouter_tcp_RouterServerService_unregisterServiceProvider);
    TRACE_ERR("Method is not implemented, this should not be called");
}

bool RouterServerService::registerServiceConsumer(const ProxyAddress & /* proxyService */)
{
    TRACE_SCOPE(mcrouter_tcp_RouterServerService_registerServiceConsumer);
    TRACE_ERR("Method is not implemented, this should not be called");
    return false;
}

void RouterServerService::unregisterServiceConsumer(const ProxyAddress & /* proxyService */, const NEService::eDisconnectReason /*reason*/ )
{
    TRACE_SCOPE(mcrouter_tcp_RouterServerService_unregisterServiceConsumer);
    TRACE_ERR("Method is not implemented, this should not be called");
}

void RouterServerService::onServiceMessageReceived(const RemoteMessage &msgReceived)
{
    TRACE_SCOPE(mcrouter_tcp_RouterServerService_onServiceMessageReceived);

    ASSERT( msgReceived.isValid() );
    NEService::eFuncIdRange msgId { static_cast<NEService::eFuncIdRange>(msgReceived.getMessageId()) };
    const ITEM_ID & source{ msgReceived.getSource() };

    TRACE_DBG("Processing received valid message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                    , NEService::getString(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(source)
                    , static_cast<uint32_t>(msgReceived.getTarget()));

    switch( msgId )
    {
    case NEService::eFuncIdRange::SystemServiceRequestRegister:
        {
            NEService::eServiceRequestType reqType;
            msgReceived >> reqType;
            TRACE_DBG("Routing service received registration request message [ %s ]", NEService::getString(reqType));
            switch ( reqType )
            {
            case NEService::eServiceRequestType::RegisterStub:
                {
                    StubAddress stubService(msgReceived);
                    stubService.setSource(source);
                    registeredRemoteServiceProvider(stubService);
                }
                break;

            case NEService::eServiceRequestType::RegisterClient:
                {
                    ProxyAddress proxyService(msgReceived);
                    proxyService.setSource(source);
                    registeredRemoteServiceConsumer(proxyService);
                }
                break;

            case NEService::eServiceRequestType::UnregisterStub:
                {
                    StubAddress stubService(msgReceived);
                    NEService::eDisconnectReason reason{NEService::eDisconnectReason::ReasonUndefined};
                    msgReceived >> reason;
                    stubService.setSource(source);
                    unregisteredRemoteServiceProvider(stubService, reason, stubService.getCookie());
                }
                break;

            case NEService::eServiceRequestType::UnregisterClient:
                {
                    ProxyAddress proxyService(msgReceived);
                    NEService::eDisconnectReason reason { NEService::eDisconnectReason::ReasonUndefined };
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

    case NEService::eFuncIdRange::SystemServiceDisconnect:
        {
            ITEM_ID cookie = NEService::COOKIE_UNKNOWN;
            msgReceived >> cookie;
            removeInstance(cookie);
            mServerConnection.closeConnection(cookie);

            TEArrayList<StubAddress>  listStubs;
            TEArrayList<ProxyAddress> listProxies;
            mServiceRegistry.getServiceSources(cookie, listStubs, listProxies);

            TRACE_DBG("Routing service received disconnect message from cookie [ %u ], [ %d ] stubs and [ %d ] proxies are going to be disconnected"
                        , static_cast<unsigned int>(cookie)
                        , listStubs.getSize()
                        , listProxies.getSize());

            for (uint32_t i = 0; i < listProxies.getSize(); ++i)
            {
                unregisteredRemoteServiceConsumer(listProxies[i], NEService::eDisconnectReason::ReasonConsumerDisconnected, cookie);
            }

            for (uint32_t i = 0; i < listStubs.getSize(); ++i)
            {
                unregisteredRemoteServiceProvider(listStubs[i], NEService::eDisconnectReason::ReasonProviderDisconnected, cookie);
            }
        }
        break;

    case NEService::eFuncIdRange::ServiceLastId:                    // fall through
    case NEService::eFuncIdRange::SystemServiceQueryInstances:      // fall through
    case NEService::eFuncIdRange::SystemServiceConnect:             // fall through
    case NEService::eFuncIdRange::RequestServiceProviderConnection: // fall through
    case NEService::eFuncIdRange::ResponseServiceProviderVersion:   // fall through
    case NEService::eFuncIdRange::RequestServiceProviderVersion:    // fall through
    case NEService::eFuncIdRange::RequestRegisterService:           // fall through
    case NEService::eFuncIdRange::ComponentCleanup:                 // fall through
    case NEService::eFuncIdRange::SystemServiceNotifyRegister:      // fall through
    case NEService::eFuncIdRange::SystemServiceNotifyConnection:
        break;

    case NEService::eFuncIdRange::ResponseServiceProviderConnection:// fall through
    case NEService::eFuncIdRange::AttributeLastId:                  // fall through
    case NEService::eFuncIdRange::AttributeFirstId:                 // fall through
    case NEService::eFuncIdRange::ResponseLastId:                   // fall through
    case NEService::eFuncIdRange::ResponseFirstId:                  // fall through
    case NEService::eFuncIdRange::RequestLastId:                    // fall through
    case NEService::eFuncIdRange::RequestFirstId:                   // fall through
    case NEService::eFuncIdRange::EmptyFunctionId:                  // fall through
    default:
        if ( NEService::isExecutableId(static_cast<uint32_t>(msgId)) || NEService::isConnectNotifyId( static_cast<uint32_t>(msgId)) )
        {
            TRACE_DBG("Message [ %u ] is executable, going to forward to target [ %u ], received from source [ %u ]"
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
            TRACE_ERR("Message [ %u ] is not executable, ignoring to forward to target [ %u ] from source [ %u ]"
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
    TRACE_SCOPE(mcrouter_tcp_RouterServerService_onServiceMessageSend);

    NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>( msgSend.getMessageId() );
    TRACE_DBG("Sending message [ %s ] of id [ 0x%X ] is going to send to target [ %u ] from source [ %u ]"
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
        TRACE_ERR("The message [ %u ] is neither executable, nor router notification. Ignoring sending message", static_cast<uint32_t>(msgId));
        ASSERT(false);
    }
}

void RouterServerService::disconnectServices(void)
{
    ServiceCommunicatonBase::disconnectServices( );

    TEArrayList<StubAddress>  stubList;
    TEArrayList<ProxyAddress> proxyList;
    extractRemoteServiceAddresses(NEService::COOKIE_ANY, stubList, proxyList);

    for ( uint32_t i = 0; i < stubList.getSize(); ++ i )
    {
        unregisteredRemoteServiceProvider( stubList[i], NEService::eDisconnectReason::ReasonServiceDisconnected, NEService::COOKIE_ANY );
    }

    for ( uint32_t i = 0; i < proxyList.getSize(); ++ i )
    {
        unregisteredRemoteServiceConsumer( proxyList[i], NEService::eDisconnectReason::ReasonServiceDisconnected, NEService::COOKIE_ANY );
    }

    mServiceRegistry.clear( );
}

void RouterServerService::extractRemoteServiceAddresses( const ITEM_ID & cookie, TEArrayList<StubAddress> & OUT out_listStubs, TEArrayList<ProxyAddress> & OUT out_lisProxies ) const
{
    mServiceRegistry.getServiceList(cookie, out_listStubs, out_lisProxies);
}

void RouterServerService::registeredRemoteServiceProvider(const StubAddress & stub)
{
    TRACE_SCOPE(mcrouter_tcp_RouterServerService_registeredRemoteServiceProvider);
    ASSERT(stub.isServicePublic());

    TRACE_DBG("Going to register remote stub [ %s ]", StubAddress::convAddressToPath(stub).getString());
    if ( mServiceRegistry.getServiceStatus(stub) != NEService::eServiceConnection::ServiceConnected )
    {
        ListServiceProxies listProxies;
        const ServiceStub & stubService = mServiceRegistry.registerServiceStub(stub, listProxies);
        if ( stubService.getServiceStatus() == NEService::eServiceConnection::ServiceConnected && listProxies.isEmpty() == false )
        {
            TRACE_DBG("Stub [ %s ] is connected, sending notification messages to [ %d ] waiting proxies"
                        , StubAddress::convAddressToPath(stubService.getServiceAddress()).getString()
                        , listProxies.getSize());

            TEArrayList<ITEM_ID> sendList;
            for (ListServiceProxiesBase::LISTPOS pos = listProxies.firstPosition(); listProxies.isValidPosition(pos); pos = listProxies.nextPosition(pos) )
            {
                const ServiceProxy & proxyService = listProxies.valueAtPosition(pos);
                const ProxyAddress & addrProxy    = proxyService.getServiceAddress();
                if ( (proxyService.getServiceStatus() == NEService::eServiceConnection::ServiceConnected) && (addrProxy.getSource() != stub.getSource()) )
                {
                    RemoteMessage msgRegisterProxy = NERemoteService::createServiceClientRegisteredNotification(addrProxy, mServerConnection.getChannelId(), stub.getSource());
                    sendMessage(msgRegisterProxy);

                    TRACE_DBG("Send to stub [ %s ] the proxy [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                                , stub.convToString().getString()
                                , addrProxy.convToString().getString()
                                , NEService::getString( static_cast<NEService::eFuncIdRange>(msgRegisterProxy.getMessageId()))
                                , static_cast<uint32_t>(msgRegisterProxy.getMessageId())
                                , static_cast<uint32_t>(msgRegisterProxy.getSource())
                                , static_cast<uint32_t>(msgRegisterProxy.getTarget()));

                    if ( sendList.addIfUnique(addrProxy.getSource()) )
                    {
                        RemoteMessage msgRegisterStub  = NERemoteService::createServiceRegisteredNotification(stub, mServerConnection.getChannelId(), addrProxy.getSource());
                        sendMessage(msgRegisterStub);

                        TRACE_DBG("Send to proxy [ %s ] the stub [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                                    , addrProxy.convToString().getString()
                                    , stub.convToString().getString()
                                    , NEService::getString( static_cast<NEService::eFuncIdRange>(msgRegisterStub.getMessageId()))
                                    , static_cast<uint32_t>(msgRegisterStub.getMessageId())
                                    , static_cast<uint32_t>(msgRegisterStub.getSource())
                                    , static_cast<uint32_t>(msgRegisterStub.getTarget()));
                    }
                    else
                    {
                        // ignore, it already has registered stub
                        TRACE_DBG("Ignoring sending stub registration message to target [ %u ], target already notified", static_cast<uint32_t>(addrProxy.getSource()));
                    }
                }
                else
                {
                     // ignore, it already has registered stub locally or proxy is not connected
                    TRACE_DBG("ignoring sending stub registration message, Stub [ %s ] and Proxy [ %s ] have same origin."
                                    , StubAddress::convAddressToPath(stub).getString()
                                    , ProxyAddress::convAddressToPath(addrProxy).getString());
                }
            }
        }
        else
        {
            // ignore, stub is not connected or list is empty
            TRACE_DBG("Ignoring sending stub [ %s ] connected notification, the proxy list is empty [ %s ]"
                            , StubAddress::convAddressToPath(stub).getString()
                            , listProxies.isEmpty() ? "YES" : "NO");
        }
    }
    else
    {
        // ignore, stub is already connected
        TRACE_DBG("Stub [ %s ] is already marked as connected, ignoring registration", StubAddress::convAddressToPath(stub).getString());
    }
}

void RouterServerService::registeredRemoteServiceConsumer(const ProxyAddress & proxy)
{
    TRACE_SCOPE(mcrouter_tcp_RouterServerService_registeredRemoteServiceConsumer);
    if ( mServiceRegistry.getServiceStatus(proxy) != NEService::eServiceConnection::ServiceConnected )
    {
        ServiceProxy proxyService;
        const ServiceStub & stubService   = mServiceRegistry.registerServiceProxy(proxy, proxyService);
        const StubAddress & addrStub      = stubService.getServiceAddress();

        TRACE_DBG("Registered proxy [ %s ], for connection with stub [ %s ], connection status is [ %s ]"
                    , ProxyAddress::convAddressToPath(proxy).getString()
                    , StubAddress::convAddressToPath(addrStub).getString()
                    , NEService::getString( proxyService.getServiceStatus()));

        if ( (proxyService.getServiceStatus() == NEService::eServiceConnection::ServiceConnected) && (proxy.getSource() != addrStub.getSource()) )
        {
            RemoteMessage msgRegisterProxy = NERemoteService::createServiceClientRegisteredNotification(proxy, mServerConnection.getChannelId(), addrStub.getSource());
            sendMessage(msgRegisterProxy);

            TRACE_DBG("Send to stub [ %s ] the proxy [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                        , addrStub.convToString().getString()
                        , proxy.convToString().getString()
                        , NEService::getString( static_cast<NEService::eFuncIdRange>(msgRegisterProxy.getMessageId()))
                        , static_cast<uint32_t>(msgRegisterProxy.getMessageId())
                        , static_cast<uint32_t>(msgRegisterProxy.getSource())
                        , static_cast<uint32_t>(msgRegisterProxy.getTarget()));

            RemoteMessage msgRegisterStub  = NERemoteService::createServiceRegisteredNotification(addrStub, mServerConnection.getChannelId(), proxy.getSource());
            sendMessage(msgRegisterStub);

            TRACE_DBG("Send to proxy [ %s ] the stub [ %s ] registration notification. Send message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                        , proxy.convToString().getString()
                        , addrStub.convToString().getString()
                        , NEService::getString( static_cast<NEService::eFuncIdRange>(msgRegisterStub.getMessageId()))
                        , static_cast<uint32_t>(msgRegisterStub.getMessageId())
                        , static_cast<uint32_t>(msgRegisterStub.getSource())
                        , static_cast<uint32_t>(msgRegisterStub.getTarget()));
        }
        else
        {
            // ignore, it is done locally
            TRACE_DBG("Ignore send stub registration message, Stub [ %s ] and Proxy [ %s ] have same origin."
                            , StubAddress::convAddressToPath(addrStub).getString()
                            , ProxyAddress::convAddressToPath(proxy).getString());
        }
    }
    else
    {
        TRACE_DBG("Proxy [ %s ] is already having connected status, ignoring registration", ProxyAddress::convAddressToPath(proxy).getString());
    }
}

void RouterServerService::unregisteredRemoteServiceProvider(const StubAddress & stub, NEService::eDisconnectReason reason, const ITEM_ID & cookie /*= NEService::COOKIE_ANY*/ )
{
    TRACE_SCOPE(mcrouter_tcp_RouterServerService_unregisteredRemoteServiceProvider);
    if ( mServiceRegistry.getServiceStatus(stub) == NEService::eServiceConnection::ServiceConnected )
    {
        ListServiceProxies listProxies;
        mServiceRegistry.unregisterServiceStub(stub, listProxies);
        TRACE_DBG("Unregistered stub [ %s ], [ %d ] proxies are going to be notified"
                        , stub.convToString().getString()
                        , listProxies.getSize());

        TRACE_DBG("Filter sources [ %u ] of proxy list", static_cast<unsigned int>(cookie));

        TEArrayList<ITEM_ID> sendList;
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

                    TRACE_INFO("Send stub [ %s ] disconnect message to proxy [ %s ]"
                                    , stub.convToString().getString()
                                    , addrProxy.convToString().getString());
                }
                else
                {
                    // ignore, it already has unregistered stub
                    TRACE_DBG("Ignore unregistered stub message for proxy [ %s ], it was already notified", addrProxy.convToString().getString());
                }
            }
            else
            {
                // ignore, it already has unregistered stub locally or proxy status did not changed
                ServiceProxy dummy;
                mServiceRegistry.unregisterServiceProxy(addrProxy, dummy);
                TRACE_DBG("Proxy [ %s ] is marked as ignored by source [ %u ], remove and skip", addrProxy.convToString().getString(), static_cast<unsigned int>(cookie));
            }
        }
    }
    else
    {
        // ignore, stub is already disconnected
        TRACE_DBG("Ignore unregistering stub [ %s ], it is already unregistered", stub.convToString().getString());
    }
}

void RouterServerService::unregisteredRemoteServiceConsumer(const ProxyAddress & proxy, NEService::eDisconnectReason reason, const ITEM_ID & cookie /*= NEService::COOKIE_ANY*/ )
{
    TRACE_SCOPE(mcrouter_tcp_RouterServerService_unregisteredRemoteServiceConsumer);
    TRACE_DBG("Unregistering services of proxy [ %s ] related to cookie [ %u ]"
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

    if ((svcStub->getServiceStatus() == NEService::eServiceConnection::ServiceConnected) && (proxy.getSource() != addrStub.getSource()))
    {
        sendMessage(NERemoteService::createServiceClientUnregisteredNotification( proxy, reason, mServerConnection.getChannelId(), addrStub.getSource( ) ) );

        TRACE_INFO("Send proxy [ %s ] disconnect message to stub [ %s ]"
                        , proxy.convToString().getString()
                        , addrStub.convToString().getString());
    }
    else
    {
        TRACE_DBG("Ignore notifying proxy [ %s ] disconnect"
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

void RouterServerService::onServiceConnectionStarted( void )
{
}

void RouterServerService::onServiceConnectionStopped( void )
{
}

void RouterServerService::onServiceConnectionLost( void )
{
}
