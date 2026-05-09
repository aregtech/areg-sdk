#ifndef AREG_BASE_SYNCTIMER_HPP
#define AREG_BASE_SYNCTIMER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/SyncTimer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Synchronization objects
 *              Declared following synchronization objects:
 *              SyncTimer           - Timer synchronization object.
 *              Wait                - High resolution timer.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SyncObject.hpp"
#include <chrono>

/**
 * \brief   This file contains timer used to synchronize data access
 *          in multi-threading enviroment. All Synchronization objects are instances of 
 *          SyncObject interface. 
 **/

/************************************************************************
 * List of declared classes and hierarchy
 ************************************************************************/
/* class SyncObject;                */
/*     class SyncTimer;             */
/*     class Wait;                  */

namespace areg {
//////////////////////////////////////////////////////////////////////////
// class SyncTimer declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Waitable timer synchronization object. Can be manual-reset or auto-reset, and optionally
 *          periodic. The timer's state is set to signaled when the specified due time expires.
 *          In this file lockable timer and high-resolution timer is provided.
 **/


class AREG_API SyncTimer final  : public SyncObject
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates a waitable timer with the specified timeout, type, and mode.
     *
     * \param   msTimeout           The timer timeout in milliseconds.
     * \param   is_periodic         If true, the timer fires repeatedly at the specified interval;
     *                              otherwise, it fires once.
     * \param   is_auto_reset       If true, the timer is auto-reset (synchronization timer);
     *                              otherwise, it is manual-reset.
     * \param   isSteady            If true, uses a steady high-resolution timer; otherwise, uses a
     *                              system clock.
     **/
    SyncTimer( uint32_t msTimeout, bool is_periodic = false, bool is_auto_reset = true, bool isSteady = true );

    virtual ~SyncTimer();

//////////////////////////////////////////////////////////////////////////
// Override operations, SyncObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Waits for the timer to fire or timeout to expire.
     *
     * \param   timeout     The timeout in milliseconds to wait for the timer to fire. Use
     *                      areg::WAIT_INFINITE to wait indefinitely.
     * \return  Returns true if the timer fired before timeout.
     **/
    inline bool lock( uint32_t timeout = areg::WAIT_INFINITE ) final;

    /**
     * \brief   Activates the timer. The timer will fire at the due time specified in the
     *          constructor.
     *
     * \return  Returns true if the timer was successfully activated.
     **/
    inline bool unlock() final;

//////////////////////////////////////////////////////////////////////////
// Operations / Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Activates the timer. Equivalent to calling unlock().
     *
     * \return  Returns true if the timer was successfully activated.
     **/
    inline bool set_timer();

    /**
     * \brief   Stops the timer without changing its signaled state. Threads waiting on the timer
     *          will remain waiting until timeout or reactivation.
     *
     * \return  Returns true if successfully stopped.
     **/
    inline bool cancel_timer();

    /**
     * \brief   Returns the due time in milliseconds.
     **/
    inline uint32_t due_time() const;

    /**
     * \brief   Returns true if the timer is periodic; false if it fires only once.
     **/
    [[nodiscard]]
    inline bool is_periodic() const noexcept;

    /**
     * \brief   Returns true if the timer is auto-reset; false if manual-reset.
     **/
    [[nodiscard]]
    inline bool is_autoreset() const noexcept;

//////////////////////////////////////////////////////////////////////////
// OS specific hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   OS-specific implementation to create the synchronization timer.
     *
     * \param   isSteady    If true, uses a steady high-resolution timer; otherwise, uses a system
     *                      clock.
     **/
    void _os_create_timer( bool isSteady );

    /**
     * \brief   OS-specific implementation to release the timer resources.
     **/
    void _os_release_time();

    /**
     * \brief   OS-specific implementation to wait for the timer to fire. Returns immediately if the
     *          timer has already fired.
     *
     * \param   timeout     The timeout in milliseconds to wait for the timer to fire.
     * \return  Returns true if the timer fired before timeout.
     **/
    bool _os_lock( uint32_t timeout );

    /**
     * \brief   OS-specific implementation to activate the timer with the timeout from the
     *          constructor.
     *
     * \return  Returns true if successfully activated.
     **/
    bool _os_set_timer();

    /**
     * \brief   OS-specific implementation to cancel the timer.
     *
     * \return  Returns true if successfully cancelled.
     **/
    bool _os_cancel_timer();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Timeout in milliseconds
     **/
    const uint32_t  mTimeout;
    /**
     * \brief   Flag containing information whether timer is periodic or not
     **/
    const bool          mIsPeriodic;
    /**
     * \brief   Flag containing information whether timer is auto-reset or not.
     **/
    const bool          mIsAutoReset;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    SyncTimer() = delete;
    AREG_NOCOPY_NOMOVE( SyncTimer );
};

//////////////////////////////////////////////////////////////////////////
// Wait class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   High-resolution wait object for suspending threads for short durations. Minimum timeout
 *          is 1 microsecond. Used for precise timing where millisecond-resolution sleep is
 *          insufficient. Not suitable for long waits or thread synchronization.
 **/
class AREG_API Wait
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
public:
    //!< The high resolution clock, close to real-time
    using SteadyTime    = std::chrono::steady_clock::time_point;
    //!< The system clock, with higher precision
    using SystemTime    = std::chrono::system_clock::time_point;
    //!< Duration in nanoseconds.
    using Duration      = std::chrono::nanoseconds;

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    //!< One nanosecond duration
    static constexpr Duration   ONE_NS      { areg::DURATION_1_NS };
    //!< One microsecond duration
    static constexpr Duration   ONE_MUS     { areg::DURATION_1_MICRO };
    //!< One millisecond duration
    static constexpr Duration   ONE_MS      { areg::DURATION_1_MILLI };
    //!< One second duration
    static constexpr Duration   ONE_SEC     { areg::DURATION_1_SEC };
    //!< The minimum waiting timeout
    static constexpr Duration   MIN_WAIT    { ONE_MS  * 5 };

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

    //!< The waiting results
    enum class WaitResolution
    {
          Invalid       = 0 //!< No waiting, due to invalid timeout
        , Ignored       = 1 //!< The waiting timeout is set, but ignored, because timeout in nanoseconds
        , Millisecond   = 2 //!< Wait thread in milliseconds or longer
        , Microsecond   = 3 //!< Wait thread in microseconds up to MIN_WAIT.
    };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    Wait();
    ~Wait();

//////////////////////////////////////////////////////////////////////////
// Static operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Converts from high-resolution steady clock to system clock.
     *
     * \param   time    The steady time value to convert.
     * \return  Returns the converted system clock time.
     **/
    [[nodiscard]]
    static inline Wait::SystemTime convert_to_system_clock(const Wait::SteadyTime& time);
    
    /**
     * \brief   Converts from system clock to high-resolution steady clock.
     *
     * \param   time    The system time value to convert.
     * \return  Returns the converted steady clock time.
     **/
    [[nodiscard]]
    static inline Wait::SteadyTime convert_to_steady_clock(const Wait::SystemTime& time);

    /**
     * \brief   Calculates the time remaining from now until a future steady clock time.
     *
     * \param   future      A steady high-resolution time point in the future.
     * \return  Returns the duration in nanoseconds until the future time. Negative if the time is
     *          already in the past.
     **/
    [[nodiscard]]
    static inline Wait::Duration from_now(const Wait::SteadyTime& future);

    /**
     * \brief   Calculates the elapsed time from a past steady clock time until now.
     *
     * \param   passed      A steady high-resolution time point in the past.
     * \return  Returns the duration in nanoseconds elapsed since the past time. Negative if the
     *          time is in the future.
     **/
    [[nodiscard]]
    static inline Wait::Duration until_now(const Wait::SteadyTime& passed);

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Suspends the thread for the specified number of milliseconds.
     *
     * \param   ms      The duration in milliseconds to suspend the thread.
     * \return  Returns the wait resolution. Any value other than WaitResolution::Invalid indicates
     *          successful operation.
     **/
    inline Wait::WaitResolution wait(uint32_t ms) const;

    /**
     * \brief   Suspends the thread for the specified duration in nanoseconds (rounded to
     *          microseconds).
     *
     * \param   timeout     The duration in nanoseconds (rounded to microseconds) to suspend the
     *                      thread. Minimum is 1 microsecond.
     * \return  Returns the wait resolution. Any value other than WaitResolution::Invalid indicates
     *          successful operation.
     **/
    inline Wait::WaitResolution wait_for(Wait::Duration timeout) const;

    /**
     * \brief   Suspends the thread until the specified steady high-resolution time is reached.
     *
     * \param   future      A future steady high-resolution time point. The thread is not suspended
     *                      if the difference is less than 1 microsecond.
     * \return  Returns the wait resolution. Any value other than WaitResolution::Invalid indicates
     *          successful operation.
     **/
    inline Wait::WaitResolution wait_until(const Wait::SteadyTime& future) const;

//////////////////////////////////////////////////////////////////////////
// Hidden OS dependent calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   OS-specific implementation to initialize the timer.
     **/
    void _os_init_timer();
    /**
     * \brief   OS-specific implementation to release the timer resources.
     **/
    void _os_release_timer();
    /**
     * \brief   OS-specific implementation to suspend the thread for a specified duration.
     *
     * \param   timeout     The duration in nanoseconds to suspend the thread.
     * \return  Returns the wait resolution. Any value other than WaitResolution::Invalid indicates
     *          successful operation.
     **/
    WaitResolution _os_wait_for(const Wait::Duration& timeout) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    TIMERHANDLE mTimer; //!< OS dependent timer.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(Wait);
};

//////////////////////////////////////////////////////////////////////////
// SyncTimer class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool SyncTimer::lock( uint32_t timeout /* = areg::WAIT_INFINITE */ )
{
    ASSERT( mSyncObject != nullptr );
    return _os_lock( timeout );
}

inline bool SyncTimer::unlock()
{
    ASSERT( mSyncObject != nullptr );
    return _os_cancel_timer( );
}

inline bool SyncTimer::set_timer()
{
    ASSERT( mSyncObject != nullptr );
    return _os_set_timer( );
}

inline bool SyncTimer::cancel_timer()
{
    ASSERT( mSyncObject != nullptr );
    return _os_cancel_timer( );
}

inline uint32_t SyncTimer::due_time() const
{
    return mTimeout;
}

inline bool SyncTimer::is_periodic() const noexcept
{
    return mIsPeriodic;
}

inline bool SyncTimer::is_autoreset() const noexcept
{
    return mIsAutoReset;
}



//////////////////////////////////////////////////////////////////////////
// Wait class inline functions
//////////////////////////////////////////////////////////////////////////

inline Wait::SystemTime Wait::convert_to_system_clock(const Wait::SteadyTime& time)
{
    SystemTime result = std::chrono::system_clock::now();
    SteadyTime steady = std::chrono::steady_clock::now();
    return std::chrono::time_point_cast<SystemTime::duration>(result + (time - steady));
}

inline Wait::SteadyTime Wait::convert_to_steady_clock(const Wait::SystemTime& time)
{
    SteadyTime result = std::chrono::steady_clock::now();
    SystemTime system = std::chrono::system_clock::now();
    return std::chrono::time_point_cast<SteadyTime::duration>(result + (time - system));
}

inline Wait::Duration Wait::from_now(const Wait::SteadyTime& future)
{
    return (future - std::chrono::steady_clock::now());
}

inline Wait::Duration Wait::until_now(const Wait::SteadyTime& passed)
{
    return (std::chrono::steady_clock::now() - passed);
}

inline Wait::WaitResolution Wait::wait(uint32_t ms) const
{
    return _os_wait_for(Wait::Duration{ ms * Wait::ONE_MS });
}

inline Wait::WaitResolution Wait::wait_for(Wait::Duration timeout) const
{
    return _os_wait_for(timeout);
}

inline Wait::WaitResolution Wait::wait_until(const Wait::SteadyTime& future) const
{
    return _os_wait_for(future - std::chrono::steady_clock::now());
}


} // namespace areq

#endif // AREG_BASE_SYNCTIMER_HPP
