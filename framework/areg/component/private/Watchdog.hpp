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

 /************************************************************************
  * Dependencies.
  ************************************************************************/
class ComponentThread;
namespace areg { class WorkerThread; }

/**
 * \brief   Watchdog is a guarding object to track thread execution.
 *          It is instantiated in threads and triggered each time the thread
 *          starts to process an event. If the watchdog timeout expired before
 *          the thread could process an event, it riggers procedure to
 *          terminate the component thread and restarts again.
 *          There is no guarantee that terminated thread will make all memory
 *          and stack cleanups. The terminated thread cleans up all components
 *          and proxies registered in the thread, all worker threads and then
 *          terminates the component thread, and restarts again.
 *          Set the watchdog timeout value big enough to process events.
 *          The watchdog timeout is set in milliseconds.
 *          If the watchdog timeout is zero (areg::WATCHDOG_IGNORE), the
 *          watchdog is ignored for the thread and thread is not terminated.
 **/
class AREG_API Watchdog  : public TimerBase
{
//////////////////////////////////////////////////////////////////////////
// Object specific types and constants
//////////////////////////////////////////////////////////////////////////
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
     * \brief   The watchdog object bind with Component Thread. Should not be instantiated
     *          for Worker Thread.
     * \param   thread      The valid instance of the Component Thread.
     * \param   msTimeout   Timeout in milliseconds of the Watchdog to check thread status.
     *                      The timeout with value zero disables Watchdog
     **/
    Watchdog(ComponentThread& thread, uint32_t msTimeout = areg::WATCHDOG_IGNORE);

    /**
     * \brief   The watchdog object bind with Worker Thread. Should not be instantiated
     *          for Component Thread.
     * \param   thread      The valid instance of the Worker Thread.
     * \param   msTimeout   Timeout in milliseconds of the Watchdog to check thread status.
     *                      The timeout with value zero disables Watchdog
     **/
    Watchdog(areg::WorkerThread& thread, uint32_t msTimeout = areg::WATCHDOG_IGNORE);

    /**
     * \brief   Destructor.
     **/
    virtual ~Watchdog();

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Call to start the watchdog.
     **/
    void startGuard();

    /**
     * \brief   Call to stop the watchdog.
     **/
    void stopGuard();

    /**
     * \brief   Returns true if watchdog object is valid and can start timer.
     *          The Watchdog is valid if the timeout is not zero.
     **/
    inline bool isValid() const;

    /**
     * \brief   Returns the watchdog ID.
     */
    inline Watchdog::GUARD_ID getId() const;

    /**
     * \brief   Returns the watchdog activation sequence number.
     **/
    inline Watchdog::SEQUENCE_ID getSequence() const;

    /**
     * \brief   Returns the instance of component thread that contains this watchdog.
     *          If watchdog belongs to worker thread, it returns the thread of owning component.
     *          The component thread of the watchdog is always valid.
     **/
    inline const ComponentThread& getComponentThread() const;

    /**
     * \brief   Out of Guard ID and Sequence number generates watchdog ID.
     *          The ID changed each time when timer is started.
     **/
    inline WATCHDOG_ID watchdogId();

    /**
     * \brief   Out of passed Guard ID and Sequence number generates watchdog ID.
     * \param   guardId     The guard ID.
     * \param   sequence    The sequence number.
     * \return  Generated watchdog ID.
     **/
    inline static WATCHDOG_ID makeWatchdogId(GUARD_ID guardId, SEQUENCE_ID sequence);

    /**
     * \brief   Extracts the Guard ID from previously generated watchdog ID.
     *
     * \param   watchdogId  The previously generated watchdog ID.
     * \return  Guard ID, which exists in watchdog ID value.
     **/
    inline static GUARD_ID makeGuardId(Watchdog::WATCHDOG_ID watchdogId);

    /**
     * \brief   Extracts the sequence number from previously generated watchdog ID.
     *
     * \param   watchdogId  The previously generated watchdog ID.
     * \return  Sequence number, which exists in watchdog ID value.
     **/
    inline static SEQUENCE_ID makeSequenceId(Watchdog::WATCHDOG_ID watchdogId);

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   This static method generates unique Guard ID for each watchdog object.
     * \return  Generated unique identifier of the Watchdog object.
     **/
    static GUARD_ID _generateId();

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

inline bool Watchdog::isValid() const
{
    return (mHandle != nullptr);
}

inline Watchdog::GUARD_ID Watchdog::getId() const
{
    return mGuardId;
}

inline Watchdog::SEQUENCE_ID Watchdog::getSequence() const
{
    return mSequence;
}

inline const ComponentThread& Watchdog::getComponentThread() const
{
    return mComponentThread;
}

inline Watchdog::WATCHDOG_ID Watchdog::watchdogId()
{
    return Watchdog::makeWatchdogId(mGuardId, mSequence);
}

inline Watchdog::WATCHDOG_ID Watchdog::makeWatchdogId(GUARD_ID guardId, SEQUENCE_ID sequence)
{
#if (AREG_TARGET_PLATFORM ==64)
    return static_cast<WATCHDOG_ID>(areg::make64(guardId, sequence));
#elif (AREG_TARGET_PLATFORM == 32)
    return static_cast<WATCHDOG_ID>(areg::make32(guardId, sequence));
#endif  // (AREG_TARGET_PLATFORM == 64)
}

inline Watchdog::GUARD_ID Watchdog::makeGuardId(Watchdog::WATCHDOG_ID watchdogId)
{
#if (AREG_TARGET_PLATFORM == 64)
    return static_cast<GUARD_ID>(areg::hiDword(watchdogId));
#elif (AREG_TARGET_PLATFORM == 32)
    return static_cast<GUARD_ID>(areg::hiWord(watchdogId));
#endif  // (AREG_TARGET_PLATFORM == 64)
}

inline Watchdog::SEQUENCE_ID Watchdog::makeSequenceId(Watchdog::WATCHDOG_ID watchdogId)
{
#if (AREG_TARGET_PLATFORM == 64)
    return static_cast<GUARD_ID>(areg::loDword(watchdogId));
#elif (AREG_TARGET_PLATFORM == 32)
    return static_cast<GUARD_ID>(areg::loWord(watchdogId));
#endif  // (AREG_TARGET_PLATFORM == 64)
}

#endif  // AREG_COMPONENT_PRIVATE_WATCHDOG_HPP

