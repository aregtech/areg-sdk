#ifndef AREG_COMPONENT_PRIVATE_WATCHDOG_HPP
#define AREG_COMPONENT_PRIVATE_WATCHDOG_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/Watchdog.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Thread watchdog object declaration
 *
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/SynchObjects.hpp"

class DispatcherThread;
class ComponentThread;
class WorkerThread;
class String;

class Watchdog
{
    friend class WatchdogManager;

public:
#if defined(BIT64)
    using SEQUENCE_ID   = unsigned int;
    using GUARD_ID      = unsigned int;
#else   // !defined(BIT64)
    using GUARD_ID      = unsigned short;
    using GUARD_ID      = unsigned short;
#endif  // defined(BIT64)

    using WATCHDOG_ID   = id_type;

    static constexpr uint32_t       NO_WATCHDOG         = static_cast<uint32_t>(0u);

    static constexpr WATCHDOG_ID    INVALID_WATCHDOG    = static_cast<WATCHDOG_ID>(0u);

public:
    Watchdog(ComponentThread& thread, uint32_t msTimeout = Watchdog::NO_WATCHDOG);
    Watchdog(WorkerThread& thread, uint32_t msTimeout = Watchdog::NO_WATCHDOG);
    ~Watchdog( void );

public:

    void startGuard( void );

    void stopGuard( void );

    void timeoutExpired(Watchdog::WATCHDOG_ID watchdogId);

    const Watchdog::GUARD_ID getId( void ) const;

    const Watchdog::SEQUENCE_ID getSequence( void ) const;

    const String & getName( void ) const;

    const uint32_t getTimeout( void ) const;

    const DispatcherThread & getThread( void ) const;

    const ComponentThread & getComponentThread( void ) const;

    const WorkerThread * getWorkerThread( void ) const;

    TIMERHANDLE getHandle( void ) const;

    bool isActive( void ) const;

    WATCHDOG_ID makeWatchdogId( void );

    static WATCHDOG_ID makeWatchdogId(GUARD_ID guardId, SEQUENCE_ID sequence);

    static GUARD_ID makeGuardId(Watchdog::WATCHDOG_ID watchdogId);

    static SEQUENCE_ID makeSequenceId(Watchdog::WATCHDOG_ID watchdogId);

    bool isValid( void ) const;

private:
    static GUARD_ID _generateId(void);

    static TIMERHANDLE _createWaitableTimer(const Watchdog& watchdog);

    static void _destroyWaitableTimer(const Watchdog& watchdog);

private:

    const GUARD_ID      mGuardId;

    const unsigned int  mTimeoutMs;

    SEQUENCE_ID         mSequence;

    ComponentThread &   mComponentThread;

    WorkerThread *      mWorkerThread;

    TIMERHANDLE         mTimer;

    bool                mIsActive;

    ResourceLock        mLock;

private:
    DECLARE_NOCOPY_NOMOVE(Watchdog);
};

#endif  // AREG_COMPONENT_PRIVATE_WATCHDOG_HPP

