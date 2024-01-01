/************************************************************************
 * \file        locsvcmesh/src/ServiceHelloWorld.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
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
    , mClientList( )
    , mRemainRequest( NEHelloWorld::MaxMessages )
{
}

void ServiceHelloWorld::requestHelloWorld( const String & roleName )
{
    TRACE_SCOPE( examples_11_locsvcmesh_ServiceHelloWorld_requestHelloWorld );
    unsigned int clientId = 0;
    if ( mClientList.find( roleName, clientId ) == false )
    {
        clientId = NEUtilities::generateUniqueId( );
        mClientList.setAt( roleName, clientId );
    }

    // use printf() because of multithreading environment
    printf( "\"Hello client [ %s ]!\", remain to process [ %d ]\n", roleName.getString( ), -- mRemainRequest );

    responseHelloWorld( roleName, clientId );

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
