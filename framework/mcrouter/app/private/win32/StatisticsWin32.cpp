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

#ifdef WINDOWS

#include "areg/base/NEMemory.hpp"

#include <windows.h>

bool Statistics::_osInitialize( void )
{
    mIsInitialized = true;
    return mIsInitialized;
}

void Statistics::_setCursorCurrentPos(const Statistics::sCoord& pos)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord{ pos.posX, pos.posY };
    SetConsoleCursorPosition(h, coord);
}

Statistics::sCoord Statistics::_getCursorCurrentPos(void)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    NEMemory::memZero(&bufferInfo, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
    GetConsoleScreenBufferInfo(h, &bufferInfo);
    const COORD & coord = reinterpret_cast<const COORD &>(bufferInfo.dwCursorPosition);
    return sCoord{ coord.X, coord.Y };
}

#endif  // WINDOWS
