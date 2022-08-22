/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/private/StatService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing, service that outputs statistics.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "mcrouter/app/private/StatService.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include "mcrouter/app/private/Console.hpp"

Component * StatService::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    return DEBUG_NEW StatService( owner, entry.mRoleName.getString( ), entry.getComponentData( ) );
}

void StatService::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry )
{
    delete (&compObject);
}

StatService::StatService( ComponentThread & masterThread, const char * const roleName, NEMemory::uAlign OPTIONAL data )
    : Component ( masterThread, roleName )
    , StubBase  ( self( ), NEService::getEmptyInterface( ) )

    , mTimer    ( self( ), "ServicingTimer" )
    , mCount    ( 0 )
{
}

void StatService::startupServiceInterface( Component & holder )
{
    StubBase::startupServiceInterface( holder );
    mTimer.startTimer( NECommon::TIMEOUT_1_SEC, Timer::CONTINUOUSLY );
    Console::initialize(true);
}

void StatService::shutdownServiceIntrface( Component & holder )
{
    mTimer.stopTimer( );
    StubBase::shutdownServiceIntrface( holder );
}

void StatService::processTimer( Timer & timer )
{
    ASSERT( &timer == &mTimer );
    if ( mTimer.isActive( ) )
    {
        printf( "Hello Service!\n" );
    }
    else
    {
        Application::signalAppQuit( );
    }
}

//////////////////////////////////////////////////////////////////////////
// These methods must exist, but can have empty body
//////////////////////////////////////////////////////////////////////////
void StatService::sendNotification( unsigned int msgId )
{
}

void StatService::errorRequest( unsigned int msgId, bool msgCancel )
{
}

void StatService::processRequestEvent( ServiceRequestEvent & eventElem )
{
}

void StatService::processAttributeEvent( ServiceRequestEvent & eventElem )
{
}
