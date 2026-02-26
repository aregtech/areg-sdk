/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceManagerEventProcessor.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The Service Manager processing helper class.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/component/private/ServiceManagerEventProcessor.hpp"

#include "areg/base/IOStream.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/private/ProxyConnectEvent.hpp"
#include "areg/component/private/StubConnectEvent.hpp"
#include "areg/component/private/ServiceManager.hpp"
#include "areg/ipc/ConnectionConsumer.hpp"
#include "areg/ipc/RegistrationProvider.hpp"

#include "areg/logging/GELog.h"

DEF_LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor__registerServer );
DEF_LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor__unregisterServer );
DEF_LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor__registerClient );
DEF_LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor__unregisterClient );
DEF_LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor__sendClientConnectedEvent );
DEF_LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor__sendClientDisconnectedEvent );
DEF_LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor__terminateComponentThread );
DEF_LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor__startComponentThread );


ServiceManagerEventProcessor::ServiceManagerEventProcessor( ServiceManager & serviceManager )
    : mServiceManager   ( serviceManager )
    , mServerList       ( )
{
}

void ServiceManagerEventProcessor::processServiceEvent(   ServiceManagerEventData::ServiceManagerCommand cmdService
                                                        , const areg::InStream& stream
                                                        , areg::ConnectionProvider& connectProvider
                                                        , areg::RegistrationProvider& registerProvider )
{
    switch ( cmdService )
    {
    case ServiceManagerEventData::ServiceManagerCommand::CMD_ShutdownService:
        {
            mServerList.clear( );
            connectProvider.disconnectServiceHost( );
            mServiceManager.removeAllEvents( );
            mServiceManager.triggerExit( );
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_StopRoutingClient:
        {
            for ( auto mapPos = mServerList.firstPosition( ); mServerList.isValidPosition( mapPos ); mapPos = mServerList.nextPosition( mapPos ) )
            {
                ServerInfo si;
                ClientList clientList;

                mServerList.getAtPosition( mapPos, si, clientList );
                for ( auto listPos = clientList.firstPosition( ); clientList.isValidPosition( listPos ); listPos = clientList.nextPosition( listPos ) )
                {
                    const ClientInfo & client = clientList.valueAtPosition( listPos );
                    if ( client.isConnected( ) )
                    {
                        _sendClientDisconnectEvent( client.getAddress(), si.getAddress( ), areg::ServiceConnectionState::Disconnected );
                    }
                }
            }

            mServerList.clear( );
            connectProvider.disconnectServiceHost( );
            mServiceManager.removeEvents( false );
            mServiceManager.pulseExit( );
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterProxy:
        {
            areg::ProxyAddress  addrProxy;
            areg::Channel       channel;
            stream >> addrProxy;
            stream >> channel;
            addrProxy.setChannel( channel );
            _registerClient( addrProxy, registerProvider);
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterProxy:
        {
            areg::ProxyAddress  addrProxy;
            areg::Channel       channel;
            areg::DisconnectReason reason{areg::DisconnectReason::UndefinedReason};
            stream >> addrProxy;
            stream >> channel;
            stream >> reason;
            addrProxy.setChannel( channel );
            _unregisterClient( addrProxy, reason, registerProvider);
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterStub:
        {
            areg::StubAddress   addrstub;
            areg::Channel       channel;
            stream >> addrstub;
            stream >> channel;
            addrstub.setChannel( channel );
            _registerServer( addrstub, registerProvider);
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterStub:
        {
            areg::StubAddress   addrstub;
            areg::Channel       channel;
            areg::DisconnectReason reason{areg::DisconnectReason::UndefinedReason};
            stream >> addrstub;
            stream >> channel;
            stream >> reason;
            addrstub.setChannel( channel );
            _unregisterServer( addrstub, reason, registerProvider);
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_ConfigureConnection:
        {
            areg::RemoteServiceKind service{ areg::RemoteServiceKind::Unknown };
            uint32_t connectTypes{ static_cast<uint32_t>(areg::ConnectionType::Undefined) };
            stream >> service;
            stream >> connectTypes;

            connectProvider.setupServiceConnectionData(service, connectTypes);
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_StartConnection:
        {
            areg::RemoteServiceKind service{ areg::RemoteServiceKind::Unknown };
            uint32_t connectTypes{ static_cast<uint32_t>(areg::ConnectionType::Undefined) };
            stream >> service;
            stream >> connectTypes;

            if (connectProvider.setupServiceConnectionData(service, connectTypes))
            {
                connectProvider.connectServiceHost();
            }
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_StartNetConnection:
        {
            areg::String   ipAddress;
            uint16_t portNr = 0;
            stream >> ipAddress;
            stream >> portNr;

            connectProvider.applyServiceConnectionData( ipAddress, portNr );
            if (connectProvider.isServiceHostSetup( ) )
            {
                connectProvider.connectServiceHost( );
            }
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_StopConnection:
        {
            connectProvider.disconnectServiceHost( );
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterConnection:
        {
            for ( ServerList::MAPPOS posMap = mServerList.firstPosition( ); mServerList.isValidPosition( posMap ); posMap = mServerList.nextPosition( posMap ) )
            {
                const areg::StubAddress & server = mServerList.keyAtPosition( posMap ).getAddress( );
                const ClientList & clientList = mServerList.valueAtPosition( posMap );

                if ( server.isServicePublic( ) && server.isLocalAddress( ) && server.isValid( ) )
                {
                    registerProvider.registerServiceProvider( server );
                }

                for ( ClientList::LISTPOS pos = clientList.firstPosition( ); clientList.isValidPosition( pos ); pos = clientList.nextPosition( pos ) )
                {
                    const areg::ProxyAddress & proxy = clientList.valueAtPosition( pos ).getAddress( );
                    if ( proxy.isServicePublic( ) && (proxy.isTargetLocal( ) == false) && proxy.isValid( ) )
                    {
                        registerProvider.registerServiceConsumer( proxy );
                    }
                }
            }
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterConnection:
    case ServiceManagerEventData::ServiceManagerCommand::CMD_LostConnection:
        {
            // Create service provider and service consumer list
            // to be able to unregister entries, because they are removing
            // elements from the existing list and it may invalidate position object.
            areg::ArrayList<areg::StubAddress> stubList;
            areg::ArrayList<areg::ProxyAddress> proxyList;
            for ( ServerList::MAPPOS posMap = mServerList.firstPosition( ); mServerList.isValidPosition( posMap ); posMap = mServerList.nextPosition( posMap ) )
            {
                const areg::StubAddress & server = mServerList.keyAtPosition( posMap ).getAddress( );
                const ClientList & clientList = mServerList.valueAtPosition( posMap );

                if ( server.isServicePublic( ) && server.isRemoteAddress( ) && server.isValid( ) )
                {
                    stubList.add( server );
                }

                for ( ClientList::LISTPOS pos = clientList.firstPosition( ); clientList.isValidPosition( pos ); pos = clientList.nextPosition( pos ) )
                {
                    const areg::ProxyAddress & proxy = clientList.valueAtPosition( pos ).getAddress( );
                    if ( proxy.isServicePublic( ) && proxy.isRemoteAddress( ) && proxy.isValid( ) )
                    {
                        proxyList.add( proxy );
                    }
                }
            }

            areg::DisconnectReason reason { areg::DisconnectReason::ProviderDisconnected };
            if ( cmdService == ServiceManagerEventData::ServiceManagerCommand::CMD_LostConnection )
            {
                reason = areg::DisconnectReason::ServiceLost;
            }

            for ( uint32_t i = 0; i < stubList.getSize( ); ++i )
            {
                _unregisterServer( stubList[ i ], reason, registerProvider);
            }

            for ( uint32_t i = 0; i < proxyList.getSize( ); ++i )
            {
                _unregisterClient( proxyList[ i ], reason, registerProvider);
            }
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_TerminateComponentThread:
        {
            areg::String threadName;
            stream >> threadName;
            if ( _terminateComponentThread( threadName ) )
            {
                ServiceManager::_requestCreateThread( threadName );
            }
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_StartComponentThread:
        {
            areg::String threadName;
            stream >> threadName;
            _startComponentThread( threadName );
        }
        break;

    default:
        ASSERT( false );
        break;
    }
}

void ServiceManagerEventProcessor::_registerServer( const areg::StubAddress & whichServer, areg::RegistrationProvider& registerProvider)
{
    LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor__registerServer );

    if ( whichServer.isLocalAddress( ) && whichServer.isServicePublic( ) )
    {
        registerProvider.registerServiceProvider( whichServer );
    }

    ClientList clientList;

#if AREG_LOGS
    const ServerInfo & server = mServerList.registerServer( whichServer, clientList );
    LOG_DBG( "Server [ %s ] is registered. Connection status [ %s ], there are [ %d ] waiting clients"
               , areg::StubAddress::convAddressToPath( server.getAddress( ) ).getString( )
               , areg::getString( server.getConnectionStatus( ) )
               , clientList.getSize( ) );
#else   // !AREG_LOGS
    mServerList.registerServer( whichServer, clientList );
#endif  // !AREG_LOGS

    for ( ClientList::LISTPOS pos = clientList.firstPosition( ); clientList.isValidPosition( pos ); pos = clientList.nextPosition( pos ) )
    {
        const ClientInfo & client{ clientList.valueAtPosition( pos ) };
        if ( client.isConnected( ) )
        {
            _sendClientConnectedEvent( client.getAddress( ), whichServer );
        }
    }
}

void ServiceManagerEventProcessor::_unregisterServer( const areg::StubAddress & whichServer, const areg::DisconnectReason reason, areg::RegistrationProvider& registerProvider)
{
    LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor__unregisterServer );

    if ( whichServer.isLocalAddress( ) && whichServer.isServicePublic( ) )
    {
        registerProvider.unregisterServiceProvider( whichServer, reason );
    }

    ClientList clientList;

#if AREG_LOGS
    ServerInfo server( mServerList.unregisterServer( whichServer, clientList ) );
    LOG_DBG( "Server [ %s ] is unregistered with reason [ %s ]. The service connection status was [ %s ], there are [ %d ] waiting clients"
               , areg::StubAddress::convAddressToPath( server.getAddress( ) ).getString( )
               , areg::getString( reason )
               , areg::getString( server.getConnectionStatus( ) )
               , clientList.getSize( ) );

#else   // AREG_LOGS

    static_cast<void>(mServerList.unregisterServer( whichServer, clientList ));

#endif  // AREG_LOGS

    areg::ServiceConnectionState status = areg::serviceConnection( reason );
    for ( ClientList::LISTPOS pos = clientList.firstPosition( ); clientList.isValidPosition( pos ); pos = clientList.nextPosition( pos ) )
    {
        const ClientInfo & client{ clientList.valueAtPosition( pos ) };
        if ( client.isConnected( ) )
        {
            _sendClientDisconnectEvent( client.getAddress( ), whichServer, status );
        }
    }
}

void ServiceManagerEventProcessor::_registerClient( const areg::ProxyAddress & whichClient, areg::RegistrationProvider& registerProvider)
{
    LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor__registerClient );

    if ( whichClient.isLocalAddress( ) && whichClient.isServicePublic( ) )
    {
        registerProvider.registerServiceConsumer( whichClient );
    }

    ClientInfo client;
    const ServerInfo & server = mServerList.registerClient( whichClient, client );

    LOG_DBG( "Client [ %s ] is registered for server [ %s ], connection status [ %s ]"
               , areg::ProxyAddress::convAddressToPath( client.getAddress( ) ).getString( )
               , areg::StubAddress::convAddressToPath( server.getAddress( ) ).getString( )
               , areg::getString( client.getConnectionStatus( ) ) );

    if ( client.isConnected( ) )
    {
        _sendClientConnectedEvent( client.getAddress(), server.getAddress( ) );
    }
    else
    {
        LOG_INFO( "The Proxy [ %s ] has NO CONNECTION yet. Noting to send", areg::ProxyAddress::convAddressToPath( client.getAddress( ) ).getString( ) );
    }
}

void ServiceManagerEventProcessor::_unregisterClient( const areg::ProxyAddress & whichClient, const areg::DisconnectReason reason, areg::RegistrationProvider& registerProvider)
{
    LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor__unregisterClient );

    if ( whichClient.isLocalAddress( ) && whichClient.isServicePublic( ) )
    {
        registerProvider.unregisterServiceConsumer( whichClient, reason );
    }

    ClientInfo client;

    ServerInfo server = mServerList.unregisterClient( whichClient, client );
    LOG_DBG( "Client [ %s ] is unregistered from server [ %s ], connection status [ %s ]"
               , areg::ProxyAddress::convAddressToPath( client.getAddress( ) ).getString( )
               , areg::StubAddress::convAddressToPath( server.getAddress( ) ).getString( )
               , areg::getString( client.getConnectionStatus( ) ) );

    // Unregister client first, then send event that client does not receive notification
    if ( client.isConnected( ) )
    {
        _sendClientDisconnectEvent( client.getAddress(), server.getAddress( ), areg::serviceConnection( reason ) );
    }
    else
    {
        LOG_INFO( "The Proxy [ %s ] has NO CONNECTION. Noting to send", areg::ProxyAddress::convAddressToPath( client.getAddress( ) ).getString( ) );
    }
}

void ServiceManagerEventProcessor::_sendClientConnectedEvent( const areg::ProxyAddress & client, const areg::StubAddress & server ) const
{
    LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor__sendClientConnectedEvent );
    if ( server.isLocalAddress( ) && server.getSource( ) != areg::SOURCE_UNKNOWN )
    {
        LOG_DBG( "Sending to Stub [ %s ] notification of connected client [ %s ]"
                   , areg::StubAddress::convAddressToPath( server ).getString( )
                   , areg::ProxyAddress::convAddressToPath( client ).getString( ) );

        StubConnectEvent * clientConnect = DEBUG_NEW StubConnectEvent( client, server, areg::ServiceConnectionState::Connected );
        if ( clientConnect != nullptr )
        {
            server.deliverServiceEvent( *clientConnect );
        }
    }

    if ( client.isLocalAddress( ) && client.getSource( ) != areg::SOURCE_UNKNOWN )
    {
        LOG_DBG( "Sending to Proxy [ %s ] notification of connection to server [ %s ]"
                   , areg::ProxyAddress::convAddressToPath( client ).getString( )
                   , areg::StubAddress::convAddressToPath( server ).getString( ) );

        ProxyConnectEvent * proxyConnect = DEBUG_NEW ProxyConnectEvent( client, server, areg::ServiceConnectionState::Connected );
        if ( proxyConnect != nullptr )
        {
            client.deliverServiceEvent( *proxyConnect );
        }
    }
}

void ServiceManagerEventProcessor::_sendClientDisconnectEvent( const areg::ProxyAddress & client
                                                             , const areg::StubAddress & server
                                                             , const areg::ServiceConnectionState status ) const
{
    LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor__sendClientDisconnectedEvent );

    if ( server.isLocalAddress( ) && server.getSource( ) != areg::SOURCE_UNKNOWN )
    {
        LOG_DBG( "Sending to Stub [ %s ] notification of disconnected client [ %s ]"
                   , areg::StubAddress::convAddressToPath( server ).getString( )
                   , areg::ProxyAddress::convAddressToPath( client ).getString( ) );

        StubConnectEvent * clientConnect = DEBUG_NEW StubConnectEvent( client, server, status );
        if ( clientConnect != nullptr )
        {
            server.deliverServiceEvent( *clientConnect );
        }
    }

    if ( client.isLocalAddress( ) )
    {
        LOG_DBG( "Sending to Proxy [ %s ] notification of disconnection from server [ %s ]"
                   , areg::ProxyAddress::convAddressToPath( client ).getString( )
                   , areg::StubAddress::convAddressToPath( server ).getString( ) );

        ProxyConnectEvent * proxyConnect = DEBUG_NEW ProxyConnectEvent( client, server, status );
        if ( proxyConnect != nullptr )
        {
            client.deliverServiceEvent( *proxyConnect );
        }
    }
}

bool ServiceManagerEventProcessor::_terminateComponentThread( const areg::String & threadName )
{
    LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor__terminateComponentThread );

    bool result{ false };

    areg::Thread * thread = areg::Thread::findThreadByName( threadName );
    areg::ComponentThread * compThread = AREG_RUNTIME_CAST( thread, areg::ComponentThread );
    if ( compThread != nullptr )
    {
        LOG_WARN( "Terminating component thread [ %s ]", compThread->getName( ).getString( ) );
        result = true;
        compThread->terminateSelf( );
    }
    else
    {
        LOG_INFO( "Was not able to find component thread [ %s ] to terminate", threadName.getString( ) );
    }

    return result;
}

void ServiceManagerEventProcessor::_startComponentThread( const areg::String & threadName )
{
    LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor__startComponentThread );

    const areg::ComponentThreadEntry & entry = areg::ComponentLoader::findThreadEntry( threadName );
    areg::Thread * thread = areg::Thread::findThreadByName( threadName );
    if ( entry.isValid( ) && (thread == nullptr) )
    {
        areg::ComponentThread * compThread = DEBUG_NEW areg::ComponentThread( entry.mThreadName, entry.mWatchdogTimeout );
        if ( (compThread != nullptr) && compThread->createThread( areg::WAIT_INFINITE ) )
        {
            LOG_DBG( "Succeeded to create and start component thread [ %s ]", threadName.getString( ) );
        }
        else
        {
            LOG_WARN( "Failed to create and start component thread [ %s ]", threadName.getString( ) );
        }
    }
    else
    {
        LOG_ERR( "The thread [ %s ] is registered in the system, ignoring to create seconds instance of the thread", threadName.getString( ) );
    }
}
