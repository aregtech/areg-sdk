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

#include "areg/logging/areg_log.h"

DEF_LOG_SCOPE(areg_component_private_ServiceManagerEventProcessor, _register_provider);
DEF_LOG_SCOPE(areg_component_private_ServiceManagerEventProcessor, _unregister_provider);
DEF_LOG_SCOPE(areg_component_private_ServiceManagerEventProcessor, _register_consumer);
DEF_LOG_SCOPE(areg_component_private_ServiceManagerEventProcessor, _unregister_consumer);
DEF_LOG_SCOPE(areg_component_private_ServiceManagerEventProcessor, _send_connected);
DEF_LOG_SCOPE(areg_component_private_ServiceManagerEventProcessor, _send_disconnected);
DEF_LOG_SCOPE(areg_component_private_ServiceManagerEventProcessor, _terminate_component_thread);
DEF_LOG_SCOPE(areg_component_private_ServiceManagerEventProcessor, _start_component_thread);

namespace areg {

ServiceManagerEventProcessor::ServiceManagerEventProcessor( ServiceManager & serviceManager )
    : mServiceManager   ( serviceManager )
    , mServerList       ( )
{
}

void ServiceManagerEventProcessor::process_service_event( ServiceManagerEventData::ServiceManagerCommand cmdService
                                                        , const InStream& stream
                                                        , ConnectionProvider& connectProvider
                                                        , RegistrationProvider& registerProvider )
{
    switch ( cmdService )
    {
    case ServiceManagerEventData::ServiceManagerCommand::CMD_ShutdownService:
        {
            mServerList.clear( );
            connectProvider.disconnect_service_host( );
            mServiceManager.remove_all_events( );
            mServiceManager.trigger_exit( );
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_StopRoutingClient:
        {
            for ( auto mapPos = mServerList.first_position( ); mServerList.is_valid_position( mapPos ); mapPos = mServerList.next_position( mapPos ) )
            {
                ServerInfo si;
                ClientList clientList;

                mServerList.at_position( mapPos, si, clientList );
                for ( auto listPos = clientList.first_position( ); clientList.is_valid_position( listPos ); listPos = clientList.next_position( listPos ) )
                {
                    const ClientInfo & client = clientList.value_at( listPos );
                    if ( client.is_connected( ) )
                    {
                        _send_disconnected( client.address(), si.address( ), areg::ServiceConnectionState::Disconnected );
                    }
                }
            }

            mServerList.clear( );
            connectProvider.disconnect_service_host( );
            mServiceManager.remove_events( false );
            mServiceManager.pulse_exit( );
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterProxy:
        {
            ProxyAddress  addrProxy;
            Channel       channel;
            addrProxy.from_stream(stream) >> channel;
            addrProxy.set_channel( channel );
            _register_consumer( addrProxy, registerProvider);
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterProxy:
        {
            ProxyAddress  addrProxy;
            Channel       channel;
            areg::DisconnectReason reason{areg::DisconnectReason::UndefinedReason};
            addrProxy.from_stream(stream) >> channel >> reason;
            addrProxy.set_channel( channel );
            _unregister_consumer( addrProxy, reason, registerProvider);
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterProvider:
        {
            StubAddress   addrstub;
            Channel       channel;
            addrstub.from_stream(stream) >> channel;
            addrstub.set_channel( channel );
            _register_provider( addrstub, registerProvider);
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterProvider:
        {
            StubAddress   addrstub;
            Channel       channel;
            areg::DisconnectReason reason{areg::DisconnectReason::UndefinedReason};
            addrstub.from_stream(stream) >> channel >> reason;
            addrstub.set_channel( channel );
            _unregister_provider( addrstub, reason, registerProvider);
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_ConfigureConnection:
        {
            areg::RemoteServiceKind service{ areg::RemoteServiceKind::Unknown };
            uint32_t connectTypes{ static_cast<uint32_t>(areg::ConnectionType::Undefined) };
            stream >> service;
            stream >> connectTypes;

            connectProvider.setup_connection_data(service, connectTypes);
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_StartConnection:
        {
            areg::RemoteServiceKind service{ areg::RemoteServiceKind::Unknown };
            uint32_t connectTypes{ static_cast<uint32_t>(areg::ConnectionType::Undefined) };
            stream >> service;
            stream >> connectTypes;

            if (connectProvider.setup_connection_data(service, connectTypes))
            {
                connectProvider.connect_service_host();
            }
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_StartNetConnection:
        {
            String   ipAddress;
            uint16_t portNr = 0;
            stream >> ipAddress;
            stream >> portNr;

            connectProvider.apply_connection_data( ipAddress, portNr );
            if (connectProvider.is_host_setup( ) )
            {
                connectProvider.connect_service_host( );
            }
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_StopConnection:
        {
            connectProvider.disconnect_service_host( );
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterConnection:
        {
            for ( ServerList::MAPPOS posMap = mServerList.first_position( ); mServerList.is_valid_position( posMap ); posMap = mServerList.next_position( posMap ) )
            {
                const StubAddress & server = mServerList.key_at( posMap ).address( );
                const ClientList & clientList = mServerList.value_at( posMap );

                if ( server.is_service_public( ) && server.is_local_address( ) && server.is_valid( ) )
                {
                    registerProvider.register_service_provider( server );
                }

                for ( ClientList::LISTPOS pos = clientList.first_position( ); clientList.is_valid_position( pos ); pos = clientList.next_position( pos ) )
                {
                    const ProxyAddress & proxy = clientList.value_at( pos ).address( );
                    if ( proxy.is_service_public( ) && (proxy.is_target_local( ) == false) && proxy.is_valid( ) )
                    {
                        registerProvider.register_service_consumer( proxy );
                    }
                }
            }
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterConnection:
    case ServiceManagerEventData::ServiceManagerCommand::CMD_LostConnection:
        {
            // Snapshot to copies; removing from live lists would invalidate iterators.
            ArrayList<StubAddress> stubList;
            ArrayList<ProxyAddress> proxyList;
            for ( ServerList::MAPPOS posMap = mServerList.first_position( ); mServerList.is_valid_position( posMap ); posMap = mServerList.next_position( posMap ) )
            {
                const StubAddress & server = mServerList.key_at( posMap ).address( );
                const ClientList & clientList = mServerList.value_at( posMap );

                if ( server.is_service_public( ) && server.is_remote_address( ) && server.is_valid( ) )
                {
                    stubList.add( server );
                }

                for ( ClientList::LISTPOS pos = clientList.first_position( ); clientList.is_valid_position( pos ); pos = clientList.next_position( pos ) )
                {
                    const ProxyAddress & proxy = clientList.value_at( pos ).address( );
                    if ( proxy.is_service_public( ) && proxy.is_remote_address( ) && proxy.is_valid( ) )
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

            for ( uint32_t i = 0; i < stubList.size( ); ++i )
            {
                _unregister_provider( stubList[ i ], reason, registerProvider);
            }

            for ( uint32_t i = 0; i < proxyList.size( ); ++i )
            {
                _unregister_consumer( proxyList[ i ], reason, registerProvider);
            }
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_TerminateComponentThread:
        {
            String threadName;
            stream >> threadName;
            if ( _terminate_component_thread( threadName ) )
            {
                ServiceManager::_request_create_thread( threadName );
            }
        }
        break;

    case ServiceManagerEventData::ServiceManagerCommand::CMD_StartComponentThread:
        {
            String threadName;
            stream >> threadName;
            _start_component_thread( threadName );
        }
        break;

    default:
        ASSERT( false );
        break;
    }
}

void ServiceManagerEventProcessor::_register_provider( const StubAddress & whichServer, RegistrationProvider& registerProvider)
{
    LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor, _register_provider);

    if ( whichServer.is_local_address( ) && whichServer.is_service_public( ) )
    {
        registerProvider.register_service_provider( whichServer );
    }

    ClientList clientList;

#if AREG_LOGGING
    const ServerInfo & server = mServerList.register_provider( whichServer, clientList );
    LOG_DBG( "Server [ %s ] is registered. Connection status [ %s ], there are [ %d ] waiting clients"
               , StubAddress::to_path( server.address( ) ).as_string( )
               , areg::as_string( server.connection_status( ) )
               , clientList.size( ) );
#else   // !AREG_LOGGING
    mServerList.register_provider( whichServer, clientList );
#endif  // !AREG_LOGGING

    for ( ClientList::LISTPOS pos = clientList.first_position( ); clientList.is_valid_position( pos ); pos = clientList.next_position( pos ) )
    {
        const ClientInfo & client{ clientList.value_at( pos ) };
        if ( client.is_connected( ) )
        {
            _send_connected( client.address( ), whichServer );
        }
    }
}

void ServiceManagerEventProcessor::_unregister_provider( const StubAddress & whichServer, const areg::DisconnectReason reason, RegistrationProvider& registerProvider)
{
    LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor, _unregister_provider);

    if ( whichServer.is_local_address( ) && whichServer.is_service_public( ) )
    {
        registerProvider.unregister_service_provider( whichServer, reason );
    }

    ClientList clientList;

#if AREG_LOGGING
    ServerInfo server( mServerList.unregister_provider( whichServer, clientList ) );
    LOG_DBG( "Server [ %s ] is unregistered with reason [ %s ]. The service connection status was [ %s ], there are [ %d ] waiting clients"
               , StubAddress::to_path( server.address( ) ).as_string( )
               , areg::as_string( reason )
               , areg::as_string( server.connection_status( ) )
               , clientList.size( ) );
#else   // AREG_LOGGING
    static_cast<void>(mServerList.unregister_provider( whichServer, clientList ));
#endif  // AREG_LOGGING

    areg::ServiceConnectionState status = areg::service_connection( reason );
    for ( ClientList::LISTPOS pos = clientList.first_position( ); clientList.is_valid_position( pos ); pos = clientList.next_position( pos ) )
    {
        const ClientInfo & client{ clientList.value_at( pos ) };
        if ( client.is_connected( ) )
        {
            _send_disconnected( client.address( ), whichServer, status );
        }
    }
}

void ServiceManagerEventProcessor::_register_consumer( const ProxyAddress & whichClient, RegistrationProvider& registerProvider)
{
    LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor, _register_consumer);

    if ( whichClient.is_local_address( ) && whichClient.is_service_public( ) )
    {
        registerProvider.register_service_consumer( whichClient );
    }

    ClientInfo client;
    const ServerInfo & server = mServerList.register_consumer( whichClient, client );

    LOG_DBG( "Client [ %s ] is registered for server [ %s ], connection status [ %s ]"
               , ProxyAddress::to_path( client.address( ) ).as_string( )
               , StubAddress::to_path( server.address( ) ).as_string( )
               , areg::as_string( client.connection_status( ) ) );

    if ( client.is_connected( ) )
    {
        _send_connected( client.address(), server.address( ) );
    }
    else
    {
        LOG_INFO( "The Proxy [ %s ] has NO CONNECTION yet. Noting to send", ProxyAddress::to_path( client.address( ) ).as_string( ) );
    }
}

void ServiceManagerEventProcessor::_unregister_consumer( const ProxyAddress & whichClient, const areg::DisconnectReason reason, RegistrationProvider& registerProvider)
{
    LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor, _unregister_consumer);

    if ( whichClient.is_local_address( ) && whichClient.is_service_public( ) )
    {
        registerProvider.unregister_service_consumer( whichClient, reason );
    }

    ClientInfo client;

    ServerInfo server = mServerList.unregister_consumer( whichClient, client );
    LOG_DBG( "Client [ %s ] is unregistered from server [ %s ], connection status [ %s ]"
               , ProxyAddress::to_path( client.address( ) ).as_string( )
               , StubAddress::to_path( server.address( ) ).as_string( )
               , areg::as_string( client.connection_status( ) ) );

    // Unregister client first, then send event that client does not receive notification
    if ( client.is_connected( ) )
    {
        _send_disconnected( client.address(), server.address( ), areg::service_connection( reason ) );
    }
    else
    {
        LOG_INFO( "The Proxy [ %s ] has NO CONNECTION. Noting to send", ProxyAddress::to_path( client.address( ) ).as_string( ) );
    }
}

void ServiceManagerEventProcessor::_send_connected( const ProxyAddress & client, const StubAddress & server ) const
{
    LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor, _send_connected );
    if ( server.is_local_address( ) && server.source( ) != areg::SOURCE_UNKNOWN )
    {
        LOG_DBG( "Sending to Stub [ %s ] notification of connected client [ %s ]"
                   , StubAddress::to_path( server ).as_string( )
                   , ProxyAddress::to_path( client ).as_string( ) );

        {
            StubConnectEvent clientConnect( client, server, areg::ServiceConnectionState::Connected );
            server.deliver_service_event( clientConnect );  // moves clientConnect into queue
        }
    }

    if ( client.is_local_address( ) && client.source( ) != areg::SOURCE_UNKNOWN )
    {
        LOG_DBG( "Sending to Proxy [ %s ] notification of connection to server [ %s ]"
                   , ProxyAddress::to_path( client ).as_string( )
                   , StubAddress::to_path( server ).as_string( ) );

        {
            ProxyConnectEvent proxyConnect( client, server, areg::ServiceConnectionState::Connected );
            client.deliver_service_event( proxyConnect );  // moves proxyConnect into queue
        }
    }
}

void ServiceManagerEventProcessor::_send_disconnected( const ProxyAddress & client
                                                             , const StubAddress & server
                                                             , const areg::ServiceConnectionState status ) const
{
    LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor, _send_disconnected);

    if ( server.is_local_address( ) && server.source( ) != areg::SOURCE_UNKNOWN )
    {
        LOG_DBG( "Sending to Stub [ %s ] notification of disconnected client [ %s ]"
                   , StubAddress::to_path( server ).as_string( )
                   , ProxyAddress::to_path( client ).as_string( ) );

        {
            StubConnectEvent clientConnect( client, server, status );
            clientConnect.set_event_priority(areg::EventPriority::HighPrio);
            server.deliver_service_event( clientConnect );
        }
    }

    if ( client.is_local_address( ) )
    {
        LOG_DBG( "Sending to Proxy [ %s ] notification of disconnection from server [ %s ]"
                   , ProxyAddress::to_path( client ).as_string( )
                   , StubAddress::to_path( server ).as_string( ) );

        {
            ProxyConnectEvent proxyConnect( client, server, status );
            proxyConnect.set_event_priority(areg::EventPriority::HighPrio);
            client.deliver_service_event( proxyConnect );
        }
    }
}

bool ServiceManagerEventProcessor::_terminate_component_thread( const String & threadName )
{
    LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor, _terminate_component_thread );

    bool result{ false };

    Thread * thread = Thread::find_by_address(ThreadAddress(threadName));
    ComponentThread * compThread = AREG_RUNTIME_CAST( thread, ComponentThread );
    if ( compThread != nullptr )
    {
        LOG_WARN( "Terminating component thread [ %s ]", compThread->name( ).as_string( ) );
        result = true;
        compThread->terminate_self( );
    }
    else
    {
        LOG_INFO( "Was not able to find component thread [ %s ] to terminate", threadName.as_string( ) );
    }

    return result;
}

void ServiceManagerEventProcessor::_start_component_thread( const String & threadName )
{
    LOG_SCOPE( areg_component_private_ServiceManagerEventProcessor, _start_component_thread );

    const areg::ComponentThreadEntry & entry = ComponentLoader::find_thread_entry( threadName );
    Thread * thread = Thread::find_by_address(ThreadAddress(threadName));
    if ( entry.is_valid( ) && (thread == nullptr) )
    {
        ComponentThread * compThread = DEBUG_NEW ComponentThread( entry.mThreadName, entry.mWatchdogTimeout );
        if ( (compThread != nullptr) && compThread->start( areg::WAIT_INFINITE ) )
        {
            LOG_DBG( "Succeeded to create and start component thread [ %s ]", threadName.as_string( ) );
        }
        else
        {
            LOG_WARN( "Failed to create and start component thread [ %s ]", threadName.as_string( ) );
        }
    }
    else
    {
        LOG_ERR( "The thread [ %s ] is registered in the system, ignoring to create seconds instance of the thread", threadName.as_string( ) );
    }
}

} // namespace areg
