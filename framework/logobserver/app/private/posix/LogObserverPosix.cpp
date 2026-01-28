/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        logobserver/private/app/posix/LogObserverPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Log Observer, POSIX specific implementation
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "logobserver/app/LogObserver.hpp"

#ifdef _POSIX

//////////////////////////////////////////////////////////////////////////
// Global functions, Begin
//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[], char* envp[])
{
    LogObserver::getInstance().logMain(static_cast<int>(argc), argv);
    return 0;
}

bool LogObserver::_osWaitUserInput(char* buffer, unsigned int bufSize)
{
#if __STDC_WANT_LIB_EXT1__
    return(gets_s(buffer, bufSize) != nullptr);
#else   // __STDC_WANT_LIB_EXT1__
    return (fgets(buffer, bufSize, stdin) != nullptr);
#endif  // __STDC_WANT_LIB_EXT1__
}

#endif  // _POSIX
