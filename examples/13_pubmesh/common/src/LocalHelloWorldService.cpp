/************************************************************************
 * \file        common/src/LocalHelloWorldService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "common/src/LocalHelloWorldService.hpp"
#include "areg/trace/GETrace.h"
#include <stdlib.h>


DEF_TRACE_SCOPE( examples_13_pubmesh_common_LocalHelloWorldService_requestHelloWorld );

LocalHelloWorldService::LocalHelloWorldService( Component & masterComp )
    : LocalHelloWorldStub( masterComp )
    , mClientList        ( )
    , mNumRequests       ( 0 )
{
}

void LocalHelloWorldService::requestHelloWorld( const String & roleName )
{
    TRACE_SCOPE( examples_13_pubmesh_common_LocalHelloWorldService_requestHelloWorld );

    NELocalHelloWorld::sConnectedClient theClient;
    ClientList::LISTPOS pos = mClientList.firstPosition( );
    for ( ; mClientList.isValidPosition( pos ); pos = mClientList.nextPosition( pos ) )
    {
        const NELocalHelloWorld::sConnectedClient & client = mClientList.valueAtPosition( pos );
        if ( roleName == client.ccName )
        {
            TRACE_DBG( "Found connected client [ %s ] with ID [ %u ] in the list.", client.ccName.getString( ), client.ccID );
            theClient = client;
            break;
        }
    }

    if ( mClientList.isInvalidPosition( pos ) )
    {
        theClient = NELocalHelloWorld::sConnectedClient( NEUtilities::generateUniqueId(), roleName );
        mClientList.pushLast( theClient );
        TRACE_INFO( "The new client component [ %s ] with ID [ %u ] sent a request", roleName.getString( ), theClient.ccID );
    }
    else
    {
    }

    // use printf() because of multithreading environment.
    printf( "\"Hello [ %s ]!\", processed [ %u ] requests.\n\r", roleName.getString( ), ++mNumRequests );
    responseHelloWorld( theClient );
}
