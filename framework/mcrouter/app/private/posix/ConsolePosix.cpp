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
 * \brief       AREG Platform, Multi-cast routing console that outputs stistics.
 *              POSIX specific part implementation.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "mcrouter/app/private/Console.hpp"

#ifdef _POSIX

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

bool Console::_osInitialize(void)
{
    if (mSetupEnv == false)
    {
        puts(CMD_ENTER_SCREEN.data());
        puts(CMD_CLEAR_SCREEN.data());
        puts(CMD_SCROLL_BACK.data());

        mSetupEnv = true;
    }

    return mSetupEnv;
}

void Console::_osUnitialize(void)
{
    if (mSetupEnv)
    {
        // restore previous mode.
        // fputs(CMD_EXIT_SCREEN.data(), stdout);

        mContext = 0;
        mSetupEnv = false;
    }
}

void Console::_setCursorCurrentPos(const Console::sCoord& pos)
{
    // printf(CMD_MOVE_CURSOR.data(), pos.posY + 1, pos.posX + 1);
	printf(Console::FORMAT_WAIT_QUIT.data(), MSG_WAIT_QUIT.data());
    printf(Console::FORMAT_ENDOF_MSG.data(), static_cast<int32_t>(MSG_WAIT_QUIT.length() + mCommand.getLength()) + 1);
}

Console::sCoord Console::_getCursorCurrentPos(void)
{
    int posX{ 0 };
    int posY{ 0 };

    struct termios oldTerm, newTerm;

    tcgetattr(STDOUT_FILENO, &oldTerm);
    newTerm.c_lflag &= ~(ECHO | ICANON);
    cfmakeraw(&newTerm);
    tcsetattr(STDOUT_FILENO, TCSANOW, &newTerm);

    puts(CMD_POS_CURSOR.data());
    fflush(stdout);
    fscanf(stdout, CMD_READ_CURSOR.data(), &posX, &posY);

    tcsetattr(STDOUT_FILENO, TCSANOW, &oldTerm);
    // printf("\033[6n");
    // scanf("\033[%d;%dR", &posX, &posY);

    return sCoord{ static_cast<int16_t>(posX), static_cast<int16_t>(posY)};
}

#endif  // _POSIX
