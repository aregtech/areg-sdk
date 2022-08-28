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
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "mcrouter/app/private/Console.hpp"

//////////////////////////////////////////////////////////////////////////
// Console class implementations.
//////////////////////////////////////////////////////////////////////////

Console& Console::getInstance(void)
{
    static Console _instance;   // singleton instance.
    return _instance;
}

Console::Console(void)
    : mIsReady  ( false )
    , mUsrInput ( )
    , mContext  ( 0 )
    , mCallback ( )
    , mEnable   (true, false)
    , mLock     (false)
{
    _osInitialize();
}

Console::~Console(void)
{
    _osUninitialize();
}

bool Console::waitForInput(void)
{
    bool result = false;
    mEnable.lock(NECommon::WAIT_INFINITE);
    if (mIsReady)
    {
        do
        {
            mUsrInput.clear();
            _osWaitInput();
            if ((static_cast<bool>(mCallback) == false) || mCallback(mUsrInput))
            {
                result = true;
                break;
            }
        } while (true);
    }

    return result;
}

void Console::outputText(NEMulticastRouterSettings::Coord pos, const char* format, ...) const
{
    va_list argptr;
    va_start(argptr, format);

    String text;
    text.formatList(format, argptr);
    va_end(argptr);

    outputText(pos, text);
}
