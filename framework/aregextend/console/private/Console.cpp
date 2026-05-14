/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/console/private/Console.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Console extension.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "aregextend/console/Console.hpp"

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// Console class implementations.
//////////////////////////////////////////////////////////////////////////

Console& Console::instance()
{
    static Console _instance;   // singleton instance.
    return _instance;
}

Console::Console() noexcept
    : mIsReady  ( false )
    , mContext  ( 0 )
    , mSavedPos { 0, 0 }
    , mEnable   (true, false)
    , mLock     (false)
{
    _os_setup();
}

Console::~Console() noexcept
{
    _os_release();
}

String Console::wait_for_input(Console::CallBack callback) const
{
    String result;

    mEnable.lock(areg::WAIT_INFINITE);

    if (mIsReady)
    {
        do
        {
            result.clear();
            _os_restore_cursor_position();
            char buffer[512]{ 0 };
            if (Console::_os_wait_input_string(buffer, 512))
            {
                result = buffer;
                if ((static_cast<bool>(callback) == false) || callback(result))
                {
                    break;
                }
            }
        } while (true);
    }

    return result;
}

bool Console::read_inputs(const char* format, ...) const
{
    va_list argptr;
    va_start(argptr, format);
    bool result = read_input_list(format, argptr);
    va_end(argptr);

    return result;
}

bool Console::read_input_list(const char* format, va_list varList) const
{
    mEnable.lock(areg::WAIT_INFINITE);
    return _os_read_input_list(format, varList);
}

String Console::read_string() const
{
    char buffer[512] { 0 };
    return String(read_inputs("%510s", buffer) ? buffer : String::empty_string());
}

void Console::output_msg(Console::Coord pos, const char* format, ...) const
{
    va_list argptr;
    va_start(argptr, format);

    String text;
    text.format_list(format, argptr);
    va_end(argptr);

    output_str(pos, text);
}

void Console::print_msg(const char* format, ...) const
{
    va_list argptr;
    va_start(argptr, format);

    String text;
    text.format_list(format, argptr);
    va_end(argptr);

    _os_output_text(text);
}

bool Console::read_console_data(char* buffer, uint32_t bufSize)
{
    return Console::_os_wait_input_string(buffer, bufSize);
}

} // namespace areg::ext
