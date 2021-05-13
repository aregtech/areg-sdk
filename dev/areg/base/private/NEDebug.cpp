/************************************************************************
 * \file        areg/base/private/NEDebug.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Debugging utilities
 *
 ************************************************************************/
#include "areg/base/private/NEDebug.hpp"

#include "areg/base/GEGlobal.h"
#include "areg/base/EContainers.hpp"

#include <stdio.h>
#include <stdarg.h>

//////////////////////////////////////////////////////////////////////////
// NEDebug namespace functions implementation
//////////////////////////////////////////////////////////////////////////

void AREG_API NEDebug::outputConsole( NEDebug::eDegubPrio priority, const char* msg, ... )
{
    va_list args;
    if (msg != NULL)
    {
#ifdef  _DEBUG
        va_start(args, msg);
        char buffer[NEDebug::MAX_DEBUG_BUFFER_SIZE];
        int lenPref = String::formatString(buffer, NEDebug::MAX_DEBUG_BUFFER_SIZE - 2, "%s", NEDebug::getPrioPrefix(priority));
        int lenMsg  = String::formatStringList(buffer + lenPref, NEDebug::MAX_DEBUG_BUFFER_SIZE - 2 - lenPref, msg, args);
        char last   = buffer[lenPref + lenMsg - 1];
        if ( last != '\n' )
        {
            buffer[lenPref + lenMsg + 0] = '\n';
            buffer[lenPref + lenMsg + 1] = '\0';
        }

        NEDebug::outputMessageOS(buffer);
        va_end(args);
#endif  // _DEBUG
    }
}

void AREG_API NEDebug::outputConsole(const char* msg, ...)
{
    va_list args;
    if (msg != NULL)
    {
#ifdef  _DEBUG
        va_start(args, msg);
        if ( args == NULL )
        {
            NEDebug::outputMessageOS( msg );
        }
        else
        {
            char buffer[NEDebug::MAX_DEBUG_BUFFER_SIZE];
            String::formatStringList(buffer, NEDebug::MAX_DEBUG_BUFFER_SIZE, msg, args);
            NEDebug::outputMessageOS( buffer );
        }

        va_end(args);
#endif  // _DEBUG
    }
}
