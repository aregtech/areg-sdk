#ifndef AREG_BASE_SYNCLOCK_HPP
#define AREG_BASE_SYNCLOCK_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/SyncPrimitives.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Synchronization objects
 *              Declared following synchronization objects:
 *              Lockable      - blocking synchronization object interface.
 *              Mutex               - Mutex synchronization object.
 *              SyncEvent           - Event synchronization object.
 *              Semaphore           - Semaphore synchronization object.
 *              CriticalSection     - Critical Section synchronization object.
 *              SpinLock            - Spin-Lock synchronization object.
 *              ResourceLock        - An OS specific resource lock object.
 *              NolockSyncObject    - No Locking synchronization object (makes no locking).
 *              SyncTimer           - Timer synchronization object.
 *              Lock                - Single synchronization auto locking object.
 *              MultiLock           - Multiple synchronization auto locking object.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/lockable.h"
#include <chrono>

namespace areg {
/************************************************************************
 * List of declared classes
 ************************************************************************/
class Lock;
class MultiLock;
class Wait;



