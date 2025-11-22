/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/NEDebug.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Debugging utilities
 *
 ************************************************************************/
#include "areg/base/private/NEDebug.hpp"

#include "areg/base/Containers.hpp"

#include <stdio.h>
#include <stdarg.h>

//////////////////////////////////////////////////////////////////////////
// NEDebug namespace functions implementation
//////////////////////////////////////////////////////////////////////////

#ifdef  _DEBUG

void AREG_API_IMPL NEDebug::outputConsole( NEDebug::eDegubPrio priority, const char * msg, ... )
{
    va_list args;
    if (msg != nullptr)
    {
        va_start(args, msg);
        char buffer[NEDebug::MAX_DEBUG_BUFFER_SIZE];
        int lenPref = String::formatString(buffer, static_cast<int>(NEDebug::MAX_DEBUG_BUFFER_SIZE) - 2, "%s", NEDebug::getPrioPrefix(priority));
        int lenMsg  = String::formatStringList(buffer + lenPref, static_cast<int>(NEDebug::MAX_DEBUG_BUFFER_SIZE) - 2 - lenPref, msg, args);
        char last   = buffer[lenPref + lenMsg - 1];
        if ( last != NEString::EndOfLine )
        {
            buffer[lenPref + lenMsg + 0] = NEString::EndOfLine;
            buffer[lenPref + lenMsg + 1] = NEString::EndOfString;
        }

        NEDebug::outputMessageOS(buffer);
        va_end(args);
    }
}

#else   // !_DEBUG

void AREG_API_IMPL NEDebug::outputConsole( NEDebug::eDegubPrio /*priority*/, const char * /*msg*/, ... )
{
}

#endif  // _DEBUG

#ifdef  _DEBUG

void AREG_API_IMPL NEDebug::outputConsole(const char * msg, ...)
{
    va_list args;
    if (msg != nullptr)
    {
        va_start(args, msg);

        char buffer[NEDebug::MAX_DEBUG_BUFFER_SIZE];
        String::formatStringList( buffer, NEDebug::MAX_DEBUG_BUFFER_SIZE, msg, args );
        NEDebug::outputMessageOS( buffer );

        va_end(args);
    }
}

#else   // !_DEBUG

void AREG_API_IMPL NEDebug::outputConsole(const char * /*msg*/, ...)
{
}

#endif  // _DEBUG
