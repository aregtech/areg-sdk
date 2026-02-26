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
#include "areg/logging/GELog.h"
#include <stdlib.h>


DEF_LOG_SCOPE( examples_16_pubmesh_common_PublicHelloWorldService_startupServiceInterface );
DEF_LOG_SCOPE( examples_16_pubmesh_common_PublicHelloWorldService_requestRegister );
DEF_LOG_SCOPE( examples_16_pubmesh_common_PublicHelloWorldService_requestUnregister );
DEF_LOG_SCOPE( examples_16_pubmesh_common_PublicHelloWorldService_requestHelloWorld );

PublicHelloWorldService::PublicHelloWorldService( areg::Component & masterComp )
    : PublicHelloWorldStub  ( masterComp )
    , mClientList           ( )
    , mNumMessages          ( 0 )
{
}

void PublicHelloWorldService::requestRegister( const areg::String & name, const areg::ServiceAddress & service, const areg::String & thread, const areg::String & process )
{
    LOG_SCOPE( examples_16_pubmesh_common_PublicHelloWorldService_requestRegister );
    LOG_DBG( "Received request to register client [ %s ] with service address [ %s ] and owner thread [ %s ] of process [ %s ]"
        , name.getString( )
        , areg::ServiceAddress::convAddressToPath( service ).getString( )
        , thread.getString( )
        , process.getString( ) );

    PublicHelloWorld::sClientRegister theClient;
    ClientList::LISTPOS pos = mClientList.firstPosition( );
    for ( ; mClientList.isValidPosition( pos ); pos = mClientList.nextPosition( pos ) )
    {
        const PublicHelloWorld::sClientRegister & client = mClientList.valueAtPosition( pos );
        if ( (client.crName == name) && (client.crService == service) && (client.crThread == thread) )
        {
            LOG_DBG( "Found connected client [ %s ] with ID [ %u ] in the list.", client.crName.getString( ), client.crID );
            theClient = client;
            break;
        }
    }

    if ( mClientList.isInvalidPosition( pos ) )
    {
        theClient = PublicHelloWorld::sClientRegister( areg::generateUniqueId( ), name, service, thread, process );
        mClientList.pushFirst( theClient );
        LOG_DBG( "Registered [ %u ] new client [ %s ] of service [ %s ] in thread [ %s ] of process [ %s ]"
            , theClient.crID
            , theClient.crName.getString( )
            , areg::ServiceAddress::convAddressToPath( theClient.crService ).getString( )
            , theClient.crThread.getString( )
            , theClient.crProcess.getString( ) );
    }

    responseRegister( theClient );
}

void PublicHelloWorldService::requestUnregister( const PublicHelloWorld::sClientRegister & client )
{
    LOG_SCOPE( examples_16_pubmesh_common_PublicHelloWorldService_requestUnregister );
    LOG_DBG( "The client [ %s ] with registered ID [ %u ] requested unregister.", client.crName.getString( ), client.crID );

    for ( ClientList::LISTPOS pos = mClientList.firstPosition( ); mClientList.isValidPosition( pos ); pos = mClientList.nextPosition( pos ) )
    {
        const PublicHelloWorld::sClientRegister & entry = mClientList.valueAtPosition( pos );
        if ( entry == client )
        {
            mClientList.removeAt( pos );
            LOG_DBG( "Removed entry, there are still [ %d ] registered clients", mClientList.getSize( ) );
            break;
        }
    }
}

void PublicHelloWorldService::requestHelloWorld( uint32_t clientID )
{
    LOG_SCOPE( examples_16_pubmesh_common_PublicHelloWorldService_requestHelloWorld );

    PublicHelloWorld::sClientRegister theClient;
    ClientList::LISTPOS pos = mClientList.firstPosition( );
    for ( ; mClientList.isValidPosition( pos ); pos = mClientList.nextPosition( pos ) )
    {
        const PublicHelloWorld::sClientRegister & client = mClientList.valueAtPosition( pos );
        if ( clientID == client.crID )
        {
            LOG_DBG( "Found connected client [ %s ] with ID [ %u ] in the list.", client.crName.getString( ), client.crID );
            theClient = client;
            break;
        }
    }

    if ( mClientList.isValidPosition( pos ) )
    {
        // use printf() because of multithreading environment.
        printf( "\">>> Public [ %s ]!\", processed [ %u ] requests.\n\r", theClient.crName.getString( ), ++mNumMessages );
    }

    responseHelloWorld( theClient.crID );
}

bool PublicHelloWorldService::clientConnected(const areg::ProxyAddress & client, areg::ServiceConnectionState status)
{
    return PublicHelloWorldStub::clientConnected(client, status);
}
