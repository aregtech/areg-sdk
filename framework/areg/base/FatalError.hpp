#ifndef AREG_BASE_FATALERROR_HPP
#define AREG_BASE_FATALERROR_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/FatalError.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the fail-fast path for errors areg cannot continue after,
 *              such as a lock that could not be created or taken.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

namespace areg {

/**
 * \brief   The function areg calls when it cannot continue safely, for example when a lock
 *          could not be created or taken: continuing would run code that relies on the lock
 *          without it. The function must not return; when it does, areg aborts the process.
 *          An embedded application installs one that brings the device to a safe state.
 *
 * \param   reason  A constant text naming what failed.
 **/
using FatalHandler = void (*)( const char * reason ) noexcept;

/**
 * \brief   Installs the fatal error handler. Thread safe.
 *
 * \param   handler     The handler to call, or nullptr to restore the default one, which writes
 *                      the reason to the standard error output and aborts the process.
 * \return  The handler installed before.
 **/
AREG_API FatalHandler set_fatal_handler( FatalHandler handler ) noexcept;

/**
 * \brief   Calls the installed fatal error handler with the reason and aborts the process when
 *          the handler returns. Never returns.
 *
 * \param   reason  A constant text naming what failed.
 **/
[[noreturn]] AREG_API void fatal_error( const char * reason ) noexcept;

/**
 * \brief   Called when areg::Lock or areg::ScopedLock could not take its synchronization object,
 *          so the code after the lock would run without it. Writes the reason to the standard
 *          error output and returns. When the environment variable AREG_LOCK_FAILURE_FATAL is
 *          set to 1, calls fatal_error() instead: CI sets it, so that a failed lock fails the build.
 *
 * \param   reason  A constant text naming what failed.
 **/
AREG_API void report_lock_failure( const char * reason ) noexcept;

} // namespace areg

#endif  // AREG_BASE_FATALERROR_HPP
