/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/appbase/private/Console.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Basic OS specific console implementation.
 *              POSIX specific implementation.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/appbase/Console.hpp"

#ifdef _POSIX

#include <ncurses.h>

//////////////////////////////////////////////////////////////////////////
// Console POSIX specific implementation
//////////////////////////////////////////////////////////////////////////

bool Console::_osSetup(void)
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

void Console::_osRelease(void)
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

void Console::_osOutputText(Console::Coord pos, const String& text) const
{
    Lock lock(mLock);

    if (mContext != 0)
    {
        ASSERT(mIsReady);
        mvwaddstr(reinterpret_cast<WINDOW *>(mContext), pos.posY, pos.posX, text.getString());
        wclrtoeol(reinterpret_cast<WINDOW *>(mContext));
    }
}

void Console::_osOutputText(Console::Coord pos, const std::string_view& text) const
{
    Lock lock(mLock);

    if (mContext != 0)
    {
        ASSERT(mIsReady);
        mvwaddstr(reinterpret_cast<WINDOW*>(mContext), pos.posY, pos.posX, text.data());
        wclrtoeol(reinterpret_cast<WINDOW *>(mContext));
    }
}

void Console::_osOutputText(const String& text) const
{
    Lock lock(mLock);

    if (mContext != 0)
    {
        ASSERT(mIsReady);
        waddstr(reinterpret_cast<WINDOW*>(mContext), text.getString());
    }
}

Console::Coord Console::_osGetCursorPosition(void) const
{
    Lock lock(mLock);

    Console::Coord pos{ -1, -1 };
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

void Console::_osSetCursorCurPosition(Console::Coord pos) const
{
    Lock lock(mLock);

    if (mContext != 0)
    {
        wmove(reinterpret_cast<WINDOW*>(mContext), pos.posY, pos.posX);
    }
}

void Console::_osWaitInput(char* buffer, uint32_t size) const
{
    if (mContext != 0)
    {
        String fmt;
        fmt.format("%%%us", size);
        wscanw(reinterpret_cast<WINDOW*>(mContext), fmt.getString(), buffer);
    }
}

void Console::_osRefreshScreen(void) const
{
    if (mContext != 0)
    {
        wrefresh(reinterpret_cast<WINDOW*>(mContext));
    }
}

void Console::_osClearLine( void ) const
{
    Lock lock(mLock);

    if (mContext != 0)
    {
        wclrtoeol(reinterpret_cast<WINDOW *>(mContext));
    }
}

bool Console::_osReadInputList(const char* format, va_list varList) const
{
    return (mContext != 0 ? vw_scanw(reinterpret_cast<WINDOW *>(mContext), format, varList) == OK : false);
}

#endif  // POSIX
