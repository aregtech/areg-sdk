/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logger/private/app/LogObserverPosix.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Logger Observer, POSIX specific implementation
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "observer/app/LogObserver.hpp"

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
