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
#include "areg/trace/GETrace.h"

#include <cstdio>
#include <iostream>

DEF_TRACE_SCOPE(mcrouter_app_ConsoleService_runWaitInput);

#if 0
void ConsoleService::runWaitInput(ConsoleService* theConsole)
{
    bool run{ true };
    struct pollfd pfd = { STDIN_FILENO, POLLIN, 0 };

    std::string line;
    int ret = 0;
    while (ret == 0)
    {
        ret = poll(&pfd, 1, 1000);  // timeout of 1000ms
        if (ret == 1) // there is something to read
        {
            std::getline(std::cin, line);
        }
        else if (ret == -1)
        {
            std::cout << "Error: " << strerror(errno) << std::endl;
        }
    }
    return line;

    do
    {
        char ch = getchar();

        if ((ch == static_cast<char>(27)) || (ch == static_cast<char>(0)))
        {
            run = false;
            ConsoleEvent::sendEvent(ConsoleEventData(ConsoleEventData::eEvent::ConsoleContinue), theConsole->getMasterThread());
        }
        else if (ch == '\n')
        {
            run = false;
            ConsoleEvent::sendEvent(ConsoleEventData(ConsoleEventData::eEvent::ConsoleCheck), theConsole->getMasterThread());
        }
        else
        {
            theConsole->mCommand += ch;
        }
    } while (run);
}
#endif

Component * ConsoleService::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    return DEBUG_NEW ConsoleService( owner, entry.mRoleName.getString( ), entry.getComponentData( ) );
}

void ConsoleService::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry )
{
    delete (&compObject);
}

bool ConsoleService::waitQuitCommand(void)
{
    String command;
    bool run{ true };

    printf(ConsoleService::FORMAT_WAIT_QUIT_W.data(), command.getString());

    do
    {
        char ch = getchar();

        if ((ch == static_cast<char>(27)) || (ch == static_cast<char>(0)))
        {
            run = false;
        }
        else if (ch == '\n')
        {
            run = false;
        }
        else
        {
            command += ch;
        }
    } while (run);

    return ((command == ConsoleService::QUIT_CH) || (command == ConsoleService::QUIT_STR));
}

ConsoleService::ConsoleService( ComponentThread & masterThread, const char * const roleName, NEMemory::uAlign OPTIONAL data )
    : Component             ( masterThread, roleName )
    , StubBase              ( self( ), NEService::getEmptyInterface( ) )
    , IETimerConsumer       ( )
    , IEConsoleEventConsumer( )
    , mTimer                ( self( ), "ServicingTimer" )
    , mCommand              ( )
    , mDispError            ( false )
    , mContext              ( 0 )
    , mOutThred             ( self() )
{
}

void ConsoleService::startupServiceInterface( Component & holder )
{
    StubBase::startupServiceInterface( holder );
    
    _osInitialize();

    ConsoleEvent::addListener(static_cast<IEConsoleEventConsumer&>(self()), holder.getMasterThread());

    _osDataRate(0, 0, true);
    mOutThred.createThread(NECommon::WAIT_INFINITE);
    mTimer.startTimer(NECommon::TIMEOUT_1_SEC, Timer::CONTINUOUSLY);
}

void ConsoleService::shutdownServiceIntrface( Component & holder )
{
    ConsoleEvent::removeListener(static_cast<IEConsoleEventConsumer&>(self()));
    mTimer.stopTimer( );

    StubBase::shutdownServiceIntrface( holder );
    _osUninitialize();
}

void ConsoleService::processTimer( Timer & timer )
{
    ASSERT( &timer == &mTimer );
    if ( mTimer.isActive( ) )
    {
        _osDataRate(MulticastRouter::getInstance().queryDataSent(), MulticastRouter::getInstance().queryDataReceived(), false);
    }
    else
    {
        Application::signalAppQuit( );
    }
}

void ConsoleService::processEvent(const ConsoleEventData& data)
{
    if (isQuitApplication())
    {
        mTimer.stopTimer();
        mOutThred.mExit.setEvent();
        fwrite(ConsoleService::QUIT_STR.data(), 1, static_cast<size_t>(ConsoleService::QUIT_STR.length()), stdout);
        fflush(stdout);
        mOutThred.destroyThread(NECommon::WAIT_100_MILLISECONDS);
        Application::signalAppQuit();
    }
    else
    {
        printf(ConsoleService::FORMAT_MSG_ERROR_W.data(), mCommand.getString());
        mDispError = true;
    }

    mCommand = String::EmptyString;
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

bool ConsoleService::isQuitApplication(void)
{
    return (mCommand == QUIT_CH) || (mCommand == QUIT_STR);
}

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

ConsoleService::OutputThread::OutputThread(ConsoleService& theService)
    : Thread            (static_cast<IEThreadConsumer &>(self()), "OutputThread")
    , IEThreadConsumer  ( )
    , mTheConsole       ( theService )
    , mExit             ( true, false )
{
}

void ConsoleService::OutputThread::onThreadRuns(void)
{
    bool run{ true };
    do
    {
        if (mExit.lock(NECommon::WAIT_10_MILLISECONDS) == false)
        {
            char ch = getchar();
            if (ch == '\n')
            {
                ConsoleEvent::sendEvent(ConsoleEventData(), mTheConsole.getMasterThread());
            }
            else if (ch != '\0')
            {
                mTheConsole.mCommand += ch;
            }
        }
    } while (run);
}
