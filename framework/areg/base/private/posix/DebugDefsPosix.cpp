/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/DebugWinPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX implementation of Debugging utilities
 *
 ************************************************************************/

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/DebugDefs.hpp"
#include "areg/base/StringDefs.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/Containers.hpp"

#ifdef  _DEBUG

void AREG_API_IMPL areg::output_message_os( const char * msg )
{
    if (areg::is_empty<char>( msg ) == false )
    {
        ::fprintf( stdout, "%s", msg );
        ::fflush( stdout );
    }
}

#else   // _DEBUG

void AREG_API_IMPL areg::output_message_os( const char * /*msg*/ )
{
}

#endif  // _DEBUG

#endif  // defined(_POSIX) || defined(POSIX)
