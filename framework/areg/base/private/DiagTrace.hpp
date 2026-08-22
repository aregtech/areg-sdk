#ifndef AREG_BASE_PRIVATE_DIAGTRACE_HPP
#define AREG_BASE_PRIVATE_DIAGTRACE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/DiagTrace.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, diagnostic trace of the shutdown path.
 *
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/base/Process.hpp"

#include <cstdarg>
#include <cstdio>

namespace areg::diag {

/**
 * \brief   Returns true when the environment variable AREG_QUIT_TRACE is set.
 *          The value is read once, on the first call.
 **/
inline bool is_trace_enabled() noexcept
{
    static const bool enabled{ areg::Process::instance().safe_env_variable("AREG_QUIT_TRACE").is_empty() == false };
    return enabled;
}

/**
 * \brief   Writes one diagnostic line to the standard error stream when the trace is
 *          enabled, and does nothing otherwise. The line carries a fixed marker, so that
 *          the example test driver can lift it out of the captured stream.
 *
 * \param   format  The printf style format of the message.
 **/
inline void trace(const char * format, ...) noexcept
{
    if (!is_trace_enabled())
        return;

    char message[512]{};
    va_list args;
    va_start(args, format);
    std::vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    std::fprintf(stderr, "\r\n[areg-quit-trace] %s\r\n", message);
    std::fflush(stderr);
}

} // namespace areg::diag

#endif  // AREG_BASE_PRIVATE_DIAGTRACE_HPP
