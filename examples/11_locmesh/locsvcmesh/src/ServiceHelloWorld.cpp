/************************************************************************
 * \file        locsvcmesh/src/ServiceHelloWorld.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains implementation of a service.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "locsvcmesh/src/ServiceHelloWorld.hpp"
#include "areg/trace/GETrace.h"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>


DEF_TRACE_SCOPE( examples_11_locsvcmesh_ServiceHelloWorld_requestHelloWorld );
DEF_TRACE_SCOPE( examples_11_locsvcmesh_ServiceHelloWorld_requestShutdownService );

ServiceHelloWorld::ServiceHelloWorld( Component & masterComp, bool isMain )
    : HelloWorldStub( masterComp )
    , mIsMain( isMain )
    , mGnerateID( 0 )
    , mClientList( )
    , mRemainRequest( NEHelloWorld::MaxMessages )
{
}

void ServiceHelloWorld::requestHelloWorld( const String & roleName )
{
    TRACE_SCOPE( examples_11_locsvcmesh_ServiceHelloWorld_requestHelloWorld );

    ClientList::LISTPOS pos = mClientList.firstPosition( );
    for ( ; mClientList.isValidPosition( pos ); pos = mClientList.nextPosition( pos ) )
    {
        const NEHelloWorld::sConnectedClient & client = mClientList.valueAtPosition( pos );
        if ( roleName == client.ccName )
        {
            TRACE_DBG( "Found connected client [ %s ] with ID [ %u ] in the list.", client.ccName.getString( ), client.ccID );
            break;
        }
    }

    if ( mClientList.isInvalidPosition( pos ) )
    {
        responseHelloWorld( NEHelloWorld::sConnectedClient{ ++mGnerateID, roleName } );
        TRACE_INFO( "The new client component [ %s ] with ID [ %u ] sent a request", roleName.getString( ), mGnerateID );
    }
    else
    {
        responseHelloWorld( mClientList.valueAtPosition( pos ) );
    }

    // User printf, it is thread safe
    printf( "\"Hello client [ %s ]!\", remain to process [ %d ]\n", roleName.getString( ), --mRemainRequest );

    if ( mRemainRequest == 0 )
    {
        TRACE_INFO( "Reached maximum to output messages, this should trigger the shutdown procedure." );
        if ( mIsMain )
        {
            TRACE_WARN( "The controller component [ %s ] broadcasts message to shutdown application", getServiceRole( ).getString( ) );
            broadcastReachedMaximum( NEHelloWorld::MaxMessages );
        }
    }
    else
    {
        TRACE_WARN( "The service [  %s ] still wait [ %d ] requests to print Hello World.", getServiceRole( ).getString( ), mRemainRequest );
    }
}

void ServiceHelloWorld::requestShutdownService( unsigned int clientID, const String & roleName )
{
    TRACE_SCOPE( examples_11_locsvcmesh_ServiceHelloWorld_requestShutdownService );
    TRACE_DBG( "A client [ %s ] with ID [ %u ] notified shutdown.", roleName.getString( ), clientID );

    if ( mIsMain )
    {
        TRACE_INFO( "All clients are set message to shutdown, all [ %d ] messages are output, going to shutdown application", NEHelloWorld::MaxMessages );
        Application::signalAppQuit( );
    }
}
