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

/**
 * \brief   Test TEStack constructors.
 **/
TEST(TEStackTest, TestLockAndNolockStackOperators)
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

    // Step 2:  Create NolockStack objects and use assigning operator to copy from source.
    NolockStack nolockCopy1;
    NolockStack nolockCopy2;
    EXPECT_TRUE(nolockCopy1.isEmpty() && nolockCopy2.isEmpty());
    nolockCopy1 = nolock;
    nolockCopy2 = lock;
    EXPECT_FALSE(nolockCopy1.isEmpty() && nolockCopy2.isEmpty());
    EXPECT_TRUE(nolockCopy1 == nolock);
    EXPECT_TRUE(nolockCopy2 == lock);

    // Step 3: Create LockStack objects and use assigning operator to copy from source.
    LockStack lockCopy1;
    LockStack lockCopy2;
    EXPECT_TRUE(lockCopy1.isEmpty() && lockCopy2.isEmpty());
    lockCopy1 = nolock;
    lockCopy2 = lock;
    EXPECT_FALSE(lockCopy1.isEmpty() && lockCopy2.isEmpty());
    EXPECT_TRUE(lockCopy1 == nolock);
    EXPECT_TRUE(lockCopy2 == lock);

    // Step 4:  Create NolockStack objects and use assigning operator to move the source.
    NolockStack nolockMove1;
    NolockStack nolockMove2;
    EXPECT_TRUE(nolockMove1.isEmpty() && nolockMove2.isEmpty());
    nolockMove1 = std::move(nolockCopy1);
    nolockMove2 = std::move(nolockCopy2);
    EXPECT_FALSE(nolockMove1.isEmpty() && nolockMove2.isEmpty());
    EXPECT_TRUE(nolockMove1 != nolockCopy1);
    EXPECT_TRUE(nolockMove2 != nolockCopy2);
    EXPECT_TRUE(nolockCopy1.isEmpty() && nolockCopy2.isEmpty());
    EXPECT_TRUE(nolockMove1 == nolock);
    EXPECT_TRUE(nolockMove2 == lock);

    // Step 5: Create LockStack objects and use assigning operator to move the source.
    LockStack lockMove1;
    LockStack lockMove2;
    EXPECT_TRUE(lockMove1.isEmpty() && lockMove2.isEmpty());
    lockMove1 = std::move(lockCopy1);
    lockMove2 = std::move(lockCopy2);
    EXPECT_FALSE(lockMove1.isEmpty() && lockMove2.isEmpty());
    EXPECT_TRUE(lockMove1 != lockCopy1);
    EXPECT_TRUE(lockMove2 != lockCopy2);
    EXPECT_TRUE(lockCopy1.isEmpty() && lockCopy2.isEmpty());
    EXPECT_TRUE(nolockMove1 == nolock);
    EXPECT_TRUE(nolockMove2 == lock);
}

/**
 * \brief   Test TEStack positioning.
 **/
TEST(TEStackTest, TestLockAndNolockStackPositioning)
{
    using Stack = TEStack<int>;
    using NolockStack = TENolockStack<int>;
    using LockStack = TELockStack<int>;
    using POS = Stack::STACKPOS;

    constexpr uint32_t count{ 10 };

    // Step 1: initialize 2 types of stacks -- lock and unlock
    NolockStack nolock;
    LockStack lock;
    EXPECT_TRUE(nolock.isEmpty() && lock.isEmpty());

    // Step 2: resize stacks, set new size equal to `count`, make sure they are not empty.
    nolock.resize(count);
    lock.resize(count);
    EXPECT_FALSE(nolock.isEmpty() && lock.isEmpty());
    EXPECT_EQ(nolock.getSize(), count);
    EXPECT_EQ(lock.getSize(), count);

    // Step 3: get the first position of the stacks and make sure that they are valid.
    POS posNolock = nolock.firstPosition();
    POS posLock = lock.firstPosition();
    EXPECT_FALSE(nolock.isInvalidPosition(posNolock));
    EXPECT_FALSE(lock.isInvalidPosition(posLock));
    EXPECT_TRUE(nolock.isFirstPosition(posNolock));
    EXPECT_TRUE(lock.isFirstPosition(posLock));

    // Step 4: in the loop check each position of the stacks, check validity and check access via positioning.
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_TRUE(nolock.isValidPosition(posNolock));
        EXPECT_TRUE(lock.isValidPosition(posLock));

        EXPECT_TRUE(nolock.checkPosition(posNolock));
        EXPECT_TRUE(lock.checkPosition(posLock));

        nolock.getAt(posNolock) = i;
        lock[posLock] = nolock[posNolock];
        EXPECT_EQ(lock.getAt(posLock), i);

        if (i == static_cast<int>(count - 1u))
        {
            EXPECT_TRUE(nolock.isLastPosition(posNolock));
            EXPECT_TRUE(lock.isLastPosition(posLock));
        }

        posNolock = nolock.nextPosition(posNolock);
        posLock = lock.nextPosition(posLock);

        EXPECT_FALSE(nolock.isFirstPosition(posNolock));
        EXPECT_FALSE(lock.isFirstPosition(posLock));
    }

    // Step 5: make sure that the positions are invalid since reached end of stack.
    EXPECT_TRUE(nolock.isInvalidPosition(posNolock));
    EXPECT_TRUE(lock.isInvalidPosition(posLock));

    // Step 6: compare 2 stacks that have same entries.
    EXPECT_EQ(nolock, lock);

    // Step 7: resize stacks, set new size 0, make sure they are empty.
    nolock.resize(0);
    lock.resize(0);
    EXPECT_TRUE(nolock.isEmpty() && lock.isEmpty());
    EXPECT_EQ(nolock.getSize(), 0);
    EXPECT_EQ(lock.getSize(), 0);
}

/**
 * \brief   Test TEStack position manipulation.
 **/
TEST(TEStackTest, TestLockAndNolockStackPositionManipulation)
{
    using Stack = TEStack<int>;
    using NolockStack = TENolockStack<int>;
    using LockStack = TELockStack<int>;
    using POS = Stack::STACKPOS;

    constexpr uint32_t count{ 10 };
    NolockStack nolock;
    LockStack lock;

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        nolock.pushLast(i);
    }

    EXPECT_EQ(nolock.firstEntry(), 0);
    EXPECT_EQ(nolock.lastEntry(), static_cast<int>(count - 1u));

    for (POS pos = nolock.firstPosition(); nolock.isValidPosition(pos); pos = nolock.nextPosition(pos))
    {
        EXPECT_EQ(nolock.getAt(pos), nolock.valueAtPosition(pos));
        int value = nolock.valueAtPosition(pos);
        lock.pushFirst(value);
        nolock.getAt(pos) = static_cast<int>(count) - (value + 1);
    }

    EXPECT_EQ(nolock, lock);
}

/**
 * \brief   Test TEStack resizing.
 **/
TEST(TEStackTest, TestLockAndNolockStackResizing)
{
    using Stack = TEStack<int>;
    using NolockStack = TENolockStack<int>;
    using LockStack = TELockStack<int>;
    using POS = Stack::STACKPOS;

    constexpr uint32_t count{ 10 };

    // Step 1: initialize 2 types of stacks -- lock and unlock
    NolockStack nolock;
    LockStack lock;
    EXPECT_TRUE(nolock.isEmpty() && lock.isEmpty());

    // Step 2: resize `nolock` stack, set new size equal to `count`, make sure they it is not empty.
    nolock.resize(count);
    EXPECT_FALSE(nolock.isEmpty());
    EXPECT_EQ(nolock.getSize(), count);

    // Step 3: get the first position of the stacks and make sure that they are valid.
    POS posNolock = nolock.firstPosition();

    // Step 4: in the loop check each position of the stacks, check validity and check access via positioning.
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_TRUE(nolock.isValidPosition(posNolock));
        nolock[posNolock] = i;
        lock.pushLast(nolock[posNolock]);

        posNolock = nolock.nextPosition(posNolock);
    }

    EXPECT_EQ(nolock, lock);

    nolock.resize(count * 2u);
    lock.resize(count / 2u);
    EXPECT_NE(nolock, lock);
    EXPECT_EQ(nolock.getSize(), count * 2u);
    EXPECT_EQ(lock.getSize(), count / 2u);

    for (int i = 0; i < static_cast<int>(count * 2u); ++i)
    {
        int val = nolock.popFirst();
        if (i < static_cast<int>(count))
        {
            EXPECT_EQ(val, i);
        }
    }

    for (int i = 0; i < static_cast<int>(count / 2u); ++i)
    {
        int val = lock.popFirst();
        EXPECT_EQ(val, i);
    }

    EXPECT_TRUE(nolock.isEmpty() && lock.isEmpty());
    EXPECT_EQ(nolock.getSize(), 0);
    EXPECT_EQ(lock.getSize(), 0);
}

/**
 * \brief   Test TEStack resizing.
 **/
TEST(TEStackTest, TestLockAndNolockStackPushing)
{
    using NolockStack = TENolockStack<int>;
    using LockStack = TELockStack<int>;

    constexpr int arr[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr uint32_t count{ MACRO_ARRAYLEN(arr) };

    // Step 1: initialize 2 types of stacks -- lock and unlock
    NolockStack nolock;
    LockStack lock;
    EXPECT_TRUE(nolock.isEmpty() && lock.isEmpty());

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        uint32_t sizeNolock = nolock.pushFirst(arr[i]);
        EXPECT_EQ(nolock.firstEntry(), arr[i]);
        uint32_t sizeLock = lock.pushLast(arr[i]);
        EXPECT_EQ(lock.lastEntry(), arr[i]);
        EXPECT_EQ(sizeNolock, sizeLock);
    }

    EXPECT_NE(nolock, lock);

    for (uint32_t i = 0; i < count; ++i)
    {
        EXPECT_EQ(nolock.getData()[i], arr[count - (i + 1u)]);
        EXPECT_EQ(lock.getData()[i], arr[i]);
    }


    for (uint32_t i = 0; i < count; ++i)
    {
        int valNolock = nolock.popFirst();
        int valLock = lock.popFirst();
        EXPECT_EQ(valNolock, arr[count - (i + 1u)]);
        EXPECT_EQ(valLock, arr[i]);
    }

    EXPECT_TRUE(nolock.isEmpty() && lock.isEmpty());
}

/**
 * \brief   Test TEStack search.
 **/
TEST(TEStackTest, TestLockAndNolockStackSearch)
{
    using Stack = TEStack<int>;
    using NolockStack = TENolockStack<int>;
    using LockStack = TELockStack<int>;
    using POS = Stack::STACKPOS;

    constexpr uint32_t count{ 10 };

    NolockStack nolock;
    LockStack lock;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_FALSE(nolock.contains(i));
        nolock.pushLast(i);
        POS pos = nolock.find(i);
        EXPECT_TRUE(nolock.isValidPosition(pos));
    }

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_TRUE(nolock.contains(i));

        POS src = nolock.find(i);
        EXPECT_TRUE(nolock.isValidPosition(src));
        
        POS dst = lock.find(i);
        EXPECT_TRUE(lock.isInvalidPosition(dst));
        
        lock.pushLast(nolock.valueAtPosition(src));
    }

    EXPECT_EQ(nolock.getData(), lock.getData());
}


/**
 * \brief   Test TEStack copy and move.
 **/
TEST(TEStackTest, TestLockAndNolockStackCopyMove)
{
    using NolockStack = TENolockStack<int>;
    using LockStack = TELockStack<int>;

    constexpr uint32_t count{ 10 };

    NolockStack nolockCopy, nolockMove;
    LockStack lockCopy, lockMove;

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        nolockCopy.pushLast(i);
    }

    lockCopy.copy(nolockCopy);
    EXPECT_EQ(nolockCopy, lockCopy);

    nolockMove = lockCopy;
    lockMove.move(std::move(nolockMove));
    EXPECT_TRUE(nolockMove.isEmpty());
    EXPECT_EQ(lockMove, lockCopy);
}

/**
 * \brief   Test TEStack streaming.
 **/
TEST(TEStackTest, TestLockAndNolockStackStreaming)
{
    using NolockStack = TENolockStack<int>;
    using LockStack = TELockStack<int>;

    constexpr uint32_t count{ 10 };

    NolockStack nolock;
    LockStack lock;
    SharedBuffer stream;

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        lock.pushLast(i);
    }

    EXPECT_EQ(lock.getSize(), count);
    stream << lock;
    EXPECT_FALSE(stream.isEmpty());

    stream.moveToBegin();
    stream >> nolock;
    EXPECT_FALSE(nolock.isEmpty());
    EXPECT_EQ(lock, nolock);
}
