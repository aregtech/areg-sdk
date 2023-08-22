/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logger/app/private/LoggerConsoleService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Logger, service that outputs statistics.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "logger/app/private/LoggerConsoleService.hpp"

#include "logger/app/Logger.hpp"

//////////////////////////////////////////////////////////////////////////
// LoggerConsoleService class implementation, static methods
//////////////////////////////////////////////////////////////////////////
Component * LoggerConsoleService::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    return DEBUG_NEW LoggerConsoleService( entry, owner, entry.getComponentData( ) );
}

void LoggerConsoleService::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry )
{
    delete (&compObject);
}

//////////////////////////////////////////////////////////////////////////
// LoggerConsoleService class implementation
//////////////////////////////////////////////////////////////////////////
LoggerConsoleService::LoggerConsoleService( const NERegistry::ComponentEntry & entry, ComponentThread & owner, NEMemory::uAlign OPT data )
    : SystemServiceConsole( NELoggerSettings::APP_TITLE, static_cast<SystemServiceBase &>(Logger::getInstance( )), entry, owner, data )
{
}
