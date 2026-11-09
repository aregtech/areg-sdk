/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/WatchdogTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the watchdog lifetime at process exit.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "units/ChildProcess.hpp"
#include "areg/base/String.hpp"
#include "areg/component/private/Watchdog.hpp"

#include <cstdlib>
#include <new>

namespace
{
    //!< The watchdog created after the exit handler that deletes it was registered.
    areg::Watchdog * gLateWatchdog{ nullptr };

    //!< Deletes the late watchdog at process exit.
    void delete_late_watchdog()
    {
        delete gLateWatchdog;
        gLateWatchdog = nullptr;
    }

    //!< Makes a failed lock end the process.
    void set_lock_failure_fatal()
    {
#if defined(_WIN32)
        static_cast<void>(::_putenv_s( "AREG_LOCK_FAILURE_FATAL", "1" ));
#else   // defined(_WIN32)
        static_cast<void>(::setenv( "AREG_LOCK_FAILURE_FATAL", "1", 1 ));
#endif  // defined(_WIN32)
    }
}

/**
 * \brief   Reproduces the defect: a watchdog deleted at process exit after the watchdog manager
 *          was destroyed took the lock of the destroyed manager, and the failed lock ended the process.
 **/
TEST( WatchdogTest, watchdog_deleted_at_exit_after_the_manager )
{
    AREG_EXPECT_IN_CHILD(
        set_lock_failure_fatal( );
        AREG_CHILD_CHECK( std::atexit( &delete_late_watchdog ) == 0 );
        gLateWatchdog = new (std::nothrow) areg::Watchdog( areg::String( "LateWatchdog" ) );
        AREG_CHILD_CHECK( gLateWatchdog != nullptr );
    );
}
