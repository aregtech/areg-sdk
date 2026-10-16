/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/FatalErrorTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the fail-fast path: the fatal error handler, and the
 *              report of a Lock or ScopedLock that could not take its object.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/FatalError.hpp"
#include "areg/base/SyncPrimitives.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>

namespace
{
    constexpr const char * LOCK_FATAL_VARIABLE{ "AREG_LOCK_FAILURE_FATAL" };

    //!< Sets or clears an environment variable.
    void set_variable( const char * name, const char * value )
    {
#if defined(_WIN32)
        static_cast<void>(::_putenv_s( name, value != nullptr ? value : "" ));
#else   // defined(_WIN32)
        if ( value != nullptr )
            static_cast<void>(::setenv( name, value, 1 ));
        else
            static_cast<void>(::unsetenv( name ));
#endif  // defined(_WIN32)
    }

    //!< Clears AREG_LOCK_FAILURE_FATAL while it exists and restores it after, as CI sets it.
    class NotFatal
    {
    public:
        NotFatal( )
            : mSaved( )
            , mWasSet( std::getenv( LOCK_FATAL_VARIABLE ) != nullptr )
        {
            if ( mWasSet )
                mSaved = std::getenv( LOCK_FATAL_VARIABLE );
            set_variable( LOCK_FATAL_VARIABLE, nullptr );
        }

        ~NotFatal( )
        {
            set_variable( LOCK_FATAL_VARIABLE, mWasSet ? mSaved.c_str( ) : nullptr );
        }

    private:
        std::string mSaved;
        const bool  mWasSet;
    };

    //!< A lock whose lock() always fails.
    class FailingLock final : public areg::Lockable
    {
    public:
        FailingLock( )
            : areg::Lockable( areg::SyncObject::SyncKind::SoSpinlock )
        {
        }

        bool lock( uint32_t /*timeout*/ ) final
        {
            return false;
        }

        bool unlock( ) final
        {
            return true;
        }

        bool try_lock( ) final
        {
            return false;
        }
    };

    //!< A handler that writes a marker and returns, as a faulty application handler could.
    void returning_handler( const char * reason ) noexcept
    {
        std::fprintf( stderr, "returning handler: %s\n", reason );
        std::fflush( stderr );
    }
}

/**
 * \brief   set_fatal_handler() returns the handler installed before; nullptr restores the default.
 **/
TEST( FatalErrorTest, SetReturnsThePreviousHandler )
{
    const areg::FatalHandler original{ areg::set_fatal_handler( &returning_handler ) };
    EXPECT_NE( original, nullptr );
    EXPECT_EQ( areg::set_fatal_handler( nullptr ), &returning_handler );
    EXPECT_EQ( areg::set_fatal_handler( nullptr ), original );
}

/**
 * \brief   Without AREG_LOCK_FAILURE_FATAL a failed lock is reported and the code continues.
 **/
TEST( FatalErrorTest, FailedLockIsReported )
{
    NotFatal notFatal;
    FailingLock failing;
    ::testing::internal::CaptureStderr( );
    {
        areg::Lock lock( failing );
    }
    {
        areg::ScopedLock lock( failing );
    }
    const std::string output{ ::testing::internal::GetCapturedStderr( ) };
    EXPECT_NE( output.find( "areg lock failure: Lock could not take" ), std::string::npos ) << output;
    EXPECT_NE( output.find( "areg lock failure: ScopedLock could not take" ), std::string::npos ) << output;
}

#if GTEST_HAS_DEATH_TEST

/**
 * \brief   The default handler writes the reason and aborts.
 **/
TEST( FatalErrorTest, DefaultHandlerWritesTheReasonAndAborts )
{
    ::testing::GTEST_FLAG( death_test_style ) = "threadsafe";
    EXPECT_DEATH( areg::fatal_error( "unit test reason" ), "areg fatal error: unit test reason" );
}

/**
 * \brief   An installed handler runs, and the process still ends when the handler returns.
 **/
TEST( FatalErrorTest, ProcessEndsWhenTheHandlerReturns )
{
    ::testing::GTEST_FLAG( death_test_style ) = "threadsafe";
    EXPECT_DEATH( { static_cast<void>(areg::set_fatal_handler( &returning_handler )); areg::fatal_error( "custom" ); }
                , "returning handler: custom" );
}

/**
 * \brief   With AREG_LOCK_FAILURE_FATAL=1 a failed lock ends the process, as in CI.
 **/
TEST( FatalErrorTest, FailedLockIsFatalWhenRequested )
{
    ::testing::GTEST_FLAG( death_test_style ) = "threadsafe";
    FailingLock failing;
    EXPECT_DEATH( { set_variable( LOCK_FATAL_VARIABLE, "1" ); areg::Lock lock( failing ); }
                , "areg fatal error: Lock could not take its synchronization object" );
    EXPECT_DEATH( { set_variable( LOCK_FATAL_VARIABLE, "1" ); areg::ScopedLock lock( failing ); }
                , "areg fatal error: ScopedLock could not take its synchronization object" );
}

#endif  // GTEST_HAS_DEATH_TEST
