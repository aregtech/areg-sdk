/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/private/Statistics.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing statistics.
 *              Windows OS specific part implementation.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "mcrouter/app/private/Statistics.hpp"

#ifdef _POSIX

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <curses.h>


bool Statistics::_osInitialize(void)
{
    if (mSetupEnv == false)
    {
        puts(CMD_ENTER_SCREEN.data())
        puts(CMD_CLEAR_SCREEN.data());
        puts(CMD_SCROLL_BACK.data());

        mSetupEnv = true;
    }

    return mSetupEnv;
}

void Statistics::_osUnitialize(void)
{
    if (mSetupEnv)
    {
        // restore previous mode.
        puts(CMD_EXIT_SCREEN.data());

        mContext = 0;
        mSetupEnv = false;
    }
}

void Statistics::_setCursorCurrentPos(const Statistics::sCoord& pos)
{
    printf(CMD_MOVE_CURSOR.data(), pos.posY + 1, pos.posX + 1);
}

Statistics::sCoord Statistics::_getCursorCurrentPos(void)
{
    int posX{ 0 };
    int posY{ 0 };

    struct termios oldTerm, newTerm;

    tcgetattr(STDOUT_FILENO, &oldTerm);
    newTerm.c_lflag &= ~(ECHO | ICANON);
    cfmakeraw(&newTerm);
    tcsetattr(STDOUT_FILENO, TCSANOW, &newTerm);

    printf(CMD_POS_CURSOR.data());
    fflush(stdout);
    scanf(CMD_READ_CURSOR.data(), &posX, &posY);

    tcsetattr(STDOUT_FILENO, TCSANOW, &oldTerm);
    // printf("\033[6n");
    // scanf("\033[%d;%dR", &posX, &posY);

    return sCoord{ static_cast<int16_t>(posX), static_cast<int16_t>(posY)};
}

#endif  // _POSIX
