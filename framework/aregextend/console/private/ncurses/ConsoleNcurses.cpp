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

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// Console POSIX specific implementation
//////////////////////////////////////////////////////////////////////////

bool Console::_os_setup()
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

void Console::_os_release()
{
    if (mIsReady)
    {
        // Ensure cursor is visible before tearing down ncurses.
        curs_set(1);

    	if (mContext != 0)
    	{
            delwin(reinterpret_cast<WINDOW *>(mContext));
    	}

        endwin();
        refresh();
        mContext = 0;
        mIsReady = false;

        // After endwin()/refresh(), ncurses hands the terminal back to the shell
        // in raw mode at whatever row ncurses left the cursor.  Move two rows below
        // the highest row ever written to, so the shell prompt appears there rather
        // than at the very bottom of the terminal window.
        const int finalRow = static_cast<int>(mMaxUsedRow + 2);
        printf("\x1B[%d;1H\n", finalRow);
        ::fflush(stdout);
    }
}

void Console::_os_output_text(Console::Coord pos, const String& text) const
{
    Lock lock(mLock);

    if (mContext != 0)
    {
        ASSERT(mIsReady);
        WINDOW* win = reinterpret_cast<WINDOW*>(mContext);
        mvwaddstr(win, pos.posY, pos.posX, text.as_string());
        wclrtoeol(win);
        // Restore cursor to the input-prompt position so that the terminal
        // cursor stays at the user-input line.  Changes become visible at the
        // next wrefresh (called by refresh_screen or _os_set_cursor_cur_position).
        wmove(win, mSavedPos.posY, mSavedPos.posX);
        if (static_cast<int32_t>(pos.posY) > mMaxUsedRow)
        {
            mMaxUsedRow = static_cast<int32_t>(pos.posY);
        }
    }
}

void Console::_os_output_text(Console::Coord pos, std::string_view text) const
{
    Lock lock(mLock);

    if (mContext != 0)
    {
        ASSERT(mIsReady);
        WINDOW* win = reinterpret_cast<WINDOW*>(mContext);
        mvwaddstr(win, pos.posY, pos.posX, text.data());
        wclrtoeol(win);
        wmove(win, mSavedPos.posY, mSavedPos.posX);
        if (static_cast<int32_t>(pos.posY) > mMaxUsedRow)
        {
            mMaxUsedRow = static_cast<int32_t>(pos.posY);
        }
    }
}

void Console::_os_output_text(const String& text) const
{
    Lock lock(mLock);

    if (mContext != 0)
    {
        ASSERT(mIsReady);
        waddstr(reinterpret_cast<WINDOW*>(mContext), text.as_string());
    }
}

void Console::_os_output_text(std::string_view text) const
{
    Lock lock(mLock);

    if (mContext != 0)
    {
        ASSERT(mIsReady);
        waddstr(reinterpret_cast<WINDOW*>(mContext), text.data());
    }
}

Console::Coord Console::_os_get_cursor_position() const
{
    Lock lock(mLock);

    Console::Coord pos{ -1, -1 };
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

void Console::_os_set_cursor_cur_position(Console::Coord pos) const
{
    Lock lock(mLock);

    // Track in software so _os_output_text can restore here without a query.
    mSavedPos = pos;
    if (mContext != 0)
    {
        wmove(reinterpret_cast<WINDOW*>(mContext), pos.posY, pos.posX);
        wrefresh(reinterpret_cast<WINDOW*>(mContext));
    }
}

bool Console::_os_wait_input_string(char* buffer, uint32_t size)
{
    ASSERT(buffer != nullptr);
    // Use getnstr which operates on stdscr (the standard screen).
    // This is a static method, so we cannot use mContext (non-static member).
    // ncurses provides stdscr as a global after initscr() is called.
    if ((stdscr == nullptr) || (getnstr(buffer, static_cast<int32_t>(size)) != OK))
        return false;

    areg::trim_all<char>(buffer);
    return (areg::is_empty<char>(buffer) == false);
}

void Console::_os_refresh_screen() const
{
    if (mContext != 0)
    {
        wrefresh(reinterpret_cast<WINDOW*>(mContext));
    }
}

void Console::_os_clear_line() const
{
    Lock lock(mLock);

    if (mContext != 0)
    {
        wclrtoeol(reinterpret_cast<WINDOW *>(mContext));
    }
}

void Console::_os_clear_line_at_position(Console::Coord pos) const
{
    Lock lock(mLock);

    if (mContext != 0)
    {
        WINDOW* win = reinterpret_cast<WINDOW*>(mContext);
        // Move to the target row, clear to end of line, then return the
        // ncurses cursor to the input-prompt anchor so the user sees the
        // cursor at the right position after each background update.
        wmove(win, static_cast<int>(pos.posY), static_cast<int>(pos.posX));
        wclrtoeol(win);
        wmove(win, static_cast<int>(mSavedPos.posY), static_cast<int>(mSavedPos.posX));
        wrefresh(win);
    }
}

void Console::_os_clear_screen() const
{
    Lock lock(mLock);

    if (mContext != 0)
    {
        clear();
    }
}

bool Console::_os_read_input_list(const char* format, va_list varList) const
{
    return (mContext != 0 ? vw_scanw(reinterpret_cast<WINDOW *>(mContext), format, varList) >= 0 : false);
}

void Console::_os_save_cursor_position() const
{
    Lock lock(mLock);
    if (mContext != 0)
    {
        // Capture the ACTUAL cursor position from ncurses.  While the user types
        // on the input line, the cursor advances past the prompt start stored in
        // mSavedPos.  Reading here ensures restore returns to the typing position.
        WINDOW* win = reinterpret_cast<WINDOW*>(mContext);
        int curY{ 0 }, curX{ 0 };
        getyx(win, curY, curX);
        mSavedPos = { static_cast<int16_t>(curX), static_cast<int16_t>(curY) };

        // Hide the cursor to prevent visible jumping during batch updates.
        curs_set(0);
    }
}

void Console::_os_restore_cursor_position() const
{
    Lock lock(mLock);

    if (mContext != 0)
    {
        // Move to the software-tracked input position and show the cursor.
        wmove(reinterpret_cast<WINDOW*>(mContext), mSavedPos.posY, mSavedPos.posX);
        curs_set(1);
    }
}

void Console::_os_move_cursor_one_line_up() const
{
    Lock lock(mLock);
    Console::Coord pos = _os_get_cursor_position();
    mvcur(pos.posY, pos.posX, pos.posY - 1, 1);
}

void Console::_os_move_cursor_one_line_down() const
{
    Lock lock(mLock);
    Console::Coord pos = _os_get_cursor_position();
    mvcur(pos.posY, pos.posX, pos.posY + 1, 1);
}

} // namespace areg::ext

#endif  // defined(POSIX) && (AREG_EXTENDED)
