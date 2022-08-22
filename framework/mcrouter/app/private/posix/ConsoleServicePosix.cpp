/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/private/ConsolePosix.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing ConsoleService that outputs statistics.
 *              POSIX specific part implementation.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "mcrouter/app/private/ConsoleService.hpp"

#ifdef _POSIX

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

void ConsoleService::_osInitialize(void)
{
    puts(CMD_ENTER_SCREEN.data());
    puts(CMD_CLEAR_SCREEN.data());
    puts(CMD_SCROLL_BACK.data());
}

void ConsoleService::_osUninitialize(void)
{
    // restore previous mode.
    // fputs(CMD_EXIT_SCREEN.data(), stdout);
    mContext = 0;
}

void ConsoleService::_osDataRate(uint32_t bytesSent, uint32_t bytesReceive, bool isInit)
{
    do
    {
        ConsoleService::DataRate dataRate(bytesSent);
        std::pair<float, std::string> rate = dataRate.getRate();
        printf(FORMAT_SEND_DATA_X.data(), rate.first, rate.second.c_str());
    } while (false);

    do
    {
        ConsoleService::DataRate dataRate(bytesReceive);
        std::pair<float, std::string> rate = dataRate.getRate();
        printf(FORMAT_RECV_DATA_X.data(), rate.first, rate.second.c_str());
    } while (false);

    printf(FORMAT_WAIT_QUIT_X.data(), mCommand.getString());

    if (mDispError)
    {
        puts(CMD_CLEAR_LINE.data());
    }

    printf(FORMAT_WAIT_QUIT_X.data(), mCommand.getString());

    mDispError = false;
}

/*
void ConsoleService::_osWaitToQuit(void)
{
    if (mDispError)
    {
        printf(FORMAT_MSG_ERROR_X.data(), mCommand.getString());
    }
}
*/

#endif  // _POSIX
