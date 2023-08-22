/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/private/RouterConsoleService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing, service that outputs statistics.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "mcrouter/app/private/RouterConsoleService.hpp"

#include "mcrouter/app/MulticastRouter.hpp"

//////////////////////////////////////////////////////////////////////////
// RouterConsoleService class implementation, static methods
//////////////////////////////////////////////////////////////////////////
Component * RouterConsoleService::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    return DEBUG_NEW RouterConsoleService( entry, owner, entry.getComponentData( ) );
}

void RouterConsoleService::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry )
{
    delete (&compObject);
}

//////////////////////////////////////////////////////////////////////////
// RouterConsoleService class implementation
//////////////////////////////////////////////////////////////////////////
RouterConsoleService::RouterConsoleService( const NERegistry::ComponentEntry & entry, ComponentThread & owner, NEMemory::uAlign OPT data )
    : SystemServiceConsole( NEMulticastRouterSettings::APP_TITLE, static_cast<SystemServiceBase &>(MulticastRouter::getInstance()), entry, owner, data )
{
}
