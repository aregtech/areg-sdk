/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/NEDebugWinPosix.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX implementation of Debugging utilities
 *
 ************************************************************************/
#include "areg/base/private/NEDebug.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/NEString.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/Containers.hpp"

#ifdef  _DEBUG

void AREG_API_IMPL NEDebug::outputMessageOS( const char * msg )
{
    if ( NEString::isEmpty<char>( msg ) == false )
    {
        ::fprintf( stdout, "%s", msg );
        ::fflush( stdout );
    }
}

#else   // _DEBUG

void AREG_API_IMPL NEDebug::outputMessageOS( const char * /*msg*/ )
{
}

#endif  // _DEBUG

#endif  // defined(_POSIX) || defined(POSIX)
