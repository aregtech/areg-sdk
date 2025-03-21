/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logcollector/app/private/LogCollectorConsoleService.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Collector console.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "logcollector/app/private/LogCollectorConsoleService.hpp"

#include "logcollector/app/LogCollector.hpp"

//////////////////////////////////////////////////////////////////////////
// LogCollectorConsoleService class implementation, static methods
//////////////////////////////////////////////////////////////////////////
Component * LogCollectorConsoleService::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    return DEBUG_NEW LogCollectorConsoleService( entry, owner, entry.getComponentData( ) );
}

void LogCollectorConsoleService::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & /* entry */ )
{
    delete (&compObject);
}

//////////////////////////////////////////////////////////////////////////
// LogCollectorConsoleService class implementation
//////////////////////////////////////////////////////////////////////////
LogCollectorConsoleService::LogCollectorConsoleService( const NERegistry::ComponentEntry & entry, ComponentThread & owner, NEMemory::uAlign OPT data )
    : SystemServiceConsole( &LogCollector::getInstance().getDataRateHelper( ), entry, owner, data )
{
}
