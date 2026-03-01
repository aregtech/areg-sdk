/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/DebugDefs.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Debugging utilities
 *
 ************************************************************************/
#include "areg/base/private/DebugDefs.hpp"

#include "areg/base/Containers.hpp"

#include <stdio.h>
#include <stdarg.h>

//////////////////////////////////////////////////////////////////////////
// Debug functions implementation
//////////////////////////////////////////////////////////////////////////

#ifdef  _DEBUG

void AREG_API_IMPL areg::output_console( areg::DebugPriority priority, const char * msg, ... )
{
    va_list args;
    if (msg != nullptr)
    {
        va_start(args, msg);
        char buffer[areg::MAX_DEBUG_BUFFER_SIZE];
        int32_t lenPref = String::format_string(buffer, static_cast<int32_t>(areg::MAX_DEBUG_BUFFER_SIZE) - 2, "%s", areg::prio_prefix(priority));
        int32_t lenMsg  = String::format_string_list(buffer + lenPref, static_cast<int32_t>(areg::MAX_DEBUG_BUFFER_SIZE) - 2 - lenPref, msg, args);
        char last   = buffer[lenPref + lenMsg - 1];
        if ( last != areg::EndOfLine )
        {
            buffer[lenPref + lenMsg + 0] = areg::EndOfLine;
            buffer[lenPref + lenMsg + 1] = areg::EndOfString;
        }

        areg::output_message_os(buffer);
        va_end(args);
    }
}

#else   // !_DEBUG

void AREG_API_IMPL areg::output_console( areg::DebugPriority /*priority*/, const char * /*msg*/, ... )
{
}

#endif  // _DEBUG

#ifdef  _DEBUG

void AREG_API_IMPL areg::output_console(const char * msg, ...)
{
    va_list args;
    if (msg != nullptr)
    {
        va_start(args, msg);

        char buffer[areg::MAX_DEBUG_BUFFER_SIZE];
        String::format_string_list( buffer, areg::MAX_DEBUG_BUFFER_SIZE, msg, args );
        areg::output_message_os( buffer );

        va_end(args);
    }
}

#else   // !_DEBUG

void AREG_API_IMPL areg::output_console(const char * /*msg*/, ...)
{
}

#endif  // _DEBUG
