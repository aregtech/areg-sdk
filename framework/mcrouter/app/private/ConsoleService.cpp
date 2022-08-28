/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/private/ConsoleService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing, service that outputs statistics.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "mcrouter/app/private/ConsoleService.hpp"

#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"

#include "mcrouter/app/MulticastRouter.hpp"
#include "mcrouter/app/NEMulticastRouterSettings.hpp"
#include "mcrouter/app/private/Console.hpp"

DEF_TRACE_SCOPE(mcrouter_app_ConsoleService_runWaitInput);

ConsoleService::DataRate::DataRate(uint32_t sizeBytes)
    : mRate ( )
{
    if (sizeBytes >= ONE_MEGABYTE)
    {
        double rate = static_cast<double>(sizeBytes) / ONE_MEGABYTE;
        mRate.first = static_cast<float>(rate);
        mRate.second= ConsoleService::MSG_MEGABYTES;
    }
    else if (sizeBytes >= ONE_KILOBYTE)
    {
        double rate = static_cast<double>(sizeBytes) / ONE_KILOBYTE;
        mRate.first = static_cast<float>(rate);
        mRate.second= ConsoleService::MSG_KILOBYTES;
    }
    else
    {
        mRate.first = static_cast<float>(sizeBytes);
        mRate.second = ConsoleService::MSG_BYTES;
    }
}

Component * ConsoleService::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    return DEBUG_NEW ConsoleService( owner, entry.mRoleName.getString( ), entry.getComponentData( ) );
}

void ConsoleService::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry )
{
    delete (&compObject);
}

ConsoleService::ConsoleService( ComponentThread & masterThread, const char * const roleName, NEMemory::uAlign OPTIONAL data )
    : Component             ( masterThread, roleName )
    , StubBase              ( self( ), NEService::getEmptyInterface( ) )
    , IETimerConsumer       ( )
    , IEConsoleEventConsumer( )
    , mTimer                ( self( ), "ServicingTimer" )
{
}

void ConsoleService::startupServiceInterface( Component & holder )
{
    StubBase::startupServiceInterface( holder );

    Console& console = Console::getInstance();

    console.outputText(NEMulticastRouterSettings::COORD_SEND_RATE, NEMulticastRouterSettings::FORMAT_SEND_DATA.data(), 0.0f, ConsoleService::MSG_BYTES.data());
    console.outputText(NEMulticastRouterSettings::COORD_RECV_RATE, NEMulticastRouterSettings::FORMAT_RECV_DATA.data(), 0.0f, ConsoleService::MSG_BYTES.data());
    console.outputText(NEMulticastRouterSettings::COORD_USER_INPUT, NEMulticastRouterSettings::FORMAT_WAIT_QUIT);

    ConsoleEvent::addListener(static_cast<IEConsoleEventConsumer&>(self()), holder.getMasterThread());
    mTimer.startTimer(NECommon::TIMEOUT_1_SEC, Timer::CONTINUOUSLY);

    console.enableConsoleInput(true);
    console.refreshScreen();
}

void ConsoleService::shutdownServiceIntrface( Component & holder )
{
    ConsoleEvent::removeListener(static_cast<IEConsoleEventConsumer&>(self()));
    mTimer.stopTimer( );
    StubBase::shutdownServiceIntrface( holder );
}

void ConsoleService::processTimer( Timer & timer )
{
    ASSERT( &timer == &mTimer );
    if ( mTimer.isActive( ) )
    {
        outputDataRate(MulticastRouter::getInstance().queryDataSent(), MulticastRouter::getInstance().queryDataReceived());
    }
    else
    {
        Application::signalAppQuit( );
    }
}

void ConsoleService::processEvent(const ConsoleEventData& data)
{
}

//////////////////////////////////////////////////////////////////////////
// These methods must exist, but can have empty body
//////////////////////////////////////////////////////////////////////////
void ConsoleService::sendNotification( unsigned int msgId )
{
}

void ConsoleService::errorRequest( unsigned int msgId, bool msgCancel )
{
}

void ConsoleService::processRequestEvent( ServiceRequestEvent & eventElem )
{
}

void ConsoleService::processAttributeEvent( ServiceRequestEvent & eventElem )
{
}

inline void ConsoleService::outputDataRate(uint32_t bytesSend, uint32_t bytesRecv)
{
    Console& console = Console::getInstance();

    NEMulticastRouterSettings::Coord oldPos = console.getCursorCurPosition();
    ConsoleService::DataRate rateSend(bytesSend);
    ConsoleService::DataRate rateRecv(bytesRecv);

    console.outputText(NEMulticastRouterSettings::COORD_SEND_RATE, NEMulticastRouterSettings::FORMAT_SEND_DATA.data(), rateSend.mRate.first, rateSend.mRate.second.c_str());
    console.outputText(NEMulticastRouterSettings::COORD_RECV_RATE, NEMulticastRouterSettings::FORMAT_RECV_DATA.data(), rateRecv.mRate.first, rateRecv.mRate.second.c_str());

    console.setCursorCurPosition(oldPos);
    console.refreshScreen();
}

bool ConsoleService::checkCommand(const String& cmd)
{
    if ((cmd == NEMulticastRouterSettings::QUIT_CH) || (cmd == NEMulticastRouterSettings::QUIT_STR))
    {
        return true; // interrupt, requested quit
    }
    else
    {
        Console& console = Console::getInstance();

        ASSERT(MulticastRouter::getInstance().isVerbose());
        console.outputText(NEMulticastRouterSettings::COORD_ERROR_MSG, NEMulticastRouterSettings::FORMAT_MSG_ERROR.data(), cmd.getString());
        console.outputText(NEMulticastRouterSettings::COORD_USER_INPUT, NEMulticastRouterSettings::FORMAT_WAIT_QUIT);
        console.refreshScreen();

        return false;
    }
}

