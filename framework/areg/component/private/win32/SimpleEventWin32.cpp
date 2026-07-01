/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/win32/SimpleEventWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, lightweight single-object event - Windows implementation.
 ************************************************************************/
/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/component/private/SimpleEvent.hpp"

#ifdef  _WIN32

#ifndef NOMINMAX
    #define NOMINMAX
#endif  // NOMINMAX
#include <Windows.h>

namespace areg {

//////////////////////////////////////////////////////////////////////////
// SimpleEvent class implementation (Windows)
//////////////////////////////////////////////////////////////////////////

SimpleEvent::SimpleEvent( bool initLock /*= true*/, bool autoReset /*= true*/ ) noexcept
    : mHandle    ( static_cast<void *>(::CreateEvent( nullptr, autoReset ? FALSE : TRUE, initLock ? FALSE : TRUE, nullptr )) )
    , mAutoReset ( autoReset )
{
}

SimpleEvent::SimpleEvent( areg::NullTag ) noexcept
    : mHandle    ( nullptr )
    , mAutoReset ( true )
{
}

SimpleEvent::~SimpleEvent()
{
    if (mHandle != nullptr)
    {
        ::CloseHandle( static_cast<HANDLE>(mHandle) );
        mHandle = nullptr;
    }
}

bool SimpleEvent::lock( uint32_t timeout /*= areg::WAIT_INFINITE*/ ) noexcept
{
    return (mHandle != nullptr)
        && (::WaitForSingleObject( static_cast<HANDLE>(mHandle), timeout ) == WAIT_OBJECT_0);
}

bool SimpleEvent::set_signaled() noexcept
{
    return (mHandle != nullptr) && (::SetEvent( static_cast<HANDLE>(mHandle) ) != FALSE);
}

bool SimpleEvent::reset() noexcept
{
    return (mHandle != nullptr) && (::ResetEvent( static_cast<HANDLE>(mHandle) ) != FALSE);
}

} // namespace areg

#endif  // _WIN32
