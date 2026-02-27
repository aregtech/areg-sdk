/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/console/private/ncurses/ConsoleNcurses.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, OS specific console implementation
 *              based on ncurses API. Works with POSIX supported OS.
 * 
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "aregextend/console/Console.hpp"

#if defined(POSIX) && (AREG_EXTENDED)

#include <ncurses.h>

namespace
{
    aregext::Console::Coord  _cursorPos{ -1, -1 };
    bool            _isSaved{ false };
}

//////////////////////////////////////////////////////////////////////////
// Console POSIX specific implementation
//////////////////////////////////////////////////////////////////////////

bool aregext::Console::_osSetup()
{
    if (mIsReady == false)
    {
        mContext = reinterpret_cast<ptr_type>(initscr());
        if (mContext != 0)
        {
            clear();
            cbreak();
            mIsReady = true;
        }
    }

    return mIsReady;
}

void aregext::Console::_osRelease()
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

void aregext::Console::_osOutputText(aregext::Console::Coord pos, const areg::String& text) const
{
    areg::Lock lock(mLock);

    if (mContext != 0)
    {
        ASSERT(mIsReady);
        mvwaddstr(reinterpret_cast<WINDOW *>(mContext), pos.posY, pos.posX, text.getString());
        wclrtoeol(reinterpret_cast<WINDOW *>(mContext));
    }
}

void aregext::Console::_osOutputText(aregext::Console::Coord pos, const std::string_view& text) const
{
    areg::Lock lock(mLock);

    if (mContext != 0)
    {
        ASSERT(mIsReady);
        mvwaddstr(reinterpret_cast<WINDOW*>(mContext), pos.posY, pos.posX, text.data());
        wclrtoeol(reinterpret_cast<WINDOW *>(mContext));
    }
}

void aregext::Console::_osOutputText(const areg::String& text) const
{
    areg::Lock lock(mLock);

    if (mContext != 0)
    {
        ASSERT(mIsReady);
        waddstr(reinterpret_cast<WINDOW*>(mContext), text.getString());
    }
}

void aregext::Console::_osOutputText(const std::string_view& text) const
{
    areg::Lock lock(mLock);

    if (mContext != 0)
    {
        ASSERT(mIsReady);
        waddstr(reinterpret_cast<WINDOW*>(mContext), text.data());
    }
}

aregext::Console::Coord aregext::Console::_osGetCursorPosition() const
{
    areg::Lock lock(mLock);

    aregext::Console::Coord pos{ -1, -1 };
    if (mContext != 0)
    {
        ASSERT(mIsReady);
        int32_t posX{ 0 }, posY{ 0 };
        getyx(reinterpret_cast<WINDOW*>(mContext), posY, posX);
        pos.posX = static_cast<int16_t>(posX);
        pos.posY = static_cast<int16_t>(posY);
    }

    return pos;
}

void aregext::Console::_osSetCursorCurPosition(aregext::Console::Coord pos) const
{
    areg::Lock lock(mLock);

    if (mContext != 0)
    {
        wmove(reinterpret_cast<WINDOW*>(mContext), pos.posY, pos.posX);
        wrefresh(reinterpret_cast<WINDOW*>(mContext));
    }
}

bool aregext::Console::_osWaitInputString(char* buffer, uint32_t size)
{
    ASSERT(buffer != nullptr);
    // Use getnstr which operates on stdscr (the standard screen).
    // This is a static method, so we cannot use mContext (non-static member).
    // ncurses provides stdscr as a global after initscr() is called.
    if ((stdscr == nullptr) || (getnstr(buffer, static_cast<int32_t>(size)) != OK))
        return false;

    areg::trimAll<char>(buffer);
    return (areg::isEmpty<char>(buffer) == false);
}

void aregext::Console::_osRefreshScreen() const
{
    if (mContext != 0)
    {
        wrefresh(reinterpret_cast<WINDOW*>(mContext));
    }
}

void aregext::Console::_osClearLine() const
{
    areg::Lock lock(mLock);

    if (mContext != 0)
    {
        wclrtoeol(reinterpret_cast<WINDOW *>(mContext));
    }
}

void aregext::Console::_osClearScreen() const
{
    areg::Lock lock(mLock);

    if (mContext != 0)
    {
        clear();
    }
}

bool aregext::Console::_osReadInputList(const char* format, va_list varList) const
{
    return (mContext != 0 ? vw_scanw(reinterpret_cast<WINDOW *>(mContext), format, varList) >= 0 : false);
}

void aregext::Console::_osSaveCursorPosition() const
{
    areg::Lock lock(mLock);
    _cursorPos = _osGetCursorPosition();
    _isSaved = true;
}

void aregext::Console::_osRestoreCursorPosition() const
{
    areg::Lock lock(mLock);

    if (_isSaved)
    {
        if (mContext != 0)
        {
            wmove(reinterpret_cast<WINDOW*>(mContext), _cursorPos.posY, _cursorPos.posX);
            wrefresh(reinterpret_cast<WINDOW*>(mContext));
        }

        _isSaved = false;
    }
}

void aregext::Console::_osMoveCursorOneLineUp() const
{
    areg::Lock lock(mLock);
    aregext::Console::Coord pos = _osGetCursorPosition();
    mvcur(pos.posY, pos.posX, pos.posY - 1, 1);
}

void aregext::Console::_osMoveCursorOneLineDown() const
{
    areg::Lock lock(mLock);
    aregext::Console::Coord pos = _osGetCursorPosition();
    mvcur(pos.posY, pos.posX, pos.posY + 1, 1);
}

#endif  // defined(POSIX) && (AREG_EXTENDED)
