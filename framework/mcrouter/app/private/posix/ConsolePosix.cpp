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
 * \brief       AREG Platform, Multi-cast routing, OS specific console.
 *              POSIX specific implementation.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "mcrouter/app/private/Console.hpp"

#ifdef _POSIX

#include <ncurses.h>

#include "mcrouter/app/NEMulticastRouterSettings.hpp"

//////////////////////////////////////////////////////////////////////////
// Console POSIX specific implementation
//////////////////////////////////////////////////////////////////////////

bool Console::_osInitialize(void)
{
    if (mIsReady == false)
    {
        mContext = reinterpret_cast<ptr_type>(initscr());
        if (mContext != 0)
        {
            clear();
            cbreak();
            mIsReady = true;
            // raw();
        }
    }

    return mIsReady;
}

void Console::_osUninitialize(void)
{
    if (mIsReady)
    {
    	if (mContext != 0)
    	{
            delwin(reinterpret_cast<WINDOW *>(mContext));
    	}

        endwin();
        refresh();
        mContext = 0;
        mIsReady = false;
    }
}

void Console::_osOutputText(NEMulticastRouterSettings::Coord pos, const String& text) const
{
    Lock lock(mLock);

    if (mContext != 0)
    {
        ASSERT(mIsReady);
        mvwaddstr(reinterpret_cast<WINDOW *>(mContext), pos.posY, pos.posX, text.getString());
    }
}

void Console::_osOutputText(NEMulticastRouterSettings::Coord pos, const std::string_view& text) const
{
    Lock lock(mLock);

    if (mContext != 0)
    {
        ASSERT(mIsReady);
        mvwaddstr(reinterpret_cast<WINDOW*>(mContext), pos.posY, pos.posX, text.data());
    }
}


NEMulticastRouterSettings::Coord Console::_osGetCursorPosition(void) const
{
    Lock lock(mLock);

    NEMulticastRouterSettings::Coord pos{ -1, -1 };
    if (mContext != 0)
    {
        ASSERT(mIsReady);
        int posX{ 0 }, posY{ 0 };
        getyx(reinterpret_cast<WINDOW*>(mContext), posY, posX);
        pos.posX = static_cast<int16_t>(posX);
        pos.posY = static_cast<int16_t>(posY);
    }

    return pos;
}

void Console::_osSetCursorCurPosition(NEMulticastRouterSettings::Coord pos)
{
    Lock lock(mLock);

    if (mContext != 0)
    {
        wmove(reinterpret_cast<WINDOW*>(mContext), pos.posY, pos.posX);
    }
}

void Console::_osWaitInput(void)
{
    if (mContext != 0)
    {
        char buffer[32]{ 0 };
        wscanw(reinterpret_cast<WINDOW*>(mContext), "%32s", buffer);
        mUsrInput = buffer;
    }
}

void Console::_osRefreshScreen(void) const
{
    if (mContext != 0)
    {
        wrefresh(reinterpret_cast<WINDOW*>(mContext));
    }
}

#endif  // POSIX
