/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        units/TEStackTest.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, AREG framework unit test file.
 *              Tests of TEMap object.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/TEStack.hpp"
#include "areg/base/SharedBuffer.hpp"

 /**
  * \brief   Test TEStack constructors.
  **/
TEST(TEStackTest, TestLockAndNolockStackConstructors)
{
    using NolockStack = TENolockStack<int>;
    using LockStack = TELockStack<int>;

    constexpr uint32_t count{ 10 };

    // Step 1: initialize 2 types of stacks -- lock and unlock
    NolockStack nolock;
    LockStack lock;

    EXPECT_TRUE(lock.isEmpty() && nolock.isEmpty());
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        nolock.pushFirst(i);
        lock.pushFirst(i + static_cast<int>(count));
    }
    EXPECT_FALSE(lock.isEmpty() && nolock.isEmpty());

    // Step 2:  Create NolockStack objects with copy constructors
    NolockStack nolockCopy1(nolock);
    NolockStack nolockCopy2(lock);
    EXPECT_FALSE(nolockCopy1.isEmpty() && nolockCopy2.isEmpty());
    EXPECT_EQ(nolockCopy1, nolock);
    EXPECT_EQ(nolockCopy2, lock);

    // Step 3: Create LockStack objects with copy constructors.
    LockStack lockCopy1(nolock);
    LockStack lockCopy2(lock);
    EXPECT_FALSE(lockCopy1.isEmpty() && lockCopy2.isEmpty());
    EXPECT_EQ(lockCopy1, nolock);
    EXPECT_EQ(lockCopy2, lock);

    // Step 4:  Create NolockStack objects with move constructors
    NolockStack nolockMove1(std::move(nolockCopy1));
    NolockStack nolockMove2(std::move(nolockCopy2));
    EXPECT_FALSE(nolockMove1.isEmpty() && nolockMove2.isEmpty());
    EXPECT_NE(nolockMove1, nolockCopy1);
    EXPECT_NE(nolockMove2, nolockCopy2);
    EXPECT_TRUE(nolockCopy1.isEmpty() && nolockCopy2.isEmpty());
    EXPECT_EQ(nolockMove1, nolock);
    EXPECT_EQ(nolockMove2, lock);

    // Step 5: Create LockStack objects with copy constructors.
    LockStack lockMove1(std::move(lockCopy1));
    LockStack lockMove2(std::move(lockCopy2));
    EXPECT_FALSE(lockMove1.isEmpty() && lockMove2.isEmpty());
    EXPECT_NE(lockMove1, lockCopy1);
    EXPECT_NE(lockMove2, lockCopy2);
    EXPECT_TRUE(lockCopy1.isEmpty() && lockCopy2.isEmpty());
    EXPECT_EQ(nolockMove1, nolock);
    EXPECT_EQ(nolockMove2, lock);
}
