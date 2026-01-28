/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        logcollector/app/private/LogObserverWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       LogObserver, Win32 specific implementation
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "logobserver/app/LogObserver.hpp"

#ifdef _WIN32

    #ifdef _MSC_VER
        #pragma comment(lib, "areg")
        #pragma comment(lib, "aregextend")
        #pragma comment(lib, "areglogger")
    #endif // _MSC_VER

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>

//////////////////////////////////////////////////////////////////////////
// Global functions, Begin
//////////////////////////////////////////////////////////////////////////

namespace
{

    inline char ** _convertArguments( TCHAR ** argv, int argc )
    {
        char ** argvTemp = argc != 0 ? DEBUG_NEW char * [static_cast<uint32_t>(argc)] : nullptr;
        if ( argvTemp != nullptr )
        {
            for ( uint32_t i = 0; i < static_cast<uint32_t>(argc); ++i )
            {
                TCHAR * entry = argv[i];
                uint32_t length = static_cast<uint32_t>(NEString::getStringLength<TCHAR>( entry ));
                uint32_t size = length + 1u;
                char * arg = DEBUG_NEW char[size];
                NEString::copyString<char, TCHAR>( arg, static_cast<NEString::CharCount>(size), entry );
                argvTemp[i] = arg;
            }
        }
        return argvTemp;
    }

    inline void _deleteArguments( char ** argv, int argc )
    {
        if ( argv != nullptr )
        {
            for ( int i = 0; i < static_cast<int>(argc); ++i )
            {
                delete[] argv[i];
            }

            delete[] argv;
        }
    }

} // namespace
    
#ifndef _MINGW
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    static_cast<void>(envp);
    char** argvTemp = _convertArguments(argv, static_cast<int>(argc));
    LogObserver::getInstance().logMain(static_cast<int>(argc), argvTemp);
    _deleteArguments(argvTemp, static_cast<int>(argc));

    return 0;
}
#else
int main(int argc, char* argv[], char* envp[])
{
    LogObserver::getInstance().logMain(static_cast<int>(argc), argv);
    return 0;
}
#endif

//////////////////////////////////////////////////////////////////////////
// Global functions, End
//////////////////////////////////////////////////////////////////////////

bool LogObserver::_osWaitUserInput(char* buffer, unsigned int bufSize)
{
#if !defined(__STDC_WANT_LIB_EXT1__) || !(__STDC_WANT_LIB_EXT1__)
    #if defined(_WIN32) && !defined(_MINGW)
        return (::gets_s(buffer, bufSize) != nullptr);
    #else   // defined(_WIN32)
        return (::fgets(buffer, bufSize, stdin) != nullptr);
    #endif  // defined(_WIN32)
#else
    return (::gets_s(buffer, bufSize) != nullptr);
#endif // _WIN32
}

#endif // _WIN32
