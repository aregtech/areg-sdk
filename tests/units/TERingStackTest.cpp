/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
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
TEST(TERingStackTest, TestConstructorsResizeOnOvelap)
{
    using LockRing = TELockRingStack<int>;
    using NolockRing = TENolockRingStack<int>;
    constexpr uint32_t count{ 10 };

    LockRing lock(count, NECommon::eRingOverlap::ResizeOnOvelap);
    NolockRing nolock(count, NECommon::eRingOverlap::ResizeOnOvelap);

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
    EXPECT_EQ(lockCopy1.getOverlap(), NECommon::eRingOverlap::ResizeOnOvelap);
    EXPECT_FALSE(lockCopy1.isFull());

    LockRing lockCopy2(nolock);
    EXPECT_EQ(lockCopy2, nolock);
    EXPECT_EQ(lockCopy2.getSize(), static_cast<uint32_t>(loop));
    EXPECT_EQ(lockCopy2.getOverlap(), NECommon::eRingOverlap::ResizeOnOvelap);
    EXPECT_FALSE(lockCopy2.isFull());

    NolockRing nolockCopy1(nolock);
    EXPECT_EQ(nolockCopy1, nolock);
    EXPECT_EQ(nolockCopy1.getSize(), static_cast<uint32_t>(loop));
    EXPECT_EQ(nolockCopy1.getOverlap(), NECommon::eRingOverlap::ResizeOnOvelap);
    EXPECT_FALSE(nolockCopy1.isFull());

    NolockRing nolockCopy2(lock);
    EXPECT_EQ(nolockCopy2, lock);
    EXPECT_EQ(nolockCopy2.getSize(), static_cast<uint32_t>(loop));
    EXPECT_EQ(nolockCopy2.getOverlap(), NECommon::eRingOverlap::ResizeOnOvelap);
    EXPECT_FALSE(nolockCopy2.isFull());

    EXPECT_NE(lockCopy2, lock);
    EXPECT_NE(nolockCopy2, nolock);

    LockRing lockMove1(std::move(lockCopy1));
    EXPECT_EQ(lockMove1, lock);
    EXPECT_EQ(lockMove1.getSize(), static_cast<uint32_t>(loop));
    EXPECT_EQ(lockMove1.getOverlap(), NECommon::eRingOverlap::ResizeOnOvelap);
    EXPECT_TRUE(lockCopy1.isEmpty());
    EXPECT_EQ(lockCopy1.getSize(), 0u);
    EXPECT_EQ(lockCopy1.capacity(), 0u);

    NolockRing nolockMove1(std::move(nolockCopy1));
    EXPECT_EQ(nolockMove1, nolock);
    EXPECT_EQ(nolockMove1.getSize(), static_cast<uint32_t>(loop));
    EXPECT_EQ(nolockMove1.getOverlap(), NECommon::eRingOverlap::ResizeOnOvelap);
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
    EXPECT_EQ(lockMove2.getOverlap(), NECommon::eRingOverlap::ResizeOnOvelap);
    EXPECT_TRUE(nolockCopy1.isEmpty());
    EXPECT_EQ(nolockCopy1.getSize(), 0u);
    EXPECT_EQ(nolockCopy1.capacity(), 0u);

    NolockRing nolockMove2(std::move(lockCopy1));
    EXPECT_EQ(nolockMove2, lock);
    EXPECT_EQ(nolockMove2.getSize(), static_cast<uint32_t>(loop));
    EXPECT_EQ(nolockMove2.getOverlap(), NECommon::eRingOverlap::ResizeOnOvelap);
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

    LockRing lockStop(count, NECommon::eRingOverlap::StopOnOverlap), lockShift(count, NECommon::eRingOverlap::ShiftOnOverlap), lockResize(count, NECommon::eRingOverlap::ResizeOnOvelap);
    NolockRing nolockStop(count, NECommon::eRingOverlap::StopOnOverlap), nolockShift(count, NECommon::eRingOverlap::ShiftOnOverlap), nolockResize(count, NECommon::eRingOverlap::ResizeOnOvelap);

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

    LockRing lockStop(count, NECommon::eRingOverlap::StopOnOverlap), lockShift(count, NECommon::eRingOverlap::ShiftOnOverlap), lockResize(count, NECommon::eRingOverlap::ResizeOnOvelap);
    NolockRing nolockStop(count, NECommon::eRingOverlap::StopOnOverlap), nolockShift(count, NECommon::eRingOverlap::ShiftOnOverlap), nolockResize(count, NECommon::eRingOverlap::ResizeOnOvelap);

    uint32_t loop = count * 2;
    for (uint32_t i = 0; i < loop; ++i)
    {
        lockStop.push(i);
        lockShift.push(i + count);

        nolockStop.push(i);
        nolockShift.push(i + count);
    }

    LockRing copyStop1(count, NECommon::eRingOverlap::StopOnOverlap), copyShift1(count, NECommon::eRingOverlap::ShiftOnOverlap), copyResize1(count, NECommon::eRingOverlap::ResizeOnOvelap);
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

    LockRing moveStop1(count, NECommon::eRingOverlap::StopOnOverlap), moveShift1(count, NECommon::eRingOverlap::ShiftOnOverlap), moveResize1(count, NECommon::eRingOverlap::ResizeOnOvelap);
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