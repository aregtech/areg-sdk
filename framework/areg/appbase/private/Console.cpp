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
#include "areg/appbase/Console.hpp"

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
    , mContext  ( 0 )
    , mEnable   (true, false)
    , mLock     (false)
{
    _osSetup();
}

Console::~Console(void)
{
    _osRelease();
}

String Console::waitForInput(Console::CallBack callback) const
{
    String result;

    mEnable.lock(NECommon::WAIT_INFINITE);

    if (mIsReady)
    {
        do
        {
            result.clear();
            char buffer[256]{ 0 };
            _osWaitInput(buffer, 256);
            result = buffer;
            if ((static_cast<bool>(callback) == false) || callback(result))
            {
                break;
            }
        } while (true);
    }

    return result;
}

bool Console::readInputs(const char* format, ...) const
{
    va_list argptr;
    va_start(argptr, format);
    bool result = readInputList(format, argptr);
    va_end(argptr);

    return result;
}

bool Console::readInputList(const char* format, va_list varList) const
{
    mEnable.lock(NECommon::WAIT_INFINITE);
    return _osReadInputList(format, varList);
}

String Console::readString(void) const
{
    char buffer[513] { 0 };
    return String(readInputs("%513s", buffer) ? buffer : String::EmptyString);
}

void Console::outputMessage(Console::Coord pos, const char* format, ...) const
{
    va_list argptr;
    va_start(argptr, format);

    String text;
    text.formatList(format, argptr);
    va_end(argptr);

    outputString(pos, text);
}
