/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/RingStackTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Tests of LinkedList object.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/RingStack.hpp"
#include "areg/base/SharedBuffer.hpp"

/**
 * \brief   Test RingStack constructors with Ring Stack type 'stop on overlap'.
 *          "Stop on overlap" means do not insert any entry if the ring is full.
 **/
TEST(RingStackTest, test_constructors_stop)
{
    using LockRing = areg::ConcurrentRingStack<int>;
    using NolockRing = areg::RingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lock(count, areg::OverlapPolicy::Stop);
    NolockRing nolock(count, areg::OverlapPolicy::Stop);

    EXPECT_TRUE(lock.is_empty());
    EXPECT_TRUE(nolock.is_empty());
    EXPECT_FALSE(lock.is_full() || nolock.is_full());
    EXPECT_EQ(lock.capacity(), count);
    EXPECT_EQ(nolock.capacity(), count);

    int loop = static_cast<int>(count * 2u);
    for (int i = 0; i < loop; ++i)
    {
        if (i < static_cast<int>(count))
        {
            EXPECT_FALSE(lock.is_full());
            EXPECT_EQ(lock.push(i), static_cast<uint32_t>(i + 1));
            EXPECT_TRUE(lock.contains(i));

            EXPECT_FALSE(nolock.is_full());
            EXPECT_EQ(nolock.push(i + static_cast<int>(count)), static_cast<uint32_t>(i + 1));
            EXPECT_TRUE(nolock.contains(i + static_cast<int>(count)));
        }
        else
        {
            // no more space
            EXPECT_TRUE(lock.is_full());
            EXPECT_EQ(lock.push(i), count);
            EXPECT_FALSE(lock.contains(i));

            EXPECT_TRUE(nolock.is_full());
            EXPECT_EQ(nolock.push(i + static_cast<int>(count)), count);
            EXPECT_FALSE(nolock.contains(i + static_cast<int>(count)));
        }
    }

    EXPECT_TRUE(lock.is_full());
    EXPECT_TRUE(nolock.is_full());
    EXPECT_NE(lock, nolock);

    LockRing lockCopy1(lock);
    EXPECT_EQ(lockCopy1, lock);
    EXPECT_EQ(lockCopy1.size(), count);
    EXPECT_EQ(lockCopy1.overlap(), areg::OverlapPolicy::Stop);
    EXPECT_TRUE(lockCopy1.is_full());

    LockRing lockCopy2(nolock);
    EXPECT_EQ(lockCopy2, nolock);
    EXPECT_EQ(lockCopy2.size(), count);
    EXPECT_EQ(lockCopy2.overlap(), areg::OverlapPolicy::Stop);
    EXPECT_TRUE(lockCopy2.is_full());

    NolockRing nolockCopy1(nolock);
    EXPECT_EQ(nolockCopy1, nolock);
    EXPECT_EQ(nolockCopy1.size(), count);
    EXPECT_EQ(nolockCopy1.overlap(), areg::OverlapPolicy::Stop);
    EXPECT_TRUE(nolockCopy1.is_full());

    NolockRing nolockCopy2(lock);
    EXPECT_EQ(nolockCopy2, lock);
    EXPECT_EQ(nolockCopy2.size(), count);
    EXPECT_EQ(nolockCopy2.overlap(), areg::OverlapPolicy::Stop);
    EXPECT_TRUE(nolockCopy2.is_full());

    EXPECT_NE(lockCopy2, lock);
    EXPECT_NE(nolockCopy2, nolock);

    LockRing lockMove1(std::move(lockCopy1));
    EXPECT_EQ(lockMove1, lock);
    EXPECT_EQ(lockMove1.size(), count);
    EXPECT_EQ(lockMove1.overlap(), areg::OverlapPolicy::Stop);
    EXPECT_TRUE(lockMove1.is_full());
    EXPECT_TRUE(lockCopy1.is_empty());
    EXPECT_EQ(lockCopy1.size(), 0u);
    EXPECT_EQ(lockCopy1.capacity(), 0u);

    NolockRing nolockMove1(std::move(nolockCopy1));
    EXPECT_EQ(nolockMove1, nolock);
    EXPECT_EQ(nolockMove1.size(), count);
    EXPECT_EQ(nolockMove1.overlap(), areg::OverlapPolicy::Stop);
    EXPECT_TRUE(nolockMove1.is_full());
    EXPECT_TRUE(nolockCopy1.is_empty());
    EXPECT_EQ(nolockCopy1.size(), 0u);
    EXPECT_EQ(nolockCopy1.capacity(), 0u);

    lockCopy1 = lock;
    EXPECT_EQ(lockCopy1, lockMove1);
    nolockCopy1 = nolock;
    EXPECT_EQ(nolockCopy1, nolockMove1);

    LockRing lockMove2(std::move(nolockCopy1));
    EXPECT_EQ(lockMove2, nolock);
    EXPECT_EQ(lockMove2.size(), count);
    EXPECT_EQ(lockMove2.overlap(), areg::OverlapPolicy::Stop);
    EXPECT_TRUE(lockMove2.is_full());
    EXPECT_TRUE(nolockCopy1.is_empty());
    EXPECT_EQ(nolockCopy1.size(), 0u);
    EXPECT_EQ(nolockCopy1.capacity(), 0u);

    NolockRing nolockMove2(std::move(lockCopy1));
    EXPECT_EQ(nolockMove2, lock);
    EXPECT_EQ(nolockMove2.size(), count);
    EXPECT_EQ(nolockMove2.overlap(), areg::OverlapPolicy::Stop);
    EXPECT_TRUE(nolockMove2.is_full());
    EXPECT_TRUE(lockCopy1.is_empty());
    EXPECT_EQ(lockCopy1.size(), 0u);
    EXPECT_EQ(lockCopy1.capacity(), 0u);
}

/**
 * \brief   Test RingStack constructors with Ring Stack type 'shift on overlap'.
 *          "Shift on overlap" means move the start position of the ring if it
 *          is full, but do not change the size of the ring.
 **/
TEST(RingStackTest, test_constructors_shift)
{
    using LockRing = areg::ConcurrentRingStack<int>;
    using NolockRing = areg::RingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lock(count, areg::OverlapPolicy::Shift);
    NolockRing nolock(count, areg::OverlapPolicy::Shift);

    EXPECT_TRUE(lock.is_empty());
    EXPECT_TRUE(nolock.is_empty());
    EXPECT_FALSE(lock.is_full() || nolock.is_full());
    EXPECT_EQ(lock.capacity(), count);
    EXPECT_EQ(nolock.capacity(), count);

    int loop = static_cast<int>(count * 2u);
    for (int i = 0; i < loop; ++i)
    {
        if (i < static_cast<int>(count))
        {
            EXPECT_FALSE(lock.is_full());
            EXPECT_EQ(lock.push(i), static_cast<uint32_t>(i + 1));
            EXPECT_TRUE(lock.contains(i));

            EXPECT_FALSE(nolock.is_full());
            EXPECT_EQ(nolock.push(i + static_cast<int>(count)), static_cast<uint32_t>(i + 1));
            EXPECT_TRUE(nolock.contains(i + static_cast<int>(count)));
        }
        else
        {
            // no more space
            EXPECT_TRUE(lock.is_full());
            EXPECT_EQ(lock.push(i), count);
            EXPECT_TRUE(lock.contains(i));
            EXPECT_FALSE(lock.contains(i - static_cast<int>(count)));

            EXPECT_TRUE(nolock.is_full());
            EXPECT_EQ(nolock.push(i + static_cast<int>(count)), count);
            EXPECT_TRUE(nolock.contains(i + static_cast<int>(count)));
            EXPECT_FALSE(nolock.contains(i));
        }
    }

    EXPECT_TRUE(lock.is_full());
    EXPECT_TRUE(nolock.is_full());
    EXPECT_NE(lock, nolock);

    LockRing lockCopy1(lock);
    EXPECT_EQ(lockCopy1, lock);
    EXPECT_EQ(lockCopy1.size(), count);
    EXPECT_EQ(lockCopy1.overlap(), areg::OverlapPolicy::Shift);
    EXPECT_TRUE(lockCopy1.is_full());

    LockRing lockCopy2(nolock);
    EXPECT_EQ(lockCopy2, nolock);
    EXPECT_EQ(lockCopy2.size(), count);
    EXPECT_EQ(lockCopy2.overlap(), areg::OverlapPolicy::Shift);
    EXPECT_TRUE(lockCopy2.is_full());

    NolockRing nolockCopy1(nolock);
    EXPECT_EQ(nolockCopy1, nolock);
    EXPECT_EQ(nolockCopy1.size(), count);
    EXPECT_EQ(nolockCopy1.overlap(), areg::OverlapPolicy::Shift);
    EXPECT_TRUE(nolockCopy1.is_full());

    NolockRing nolockCopy2(lock);
    EXPECT_EQ(nolockCopy2, lock);
    EXPECT_EQ(nolockCopy2.size(), count);
    EXPECT_EQ(nolockCopy2.overlap(), areg::OverlapPolicy::Shift);
    EXPECT_TRUE(nolockCopy2.is_full());

    EXPECT_NE(lockCopy2, lock);
    EXPECT_NE(nolockCopy2, nolock);

    LockRing lockMove1(std::move(lockCopy1));
    EXPECT_EQ(lockMove1, lock);
    EXPECT_EQ(lockMove1.size(), count);
    EXPECT_EQ(lockMove1.overlap(), areg::OverlapPolicy::Shift);
    EXPECT_TRUE(lockMove1.is_full());
    EXPECT_TRUE(lockCopy1.is_empty());
    EXPECT_EQ(lockCopy1.size(), 0u);
    EXPECT_EQ(lockCopy1.capacity(), 0u);

    NolockRing nolockMove1(std::move(nolockCopy1));
    EXPECT_EQ(nolockMove1, nolock);
    EXPECT_EQ(nolockMove1.size(), count);
    EXPECT_EQ(nolockMove1.overlap(), areg::OverlapPolicy::Shift);
    EXPECT_TRUE(nolockMove1.is_full());
    EXPECT_TRUE(nolockCopy1.is_empty());
    EXPECT_EQ(nolockCopy1.size(), 0u);
    EXPECT_EQ(nolockCopy1.capacity(), 0u);

    lockCopy1 = lock;
    EXPECT_EQ(lockCopy1, lockMove1);
    nolockCopy1 = nolock;
    EXPECT_EQ(nolockCopy1, nolockMove1);

    LockRing lockMove2(std::move(nolockCopy1));
    EXPECT_EQ(lockMove2, nolock);
    EXPECT_EQ(lockMove2.size(), count);
    EXPECT_EQ(lockMove2.overlap(), areg::OverlapPolicy::Shift);
    EXPECT_TRUE(lockMove2.is_full());
    EXPECT_TRUE(nolockCopy1.is_empty());
    EXPECT_EQ(nolockCopy1.size(), 0u);
    EXPECT_EQ(nolockCopy1.capacity(), 0u);

    NolockRing nolockMove2(std::move(lockCopy1));
    EXPECT_EQ(nolockMove2, lock);
    EXPECT_EQ(nolockMove2.size(), count);
    EXPECT_EQ(nolockMove2.overlap(), areg::OverlapPolicy::Shift);
    EXPECT_TRUE(nolockMove2.is_full());
    EXPECT_TRUE(lockCopy1.is_empty());
    EXPECT_EQ(lockCopy1.size(), 0u);
    EXPECT_EQ(lockCopy1.capacity(), 0u);
}

/**
 * \brief   Test RingStack constructors with Ring Stack type 'resize on overlap'.
 *          "Resize on overlap" means resize the ring if it is full, so that 
 *          the start element is not changed and new element is inserted.
 **/
TEST(RingStackTest, test_constructors_resize)
{
    using LockRing = areg::ConcurrentRingStack<int>;
    using NolockRing = areg::RingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lock(count, areg::OverlapPolicy::Resize);
    NolockRing nolock(count, areg::OverlapPolicy::Resize);

    EXPECT_TRUE(lock.is_empty());
    EXPECT_TRUE(nolock.is_empty());
    EXPECT_FALSE(lock.is_full() || nolock.is_full());
    EXPECT_EQ(lock.capacity(), count);
    EXPECT_EQ(nolock.capacity(), count);

    int loop = static_cast<int>(count * 2u);
    for (int i = 0; i < loop; ++i)
    {
        EXPECT_FALSE(lock.is_full());
        EXPECT_EQ(lock.push(i), static_cast<uint32_t>(i + 1));
        EXPECT_TRUE(lock.contains(i));

        EXPECT_FALSE(nolock.is_full());
        EXPECT_EQ(nolock.push(i + static_cast<int>(count)), static_cast<uint32_t>(i + 1));
        EXPECT_TRUE(nolock.contains(i + static_cast<int>(count)));

        if (i >= static_cast<int>(count))
        {
            // no more space
            EXPECT_TRUE(lock.contains(i - static_cast<int>(count)));
            EXPECT_TRUE(nolock.contains(i));
        }
    }

    EXPECT_NE(lock, nolock);

    LockRing lockCopy1(lock);
    EXPECT_EQ(lockCopy1, lock);
    EXPECT_EQ(lockCopy1.size(), static_cast<uint32_t>(loop));
    EXPECT_EQ(lockCopy1.overlap(), areg::OverlapPolicy::Resize);
    EXPECT_FALSE(lockCopy1.is_full());

    LockRing lockCopy2(nolock);
    EXPECT_EQ(lockCopy2, nolock);
    EXPECT_EQ(lockCopy2.size(), static_cast<uint32_t>(loop));
    EXPECT_EQ(lockCopy2.overlap(), areg::OverlapPolicy::Resize);
    EXPECT_FALSE(lockCopy2.is_full());

    NolockRing nolockCopy1(nolock);
    EXPECT_EQ(nolockCopy1, nolock);
    EXPECT_EQ(nolockCopy1.size(), static_cast<uint32_t>(loop));
    EXPECT_EQ(nolockCopy1.overlap(), areg::OverlapPolicy::Resize);
    EXPECT_FALSE(nolockCopy1.is_full());

    NolockRing nolockCopy2(lock);
    EXPECT_EQ(nolockCopy2, lock);
    EXPECT_EQ(nolockCopy2.size(), static_cast<uint32_t>(loop));
    EXPECT_EQ(nolockCopy2.overlap(), areg::OverlapPolicy::Resize);
    EXPECT_FALSE(nolockCopy2.is_full());

    EXPECT_NE(lockCopy2, lock);
    EXPECT_NE(nolockCopy2, nolock);

    LockRing lockMove1(std::move(lockCopy1));
    EXPECT_EQ(lockMove1, lock);
    EXPECT_EQ(lockMove1.size(), static_cast<uint32_t>(loop));
    EXPECT_EQ(lockMove1.overlap(), areg::OverlapPolicy::Resize);
    EXPECT_TRUE(lockCopy1.is_empty());
    EXPECT_EQ(lockCopy1.size(), 0u);
    EXPECT_EQ(lockCopy1.capacity(), 0u);

    NolockRing nolockMove1(std::move(nolockCopy1));
    EXPECT_EQ(nolockMove1, nolock);
    EXPECT_EQ(nolockMove1.size(), static_cast<uint32_t>(loop));
    EXPECT_EQ(nolockMove1.overlap(), areg::OverlapPolicy::Resize);
    EXPECT_TRUE(nolockCopy1.is_empty());
    EXPECT_EQ(nolockCopy1.size(), 0u);
    EXPECT_EQ(nolockCopy1.capacity(), 0u);

    lockCopy1 = lock;
    EXPECT_EQ(lockCopy1, lockMove1);
    nolockCopy1 = nolock;
    EXPECT_EQ(nolockCopy1, nolockMove1);

    LockRing lockMove2(std::move(nolockCopy1));
    EXPECT_EQ(lockMove2, nolock);
    EXPECT_EQ(lockMove2.size(), static_cast<uint32_t>(loop));
    EXPECT_EQ(lockMove2.overlap(), areg::OverlapPolicy::Resize);
    EXPECT_TRUE(nolockCopy1.is_empty());
    EXPECT_EQ(nolockCopy1.size(), 0u);
    EXPECT_EQ(nolockCopy1.capacity(), 0u);

    NolockRing nolockMove2(std::move(lockCopy1));
    EXPECT_EQ(nolockMove2, lock);
    EXPECT_EQ(nolockMove2.size(), static_cast<uint32_t>(loop));
    EXPECT_EQ(nolockMove2.overlap(), areg::OverlapPolicy::Resize);
    EXPECT_TRUE(lockCopy1.is_empty());
    EXPECT_EQ(lockCopy1.size(), 0u);
    EXPECT_EQ(lockCopy1.capacity(), 0u);
}

/**
 * \brief   Test RingStack operators, access element by index, equality.
 **/
TEST(RingStackTest, test_operators_index)
{
    using LockRing = areg::ConcurrentRingStack<uint32_t>;
    using NolockRing = areg::RingStack<uint32_t>;
    constexpr uint32_t count{ 10 };

    LockRing lockStop(count, areg::OverlapPolicy::Stop), lockShift(count, areg::OverlapPolicy::Shift), lockResize(count, areg::OverlapPolicy::Resize);
    NolockRing nolockStop(count, areg::OverlapPolicy::Stop), nolockShift(count, areg::OverlapPolicy::Shift), nolockResize(count, areg::OverlapPolicy::Resize);

    EXPECT_TRUE((lockStop == lockShift) && (lockShift == lockResize) && (nolockStop == nolockShift) && (nolockShift == nolockResize));
    EXPECT_TRUE((lockStop == nolockStop) && (lockShift == nolockShift) && (lockResize == nolockResize));

    uint32_t loop = count * 2;
    for (uint32_t i = 0; i < loop; ++i)
    {
        if (i < count)
        {
            EXPECT_EQ(lockStop.push(i), i + 1u);
            EXPECT_EQ(lockStop[i], i);

            EXPECT_EQ(lockShift.push(i + count), i + 1u);
            EXPECT_EQ(lockShift[i], i + count);

            EXPECT_EQ(lockResize.push(i), i + 1u);
            EXPECT_EQ(lockResize[i], i);
            lockResize[i] = i + loop;
            EXPECT_EQ(lockResize[i], i + loop);

            EXPECT_EQ(nolockStop.push(i), i + 1u);
            EXPECT_EQ(nolockStop[i], i);

            EXPECT_EQ(nolockShift.push(i + count), i + 1u);
            EXPECT_EQ(nolockShift[i], i + count);

            EXPECT_EQ(nolockResize.push(i), i + 1u);
            EXPECT_EQ(nolockResize[i], i);
            nolockResize[i] = i + loop;
            EXPECT_EQ(nolockResize[i], i + loop);
        }
        else
        {
            EXPECT_EQ(lockStop.push(i), count);
            EXPECT_FALSE(lockStop.is_valid_index(i));
            EXPECT_EQ(lockStop[i - count], i - count);

            EXPECT_EQ(lockShift.push(i + count), count);
            EXPECT_FALSE(lockStop.is_valid_index(i));
            EXPECT_EQ(lockShift[lockShift.size() - 1], i + count);
            EXPECT_EQ(lockShift[0], i + 1);

            EXPECT_EQ(lockResize.push(i), i + 1);
            EXPECT_EQ(lockResize[i], i);
            lockResize[i] = i + loop;
            EXPECT_EQ(lockResize[i], i + loop);

            EXPECT_EQ(nolockStop.push(i), count);
            EXPECT_FALSE(nolockStop.is_valid_index(i));
            EXPECT_EQ(nolockStop[i - count], i - count);

            EXPECT_EQ(nolockShift.push(i + count), count);
            EXPECT_FALSE(nolockStop.is_valid_index(i));
            EXPECT_EQ(nolockShift[lockShift.size() - 1], i + count);
            EXPECT_EQ(nolockShift[0], i + 1);

            EXPECT_EQ(nolockResize.push(i), i + 1);
            EXPECT_EQ(nolockResize[i], i);
            nolockResize[i] = i + loop;
            EXPECT_EQ(nolockResize[i], i + loop);
        }
    }

    EXPECT_TRUE((lockStop == nolockStop) && (lockShift == nolockShift) && (lockResize == nolockResize));
    EXPECT_TRUE((lockStop != lockShift) && (lockStop != lockResize) && (lockShift != nolockResize));
    EXPECT_TRUE((nolockStop != nolockShift) && (nolockStop != nolockResize) && (nolockShift != lockResize));
}


/**
 * \brief   Test RingStack operators, access element by index, equality.
 **/
TEST(RingStackTest, test_operators_copy_move)
{
    using LockRing = areg::ConcurrentRingStack<uint32_t>;
    using NolockRing = areg::RingStack<uint32_t>;
    constexpr uint32_t count{ 10 };

    LockRing lockStop(count, areg::OverlapPolicy::Stop), lockShift(count, areg::OverlapPolicy::Shift), lockResize(count, areg::OverlapPolicy::Resize);
    NolockRing nolockStop(count, areg::OverlapPolicy::Stop), nolockShift(count, areg::OverlapPolicy::Shift), nolockResize(count, areg::OverlapPolicy::Resize);

    uint32_t loop = count * 2;
    for (uint32_t i = 0; i < loop; ++i)
    {
        lockStop.push(i);
        lockShift.push(i + count);

        nolockStop.push(i);
        nolockShift.push(i + count);
    }

    LockRing copyStop1(count, areg::OverlapPolicy::Stop), copyShift1(count, areg::OverlapPolicy::Shift), copyResize1(count, areg::OverlapPolicy::Resize);
    for (uint32_t i = 0; i < count; ++i)
    {
        copyStop1.push(i + count);
        copyShift1.push(i);
    }

    EXPECT_NE(copyStop1, lockStop);
    EXPECT_NE(copyShift1, nolockShift);

    copyStop1 = lockStop;
    EXPECT_EQ(copyStop1, lockStop);

    copyShift1 = nolockShift;
    EXPECT_EQ(copyShift1, nolockShift);

    lockResize = copyResize1;
    EXPECT_EQ(copyResize1, lockResize);

    LockRing moveStop1(count, areg::OverlapPolicy::Stop), moveShift1(count, areg::OverlapPolicy::Shift), moveResize1(count, areg::OverlapPolicy::Resize);
    for (uint32_t i = 0; i < count; ++i)
    {
        moveStop1.push(i + count);
        moveShift1.push(i);
    }

    EXPECT_NE(moveStop1, lockStop);
    moveStop1 = std::move(lockStop);
    EXPECT_EQ(moveStop1, copyStop1);

    EXPECT_NE(moveShift1, nolockShift);
    moveShift1 = std::move(nolockShift);
    EXPECT_EQ(moveShift1, copyShift1);

    lockResize = std::move(moveResize1);
    EXPECT_TRUE(lockResize.is_empty());
    EXPECT_EQ(moveResize1, nolockResize);
}

/**
 * \brief   Test push and pop methods for RingStack of Stop type.
 **/
TEST(RingStackTest, test_push_pop_stop)
{
    using LockRing = areg::ConcurrentRingStack<int>;
    using NolockRing = areg::RingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lockRing(count, areg::OverlapPolicy::Stop);
    NolockRing nolockRing(count, areg::OverlapPolicy::Stop);
    int loop = static_cast<int>(count) * 2;
    for (int i = 0; i < loop; ++i)
    {
        if (i < static_cast<int>(count))
        {
            EXPECT_EQ(lockRing.push(i), static_cast<uint32_t>(i + 1));
            EXPECT_EQ(nolockRing.push(i + static_cast<int>(count)), static_cast<uint32_t>(i + 1));
        }
        else
        {
            EXPECT_EQ(lockRing.push(i), count);
            EXPECT_EQ(nolockRing.push(i + static_cast<int>(count)), count);
        }
    }

    EXPECT_EQ(lockRing.size(), count);
    EXPECT_EQ(nolockRing.size(), count);

    int half = static_cast<int>(count) / 2;
    for (int i = 0; i < half; ++i)
    {
        int valLock = lockRing[0];
        int resLock = i;
        EXPECT_EQ(valLock, resLock);
        EXPECT_EQ(lockRing.pop(), resLock);

        int valNolock = nolockRing[0];
        int resNolock = static_cast<int>(count) + i;
        EXPECT_EQ(valNolock, resNolock);
        EXPECT_EQ(nolockRing.pop(), resNolock);
    }

    uint32_t remain = count - static_cast<uint32_t>(half);
    EXPECT_EQ(lockRing.size(), remain);
    EXPECT_EQ(nolockRing.size(), remain);

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        int valLock = -1 * (static_cast<int>(count) + i);
        int valNolock = -1 * i;
        if (i < half)
        {
            EXPECT_EQ(lockRing.push(valLock), remain + static_cast<uint32_t>(i + 1));
            EXPECT_EQ(nolockRing.push(valNolock), remain + static_cast<uint32_t>(i + 1));
        }
        else
        {
            EXPECT_EQ(lockRing.push(valLock), count);
            EXPECT_EQ(nolockRing.push(valNolock), count);
        }
    }

    for (int i = 0; i < static_cast<int>(remain); ++i)
    {
        int valLock = lockRing[0];
        int resLock = static_cast<int>(half) + i;
        EXPECT_EQ(valLock, resLock);
        EXPECT_EQ(lockRing.pop(), resLock);

        int valNolock = nolockRing[0];
        int resNolock = static_cast<int>(count) + half + i;
        EXPECT_EQ(valNolock, resNolock);
        EXPECT_EQ(nolockRing.pop(), resNolock);
    }

    for (int i = 0; i < static_cast<int>(half); ++i)
    {
        int valLock = lockRing[0];
        int resLock = -1 * (static_cast<int>(count) + i);
        EXPECT_EQ(valLock, resLock);
        EXPECT_EQ(lockRing.pop(), resLock);

        int valNolock = nolockRing[0];
        int resNolock = -1 * i;
        EXPECT_EQ(valNolock, resNolock);
        EXPECT_EQ(nolockRing.pop(), resNolock);
    }

    EXPECT_TRUE(lockRing.is_empty());
    EXPECT_TRUE(nolockRing.is_empty());
}

/**
 * \brief   Test push and pop methods for RingStack of Shift type.
 **/
TEST(RingStackTest, test_push_pop_shift)
{
    using LockRing = areg::ConcurrentRingStack<int>;
    using NolockRing = areg::RingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lockRing(count, areg::OverlapPolicy::Shift);
    NolockRing nolockRing(count, areg::OverlapPolicy::Shift);
    int loop = static_cast<int>(count) * 2;
    for (int i = 0; i < loop; ++i)
    {
        if (i < static_cast<int>(count))
        {
            EXPECT_EQ(lockRing.push(i), static_cast<uint32_t>(i + 1));
            EXPECT_EQ(nolockRing.push(i + static_cast<int>(count)), static_cast<uint32_t>(i + 1));
        }
        else
        {
            EXPECT_EQ(lockRing.push(i), count);
            EXPECT_EQ(nolockRing.push(i + static_cast<int>(count)), count);
        }
    }

    EXPECT_EQ(lockRing.size(), count);
    EXPECT_EQ(nolockRing.size(), count);

    int half = static_cast<int>(count) / 2;
    for (int i = 0; i < half; ++i)
    {
        int valLock = lockRing[0];
        int resLock = static_cast<int>(count) + i;
        EXPECT_EQ(valLock, resLock);
        EXPECT_EQ(lockRing.pop(), resLock);

        int valNolock = nolockRing[0];
        int resNolock = 2 * static_cast<int>(count) + i;
        EXPECT_EQ(valNolock, resNolock);
        EXPECT_EQ(nolockRing.pop(), resNolock);
    }

    uint32_t remain = count - static_cast<uint32_t>(half);
    EXPECT_EQ(lockRing.size(), remain);
    EXPECT_EQ(nolockRing.size(), remain);

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        int valLock = -1 * (static_cast<int>(count) + i);
        int valNolock = -1 * i;
        if (i < half)
        {
            EXPECT_EQ(lockRing.push(valLock), remain + static_cast<uint32_t>(i + 1));
            EXPECT_EQ(nolockRing.push(valNolock), remain + static_cast<uint32_t>(i + 1));
        }
        else
        {
            EXPECT_EQ(lockRing.push(valLock), count);
            EXPECT_EQ(nolockRing.push(valNolock), count);
        }
    }

    EXPECT_EQ(lockRing.size(), count);
    EXPECT_EQ(nolockRing.size(), count);

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        int valLock = lockRing[0];
        int resLock = -1 * (static_cast<int>(count) + i);
        EXPECT_EQ(valLock, resLock);
        EXPECT_EQ(lockRing.pop(), resLock);

        int valNolock = nolockRing[0];
        int resNolock = -1 * i;
        EXPECT_EQ(valNolock, resNolock);
        EXPECT_EQ(nolockRing.pop(), resNolock);
    }

    EXPECT_TRUE(lockRing.is_empty());
    EXPECT_TRUE(nolockRing.is_empty());
}

/**
 * \brief   Test push and pop methods of the RingStack Resize type.
 **/
TEST(RingStackTest, test_push_pop_resize)
{
    using LockRing = areg::ConcurrentRingStack<int>;
    using NolockRing = areg::RingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lockRing(count, areg::OverlapPolicy::Resize);
    NolockRing nolockRing(count, areg::OverlapPolicy::Resize);
    int loop = static_cast<int>(count) * 2;
    for (int i = 0; i < loop; ++i)
    {
        EXPECT_EQ(lockRing.push(i), static_cast<uint32_t>(i + 1));
        EXPECT_EQ(nolockRing.push(i + static_cast<int>(count)), static_cast<uint32_t>(i + 1));
    }

    EXPECT_EQ(lockRing.size(), static_cast<uint32_t>(loop));
    EXPECT_EQ(nolockRing.size(), static_cast<uint32_t>(loop));

    int half = static_cast<int>(count) / 2;
    for (int i = 0; i < half; ++i)
    {
        int valLock = lockRing[0];
        int resLock = i;
        EXPECT_EQ(valLock, resLock);
        EXPECT_EQ(lockRing.pop(), resLock);

        int valNolock = nolockRing[0];
        int resNolock = static_cast<int>(count) + i;
        EXPECT_EQ(valNolock, resNolock);
        EXPECT_EQ(nolockRing.pop(), resNolock);
    }

    uint32_t remain = static_cast<uint32_t>(loop - half);
    EXPECT_EQ(lockRing.size(), remain);
    EXPECT_EQ(nolockRing.size(), remain);

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        int valLock = -1 * (static_cast<int>(count) + i);
        int valNolock = -1 * i;
        EXPECT_EQ(lockRing.push(valLock), remain + static_cast<uint32_t>(i + 1));
        EXPECT_EQ(nolockRing.push(valNolock), remain + static_cast<uint32_t>(i + 1));
    }

    uint32_t size = remain + count;
    EXPECT_EQ(lockRing.size(), size);
    EXPECT_EQ(nolockRing.size(), size);

    for (int i = 0; i < static_cast<int>(remain); ++i)
    {
        int valLock = lockRing[0];
        int resLock = static_cast<int>(half) + i;
        EXPECT_EQ(valLock, resLock);
        EXPECT_EQ(lockRing.pop(), resLock);

        int valNolock = nolockRing[0];
        int resNolock = static_cast<int>(count) + half + i;
        EXPECT_EQ(valNolock, resNolock);
        EXPECT_EQ(nolockRing.pop(), resNolock);
    }

    EXPECT_EQ(lockRing.size(), count);
    EXPECT_EQ(nolockRing.size(), count);

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        int valLock = lockRing[0];
        int resLock = -1 * (static_cast<int>(count) + i);
        EXPECT_EQ(valLock, resLock);
        EXPECT_EQ(lockRing.pop(), resLock);

        int valNolock = nolockRing[0];
        int resNolock = -1 * i;
        EXPECT_EQ(valNolock, resNolock);
        EXPECT_EQ(nolockRing.pop(), resNolock);
    }

    EXPECT_TRUE(lockRing.is_empty());
    EXPECT_TRUE(nolockRing.is_empty());
}

/**
 * \brief   Test push and pop methods of the RingStack.
 **/
TEST(RingStackTest, test_clear_free_extra)
{
    using LockRing = areg::ConcurrentRingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lockStop(count, areg::OverlapPolicy::Stop), lockShift(count, areg::OverlapPolicy::Shift), lockResize(count, areg::OverlapPolicy::Resize);
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        lockShift.push(i);
        lockResize.push(i);
    }

    int half = static_cast<int>(count / 2u);
    for (int i = 0; i < half; ++i)
    {
        lockStop.push(i);
        lockResize.push(i);
    }

    uint32_t sizeStop = lockStop.size();
    uint32_t sizeShift = lockShift.size();
    uint32_t sizeResize = lockResize.size();

    lockStop.free_extra();
    EXPECT_EQ(lockStop.size(), sizeStop);
    EXPECT_EQ(lockStop.capacity(), sizeStop);
    EXPECT_TRUE(lockStop.is_full());
    lockStop.clear();
    EXPECT_TRUE(lockStop.is_empty());

    EXPECT_TRUE(lockShift.is_full());
    lockShift.free_extra();
    EXPECT_EQ(lockShift.size(), sizeShift);
    EXPECT_EQ(lockShift.capacity(), sizeShift);
    EXPECT_TRUE(lockShift.is_full());
    lockShift.release();
    EXPECT_TRUE(lockShift.is_empty());

    EXPECT_FALSE(lockResize.is_full());
    lockResize.free_extra();
    EXPECT_EQ(lockResize.size(), sizeResize);
    EXPECT_EQ(lockResize.capacity(), sizeResize);
    EXPECT_FALSE(lockResize.is_full());
    lockResize.clear();
    EXPECT_TRUE(lockResize.is_empty());
}

/**
 * \brief   Test add method of the RingStack.
 **/
TEST(RingStackTest, test_add)
{
    using LockRing = areg::ConcurrentRingStack<int>;
    using NolockRing = areg::RingStack<int>;
    constexpr uint32_t count{ 10 };

    NolockRing nolock(count, areg::OverlapPolicy::Resize);
    LockRing lockStop(count, areg::OverlapPolicy::Stop), lockShift(count, areg::OverlapPolicy::Shift), lockResize(count, areg::OverlapPolicy::Resize);
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        lockShift.push(i);
        lockResize.push(i);
        nolock.push(-1 * i);
    }

    int half = static_cast<int>(count / 2u);
    for (int i = 0; i < half; ++i)
    {
        lockStop.push(i);
        lockResize.push(i);
    }

    uint32_t added = count - static_cast<uint32_t>(half);
    EXPECT_EQ(lockStop.add(nolock), added);
    EXPECT_EQ(lockStop.size(), count);
    EXPECT_TRUE(lockStop.is_full());
    for (int i = 0; i < static_cast<int>(added); ++i)
    {
        int val = nolock[static_cast<uint32_t>(i)];
        EXPECT_EQ(val, -1 * i);
        EXPECT_EQ(lockStop[static_cast<uint32_t>(i + half)], val);
    }

    EXPECT_EQ(lockShift.add(nolock), 0u);
    EXPECT_EQ(lockShift.size(), count);
    EXPECT_TRUE(lockShift.is_full());
    EXPECT_EQ(lockShift, nolock);

    uint32_t size = lockResize.size();
    EXPECT_EQ(lockResize.add(nolock), nolock.size());
    EXPECT_EQ(lockResize.size(), size + nolock.size());
    for (int i = 0; i < static_cast<int>(nolock.size()); ++i)
    {
        int val = nolock[static_cast<uint32_t>(i)];
        EXPECT_EQ(val, -1 * i);
        EXPECT_EQ(lockResize[static_cast<uint32_t>(i) + size], val);
    }
}

/**
 * \brief   Test copy methods of the RingStack.
 **/
TEST(RingStackTest, test_copy)
{
    using LockRing = areg::ConcurrentRingStack<int>;
    using NolockRing = areg::RingStack<int>;
    constexpr uint32_t count{ 10 };

    NolockRing nolock(count, areg::OverlapPolicy::Resize);
    LockRing lockStop(count, areg::OverlapPolicy::Stop), lockShift(count, areg::OverlapPolicy::Shift), lockResize(count, areg::OverlapPolicy::Resize);
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        lockShift.push(i);
        lockResize.push(i);
        nolock.push(-1 * i);
    }

    int half = static_cast<int>(count / 2u);
    for (int i = 0; i < half; ++i)
    {
        lockStop.push(i);
        lockResize.push(i);
    }

    lockStop.copy(nolock);
    EXPECT_EQ(lockStop.size(), count);
    EXPECT_EQ(lockStop, nolock);
    EXPECT_TRUE(lockStop.is_full());

    lockShift.copy(nolock);
    EXPECT_EQ(lockShift.size(), count);
    EXPECT_EQ(lockShift, nolock);
    EXPECT_TRUE(lockShift.is_full());

    lockResize.copy(nolock);
    EXPECT_EQ(lockResize.size(), count);
    EXPECT_EQ(lockResize, nolock);
    EXPECT_FALSE(lockResize.is_full());

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        int val = nolock[static_cast<uint32_t>(i)];
        EXPECT_EQ(val, -1 * i);
        EXPECT_EQ(lockStop[static_cast<uint32_t>(i)], val);
        EXPECT_EQ(lockShift[static_cast<uint32_t>(i)], val);
        EXPECT_EQ(lockResize[static_cast<uint32_t>(i)], val);
    }
}

/**
 * \brief   Test move methods of the RingStack.
 **/
TEST(RingStackTest, test_move)
{
    using LockRing = areg::ConcurrentRingStack<int>;
    using NolockRing = areg::RingStack<int>;
    constexpr uint32_t count{ 10 };

    NolockRing nolock(count, areg::OverlapPolicy::Resize);
    LockRing lockStop(count, areg::OverlapPolicy::Stop), lockShift(count, areg::OverlapPolicy::Shift), lockResize(count, areg::OverlapPolicy::Resize);
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        lockStop.push(i);
        lockResize.push(i);
        nolock.push(-1 * i);
    }

    int half = static_cast<int>(count / 2u);
    for (int i = 0; i < half; ++i)
    {
        lockShift.push(i);
        lockResize.push(i + static_cast<int>(count));
    }

    nolock.move(std::move(lockStop));
    EXPECT_EQ(lockStop.size(), count);
    EXPECT_EQ(nolock.size(), count);
    EXPECT_NE(lockStop, nolock);
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_EQ(lockStop[static_cast<uint32_t>(i)], -1 * i);
        EXPECT_EQ(nolock[static_cast<uint32_t>(i)], i);
    }

    lockStop.move(std::move(lockShift));
    EXPECT_EQ(lockShift.size(), count);
    EXPECT_EQ(lockStop.size(), static_cast<uint32_t>(half));
    EXPECT_NE(lockShift, lockStop);
    EXPECT_TRUE(lockShift.is_full());
    EXPECT_FALSE(lockStop.is_full());
    EXPECT_EQ(lockStop.overlap(), areg::OverlapPolicy::Stop);
    EXPECT_EQ(lockShift.overlap(), areg::OverlapPolicy::Shift);
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_EQ(lockShift[static_cast<uint32_t>(i)], -1 * i);
        if (i < half)
        {
            EXPECT_EQ(lockStop[static_cast<uint32_t>(i)], i);
        }
    }

    lockResize.move(std::move(lockShift));
    EXPECT_EQ(lockResize.size(), count);
    EXPECT_EQ(lockShift.size(), count + static_cast<uint32_t>(half));
    EXPECT_NE(lockResize, nolock);
    EXPECT_FALSE(lockResize.is_full());
    EXPECT_EQ(lockResize.overlap(), areg::OverlapPolicy::Resize);
    EXPECT_EQ(lockShift.overlap(), areg::OverlapPolicy::Shift);
    for (int i = 0; i < static_cast<int>(count) + half; ++i)
    {
        EXPECT_EQ(lockShift[static_cast<uint32_t>(i)], i);
        if (i < static_cast<int>(count))
        {
            EXPECT_EQ(lockResize[static_cast<uint32_t>(i)], -1 * i);
        }
    }
}

/**
 * \brief   Test searching methods of the RingStack.
 **/
TEST(RingStackTest, test_searching)
{
    using LockRing = areg::ConcurrentRingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lockStop(count, areg::OverlapPolicy::Stop), lockShift(count, areg::OverlapPolicy::Shift), lockResize(count, areg::OverlapPolicy::Resize);
    for (int i = 0; i < static_cast<int>(2 * count); ++i)
    {
        lockStop.push(i);
        lockShift.push(i);
        lockResize.push(i);
    }

    for (int i = 0; i < static_cast<int>(2 * count); ++i)
    {
        if (i < static_cast<int>(count))
        {
            EXPECT_TRUE(lockStop.is_valid_index(static_cast<uint32_t>(i)));
            EXPECT_TRUE(lockShift.is_valid_index(static_cast<uint32_t>(i)));

            EXPECT_TRUE(lockStop.contains(i));
            uint32_t idx = lockStop.find(i);
            EXPECT_EQ(idx, static_cast<uint32_t>(i));
            EXPECT_TRUE(lockStop.is_valid_index(idx));

            EXPECT_FALSE(lockShift.contains(i));
            EXPECT_EQ(lockShift.find(i), static_cast<uint32_t>(areg::INVALID_INDEX));
        }
        else
        {
            EXPECT_FALSE(lockStop.is_valid_index(static_cast<uint32_t>(i)));
            EXPECT_FALSE(lockShift.is_valid_index(static_cast<uint32_t>(i)));

            EXPECT_FALSE(lockStop.contains(i));
            EXPECT_EQ(lockStop.find(i), static_cast<uint32_t>(areg::INVALID_INDEX));

            EXPECT_TRUE(lockShift.contains(i));
            uint32_t idx = lockShift.find(i);
            EXPECT_EQ(idx, static_cast<uint32_t>(i) - count);
            EXPECT_TRUE(lockShift.is_valid_index(idx));
        }

        EXPECT_TRUE(lockResize.is_valid_index(static_cast<uint32_t>(i)));
        EXPECT_TRUE(lockResize.contains(i));
        uint32_t idx = lockResize.find(i);
        EXPECT_EQ(idx, static_cast<uint32_t>(i));
        EXPECT_TRUE(lockResize.is_valid_index(idx));
    }
}

/**
 * \brief   Test streaming operators of the RingStack.
 **/
TEST(RingStackTest, test_streaming)
{
    using LockRing = areg::ConcurrentRingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lockStop(count, areg::OverlapPolicy::Stop), lockShift(0, areg::OverlapPolicy::Shift);
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_EQ(lockStop.push(i), static_cast<uint32_t>(i + 1));
        EXPECT_EQ(lockShift.push(i), 0u);
    }

    areg::SharedBuffer stream;
    stream << lockStop;
    EXPECT_FALSE(stream.is_empty());
    stream.move_to_begin();
    stream >> lockShift;

    EXPECT_EQ(lockStop, lockShift);
}
