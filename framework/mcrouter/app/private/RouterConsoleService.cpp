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

#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include "extensions/console/Console.hpp"

#include "mcrouter/app/MulticastRouter.hpp"
#include "mcrouter/app/NEMulticastRouterSettings.hpp"

 //////////////////////////////////////////////////////////////////////////
 // RouterConsoleService::DataRate helper class implementation
 //////////////////////////////////////////////////////////////////////////
RouterConsoleService::DataRate::DataRate(uint32_t sizeBytes)
    : mRate ( )
{
    if (sizeBytes >= ONE_MEGABYTE)
    {
        double rate = static_cast<double>(sizeBytes) / ONE_MEGABYTE;
        mRate.first = static_cast<float>(rate);
        mRate.second= RouterConsoleService::MSG_MEGABYTES;
    }
    else if (sizeBytes >= ONE_KILOBYTE)
    {
        double rate = static_cast<double>(sizeBytes) / ONE_KILOBYTE;
        mRate.first = static_cast<float>(rate);
        mRate.second= RouterConsoleService::MSG_KILOBYTES;
    }
    else
    {
        mRate.first = static_cast<float>(sizeBytes);
        mRate.second = RouterConsoleService::MSG_BYTES;
    }
}

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
    : Component         ( entry, owner )
    , StubBase          ( self( ), NEService::getEmptyInterface( ) )
    , IETimerConsumer   ( )

    , mTimer            ( self( ), "ServicingTimer" )
{
}

void RouterConsoleService::startupServiceInterface( Component & holder )
{
    StubBase::startupServiceInterface( holder );

    Console& console = Console::getInstance();

    console.outputMsg(NEMulticastRouterSettings::COORD_SEND_RATE, NEMulticastRouterSettings::FORMAT_SEND_DATA.data(), 0.0f, RouterConsoleService::MSG_BYTES.data());
    console.outputMsg(NEMulticastRouterSettings::COORD_RECV_RATE, NEMulticastRouterSettings::FORMAT_RECV_DATA.data(), 0.0f, RouterConsoleService::MSG_BYTES.data());
    console.outputTxt(NEMulticastRouterSettings::COORD_USER_INPUT, NEMulticastRouterSettings::FORMAT_WAIT_QUIT);

    mTimer.startTimer(NECommon::TIMEOUT_1_SEC, Timer::CONTINUOUSLY);

    console.enableConsoleInput(true);
    console.refreshScreen();
}

void RouterConsoleService::shutdownServiceIntrface( Component & holder )
{
    mTimer.stopTimer( );
    StubBase::shutdownServiceIntrface( holder );
}

void RouterConsoleService::processTimer( Timer & timer )
{
    ASSERT( &timer == &mTimer );
    if ( mTimer.isActive( ) )
    {
        _outputDataRate(MulticastRouter::getInstance().queryDataSent(), MulticastRouter::getInstance().queryDataReceived());
    }
}

//////////////////////////////////////////////////////////////////////////
// These methods must exist, but can have empty body
//////////////////////////////////////////////////////////////////////////
void RouterConsoleService::sendNotification( unsigned int msgId )
{
}

void RouterConsoleService::errorRequest( unsigned int msgId, bool msgCancel )
{
}

void RouterConsoleService::processRequestEvent( ServiceRequestEvent & eventElem )
{
}

void RouterConsoleService::processAttributeEvent( ServiceRequestEvent & eventElem )
{
}

inline void RouterConsoleService::_outputDataRate(uint32_t bytesSend, uint32_t bytesRecv)
{
    Console& console = Console::getInstance();

    console.saveCursorPosition();
    RouterConsoleService::DataRate rateSend(bytesSend);
    RouterConsoleService::DataRate rateRecv(bytesRecv);

    console.outputMsg(NEMulticastRouterSettings::COORD_SEND_RATE, NEMulticastRouterSettings::FORMAT_SEND_DATA.data(), rateSend.mRate.first, rateSend.mRate.second.c_str());
    console.outputMsg(NEMulticastRouterSettings::COORD_RECV_RATE, NEMulticastRouterSettings::FORMAT_RECV_DATA.data(), rateRecv.mRate.first, rateRecv.mRate.second.c_str());

    console.restoreCursorPosition();
    console.refreshScreen();
}

bool RouterConsoleService::checkCommand(const String& cmd)
{
    String command(cmd);
    command.makeLower();
    if ((command != NEMulticastRouterSettings::QUIT_CH) && (command != NEMulticastRouterSettings::QUIT_STR))
    {
        Console& console = Console::getInstance();

        ASSERT(MulticastRouter::getInstance().isVerbose());
        console.outputMsg(NEMulticastRouterSettings::COORD_ERROR_MSG, NEMulticastRouterSettings::FORMAT_MSG_ERROR.data(), cmd.getString());
        console.outputTxt(NEMulticastRouterSettings::COORD_USER_INPUT, NEMulticastRouterSettings::FORMAT_WAIT_QUIT);
        console.refreshScreen();

        return false;
    }

    return true;
}