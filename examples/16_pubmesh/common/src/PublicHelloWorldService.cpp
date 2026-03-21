/************************************************************************
 * \file        common/src/PublicHelloWorldService.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "common/src/PublicHelloWorldService.hpp"
#include "areg/logging/areg_log.h"
#include <stdlib.h>


DEF_LOG_SCOPE( examples_16_pubmesh_common_PublicHelloWorldService_startupServiceInterface );
DEF_LOG_SCOPE( examples_16_pubmesh_common_PublicHelloWorldService_request_register );
DEF_LOG_SCOPE( examples_16_pubmesh_common_PublicHelloWorldService_request_unregister );
DEF_LOG_SCOPE( examples_16_pubmesh_common_PublicHelloWorldService_request_hello_world );

PublicHelloWorldService::PublicHelloWorldService( areg::Component & masterComp )
    : PublicHelloWorldProviderBase  ( masterComp )
    , mClientList           ( )
    , mNumMessages          ( 0 )
{
}

void PublicHelloWorldService::request_register( const areg::String & name, const areg::ServiceAddress & service, const areg::String & thread, const areg::String & process )
{
    LOG_SCOPE( examples_16_pubmesh_common_PublicHelloWorldService_request_register );
    LOG_DBG( "Received request to register client [ %s ] with service address [ %s ] and owner thread [ %s ] of process [ %s ]"
        , name.as_string( )
        , areg::ServiceAddress::to_path( service ).as_string( )
        , thread.as_string( )
        , process.as_string( ) );

    PublicHelloWorld::sClientRegister theClient;
    ClientList::LISTPOS pos = mClientList.first_position( );
    for ( ; mClientList.is_valid_position( pos ); pos = mClientList.next_position( pos ) )
    {
        const PublicHelloWorld::sClientRegister & client = mClientList.value_at( pos );
        if ( (client.crName == name) && (client.crService == service) && (client.crThread == thread) )
        {
            LOG_DBG( "Found connected client [ %s ] with ID [ %u ] in the list.", client.crName.as_string( ), client.crID );
            theClient = client;
            break;
        }
    }

    if ( !mClientList.is_valid_position( pos ) )
    {
        theClient = PublicHelloWorld::sClientRegister( areg::generate_unique_id( ), name, service, thread, process );
        mClientList.push_first( theClient );
        LOG_DBG( "Registered [ %u ] new client [ %s ] of service [ %s ] in thread [ %s ] of process [ %s ]"
            , theClient.crID
            , theClient.crName.as_string( )
            , areg::ServiceAddress::to_path( theClient.crService ).as_string( )
            , theClient.crThread.as_string( )
            , theClient.crProcess.as_string( ) );
    }

    response_register( theClient );
}

void PublicHelloWorldService::request_unregister( const PublicHelloWorld::sClientRegister & client )
{
    LOG_SCOPE( examples_16_pubmesh_common_PublicHelloWorldService_request_unregister );
    LOG_DBG( "The client [ %s ] with registered ID [ %u ] requested unregister.", client.crName.as_string( ), client.crID );

    for ( ClientList::LISTPOS pos = mClientList.first_position( ); mClientList.is_valid_position( pos ); pos = mClientList.next_position( pos ) )
    {
        const PublicHelloWorld::sClientRegister & entry = mClientList.value_at( pos );
        if ( entry == client )
        {
            mClientList.remove_at( pos );
            LOG_DBG( "Removed entry, there are still [ %d ] registered clients", mClientList.size( ) );
            break;
        }
    }
}

void PublicHelloWorldService::request_hello_world( uint32_t clientID )
{
    LOG_SCOPE( examples_16_pubmesh_common_PublicHelloWorldService_request_hello_world );

    PublicHelloWorld::sClientRegister theClient;
    ClientList::LISTPOS pos = mClientList.first_position( );
    for ( ; mClientList.is_valid_position( pos ); pos = mClientList.next_position( pos ) )
    {
        const PublicHelloWorld::sClientRegister & client = mClientList.value_at( pos );
        if ( clientID == client.crID )
        {
            LOG_DBG( "Found connected client [ %s ] with ID [ %u ] in the list.", client.crName.as_string( ), client.crID );
            theClient = client;
            break;
        }
    }

    if ( mClientList.is_valid_position( pos ) )
    {
        // use printf() because of multithreading environment.
        printf( "\">>> Public [ %s ]!\", processed [ %u ] requests.\n\r", theClient.crName.as_string( ), ++mNumMessages );
    }

    response_hello_world( theClient.crID );
}

bool PublicHelloWorldService::client_connected(const areg::ProxyAddress & client, areg::ServiceConnectionState status)
{
    return PublicHelloWorldProviderBase::client_connected(client, status);
}
