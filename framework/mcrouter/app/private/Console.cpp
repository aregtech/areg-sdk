/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/private/Console.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing console that outputs stistics.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "mcrouter/app/private/Console.hpp"
#include "areg/base/NECommon.hpp"
#include "areg/base/String.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(mcrouter_app_Console_waitQuitCommand);

Console::Console(void)
    : mSetupEnv     ( false )
    , mVerbose      ( false )
    , mStatSent     ( )
    , mStatReceived ( )
    , mContext      ( 0 )
    , mLock         (false)
{
}

Console::~Console(void)
{
    _osUnitialize();
}

Console & Console::_getInstance( void )
{
    static Console   _instance;
    return _instance;
}

void Console::initialize(bool verbose)
{
    Console & console = Console::_getInstance();
    console.mVerbose = verbose;
    if (verbose && console._osInitialize())
    {
        Timestamp now = Clock::now();

        console.mStatSent.statLastUpdate = now;
        console.mStatSent.statDataSize = 0;

        console.mStatReceived.statLastUpdate = now;
        console.mStatReceived.statDataSize = 0;

        printf(FORMAT_SEND_DATA.data(), 0.0f, MSG_BYTES.data());
        printf(FORMAT_RECV_DATA.data(), 0.0f, MSG_BYTES.data());
    }
}

void Console::sentBytes(uint32_t bytesSent)
{
    Console & console = Console::_getInstance();

    if (console.mVerbose == false)
        return; // do nothing

    if (bytesSent != 0)
    {
        console.mStatSent.statDataSize += bytesSent;

        Timestamp now = Clock::now();
        std::chrono::duration<float, Seconds> duration = now - console.mStatSent.statLastUpdate;
        uint32_t elapsed = static_cast<uint32_t>(duration.count());
        if (elapsed >= 1)
        {
            Lock lock(console.mLock);

            console._outputSendBytes(elapsed);

            console.mStatSent.statDataSize = 0;
            console.mStatSent.statLastUpdate = now;
        }
    }
    else
    {
        Lock lock(console.mLock);
        console._outputSendBytes(0);
    }
}

void Console::receivedBytes(uint32_t bytesReceived)
{
    Console & console = Console::_getInstance();

    if (console.mVerbose == false)
        return; // do nothing
    
    if (bytesReceived != 0)
    {
        console.mStatReceived.statDataSize += bytesReceived;

        Timestamp now = Clock::now();
        std::chrono::duration<float, Seconds> duration = now - console.mStatReceived.statLastUpdate;
        uint32_t elapsed = static_cast<uint32_t>(duration.count());
        if (elapsed >= 1)
        {

            Lock lock(console.mLock);

            console._outputReceiveBytes(elapsed);

            console.mStatReceived.statDataSize = 0;
            console.mStatReceived.statLastUpdate = now;
        }
    }
    else
    {
        Lock lock(console.mLock);
        console._outputReceiveBytes(0);
    }
}

void Console::waitQuitCommand( void )
{
    TRACE_SCOPE(mcrouter_app_Console_waitQuitCommand);
    Console & console = Console::_getInstance();
    console.mCommand.clear();

    printf(Console::FORMAT_WAIT_QUIT.data(), Console::MSG_WAIT_QUIT.data());

    bool doQuit = false;
    do
    {
        char ch = static_cast<char>(getchar());

        TRACE_DBG("Got a character with code [ %d ]", static_cast<int>(ch));

        if (ch == '\n')
        {
            if ((console.mCommand == Console::QUIT_CH) || (console.mCommand == Console::QUIT_STR))
            {
                doQuit = true;
                TRACE_DBG("Quiting console....");
            }
            else
            {
                console.mCommand.clear();
                printf(FORMAT_MSG_ERROR.data(), console.mCommand.getString());
                puts(Console::CMD_CLEAR_LINE.data());
                printf(Console::FORMAT_WAIT_QUIT.data(), Console::MSG_WAIT_QUIT.data());
            }
        }
        else if (NEString::isAlphanumeric(ch))
        {
            console.mCommand += NEString::makeAsciiLower(ch);
        }

    } while (doQuit == false);

    console._osUnitialize();
}

void Console::_outputSendBytes(uint32_t duration)
{
    _setCursorCurrentPos(COORD_SEND_DATA);

    if (duration == 0)
    {
        printf(FORMAT_SEND_DATA.data(), 0.0f, MSG_BYTES.data());
    }
    else
    {
        float rate = static_cast<float>(mStatSent.statDataSize) / duration;
        if (static_cast<uint32_t>(rate) > ONE_MEGABYTE)
        {
            rate /= ONE_MEGABYTE;
            printf(FORMAT_SEND_DATA.data(), rate, MSG_MEGABYTES.data());
        }
        else if (static_cast<uint32_t>(rate) > ONE_KILOBYTE)
        {
            rate /= ONE_KILOBYTE;
            printf(FORMAT_SEND_DATA.data(), rate, MSG_KILOBYTES.data());
        }
        else
        {
            printf(FORMAT_SEND_DATA.data(), rate, MSG_BYTES.data());
        }
    }

	printf(Console::FORMAT_WAIT_QUIT.data(), MSG_WAIT_QUIT.data());

    //_setCursorCurrentPos(sCoord{static_cast<int16_t>(Console::MSG_WAIT_QUIT.length() + mCommand.getLength()), Console::COORD_OUTPUT_MSG.posY});
}

void Console::_outputReceiveBytes(uint32_t duration)
{
    if (duration == 0)
    {
        printf(FORMAT_RECV_DATA.data(), 0.0f, MSG_BYTES.data());
    }
    else
    {
        float rate = static_cast<float>(mStatReceived.statDataSize) / duration;
        if (static_cast<uint32_t>(rate) > ONE_MEGABYTE)
        {
            rate /= ONE_MEGABYTE;
            printf(FORMAT_RECV_DATA.data(), rate, MSG_MEGABYTES.data());
        }
        else if (static_cast<uint32_t>(rate) > ONE_KILOBYTE)
        {
            rate /= ONE_KILOBYTE;
            printf(FORMAT_RECV_DATA.data(), rate, MSG_KILOBYTES.data());
        }
        else
        {
            printf(FORMAT_RECV_DATA.data(), rate, MSG_BYTES.data());
        }
    }

	printf(Console::FORMAT_WAIT_QUIT.data(), MSG_WAIT_QUIT.data());

    // _setCursorCurrentPos(sCoord{static_cast<int16_t>(MSG_WAIT_QUIT.length() + mCommand.getLength()), COORD_OUTPUT_MSG.posY});
}

