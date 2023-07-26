/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ServiceManagerEventProcessor.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, The Service Manager processing helper class.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/component/private/ServiceManagerEventProcessor.hpp"

#include "areg/base/IEIOStream.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/private/ProxyConnectEvent.hpp"
#include "areg/component/private/StubConnectEvent.hpp"
#include "areg/component/private/ServiceManager.hpp"
#include "areg/ipc/IERemoteServiceConnection.hpp"

#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE( areg_component_private_ServiceManagerEventProcessor__registerServer );
DEF_TRACE_SCOPE( areg_component_private_ServiceManagerEventProcessor__unregisterServer );
DEF_TRACE_SCOPE( areg_component_private_ServiceManagerEventProcessor__registerClient );
DEF_TRACE_SCOPE( areg_component_private_ServiceManagerEventProcessor__unregisterClient );
DEF_TRACE_SCOPE( areg_component_private_ServiceManagerEventProcessor__sendClientConnectedEvent );
DEF_TRACE_SCOPE( areg_component_private_ServiceManagerEventProcessor__sendClientDisconnectedEvent );
DEF_TRACE_SCOPE( areg_component_private_ServiceManagerEventProcessor__terminateComponentThread );
DEF_TRACE_SCOPE( areg_component_private_ServiceManagerEventProcessor__startComponentThread );


ServiceManagerEventProcessor::ServiceManagerEventProcessor( ServiceManager & serviceManager )
    : mServiceManager   ( serviceManager )
    , mServerList       ( )
{
}

void ServiceManagerEventProcessor::processServiceEvent( ServiceManagerEventData::eServiceManagerCommands cmdService
                                              , const IEInStream & stream
                                              , IERemoteServiceConnection & serviceConnection )
{
    switch ( cmdService )
    {
    case ServiceManagerEventData::eServiceManagerCommands::CMD_ShutdownService:
        {
            mServerList.clear( );
            serviceConnection.disconnectServiceHost( );
            mServiceManager.removeAllEvents( );
            mServiceManager.triggerExitEvent( );
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_StopRoutingClient:
        {
            for ( ServerList::MAPPOS pos = mServerList.firstPosition( ); mServerList.isValidPosition( pos ); pos = mServerList.nextPosition( pos ) )
            {
                ServerInfo si;
                ClientList clientList;

                mServerList.getAtPosition( pos, si, clientList );
                for ( ClientList::LISTPOS pos = clientList.firstPosition( ); clientList.isValidPosition( pos ); pos = clientList.nextPosition( pos ) )
                {
                    const ClientInfo & client = clientList.valueAtPosition( pos );
                    _sendClientDisconnectEvent( client, si.getAddress( ), NEService::eServiceConnection::ServiceDisconnected );
                }
            }

            mServerList.clear( );
            serviceConnection.disconnectServiceHost( );
            mServiceManager.removeEvents( false );
            mServiceManager.pulseExit( );
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_RegisterProxy:
        {
            ProxyAddress  addrProxy;
            Channel       channel;
            stream >> addrProxy;
            stream >> channel;
            addrProxy.setChannel( channel );
            _registerClient( addrProxy, serviceConnection );
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_UnregisterProxy:
        {
            ProxyAddress  addrProxy;
            Channel       channel;
            NEService::eDisconnectReason reason{NEService::eDisconnectReason::ReasonUndefined};
            stream >> addrProxy;
            stream >> channel;
            stream >> reason;
            addrProxy.setChannel( channel );
            _unregisterClient( addrProxy, reason, serviceConnection );
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_RegisterStub:
        {
            StubAddress   addrstub;
            Channel       channel;
            stream >> addrstub;
            stream >> channel;
            addrstub.setChannel( channel );
            _registerServer( addrstub, serviceConnection );
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_UnregisterStub:
        {
            StubAddress   addrstub;
            Channel       channel;
            NEService::eDisconnectReason reason{NEService::eDisconnectReason::ReasonUndefined};
            stream >> addrstub;
            stream >> channel;
            stream >> reason;
            addrstub.setChannel( channel );
            _unregisterServer( addrstub, reason, serviceConnection );
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_ConfigureConnection:
        {
            String   configFile;
            stream >> configFile;
            serviceConnection.enableRemoteServicing( true );
            if ( configFile.isEmpty( ) == false )
            {
                serviceConnection.setupServiceConnectionHost( configFile );
            }
            else if ( serviceConnection.isServiceHostSetup( ) == false )
            {
                serviceConnection.setupServiceConnectionHost( String::getEmptyString( ) );
            }
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_StartConnection:
        {
            String   configFile;
            stream >> configFile;
            bool isConfigured = false;
            serviceConnection.enableRemoteServicing( true );
            if ( configFile.isEmpty( ) == false )
            {
                isConfigured = serviceConnection.setupServiceConnectionHost( configFile );
            }
            else if ( serviceConnection.isServiceHostSetup( ) == false )
            {
                isConfigured = serviceConnection.setupServiceConnectionHost( String::getEmptyString( ) );
            }
            else
            {
                isConfigured = true;
            }

            if ( isConfigured )
            {
                serviceConnection.connectServiceHost( );
            }
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_StartNetConnection:
        {
            String   ipAddress;
            unsigned short portNr = 0;
            stream >> ipAddress;
            stream >> portNr;

            serviceConnection.enableRemoteServicing( true );
            serviceConnection.applyServiceConnectionData( ipAddress, portNr );
            if ( serviceConnection.isServiceHostSetup( ) )
            {
                serviceConnection.connectServiceHost( );
            }
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_StopConnection:
        {
            serviceConnection.disconnectServiceHost( );
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_SetEnableService:
        {
            bool enable = false;
            stream >> enable;
            serviceConnection.enableRemoteServicing( enable );
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_RegisterConnection:
        {
            for ( ServerList::MAPPOS posMap = mServerList.firstPosition( ); mServerList.isValidPosition( posMap ); posMap = mServerList.nextPosition( posMap ) )
            {
                const StubAddress & server = mServerList.keyAtPosition( posMap ).getAddress( );
                const ClientList & clientList = mServerList.valueAtPosition( posMap );

                if ( server.isServicePublic( ) && server.isLocalAddress( ) && server.isValid( ) )
                {
                    serviceConnection.registerServiceProvider( server );
                }

                for ( ClientList::LISTPOS pos = clientList.firstPosition( ); clientList.isValidPosition( pos ); pos = clientList.nextPosition( pos ) )
                {
                    const ProxyAddress & proxy = clientList.valueAtPosition( pos ).getAddress( );
                    if ( proxy.isServicePublic( ) && proxy.isLocalAddress( ) && proxy.isValid( ) )
                    {
                        serviceConnection.registerServiceConsumer( proxy );
                    }
                }
            }
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_UnregisterConnection:
    case ServiceManagerEventData::eServiceManagerCommands::CMD_LostConnection:
        {
            // Create service provider and service consumer list
            // to be able to unregister entries, because they are removing
            // elements from the existing list and it may invalidate position object.
            TEArrayList<StubAddress> stubList;
            TEArrayList<ProxyAddress> proxyList;
            for ( ServerList::MAPPOS posMap = mServerList.firstPosition( ); mServerList.isValidPosition( posMap ); posMap = mServerList.nextPosition( posMap ) )
            {
                const StubAddress & server = mServerList.keyAtPosition( posMap ).getAddress( );
                const ClientList & clientList = mServerList.valueAtPosition( posMap );

                if ( server.isServicePublic( ) && server.isRemoteAddress( ) && server.isValid( ) )
                {
                    stubList.add( server );
                }

                for ( ClientList::LISTPOS pos = clientList.firstPosition( ); clientList.isValidPosition( pos ); pos = clientList.nextPosition( pos ) )
                {
                    const ProxyAddress & proxy = clientList.valueAtPosition( pos ).getAddress( );
                    if ( proxy.isServicePublic( ) && proxy.isRemoteAddress( ) && proxy.isValid( ) )
                    {
                        proxyList.add( proxy );
                    }
                }
            }

            NEService::eDisconnectReason reason { NEService::eDisconnectReason::ReasonProviderDisconnected };
            if ( cmdService == ServiceManagerEventData::eServiceManagerCommands::CMD_LostConnection )
            {
                reason = NEService::eDisconnectReason::ReasonRouterLost;
            }

            for ( uint32_t i = 0; i < stubList.getSize( ); ++i )
            {
                _unregisterServer( stubList[ i ], reason, serviceConnection );
            }

            for ( uint32_t i = 0; i < proxyList.getSize( ); ++i )
            {
                _unregisterClient( proxyList[ i ], reason, serviceConnection );
            }
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_TerminateComponentThread:
        {
            String threadName;
            stream >> threadName;
            if ( _terminateComponentThread( threadName ) )
            {
                ServiceManager::_requestCreateThread( threadName );
            }
        }
        break;

    case ServiceManagerEventData::eServiceManagerCommands::CMD_StartComponentThread:
        {
            String threadName;
            stream >> threadName;
            _startComponentThread( threadName );
        }
        break;

    default:
        ASSERT( false );
        break;
    }
}

void ServiceManagerEventProcessor::_registerServer( const StubAddress & whichServer, IERemoteServiceConnection & serviceConnection )
{
    TRACE_SCOPE( areg_component_private_ServiceManagerEventProcessor__registerServer );

    if ( whichServer.isLocalAddress( ) && whichServer.isServicePublic( ) )
    {
        serviceConnection.registerServiceProvider( whichServer );
    }

    ClientList clientList;

#if AREG_LOGS
    const ServerInfo & server = mServerList.registerServer( whichServer, clientList );
    TRACE_DBG( "Server [ %s ] is registered. Connection status [ %s ], there are [ %d ] waiting clients"
               , StubAddress::convAddressToPath( server.getAddress( ) ).getString( )
               , NEService::getString( server.getConnectionStatus( ) )
               , clientList.getSize( ) );
#else   // !AREG_LOGS
    mServerList.registerServer( whichServer, clientList );
#endif  // !AREG_LOGS

    for ( ClientList::LISTPOS pos = clientList.firstPosition( ); clientList.isValidPosition( pos ); pos = clientList.nextPosition( pos ) )
    {
        _sendClientConnectedEvent( clientList.valueAtPosition( pos ), whichServer );
    }
}

void ServiceManagerEventProcessor::_unregisterServer( const StubAddress & whichServer, const NEService::eDisconnectReason reason, IERemoteServiceConnection & serviceConnection )
{
    TRACE_SCOPE( areg_component_private_ServiceManagerEventProcessor__unregisterServer );

    if ( whichServer.isLocalAddress( ) && whichServer.isServicePublic( ) )
    {
        serviceConnection.unregisterServiceProvider( whichServer, reason );
    }

    ClientList clientList;

#if AREG_LOGS
    ServerInfo server( mServerList.unregisterServer( whichServer, clientList ) );
    TRACE_DBG( "Server [ %s ] is unregistered with reason [ %s ]. The service connection status was [ %s ], there are [ %d ] waiting clients"
               , StubAddress::convAddressToPath( server.getAddress( ) ).getString( )
               , NEService::getString( reason )
               , NEService::getString( server.getConnectionStatus( ) )
               , clientList.getSize( ) );

#else   // AREG_LOGS

    static_cast<void>(mServerList.unregisterServer( whichServer, clientList ));

#endif  // AREG_LOGS

    NEService::eServiceConnection status = NEService::serviceConnection( reason );
    for ( ClientList::LISTPOS pos = clientList.firstPosition( ); clientList.isValidPosition( pos ); pos = clientList.nextPosition( pos ) )
    {
        _sendClientDisconnectEvent( clientList.valueAtPosition( pos ), whichServer, status );
    }
}

void ServiceManagerEventProcessor::_registerClient( const ProxyAddress & whichClient, IERemoteServiceConnection & serviceConnection )
{
    TRACE_SCOPE( areg_component_private_ServiceManagerEventProcessor__registerClient );

    if ( whichClient.isLocalAddress( ) && whichClient.isServicePublic( ) )
    {
        serviceConnection.registerServiceConsumer( whichClient );
    }

    ClientInfo client;
    const ServerInfo & server = mServerList.registerClient( whichClient, client );

    TRACE_DBG( "Client [ %s ] is registered for server [ %s ], connection status [ %s ]"
               , ProxyAddress::convAddressToPath( client.getAddress( ) ).getString( )
               , StubAddress::convAddressToPath( server.getAddress( ) ).getString( )
               , NEService::getString( client.getConnectionStatus( ) ) );

    _sendClientConnectedEvent( client, server.getAddress( ) );
}

void ServiceManagerEventProcessor::_unregisterClient( const ProxyAddress & whichClient, const NEService::eDisconnectReason reason, IERemoteServiceConnection & serviceConnection )
{
    TRACE_SCOPE( areg_component_private_ServiceManagerEventProcessor__unregisterClient );

    if ( whichClient.isLocalAddress( ) && whichClient.isServicePublic( ) )
    {
        serviceConnection.unregisterServiceConsumer( whichClient, reason );
    }

    ClientInfo client;

    ServerInfo server = mServerList.unregisterClient( whichClient, client );
    TRACE_DBG( "Client [ %s ] is unregistered from server [ %s ], connection status [ %s ]"
               , ProxyAddress::convAddressToPath( client.getAddress( ) ).getString( )
               , StubAddress::convAddressToPath( server.getAddress( ) ).getString( )
               , NEService::getString( client.getConnectionStatus( ) ) );

    // Unregister client first, then send event that client does not receive notification
    _sendClientDisconnectEvent( client, server.getAddress( ), NEService::serviceConnection( reason ) );
}

void ServiceManagerEventProcessor::_sendClientConnectedEvent( const ClientInfo & client, const StubAddress & addrStub ) const
{
    TRACE_SCOPE( areg_component_private_ServiceManagerEventProcessor__sendClientConnectedEvent );

    if ( client.isConnected( ) )
    {
        const ProxyAddress & addrProxy = client.getAddress( );
        if ( addrStub.isLocalAddress( ) && addrStub.getSource( ) != NEService::SOURCE_UNKNOWN )
        {
            TRACE_DBG( "Sending to Stub [ %s ] notification of connected client [ %s ]"
                       , StubAddress::convAddressToPath( addrStub ).getString( )
                       , ProxyAddress::convAddressToPath( addrProxy ).getString( ) );

            StubConnectEvent * clientConnect = DEBUG_NEW StubConnectEvent( addrProxy, addrStub, NEService::eServiceConnection::ServiceConnected );
            if ( clientConnect != nullptr )
            {
                addrStub.deliverServiceEvent( *clientConnect );
            }
        }

        if ( addrProxy.isLocalAddress( ) && addrProxy.getSource( ) != NEService::SOURCE_UNKNOWN )
        {
            TRACE_DBG( "Sending to Proxy [ %s ] notification of connection to server [ %s ]"
                       , ProxyAddress::convAddressToPath( addrProxy ).getString( )
                       , StubAddress::convAddressToPath( addrStub ).getString( ) );

            ProxyConnectEvent * proxyConnect = DEBUG_NEW ProxyConnectEvent( addrProxy, addrStub, NEService::eServiceConnection::ServiceConnected );
            if ( proxyConnect != nullptr )
            {
                addrProxy.deliverServiceEvent( *proxyConnect );
            }
        }
    }
    else
    {
        TRACE_INFO( "The client Proxy [ %s ] has NO CONNECTION yet. Noting to send", ProxyAddress::convAddressToPath( client.getAddress( ) ).getString( ) );
    }
}

void ServiceManagerEventProcessor::_sendClientDisconnectEvent( const ClientInfo & client
                                                 , const StubAddress & addrStub
                                                 , const NEService::eServiceConnection status ) const
{
    TRACE_SCOPE( areg_component_private_ServiceManagerEventProcessor__sendClientDisconnectedEvent );

    if ( client.isConnected( ) )
    {
        const ProxyAddress & addrProxy = client.getAddress( );
        if ( addrStub.isLocalAddress( ) && addrStub.getSource( ) != NEService::SOURCE_UNKNOWN )
        {
            TRACE_DBG( "Sending to Stub [ %s ] notification of disconnected client [ %s ]"
                       , StubAddress::convAddressToPath( addrStub ).getString( )
                       , ProxyAddress::convAddressToPath( addrProxy ).getString( ) );

            StubConnectEvent * clientConnect = DEBUG_NEW StubConnectEvent( addrProxy, addrStub, status );
            if ( clientConnect != nullptr )
            {
                addrStub.deliverServiceEvent( *clientConnect );
            }
        }

        if ( addrProxy.isLocalAddress( ) )
        {
            TRACE_DBG( "Sending to Proxy [ %s ] notification of disconnection from server [ %s ]"
                       , ProxyAddress::convAddressToPath( addrProxy ).getString( )
                       , StubAddress::convAddressToPath( addrStub ).getString( ) );

            ProxyConnectEvent * proxyConnect = DEBUG_NEW ProxyConnectEvent( addrProxy, addrStub, status );
            if ( proxyConnect != nullptr )
            {
                addrProxy.deliverServiceEvent( *proxyConnect );
            }
        }
    }
}

bool ServiceManagerEventProcessor::_terminateComponentThread( const String & threadName )
{
    TRACE_SCOPE( areg_component_private_ServiceManagerEventProcessor__terminateComponentThread );

    bool result{ false };

    Thread * thread = Thread::findThreadByName( threadName );
    ComponentThread * compThread = RUNTIME_CAST( thread, ComponentThread );
    if ( compThread != nullptr )
    {
        TRACE_WARN( "Terminating component thread [ %s ]", compThread->getName( ).getString( ) );
        result = true;
        compThread->terminateSelf( );
    }
    else
    {
        TRACE_INFO( "Was not able to find component thread [ %s ] to terminate", threadName.getString( ) );
    }

    return result;
}

void ServiceManagerEventProcessor::_startComponentThread( const String & threadName )
{
    TRACE_SCOPE( areg_component_private_ServiceManagerEventProcessor__startComponentThread );

    const NERegistry::ComponentThreadEntry & entry = ComponentLoader::findThreadEntry( threadName );
    Thread * thread = Thread::findThreadByName( threadName );
    if ( entry.isValid( ) && (thread == nullptr) )
    {
        ComponentThread * compThread = DEBUG_NEW ComponentThread( entry.mThreadName, entry.mWatchdogTimeout );
        if ( (compThread != nullptr) && compThread->createThread( NECommon::WAIT_INFINITE ) )
        {
            TRACE_DBG( "Succeeded to create and start component thread [ %s ]", threadName.getString( ) );
        }
        else
        {
            TRACE_WARN( "Failed to create and start component thread [ %s ]", threadName.getString( ) );
        }
    }
    else
    {
        TRACE_ERR( "The thread [ %s ] is registered in the system, ignoring to create seconds instance of the thread", threadName.getString( ) );
    }
}

