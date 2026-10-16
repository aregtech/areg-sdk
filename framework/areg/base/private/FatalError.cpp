/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/FatalError.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the fail-fast path for errors areg cannot continue after.
 ************************************************************************/
#include "areg/base/FatalError.hpp"

#include <atomic>
#include <cstdio>
#include <cstdlib>

namespace
{
    //!< Writes the reason to the standard error output and aborts.
    void _default_fatal_handler( const char * reason ) noexcept
    {
        std::fputs( "areg fatal error: ", stderr );
        std::fputs( reason != nullptr ? reason : "unknown", stderr );
        std::fputc( '\n', stderr );
        std::fflush( stderr );
        std::abort( );
    }

    //!< The installed handler.
    std::atomic<areg::FatalHandler> _fatalHandler{ &_default_fatal_handler };
}

AREG_API_IMPL areg::FatalHandler areg::set_fatal_handler( areg::FatalHandler handler ) noexcept
{
    return _fatalHandler.exchange( handler != nullptr ? handler : &_default_fatal_handler, std::memory_order_acq_rel );
}

AREG_API_IMPL void areg::fatal_error( const char * reason ) noexcept
{
    areg::FatalHandler handler{ _fatalHandler.load( std::memory_order_acquire ) };
    handler( reason );
    std::abort( );
}

AREG_API_IMPL void areg::report_lock_failure( const char * reason ) noexcept
{
    const char * fatal{ std::getenv( "AREG_LOCK_FAILURE_FATAL" ) };
    if ( ( fatal != nullptr ) && ( fatal[0] == '1' ) )
    {
        areg::fatal_error( reason );
    }

    std::fputs( "areg lock failure: ", stderr );
    std::fputs( reason != nullptr ? reason : "unknown", stderr );
    std::fputc( '\n', stderr );
    std::fflush( stderr );
}
