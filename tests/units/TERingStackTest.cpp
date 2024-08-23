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
  * \brief   Test TERingStack constructors.
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

    for (int i = 0; i < static_cast<int>(count * 2u); ++i)
    {
        if (i < static_cast<int>(count))
        {
            EXPECT_FALSE(lock.isFull());
            EXPECT_FALSE(nolock.isFull());
            EXPECT_EQ(lock.pushFirst(i), static_cast<uint32_t>(i + 1));
            EXPECT_EQ(nolock.pushLast(i), static_cast<uint32_t>(i + 1));
        }
        else
        {
            // no more space
            EXPECT_TRUE(lock.isFull());
            EXPECT_TRUE(nolock.isFull());
            EXPECT_EQ(lock.pushFirst(i), count);
            EXPECT_EQ(nolock.pushLast(i), count);
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
