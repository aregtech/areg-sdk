/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/win32/DebugDefsWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Windows OS specific implementation of Debugging utilities
 *
 ************************************************************************/

#ifdef  _WIN32

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/private/DebugDefs.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/Containers.hpp"

#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX
#include <Windows.h>

#ifdef  DEBUG
void AREG_API_IMPL areg::output_message_os( const char * msg )
{
    if (areg::is_empty<char>(msg) == false)
    {
        ::OutputDebugStringA(msg);
    }
}
#else   // DEBUG
void AREG_API_IMPL areg::output_message_os(const char* /*msg*/)
{
}
#endif  // DEBUG

#endif  // _WIN32
