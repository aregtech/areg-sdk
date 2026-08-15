/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/MultiLockTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests for MultiLock.
 *              Covers waiting for any one object, waiting for all of them,
 *              timeouts, the returned index, selective and full unlocking,
 *              auto-locking on construction and release on destruction, and
 *              waking up from another thread.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"

#include <thread>

namespace
{
    using areg::MultiLock;
    using areg::Semaphore;
    using areg::SyncEvent;
    using areg::SyncObject;

    //!< Short timeout for the calls that are expected to give up.
    constexpr uint32_t  SHORT_WAIT_MS   { 200u };

    //!< Delay before a helper thread signals, long enough for the waiter to park.
    constexpr uint32_t  SIGNAL_DELAY_MS { 120u };

    // Local copies of the MultiLock result indexes. EXPECT_EQ binds its arguments to a
    // constant reference, which odr-uses the member and asks the linker for its address.
    // On the targets where 'areg' is a shared library with an imported class -- cygwin
    // and MinGW mark every member of a dllimport class as '__imp_' -- that address does
    // not exist, because a constexpr member is never emitted as data. Comparing against
    // a local constant keeps the check a pure compile time value.
    constexpr int32_t   INDEX_TIMEOUT   { MultiLock::LOCK_INDEX_TIMEOUT };
    constexpr int32_t   INDEX_ALL       { MultiLock::LOCK_INDEX_ALL };
}

//////////////////////////////////////////////////////////////////////////
// Waiting for any single object
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Only the second object is signaled, so the returned index names it.
 **/
TEST(MultiLockTest, WaitAnyReturnsIndexOfSignaledObject)
{
    SyncEvent evtOne(true, false);      // non-signaled, manual reset
    SyncEvent evtTwo(false, false);     // signaled, manual reset

    SyncObject * objects[]{ static_cast<SyncObject *>(&evtOne), static_cast<SyncObject *>(&evtTwo) };
    MultiLock multi(objects, 2, false);

    EXPECT_EQ(multi.lock(SHORT_WAIT_MS, false), 1);
    EXPECT_TRUE(multi.unlock());
}

/**
 * \brief   The first signaled object wins when more than one is ready.
 **/
TEST(MultiLockTest, WaitAnyPrefersTheFirstSignaledObject)
{
    SyncEvent evtOne(false, false);
    SyncEvent evtTwo(false, false);

    SyncObject * objects[]{ static_cast<SyncObject *>(&evtOne), static_cast<SyncObject *>(&evtTwo) };
    MultiLock multi(objects, 2, false);

    EXPECT_EQ(multi.lock(SHORT_WAIT_MS, false), 0);
    EXPECT_TRUE(multi.unlock());
}

/**
 * \brief   Nothing is signaled, so the wait gives up and reports the timeout.
 **/
TEST(MultiLockTest, WaitAnyTimesOutWhenNothingIsSignaled)
{
    SyncEvent evtOne(true, false);
    SyncEvent evtTwo(true, false);

    SyncObject * objects[]{ static_cast<SyncObject *>(&evtOne), static_cast<SyncObject *>(&evtTwo) };
    MultiLock multi(objects, 2, false);

    EXPECT_EQ(multi.lock(SHORT_WAIT_MS, false), INDEX_TIMEOUT);
}

//////////////////////////////////////////////////////////////////////////
// Waiting for every object
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Every object is signaled, so the wait reports that it holds them all.
 **/
TEST(MultiLockTest, WaitAllSucceedsWhenEveryObjectIsSignaled)
{
    SyncEvent evtOne(false, false);
    SyncEvent evtTwo(false, false);
    SyncEvent evtThree(false, false);

    SyncObject * objects[]{ static_cast<SyncObject *>(&evtOne)
                          , static_cast<SyncObject *>(&evtTwo)
                          , static_cast<SyncObject *>(&evtThree) };
    MultiLock multi(objects, 3, false);

    EXPECT_EQ(multi.lock(SHORT_WAIT_MS, true), INDEX_ALL);
    EXPECT_TRUE(multi.unlock());
}

/**
 * \brief   One object short of the full set is still a timeout, not a partial success.
 **/
TEST(MultiLockTest, WaitAllTimesOutWhenOneObjectIsMissing)
{
    SyncEvent evtOne(false, false);
    SyncEvent evtTwo(true, false);

    SyncObject * objects[]{ static_cast<SyncObject *>(&evtOne), static_cast<SyncObject *>(&evtTwo) };
    MultiLock multi(objects, 2, false);

    EXPECT_EQ(multi.lock(SHORT_WAIT_MS, true), INDEX_TIMEOUT);
}

//////////////////////////////////////////////////////////////////////////
// Locking on construction, releasing on destruction
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   With auto-locking the objects are taken in the constructor. Semaphores show it,
 *          since a count that was consumed cannot be taken twice.
 **/
TEST(MultiLockTest, AutoLockTakesTheObjectsOnConstruction)
{
    Semaphore semOne(1, 1);
    Semaphore semTwo(1, 1);

    SyncObject * objects[]{ static_cast<SyncObject *>(&semOne), static_cast<SyncObject *>(&semTwo) };

    do
    {
        MultiLock multi(objects, 2, true);

        // Both counts are consumed by the auto lock, so neither can be taken again.
        EXPECT_FALSE(semOne.lock(areg::DO_NOT_WAIT));
        EXPECT_FALSE(semTwo.lock(areg::DO_NOT_WAIT));
    } while (false);

    // Leaving the scope releases both, so each count is available once more.
    EXPECT_TRUE(semOne.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(semTwo.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(semOne.unlock());
    EXPECT_TRUE(semTwo.unlock());
}

/**
 * \brief   Without auto-locking the constructor takes nothing.
 **/
TEST(MultiLockTest, NoAutoLockLeavesTheObjectsAlone)
{
    Semaphore semOne(1, 1);
    Semaphore semTwo(1, 1);

    SyncObject * objects[]{ static_cast<SyncObject *>(&semOne), static_cast<SyncObject *>(&semTwo) };
    MultiLock multi(objects, 2, false);

    EXPECT_TRUE(semOne.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(semTwo.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(semOne.unlock());
    EXPECT_TRUE(semTwo.unlock());
}

//////////////////////////////////////////////////////////////////////////
// Unlocking
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Unlocking one index releases that object and leaves the rest held.
 **/
TEST(MultiLockTest, UnlockByIndexReleasesOnlyThatObject)
{
    Semaphore semOne(1, 1);
    Semaphore semTwo(1, 1);

    SyncObject * objects[]{ static_cast<SyncObject *>(&semOne), static_cast<SyncObject *>(&semTwo) };
    MultiLock multi(objects, 2, true);

    EXPECT_TRUE(multi.unlock(0));
    EXPECT_TRUE(semOne.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(semTwo.lock(areg::DO_NOT_WAIT));

    EXPECT_TRUE(semOne.unlock());
}

/**
 * \brief   An index outside the list is refused, and nothing is released.
 **/
TEST(MultiLockTest, UnlockRefusesAnIndexOutsideTheList)
{
    Semaphore semOne(1, 1);

    SyncObject * objects[]{ static_cast<SyncObject *>(&semOne) };
    MultiLock multi(objects, 1, true);

    EXPECT_FALSE(multi.unlock(1));
    EXPECT_FALSE(multi.unlock(MultiLock::LOCK_INDEX_INVALID));
    EXPECT_FALSE(semOne.lock(areg::DO_NOT_WAIT));
}

/**
 * \brief   Unlocking twice is harmless, the second call has nothing left to release.
 **/
TEST(MultiLockTest, UnlockTwiceIsHarmless)
{
    Semaphore semOne(1, 1);
    Semaphore semTwo(1, 1);

    SyncObject * objects[]{ static_cast<SyncObject *>(&semOne), static_cast<SyncObject *>(&semTwo) };
    MultiLock multi(objects, 2, true);

    EXPECT_TRUE(multi.unlock());
    EXPECT_TRUE(multi.unlock());

    // Each semaphore is back to exactly one count. The second release added nothing,
    // otherwise the count could be taken twice.
    EXPECT_TRUE(semOne.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(semOne.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(semTwo.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(semTwo.lock(areg::DO_NOT_WAIT));
}

//////////////////////////////////////////////////////////////////////////
// Waking up from another thread
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A waiter parked on a set of objects is released by whichever one another
 *          thread signals, and the index names it.
 **/
TEST(MultiLockTest, WaitAnyWakesOnASignalFromAnotherThread)
{
    SyncEvent evtOne(true, false);
    SyncEvent evtTwo(true, false);

    std::thread signaller([&evtTwo]()
    {
        areg::Thread::sleep(SIGNAL_DELAY_MS);
        evtTwo.set_signaled();
    });

    SyncObject * objects[]{ static_cast<SyncObject *>(&evtOne), static_cast<SyncObject *>(&evtTwo) };
    MultiLock multi(objects, 2, false);

    EXPECT_EQ(multi.lock(areg::WAIT_INFINITE, false), 1);
    signaller.join();

    EXPECT_TRUE(multi.unlock());
}

/**
 * \brief   A waiter that needs every object stays parked until the last one arrives.
 **/
TEST(MultiLockTest, WaitAllWakesOnlyAfterTheLastSignal)
{
    SyncEvent evtOne(true, false);
    SyncEvent evtTwo(true, false);

    std::thread signaller([&evtOne, &evtTwo]()
    {
        areg::Thread::sleep(SIGNAL_DELAY_MS);
        evtOne.set_signaled();
        areg::Thread::sleep(SIGNAL_DELAY_MS);
        evtTwo.set_signaled();
    });

    SyncObject * objects[]{ static_cast<SyncObject *>(&evtOne), static_cast<SyncObject *>(&evtTwo) };
    MultiLock multi(objects, 2, false);

    EXPECT_EQ(multi.lock(areg::WAIT_INFINITE, true), INDEX_ALL);
    signaller.join();

    EXPECT_TRUE(multi.unlock());
}
