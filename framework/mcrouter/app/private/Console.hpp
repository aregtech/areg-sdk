#ifndef AREG_MCROUTER_APP_CONSOLE_HPP
#define AREG_MCROUTER_APP_CONSOLE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/private/Console.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing, OS specific console.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "mcrouter/app/NEMulticastRouterSettings.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SynchObjects.hpp"

#include <functional>

class Console
{
private:
    //!< Clear the screen.
    static constexpr std::string_view   CMD_CLEAR_SCREEN    { "\x1B[2J" };
    //!< Scroll cursor back.
    static constexpr std::string_view   CMD_SCROLL_BACK     { "\x1B[3J" };
    //!< Clear line.
    static constexpr std::string_view   CMD_CLEAR_LINE      { "\33[2K" };

public:
    using CallBack = std::function<bool(const String &)>;

private:
    Console(void);
    ~Console(void);

public:

    static Console& getInstance(void);

    bool initialize(void);

    void uninitialize(void);

    bool waitForInput(void);

    void setCallback(CallBack callback);

    bool isReady( void ) const;

    bool enableConsoleInput(bool enable);

    void outputText(NEMulticastRouterSettings::Coord pos, const String& text) const;

    void outputText(NEMulticastRouterSettings::Coord pos, const std::string_view& text) const;

    void outputText(NEMulticastRouterSettings::Coord pos, const char* format, ...) const;

    NEMulticastRouterSettings::Coord getCursorCurPosition(void) const;

    void setCursorCurPosition(NEMulticastRouterSettings::Coord pos);

    const String& getUserInput(void) const;

    void refreshScreen(void) const;

private:
    bool _osInitialize(void);

    void _osUninitialize(void);

    void _osOutputText(NEMulticastRouterSettings::Coord pos, const String& text) const;

    void _osOutputText(NEMulticastRouterSettings::Coord pos, const std::string_view& text) const;

    NEMulticastRouterSettings::Coord _osGetCursorPosition(void) const;

    void _osSetCursorCurPosition(NEMulticastRouterSettings::Coord pos);

    void _osWaitInput(void);

    void _osRefreshScreen(void) const;

private:
    bool            mIsReady;
    String          mInput;
    ptr_type        mContext;
    String          mUsrInput;
    CallBack        mCallback;
    SynchEvent      mEnable;
    mutable ResourceLock    mLock;
};

#endif  // AREG_MCROUTER_APP_CONSOLE_HPP

