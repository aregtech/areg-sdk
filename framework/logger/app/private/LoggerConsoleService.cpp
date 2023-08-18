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

#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include "extensions/console/Console.hpp"

#include "logger/app/Logger.hpp"
#include "logger/app/NELoggerSettings.hpp"

 //////////////////////////////////////////////////////////////////////////
 // LoggerConsoleService::DataRate helper class implementation
 //////////////////////////////////////////////////////////////////////////
LoggerConsoleService::DataRate::DataRate(uint32_t sizeBytes)
    : mRate ( )
{
    if (sizeBytes >= ONE_MEGABYTE)
    {
        double rate = static_cast<double>(sizeBytes) / ONE_MEGABYTE;
        mRate.first = static_cast<float>(rate);
        mRate.second= LoggerConsoleService::MSG_MEGABYTES;
    }
    else if (sizeBytes >= ONE_KILOBYTE)
    {
        double rate = static_cast<double>(sizeBytes) / ONE_KILOBYTE;
        mRate.first = static_cast<float>(rate);
        mRate.second= LoggerConsoleService::MSG_KILOBYTES;
    }
    else
    {
        mRate.first = static_cast<float>(sizeBytes);
        mRate.second = LoggerConsoleService::MSG_BYTES;
    }
}

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
    : Component         ( entry, owner )
    , StubBase          ( self( ), NEService::getEmptyInterface( ) )
    , IETimerConsumer   ( )

    , mTimer            ( self( ), "ServicingTimer" )
{
}

void LoggerConsoleService::startupServiceInterface( Component & holder )
{
    StubBase::startupServiceInterface( holder );

    Console& console = Console::getInstance();

    console.outputMsg(NELoggerSettings::COORD_SEND_RATE, NELoggerSettings::FORMAT_SEND_DATA.data(), 0.0f, LoggerConsoleService::MSG_BYTES.data());
    console.outputMsg(NELoggerSettings::COORD_RECV_RATE, NELoggerSettings::FORMAT_RECV_DATA.data(), 0.0f, LoggerConsoleService::MSG_BYTES.data());
    console.outputTxt(NELoggerSettings::COORD_USER_INPUT, NELoggerSettings::FORMAT_WAIT_QUIT);

    mTimer.startTimer(NECommon::TIMEOUT_1_SEC, Timer::CONTINUOUSLY);

    console.enableConsoleInput(true);
    console.refreshScreen();
}

void LoggerConsoleService::shutdownServiceIntrface( Component & holder )
{
    mTimer.stopTimer( );
    StubBase::shutdownServiceIntrface( holder );
}

void LoggerConsoleService::processTimer( Timer & timer )
{
    ASSERT( &timer == &mTimer );
    if ( mTimer.isActive( ) )
    {
        _outputDataRate(Logger::getInstance().queryDataSent(), Logger::getInstance().queryDataReceived());
    }
}

//////////////////////////////////////////////////////////////////////////
// These methods must exist, but can have empty body
//////////////////////////////////////////////////////////////////////////
void LoggerConsoleService::sendNotification( unsigned int msgId )
{
}

void LoggerConsoleService::errorRequest( unsigned int msgId, bool msgCancel )
{
}

void LoggerConsoleService::processRequestEvent( ServiceRequestEvent & eventElem )
{
}

void LoggerConsoleService::processAttributeEvent( ServiceRequestEvent & eventElem )
{
}

inline void LoggerConsoleService::_outputDataRate(uint32_t bytesSend, uint32_t bytesRecv)
{
    Console& console = Console::getInstance();

    console.saveCursorPosition();
    LoggerConsoleService::DataRate rateSend(bytesSend);
    LoggerConsoleService::DataRate rateRecv(bytesRecv);

    console.outputMsg(NELoggerSettings::COORD_SEND_RATE, NELoggerSettings::FORMAT_SEND_DATA.data(), rateSend.mRate.first, rateSend.mRate.second.c_str());
    console.outputMsg(NELoggerSettings::COORD_RECV_RATE, NELoggerSettings::FORMAT_RECV_DATA.data(), rateRecv.mRate.first, rateRecv.mRate.second.c_str());

    console.restoreCursorPosition();
    console.refreshScreen();
}
