#ifndef AREG_COMPONENT_PRIVATE_WATCHDOG_HPP
#define AREG_COMPONENT_PRIVATE_WATCHDOG_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/Watchdog.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Thread watchdog object declaration
 *
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/component/TimerBase.hpp"
#include "areg/base/MathDefs.hpp"
namespace areg {

/************************************************************************
 * Dependencies.
 ************************************************************************/
class ComponentThread;
class WorkerThread;

/**
 * \brief   Guards thread execution by monitoring event processing; terminates and restarts threads
 *          that exceed timeout thresholds.
 **/
class AREG_API Watchdog final : public TimerBase
{
//////////////////////////////////////////////////////////////////////////
// Object specific types and constants
//////////////////////////////////////////////////////////////////////////
    friend class WatchdogManager;

public:
#if (AREG_TARGET_PLATFORM == 64)
    /**
     * \brief   The Sequence number changed each time watchdog is started. It can be zero.
     **/
    using SEQUENCE_ID   = uint32_t;
    /**
     * \brief   The unique ID of watchdog guard. It is not a zero and the maximum value is 0xFFFFFFFF.
     **/
    using GUARD_ID      = uint32_t;
#elif (AREG_TARGET_PLATFORM == 32)
    /**
     * \brief   The Sequence number changed each time watchdog is started. It can be zero.
     **/
    using SEQUENCE_ID   = uint16_t;
    /**
     * \brief   The unique ID of watchdog guard. It is not a zero and the maximum value is 0xFFFF.
     **/
    using GUARD_ID      = uint16_t;
#endif  // (AREG_TARGET_PLATFORM == 64)

    /**
     * \brief   The watchdog ID, which is generated when the watchdog is started.
     **/
    using WATCHDOG_ID   = ptr_type;

    /**
     * \brief   Identifies invalid watchdog ID.
     */
    static constexpr WATCHDOG_ID    INVALID_WATCHDOG    { static_cast<WATCHDOG_ID>(0u) };

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Binds the watchdog to a component thread with an optional timeout.
     *
     * \param   thread          The component thread to monitor.
     * \param   msTimeout       Timeout in milliseconds; zero disables watchdog.
     **/
    Watchdog(ComponentThread& thread, uint32_t msTimeout = areg::WATCHDOG_IGNORE);

    /**
     * \brief   Binds the watchdog to a worker thread with an optional timeout.
     *
     * \param   thread          The worker thread to monitor.
     * \param   msTimeout       Timeout in milliseconds; zero disables watchdog.
     **/
    Watchdog(WorkerThread& thread, uint32_t msTimeout = areg::WATCHDOG_IGNORE);

    virtual ~Watchdog();

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Starts the watchdog monitoring.
     **/
    void start_guard();

    /**
     * \brief   Stops the watchdog monitoring.
     **/
    void stop_guard();

    /**
     * \brief   Returns true if the watchdog is valid and enabled (timeout is not zero).
     **/
    [[nodiscard]]
    inline bool is_valid() const noexcept;

    /**
     * \brief   Returns the watchdog guard identifier.
     **/
    [[nodiscard]]
    inline Watchdog::GUARD_ID id() const noexcept;

    /**
     * \brief   Returns the watchdog activation sequence number.
     **/
    [[nodiscard]]
    inline Watchdog::SEQUENCE_ID sequence() const noexcept;

    /**
     * \brief   Returns the component thread associated with the watchdog.
     **/
    [[nodiscard]]
    inline const ComponentThread& component_thread() const noexcept;

    /**
     * \brief   Generates a watchdog ID from the current guard ID and sequence number.
     **/
    [[nodiscard]]
    inline WATCHDOG_ID watchdog_id() const noexcept;

    /**
     * \brief   Generates a watchdog ID from the given guard ID and sequence number.
     *
     * \param   guardId     The guard identifier.
     * \param   sequence    The sequence number.
     * \return  The generated watchdog ID.
     **/
    [[nodiscard]]
    inline static constexpr WATCHDOG_ID make_watchdog_id(GUARD_ID guardId, SEQUENCE_ID sequence) noexcept;

    /**
     * \brief   Extracts the guard ID from a watchdog ID.
     *
     * \param   watchdog_id     The watchdog ID.
     * \return  The extracted guard ID.
     **/
    [[nodiscard]]
    inline static constexpr GUARD_ID make_guard_id(Watchdog::WATCHDOG_ID watchdog_id) noexcept;

    /**
     * \brief   Extracts the sequence number from a watchdog ID.
     *
     * \param   watchdog_id     The watchdog ID.
     * \return  The extracted sequence number.
     **/
    [[nodiscard]]
    inline static constexpr SEQUENCE_ID make_sequence_id(Watchdog::WATCHDOG_ID watchdog_id) noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Generates a unique guard ID for each watchdog.
     *
     * \return  The generated unique guard ID.
     **/
    static GUARD_ID _generate_id();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   The unique identifier of the Watchdog object.
     **/
    const GUARD_ID      mGuardId;
    /**
     * \brief   The sequence number of the Watchdog. The number is changed each time when timer starts.
     */
    SEQUENCE_ID         mSequence;
    /**
     * \brief   The valid instance of the component thread to trigger restart if timeout expired.
     **/
    ComponentThread &   mComponentThread;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    Watchdog() = delete;
    AREG_NOCOPY_NOMOVE(Watchdog);
};

//////////////////////////////////////////////////////////////////////////
// Watchdog inline methods.
//////////////////////////////////////////////////////////////////////////

inline bool Watchdog::is_valid() const noexcept
{
    return (mHandle != nullptr);
}

inline Watchdog::GUARD_ID Watchdog::id() const noexcept
{
    return mGuardId;
}

inline Watchdog::SEQUENCE_ID Watchdog::sequence() const noexcept
{
    return mSequence;
}

inline const ComponentThread& Watchdog::component_thread() const noexcept
{
    return mComponentThread;
}

inline Watchdog::WATCHDOG_ID Watchdog::watchdog_id() const noexcept
{
    return Watchdog::make_watchdog_id(mGuardId, mSequence);
}

inline constexpr Watchdog::WATCHDOG_ID Watchdog::make_watchdog_id(GUARD_ID guardId, SEQUENCE_ID sequence) noexcept
{
#if (AREG_TARGET_PLATFORM ==64)
    return static_cast<WATCHDOG_ID>(areg::make64(guardId, sequence));
#elif (AREG_TARGET_PLATFORM == 32)
    return static_cast<WATCHDOG_ID>(areg::make32(guardId, sequence));
#endif  // (AREG_TARGET_PLATFORM == 64)
}

inline constexpr Watchdog::GUARD_ID Watchdog::make_guard_id(Watchdog::WATCHDOG_ID watchdog_id) noexcept
{
#if (AREG_TARGET_PLATFORM == 64)
    return static_cast<GUARD_ID>(areg::hi_dword(watchdog_id));
#elif (AREG_TARGET_PLATFORM == 32)
    return static_cast<GUARD_ID>(areg::hi_word(watchdog_id));
#endif  // (AREG_TARGET_PLATFORM == 64)
}

inline constexpr Watchdog::SEQUENCE_ID Watchdog::make_sequence_id(Watchdog::WATCHDOG_ID watchdog_id) noexcept
{
#if (AREG_TARGET_PLATFORM == 64)
    return static_cast<SEQUENCE_ID>(areg::lo_dword(watchdog_id));
#elif (AREG_TARGET_PLATFORM == 32)
    return static_cast<SEQUENCE_ID>(areg::lo_word(watchdog_id));
#endif  // (AREG_TARGET_PLATFORM == 64)
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_WATCHDOG_HPP
