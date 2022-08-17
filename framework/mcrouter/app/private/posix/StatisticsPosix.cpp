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
#include <curses.h> // similar name for windows


namespace
{
    WINDOW* _newTerminal()
    {

        newterm(getenv("TERM"), stdout, stdin);
        return stdscr;
    }

    WINDOW * _getTerminal()
    {
        static WINDOW * win = stdscr == nullptr ? curscr == nullptr ? _newTerminal() : curscr : stdscr;
        return win;
    }
}

bool Statistics::_osInitialize( void )
{
    if (mIsInitialized == false)
    {
        initscr();
        mIsInitialized = true;
    }

    return mIsInitialized;
}

void Statistics::_setCursorCurrentPos(const Statistics::sCoord& pos)
{
    printf("\033[%d;%dH", pos.posY + 1, pos.posX + 1);
}

Statistics::sCoord Statistics::_getCursorCurrentPos(void)
{
    int posX{ 0 };
    int posY{ 0 };

    WINDOW * scr = stdscr;
    getyx(scr, posY, posX);
    // printf("\033[6n");
    // scanf("\033[%d;%dR", &posX, &posY);

    return sCoord{ static_cast<int16_t>(posX), static_cast<int16_t>(posY)};
}

#endif  // _POSIX
