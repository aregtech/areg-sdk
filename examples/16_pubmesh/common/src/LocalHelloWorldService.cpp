/************************************************************************
 * \file        common/src/LocalHelloWorldService.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "common/src/LocalHelloWorldService.hpp"
#include "areg/logging/areg_log.h"
#include <stdlib.h>


DEF_LOG_SCOPE( examples_16_pubmesh_common_LocalHelloWorldService_request_hello_world );

LocalHelloWorldService::LocalHelloWorldService( areg::Component & masterComp )
    : LocalHelloWorldProviderBase( masterComp )
    , mClientList        ( )
    , mNumRequests       ( 0 )
{
}

void LocalHelloWorldService::request_hello_world( const areg::String & roleName )
{
    LOG_SCOPE( examples_16_pubmesh_common_LocalHelloWorldService_request_hello_world );

    LocalHelloWorld::sConnectedClient theClient;
    ClientList::LISTPOS pos = mClientList.first_position( );
    for ( ; mClientList.is_valid_position( pos ); pos = mClientList.next_position( pos ) )
    {
        const LocalHelloWorld::sConnectedClient & client = mClientList.value_at( pos );
        if ( roleName == client.ccName )
        {
            LOG_DBG( "Found connected client [ %s ] with ID [ %u ] in the list.", client.ccName.as_string( ), client.ccID );
            theClient = client;
            break;
        }
    }

    if ( !mClientList.is_valid_position( pos ) )
    {
        theClient = LocalHelloWorld::sConnectedClient( areg::generate_unique_id(), roleName );
        mClientList.push_last( theClient );
        LOG_INFO( "The new client component [ %s ] with ID [ %u ] sent a request", roleName.as_string( ), theClient.ccID );
    }

    // use printf() because of multithreading environment.
    printf( "\"Hello [ %s ]!\", processed [ %u ] requests.\n\r", roleName.as_string( ), ++mNumRequests );
    response_hello_world( theClient );
}
