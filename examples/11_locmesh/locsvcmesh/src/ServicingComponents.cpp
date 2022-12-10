/************************************************************************
 * \file        HelloWorldService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "locsvcmesh/src/ServicingComponents.hpp"
#include "areg/trace/GETrace.h"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>


DEF_TRACE_SCOPE(examples_11_locsvcmesh_HelloWorldService_requestHelloWorld);
DEF_TRACE_SCOPE(examples_11_locsvcmesh_HelloWorldService_requestClientShutdown);

// 

HelloWorldService::HelloWorldService( Component & masterComp, bool isMain )
    : HelloWorldStub    ( masterComp )
    , mIsMain           ( isMain )
    , mGnerateID        ( 0 )
    , mClientList       ( )
    , mRemainRequest    ( NEHelloWorld::MaxMessages )
{
}

void HelloWorldService::requestHelloWorld(const String & roleName)
{
    TRACE_SCOPE(examples_11_locsvcmesh_HelloWorldService_requestHelloWorld);

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
            TRACE_WARN( "The controller component [ %s ] broadcasts message to shutdown application", mComponent.getRoleName( ).getString( ) );
            broadcastReachedMaximum( NEHelloWorld::MaxMessages );
        }
    }
    else
    {
        TRACE_WARN( "The service [  %s ] still wait [ %d ] requests to print Hello World.", mComponent.getRoleName().getString(), mRemainRequest);
    }
}

void HelloWorldService::requestClientShutdown(unsigned int clientID, const String & roleName)
{
    TRACE_SCOPE(examples_11_locsvcmesh_HelloWorldService_requestClientShutdown);
    TRACE_DBG("A client [ %s ] with ID [ %u ] notified shutdown.", roleName.getString(), clientID);

    if (mIsMain)
    {
        TRACE_INFO("All clients are set message to shutdown, all [ %d ] messages are output, going to shutdown application", NEHelloWorld::MaxMessages);
        Application::signalAppQuit();
    }
}

//////////////////////////////////////////////////////////////////////////
// ControllerComponent implementations
//////////////////////////////////////////////////////////////////////////

Component * ControllerComponent::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    return DEBUG_NEW ControllerComponent( entry, owner );
}

void ControllerComponent::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & /*entry*/ )
{
    delete (&compObject);
}

ControllerComponent::ControllerComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    : Component ( owner, entry.mRoleName )
    , mService  ( static_cast<Component &>(self( )), entry.getComponentData( ).alignBool.mElement )
{
}

//////////////////////////////////////////////////////////////////////////
// SecondaryComponent implementations
//////////////////////////////////////////////////////////////////////////

Component * SecondaryComponent::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    return DEBUG_NEW SecondaryComponent( entry, owner );
}

void SecondaryComponent::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & /*entry*/ )
{
    delete (&compObject);
}

SecondaryComponent::SecondaryComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    : Component         ( owner, entry.mRoleName )
    , mService          ( static_cast<Component &>(self( )), entry.getComponentData( ).alignBool.mElement )
    , mClientMain       ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self( )) )
    , mClientSecondary  ( entry.mDependencyServices[1].mRoleName, static_cast<Component &>(self( )) )
{
}
