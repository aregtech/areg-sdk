/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        units/TERingStackTest.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, AREG framework unit test file.
 *              Tests of TELinkedList object.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/TERingStack.hpp"
#include "areg/base/SharedBuffer.hpp"

/**
 * \brief   Test TERingStack constructors with Ring Stack type 'stop on overlap'.
 *          "Stop on overlap" means do not insert any entry if the ring is full.
 **/
TEST(TERingStackTest, TestConstructorsStopOnOverlap)
{
    using LockRing = TELockRingStack<int>;
    using NolockRing = TENolockRingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lock(count, NECommon::eRingOverlap::StopOnOverlap);
    NolockRing nolock(count, NECommon::eRingOverlap::StopOnOverlap);

    EXPECT_TRUE(lock.isEmpty());
    EXPECT_TRUE(nolock.isEmpty());
    EXPECT_FALSE(lock.isFull() || nolock.isFull());
    EXPECT_EQ(lock.capacity(), count);
    EXPECT_EQ(nolock.capacity(), count);

    int loop = static_cast<int>(count * 2u);
    for (int i = 0; i < loop; ++i)
    {
        if (i < static_cast<int>(count))
        {
            EXPECT_FALSE(lock.isFull());
            EXPECT_EQ(lock.push(i), static_cast<uint32_t>(i + 1));
            EXPECT_TRUE(lock.contains(i));

            EXPECT_FALSE(nolock.isFull());
            EXPECT_EQ(nolock.push(i + static_cast<int>(count)), static_cast<uint32_t>(i + 1));
            EXPECT_TRUE(nolock.contains(i + static_cast<int>(count)));
        }
        else
        {
            // no more space
            EXPECT_TRUE(lock.isFull());
            EXPECT_EQ(lock.push(i), count);
            EXPECT_FALSE(lock.contains(i));

            EXPECT_TRUE(nolock.isFull());
            EXPECT_EQ(nolock.push(i + static_cast<int>(count)), count);
            EXPECT_FALSE(nolock.contains(i + static_cast<int>(count)));
        }
    }

    EXPECT_TRUE(lock.isFull());
    EXPECT_TRUE(nolock.isFull());
    EXPECT_NE(lock, nolock);

    LockRing lockCopy1(lock);
    EXPECT_EQ(lockCopy1, lock);
    EXPECT_EQ(lockCopy1.getSize(), count);
    EXPECT_EQ(lockCopy1.getOverlap(), NECommon::eRingOverlap::StopOnOverlap);
    EXPECT_TRUE(lockCopy1.isFull());

    LockRing lockCopy2(nolock);
    EXPECT_EQ(lockCopy2, nolock);
    EXPECT_EQ(lockCopy2.getSize(), count);
    EXPECT_EQ(lockCopy2.getOverlap(), NECommon::eRingOverlap::StopOnOverlap);
    EXPECT_TRUE(lockCopy2.isFull());

    NolockRing nolockCopy1(nolock);
    EXPECT_EQ(nolockCopy1, nolock);
    EXPECT_EQ(nolockCopy1.getSize(), count);
    EXPECT_EQ(nolockCopy1.getOverlap(), NECommon::eRingOverlap::StopOnOverlap);
    EXPECT_TRUE(nolockCopy1.isFull());

    NolockRing nolockCopy2(lock);
    EXPECT_EQ(nolockCopy2, lock);
    EXPECT_EQ(nolockCopy2.getSize(), count);
    EXPECT_EQ(nolockCopy2.getOverlap(), NECommon::eRingOverlap::StopOnOverlap);
    EXPECT_TRUE(nolockCopy2.isFull());

    EXPECT_NE(lockCopy2, lock);
    EXPECT_NE(nolockCopy2, nolock);

    LockRing lockMove1(std::move(lockCopy1));
    EXPECT_EQ(lockMove1, lock);
    EXPECT_EQ(lockMove1.getSize(), count);
    EXPECT_EQ(lockMove1.getOverlap(), NECommon::eRingOverlap::StopOnOverlap);
    EXPECT_TRUE(lockMove1.isFull());
    EXPECT_TRUE(lockCopy1.isEmpty());
    EXPECT_EQ(lockCopy1.getSize(), 0u);
    EXPECT_EQ(lockCopy1.capacity(), 0u);

    NolockRing nolockMove1(std::move(nolockCopy1));
    EXPECT_EQ(nolockMove1, nolock);
    EXPECT_EQ(nolockMove1.getSize(), count);
    EXPECT_EQ(nolockMove1.getOverlap(), NECommon::eRingOverlap::StopOnOverlap);
    EXPECT_TRUE(nolockMove1.isFull());
    EXPECT_TRUE(nolockCopy1.isEmpty());
    EXPECT_EQ(nolockCopy1.getSize(), 0u);
    EXPECT_EQ(nolockCopy1.capacity(), 0u);

    lockCopy1 = lock;
    EXPECT_EQ(lockCopy1, lockMove1);
    nolockCopy1 = nolock;
    EXPECT_EQ(nolockCopy1, nolockMove1);

    LockRing lockMove2(std::move(nolockCopy1));
    EXPECT_EQ(lockMove2, nolock);
    EXPECT_EQ(lockMove2.getSize(), count);
    EXPECT_EQ(lockMove2.getOverlap(), NECommon::eRingOverlap::StopOnOverlap);
    EXPECT_TRUE(lockMove2.isFull());
    EXPECT_TRUE(nolockCopy1.isEmpty());
    EXPECT_EQ(nolockCopy1.getSize(), 0u);
    EXPECT_EQ(nolockCopy1.capacity(), 0u);

    NolockRing nolockMove2(std::move(lockCopy1));
    EXPECT_EQ(nolockMove2, lock);
    EXPECT_EQ(nolockMove2.getSize(), count);
    EXPECT_EQ(nolockMove2.getOverlap(), NECommon::eRingOverlap::StopOnOverlap);
    EXPECT_TRUE(nolockMove2.isFull());
    EXPECT_TRUE(lockCopy1.isEmpty());
    EXPECT_EQ(lockCopy1.getSize(), 0u);
    EXPECT_EQ(lockCopy1.capacity(), 0u);
}

/**
 * \brief   Test TERingStack constructors with Ring Stack type 'shift on overlap'.
 *          "Shift on overlap" means move the start position of the ring if it
 *          is full, but do not change the size of the ring.
 **/
TEST(TERingStackTest, TestConstructorsShiftOnOverlap)
{
    using LockRing = TELockRingStack<int>;
    using NolockRing = TENolockRingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lock(count, NECommon::eRingOverlap::ShiftOnOverlap);
    NolockRing nolock(count, NECommon::eRingOverlap::ShiftOnOverlap);

    EXPECT_TRUE(lock.isEmpty());
    EXPECT_TRUE(nolock.isEmpty());
    EXPECT_FALSE(lock.isFull() || nolock.isFull());
    EXPECT_EQ(lock.capacity(), count);
    EXPECT_EQ(nolock.capacity(), count);

    int loop = static_cast<int>(count * 2u);
    for (int i = 0; i < loop; ++i)
    {
        if (i < static_cast<int>(count))
        {
            EXPECT_FALSE(lock.isFull());
            EXPECT_EQ(lock.push(i), static_cast<uint32_t>(i + 1));
            EXPECT_TRUE(lock.contains(i));

            EXPECT_FALSE(nolock.isFull());
            EXPECT_EQ(nolock.push(i + static_cast<int>(count)), static_cast<uint32_t>(i + 1));
            EXPECT_TRUE(nolock.contains(i + static_cast<int>(count)));
        }
        else
        {
            // no more space
            EXPECT_TRUE(lock.isFull());
            EXPECT_EQ(lock.push(i), count);
            EXPECT_TRUE(lock.contains(i));
            EXPECT_FALSE(lock.contains(i - static_cast<int>(count)));

            EXPECT_TRUE(nolock.isFull());
            EXPECT_EQ(nolock.push(i + static_cast<int>(count)), count);
            EXPECT_TRUE(nolock.contains(i + static_cast<int>(count)));
            EXPECT_FALSE(nolock.contains(i));
        }
    }

    EXPECT_TRUE(lock.isFull());
    EXPECT_TRUE(nolock.isFull());
    EXPECT_NE(lock, nolock);

    LockRing lockCopy1(lock);
    EXPECT_EQ(lockCopy1, lock);
    EXPECT_EQ(lockCopy1.getSize(), count);
    EXPECT_EQ(lockCopy1.getOverlap(), NECommon::eRingOverlap::ShiftOnOverlap);
    EXPECT_TRUE(lockCopy1.isFull());

    LockRing lockCopy2(nolock);
    EXPECT_EQ(lockCopy2, nolock);
    EXPECT_EQ(lockCopy2.getSize(), count);
    EXPECT_EQ(lockCopy2.getOverlap(), NECommon::eRingOverlap::ShiftOnOverlap);
    EXPECT_TRUE(lockCopy2.isFull());

    NolockRing nolockCopy1(nolock);
    EXPECT_EQ(nolockCopy1, nolock);
    EXPECT_EQ(nolockCopy1.getSize(), count);
    EXPECT_EQ(nolockCopy1.getOverlap(), NECommon::eRingOverlap::ShiftOnOverlap);
    EXPECT_TRUE(nolockCopy1.isFull());

    NolockRing nolockCopy2(lock);
    EXPECT_EQ(nolockCopy2, lock);
    EXPECT_EQ(nolockCopy2.getSize(), count);
    EXPECT_EQ(nolockCopy2.getOverlap(), NECommon::eRingOverlap::ShiftOnOverlap);
    EXPECT_TRUE(nolockCopy2.isFull());

    EXPECT_NE(lockCopy2, lock);
    EXPECT_NE(nolockCopy2, nolock);

    LockRing lockMove1(std::move(lockCopy1));
    EXPECT_EQ(lockMove1, lock);
    EXPECT_EQ(lockMove1.getSize(), count);
    EXPECT_EQ(lockMove1.getOverlap(), NECommon::eRingOverlap::ShiftOnOverlap);
    EXPECT_TRUE(lockMove1.isFull());
    EXPECT_TRUE(lockCopy1.isEmpty());
    EXPECT_EQ(lockCopy1.getSize(), 0u);
    EXPECT_EQ(lockCopy1.capacity(), 0u);

    NolockRing nolockMove1(std::move(nolockCopy1));
    EXPECT_EQ(nolockMove1, nolock);
    EXPECT_EQ(nolockMove1.getSize(), count);
    EXPECT_EQ(nolockMove1.getOverlap(), NECommon::eRingOverlap::ShiftOnOverlap);
    EXPECT_TRUE(nolockMove1.isFull());
    EXPECT_TRUE(nolockCopy1.isEmpty());
    EXPECT_EQ(nolockCopy1.getSize(), 0u);
    EXPECT_EQ(nolockCopy1.capacity(), 0u);

    lockCopy1 = lock;
    EXPECT_EQ(lockCopy1, lockMove1);
    nolockCopy1 = nolock;
    EXPECT_EQ(nolockCopy1, nolockMove1);

    LockRing lockMove2(std::move(nolockCopy1));
    EXPECT_EQ(lockMove2, nolock);
    EXPECT_EQ(lockMove2.getSize(), count);
    EXPECT_EQ(lockMove2.getOverlap(), NECommon::eRingOverlap::ShiftOnOverlap);
    EXPECT_TRUE(lockMove2.isFull());
    EXPECT_TRUE(nolockCopy1.isEmpty());
    EXPECT_EQ(nolockCopy1.getSize(), 0u);
    EXPECT_EQ(nolockCopy1.capacity(), 0u);

    NolockRing nolockMove2(std::move(lockCopy1));
    EXPECT_EQ(nolockMove2, lock);
    EXPECT_EQ(nolockMove2.getSize(), count);
    EXPECT_EQ(nolockMove2.getOverlap(), NECommon::eRingOverlap::ShiftOnOverlap);
    EXPECT_TRUE(nolockMove2.isFull());
    EXPECT_TRUE(lockCopy1.isEmpty());
    EXPECT_EQ(lockCopy1.getSize(), 0u);
    EXPECT_EQ(lockCopy1.capacity(), 0u);
}

/**
 * \brief   Test TERingStack constructors with Ring Stack type 'resize on overlap'.
 *          "Resize on overlap" means resize the ring if it is full, so that 
 *          the start element is not changed and new element is inserted.
 **/
TEST(TERingStackTest, TestConstructorsResizeOnOverlap)
{
    using LockRing = TELockRingStack<int>;
    using NolockRing = TENolockRingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lock(count, NECommon::eRingOverlap::ResizeOnOverlap);
    NolockRing nolock(count, NECommon::eRingOverlap::ResizeOnOverlap);

    EXPECT_TRUE(lock.isEmpty());
    EXPECT_TRUE(nolock.isEmpty());
    EXPECT_FALSE(lock.isFull() || nolock.isFull());
    EXPECT_EQ(lock.capacity(), count);
    EXPECT_EQ(nolock.capacity(), count);

    int loop = static_cast<int>(count * 2u);
    for (int i = 0; i < loop; ++i)
    {
        EXPECT_FALSE(lock.isFull());
        EXPECT_EQ(lock.push(i), static_cast<uint32_t>(i + 1));
        EXPECT_TRUE(lock.contains(i));

        EXPECT_FALSE(nolock.isFull());
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
    EXPECT_EQ(lockCopy1.getSize(), static_cast<uint32_t>(loop));
    EXPECT_EQ(lockCopy1.getOverlap(), NECommon::eRingOverlap::ResizeOnOverlap);
    EXPECT_FALSE(lockCopy1.isFull());

    LockRing lockCopy2(nolock);
    EXPECT_EQ(lockCopy2, nolock);
    EXPECT_EQ(lockCopy2.getSize(), static_cast<uint32_t>(loop));
    EXPECT_EQ(lockCopy2.getOverlap(), NECommon::eRingOverlap::ResizeOnOverlap);
    EXPECT_FALSE(lockCopy2.isFull());

    NolockRing nolockCopy1(nolock);
    EXPECT_EQ(nolockCopy1, nolock);
    EXPECT_EQ(nolockCopy1.getSize(), static_cast<uint32_t>(loop));
    EXPECT_EQ(nolockCopy1.getOverlap(), NECommon::eRingOverlap::ResizeOnOverlap);
    EXPECT_FALSE(nolockCopy1.isFull());

    NolockRing nolockCopy2(lock);
    EXPECT_EQ(nolockCopy2, lock);
    EXPECT_EQ(nolockCopy2.getSize(), static_cast<uint32_t>(loop));
    EXPECT_EQ(nolockCopy2.getOverlap(), NECommon::eRingOverlap::ResizeOnOverlap);
    EXPECT_FALSE(nolockCopy2.isFull());

    EXPECT_NE(lockCopy2, lock);
    EXPECT_NE(nolockCopy2, nolock);

    LockRing lockMove1(std::move(lockCopy1));
    EXPECT_EQ(lockMove1, lock);
    EXPECT_EQ(lockMove1.getSize(), static_cast<uint32_t>(loop));
    EXPECT_EQ(lockMove1.getOverlap(), NECommon::eRingOverlap::ResizeOnOverlap);
    EXPECT_TRUE(lockCopy1.isEmpty());
    EXPECT_EQ(lockCopy1.getSize(), 0u);
    EXPECT_EQ(lockCopy1.capacity(), 0u);

    NolockRing nolockMove1(std::move(nolockCopy1));
    EXPECT_EQ(nolockMove1, nolock);
    EXPECT_EQ(nolockMove1.getSize(), static_cast<uint32_t>(loop));
    EXPECT_EQ(nolockMove1.getOverlap(), NECommon::eRingOverlap::ResizeOnOverlap);
    EXPECT_TRUE(nolockCopy1.isEmpty());
    EXPECT_EQ(nolockCopy1.getSize(), 0u);
    EXPECT_EQ(nolockCopy1.capacity(), 0u);

    lockCopy1 = lock;
    EXPECT_EQ(lockCopy1, lockMove1);
    nolockCopy1 = nolock;
    EXPECT_EQ(nolockCopy1, nolockMove1);

    LockRing lockMove2(std::move(nolockCopy1));
    EXPECT_EQ(lockMove2, nolock);
    EXPECT_EQ(lockMove2.getSize(), static_cast<uint32_t>(loop));
    EXPECT_EQ(lockMove2.getOverlap(), NECommon::eRingOverlap::ResizeOnOverlap);
    EXPECT_TRUE(nolockCopy1.isEmpty());
    EXPECT_EQ(nolockCopy1.getSize(), 0u);
    EXPECT_EQ(nolockCopy1.capacity(), 0u);

    NolockRing nolockMove2(std::move(lockCopy1));
    EXPECT_EQ(nolockMove2, lock);
    EXPECT_EQ(nolockMove2.getSize(), static_cast<uint32_t>(loop));
    EXPECT_EQ(nolockMove2.getOverlap(), NECommon::eRingOverlap::ResizeOnOverlap);
    EXPECT_TRUE(lockCopy1.isEmpty());
    EXPECT_EQ(lockCopy1.getSize(), 0u);
    EXPECT_EQ(lockCopy1.capacity(), 0u);
}

/**
 * \brief   Test TERingStack operators, access element by index, equality.
 **/
TEST(TERingStackTest, TestOperatorsIndex)
{
    using LockRing = TELockRingStack<uint32_t>;
    using NolockRing = TENolockRingStack<uint32_t>;
    constexpr uint32_t count{ 10 };

    LockRing lockStop(count, NECommon::eRingOverlap::StopOnOverlap), lockShift(count, NECommon::eRingOverlap::ShiftOnOverlap), lockResize(count, NECommon::eRingOverlap::ResizeOnOverlap);
    NolockRing nolockStop(count, NECommon::eRingOverlap::StopOnOverlap), nolockShift(count, NECommon::eRingOverlap::ShiftOnOverlap), nolockResize(count, NECommon::eRingOverlap::ResizeOnOverlap);

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
            EXPECT_FALSE(lockStop.isValidIndex(i));
            EXPECT_EQ(lockStop[i - count], i - count);

            EXPECT_EQ(lockShift.push(i + count), count);
            EXPECT_FALSE(lockStop.isValidIndex(i));
            EXPECT_EQ(lockShift[lockShift.getSize() - 1], i + count);
            EXPECT_EQ(lockShift[0], i + 1);

            EXPECT_EQ(lockResize.push(i), i + 1);
            EXPECT_EQ(lockResize[i], i);
            lockResize[i] = i + loop;
            EXPECT_EQ(lockResize[i], i + loop);

            EXPECT_EQ(nolockStop.push(i), count);
            EXPECT_FALSE(nolockStop.isValidIndex(i));
            EXPECT_EQ(nolockStop[i - count], i - count);

            EXPECT_EQ(nolockShift.push(i + count), count);
            EXPECT_FALSE(nolockStop.isValidIndex(i));
            EXPECT_EQ(nolockShift[lockShift.getSize() - 1], i + count);
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
 * \brief   Test TERingStack operators, access element by index, equality.
 **/
TEST(TERingStackTest, TestOperatorsCopyMove)
{
    using LockRing = TELockRingStack<uint32_t>;
    using NolockRing = TENolockRingStack<uint32_t>;
    constexpr uint32_t count{ 10 };

    LockRing lockStop(count, NECommon::eRingOverlap::StopOnOverlap), lockShift(count, NECommon::eRingOverlap::ShiftOnOverlap), lockResize(count, NECommon::eRingOverlap::ResizeOnOverlap);
    NolockRing nolockStop(count, NECommon::eRingOverlap::StopOnOverlap), nolockShift(count, NECommon::eRingOverlap::ShiftOnOverlap), nolockResize(count, NECommon::eRingOverlap::ResizeOnOverlap);

    uint32_t loop = count * 2;
    for (uint32_t i = 0; i < loop; ++i)
    {
        lockStop.push(i);
        lockShift.push(i + count);

        nolockStop.push(i);
        nolockShift.push(i + count);
    }

    LockRing copyStop1(count, NECommon::eRingOverlap::StopOnOverlap), copyShift1(count, NECommon::eRingOverlap::ShiftOnOverlap), copyResize1(count, NECommon::eRingOverlap::ResizeOnOverlap);
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

    LockRing moveStop1(count, NECommon::eRingOverlap::StopOnOverlap), moveShift1(count, NECommon::eRingOverlap::ShiftOnOverlap), moveResize1(count, NECommon::eRingOverlap::ResizeOnOverlap);
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
    EXPECT_TRUE(lockResize.isEmpty());
    EXPECT_EQ(moveResize1, nolockResize);
}

/**
 * \brief   Test push and pop methods for TERingStack of StopOnOverlap type.
 **/
TEST(TERingStackTest, TestPushPopStopOnOverlap)
{
    using LockRing = TELockRingStack<int>;
    using NolockRing = TENolockRingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lockRing(count, NECommon::eRingOverlap::StopOnOverlap);
    NolockRing nolockRing(count, NECommon::eRingOverlap::StopOnOverlap);
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

    EXPECT_EQ(lockRing.getSize(), count);
    EXPECT_EQ(nolockRing.getSize(), count);

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
    EXPECT_EQ(lockRing.getSize(), remain);
    EXPECT_EQ(nolockRing.getSize(), remain);

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

    EXPECT_TRUE(lockRing.isEmpty());
    EXPECT_TRUE(nolockRing.isEmpty());
}

/**
 * \brief   Test push and pop methods for TERingStack of ShiftOnOverlap type.
 **/
TEST(TERingStackTest, TestPushPopShiftOnOverlap)
{
    using LockRing = TELockRingStack<int>;
    using NolockRing = TENolockRingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lockRing(count, NECommon::eRingOverlap::ShiftOnOverlap);
    NolockRing nolockRing(count, NECommon::eRingOverlap::ShiftOnOverlap);
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

    EXPECT_EQ(lockRing.getSize(), count);
    EXPECT_EQ(nolockRing.getSize(), count);

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
    EXPECT_EQ(lockRing.getSize(), remain);
    EXPECT_EQ(nolockRing.getSize(), remain);

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

    EXPECT_EQ(lockRing.getSize(), count);
    EXPECT_EQ(nolockRing.getSize(), count);

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

    EXPECT_TRUE(lockRing.isEmpty());
    EXPECT_TRUE(nolockRing.isEmpty());
}

/**
 * \brief   Test push and pop methods of the TERingStack ResizeOnOverlap type.
 **/
TEST(TERingStackTest, TestPushPopResizeOnOverlap)
{
    using LockRing = TELockRingStack<int>;
    using NolockRing = TENolockRingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lockRing(count, NECommon::eRingOverlap::ResizeOnOverlap);
    NolockRing nolockRing(count, NECommon::eRingOverlap::ResizeOnOverlap);
    int loop = static_cast<int>(count) * 2;
    for (int i = 0; i < loop; ++i)
    {
        EXPECT_EQ(lockRing.push(i), static_cast<uint32_t>(i + 1));
        EXPECT_EQ(nolockRing.push(i + static_cast<int>(count)), static_cast<uint32_t>(i + 1));
    }

    EXPECT_EQ(lockRing.getSize(), static_cast<uint32_t>(loop));
    EXPECT_EQ(nolockRing.getSize(), static_cast<uint32_t>(loop));

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
    EXPECT_EQ(lockRing.getSize(), remain);
    EXPECT_EQ(nolockRing.getSize(), remain);

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        int valLock = -1 * (static_cast<int>(count) + i);
        int valNolock = -1 * i;
        EXPECT_EQ(lockRing.push(valLock), remain + static_cast<uint32_t>(i + 1));
        EXPECT_EQ(nolockRing.push(valNolock), remain + static_cast<uint32_t>(i + 1));
    }

    uint32_t size = remain + count;
    EXPECT_EQ(lockRing.getSize(), size);
    EXPECT_EQ(nolockRing.getSize(), size);

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

    EXPECT_EQ(lockRing.getSize(), count);
    EXPECT_EQ(nolockRing.getSize(), count);

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

    EXPECT_TRUE(lockRing.isEmpty());
    EXPECT_TRUE(nolockRing.isEmpty());
}

/**
 * \brief   Test push and pop methods of the TERingStack.
 **/
TEST(TERingStackTest, TestClearFreeExtra)
{
    using LockRing = TELockRingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lockStop(count, NECommon::eRingOverlap::StopOnOverlap), lockShift(count, NECommon::eRingOverlap::ShiftOnOverlap), lockResize(count, NECommon::eRingOverlap::ResizeOnOverlap);
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

    uint32_t sizeStop = lockStop.getSize();
    uint32_t sizeShift = lockShift.getSize();
    uint32_t sizeResize = lockResize.getSize();

    lockStop.freeExtra();
    EXPECT_EQ(lockStop.getSize(), sizeStop);
    EXPECT_EQ(lockStop.capacity(), sizeStop);
    EXPECT_TRUE(lockStop.isFull());
    lockStop.clear();
    EXPECT_TRUE(lockStop.isEmpty());

    EXPECT_TRUE(lockShift.isFull());
    lockShift.freeExtra();
    EXPECT_EQ(lockShift.getSize(), sizeShift);
    EXPECT_EQ(lockShift.capacity(), sizeShift);
    EXPECT_TRUE(lockShift.isFull());
    lockShift.release();
    EXPECT_TRUE(lockShift.isEmpty());

    EXPECT_FALSE(lockResize.isFull());
    lockResize.freeExtra();
    EXPECT_EQ(lockResize.getSize(), sizeResize);
    EXPECT_EQ(lockResize.capacity(), sizeResize);
    EXPECT_FALSE(lockResize.isFull());
    lockResize.clear();
    EXPECT_TRUE(lockResize.isEmpty());
}

/**
 * \brief   Test add method of the TERingStack.
 **/
TEST(TERingStackTest, TestAdd)
{
    using LockRing = TELockRingStack<int>;
    using NolockRing = TENolockRingStack<int>;
    constexpr uint32_t count{ 10 };

    NolockRing nolock(count, NECommon::eRingOverlap::ResizeOnOverlap);
    LockRing lockStop(count, NECommon::eRingOverlap::StopOnOverlap), lockShift(count, NECommon::eRingOverlap::ShiftOnOverlap), lockResize(count, NECommon::eRingOverlap::ResizeOnOverlap);
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
    EXPECT_EQ(lockStop.getSize(), count);
    EXPECT_TRUE(lockStop.isFull());
    for (int i = 0; i < static_cast<int>(added); ++i)
    {
        int val = nolock[static_cast<uint32_t>(i)];
        EXPECT_EQ(val, -1 * i);
        EXPECT_EQ(lockStop[static_cast<uint32_t>(i + half)], val);
    }

    EXPECT_EQ(lockShift.add(nolock), 0u);
    EXPECT_EQ(lockShift.getSize(), count);
    EXPECT_TRUE(lockShift.isFull());
    EXPECT_EQ(lockShift, nolock);

    uint32_t size = lockResize.getSize();
    EXPECT_EQ(lockResize.add(nolock), nolock.getSize());
    EXPECT_EQ(lockResize.getSize(), size + nolock.getSize());
    for (int i = 0; i < static_cast<int>(nolock.getSize()); ++i)
    {
        int val = nolock[static_cast<uint32_t>(i)];
        EXPECT_EQ(val, -1 * i);
        EXPECT_EQ(lockResize[static_cast<uint32_t>(i) + size], val);
    }
}

/**
 * \brief   Test copy methods of the TERingStack.
 **/
TEST(TERingStackTest, TestCopy)
{
    using LockRing = TELockRingStack<int>;
    using NolockRing = TENolockRingStack<int>;
    constexpr uint32_t count{ 10 };

    NolockRing nolock(count, NECommon::eRingOverlap::ResizeOnOverlap);
    LockRing lockStop(count, NECommon::eRingOverlap::StopOnOverlap), lockShift(count, NECommon::eRingOverlap::ShiftOnOverlap), lockResize(count, NECommon::eRingOverlap::ResizeOnOverlap);
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
    EXPECT_EQ(lockStop.getSize(), count);
    EXPECT_EQ(lockStop, nolock);
    EXPECT_TRUE(lockStop.isFull());

    lockShift.copy(nolock);
    EXPECT_EQ(lockShift.getSize(), count);
    EXPECT_EQ(lockShift, nolock);
    EXPECT_TRUE(lockShift.isFull());

    lockResize.copy(nolock);
    EXPECT_EQ(lockResize.getSize(), count);
    EXPECT_EQ(lockResize, nolock);
    EXPECT_FALSE(lockResize.isFull());

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
 * \brief   Test move methods of the TERingStack.
 **/
TEST(TERingStackTest, TestMove)
{
    using LockRing = TELockRingStack<int>;
    using NolockRing = TENolockRingStack<int>;
    constexpr uint32_t count{ 10 };

    NolockRing nolock(count, NECommon::eRingOverlap::ResizeOnOverlap);
    LockRing lockStop(count, NECommon::eRingOverlap::StopOnOverlap), lockShift(count, NECommon::eRingOverlap::ShiftOnOverlap), lockResize(count, NECommon::eRingOverlap::ResizeOnOverlap);
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
    EXPECT_EQ(lockStop.getSize(), count);
    EXPECT_EQ(nolock.getSize(), count);
    EXPECT_NE(lockStop, nolock);
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_EQ(lockStop[static_cast<uint32_t>(i)], -1 * i);
        EXPECT_EQ(nolock[static_cast<uint32_t>(i)], i);
    }

    lockStop.move(std::move(lockShift));
    EXPECT_EQ(lockShift.getSize(), count);
    EXPECT_EQ(lockStop.getSize(), static_cast<uint32_t>(half));
    EXPECT_NE(lockShift, lockStop);
    EXPECT_TRUE(lockShift.isFull());
    EXPECT_FALSE(lockStop.isFull());
    EXPECT_EQ(lockStop.getOverlap(), NECommon::eRingOverlap::StopOnOverlap);
    EXPECT_EQ(lockShift.getOverlap(), NECommon::eRingOverlap::ShiftOnOverlap);
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_EQ(lockShift[static_cast<uint32_t>(i)], -1 * i);
        if (i < half)
        {
            EXPECT_EQ(lockStop[static_cast<uint32_t>(i)], i);
        }
    }

    lockResize.move(std::move(lockShift));
    EXPECT_EQ(lockResize.getSize(), count);
    EXPECT_EQ(lockShift.getSize(), count + static_cast<uint32_t>(half));
    EXPECT_NE(lockResize, nolock);
    EXPECT_FALSE(lockResize.isFull());
    EXPECT_EQ(lockResize.getOverlap(), NECommon::eRingOverlap::ResizeOnOverlap);
    EXPECT_EQ(lockShift.getOverlap(), NECommon::eRingOverlap::ShiftOnOverlap);
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
 * \brief   Test searching methods of the TERingStack.
 **/
TEST(TERingStackTest, TestSearching)
{
    using LockRing = TELockRingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lockStop(count, NECommon::eRingOverlap::StopOnOverlap), lockShift(count, NECommon::eRingOverlap::ShiftOnOverlap), lockResize(count, NECommon::eRingOverlap::ResizeOnOverlap);
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
            EXPECT_TRUE(lockStop.isValidIndex(static_cast<uint32_t>(i)));
            EXPECT_TRUE(lockShift.isValidIndex(static_cast<uint32_t>(i)));

            EXPECT_TRUE(lockStop.contains(i));
            uint32_t idx = lockStop.find(i);
            EXPECT_EQ(idx, static_cast<uint32_t>(i));
            EXPECT_TRUE(lockStop.isValidIndex(idx));

            EXPECT_FALSE(lockShift.contains(i));
            EXPECT_EQ(lockShift.find(i), static_cast<uint32_t>(NECommon::INVALID_INDEX));
        }
        else
        {
            EXPECT_FALSE(lockStop.isValidIndex(static_cast<uint32_t>(i)));
            EXPECT_FALSE(lockShift.isValidIndex(static_cast<uint32_t>(i)));

            EXPECT_FALSE(lockStop.contains(i));
            EXPECT_EQ(lockStop.find(i), static_cast<uint32_t>(NECommon::INVALID_INDEX));

            EXPECT_TRUE(lockShift.contains(i));
            uint32_t idx = lockShift.find(i);
            EXPECT_EQ(idx, static_cast<uint32_t>(i) - count);
            EXPECT_TRUE(lockShift.isValidIndex(idx));
        }

        EXPECT_TRUE(lockResize.isValidIndex(static_cast<uint32_t>(i)));
        EXPECT_TRUE(lockResize.contains(i));
        uint32_t idx = lockResize.find(i);
        EXPECT_EQ(idx, static_cast<uint32_t>(i));
        EXPECT_TRUE(lockResize.isValidIndex(idx));
    }
}

/**
 * \brief   Test streaming operators of the TERingStack.
 **/
TEST(TERingStackTest, TestStreaming)
{
    using LockRing = TELockRingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lockStop(count, NECommon::eRingOverlap::StopOnOverlap), lockShift(0, NECommon::eRingOverlap::ShiftOnOverlap);
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_EQ(lockStop.push(i), static_cast<uint32_t>(i + 1));
        EXPECT_EQ(lockShift.push(i), 0u);
    }

    SharedBuffer stream;
    stream << lockStop;
    EXPECT_FALSE(stream.isEmpty());
    stream.moveToBegin();
    stream >> lockShift;

    EXPECT_EQ(lockStop, lockShift);
}
