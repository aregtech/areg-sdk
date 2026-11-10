/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/StackTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Tests of StackBase object.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/Stack.hpp"
#include "areg/base/SharedBuffer.hpp"

/**
 * \brief   Test StackBase positioning.
 **/
TEST(StackTest, test_lock_and_nolock_stack_positioning)
{
    using StackBase = areg::StackBase<int>;
    using NolockStack = areg::Stack<int>;
    using LockStack = areg::ConcurrentStack<int>;
    using POS = StackBase::STACKPOS;

    constexpr uint32_t count{ 10 };

    // Step 1: initialize 2 types of stacks -- lock and unlock
    NolockStack nolock;
    LockStack lock;
    EXPECT_TRUE(nolock.is_empty() && lock.is_empty());

    // Step 2: resize stacks, set new size equal to `count`, make sure they are not empty.
    nolock.resize(count);
    lock.resize(count);
    EXPECT_FALSE(nolock.is_empty() && lock.is_empty());
    EXPECT_EQ(nolock.size(), count);
    EXPECT_EQ(lock.size(), count);

    // Step 3: get the first position of the stacks and make sure that they are valid.
    POS posNolock = nolock.first_position();
    POS posLock = lock.first_position();
    EXPECT_FALSE(!nolock.is_valid_position(posNolock));
    EXPECT_FALSE(!lock.is_valid_position(posLock));
    EXPECT_TRUE(nolock.is_first_position(posNolock));
    EXPECT_TRUE(lock.is_first_position(posLock));

    // Step 4: in the loop check each position of the stacks, check validity and check access via positioning.
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_TRUE(nolock.is_valid_position(posNolock));
        EXPECT_TRUE(lock.is_valid_position(posLock));

        EXPECT_TRUE(nolock.check_position(posNolock));
        EXPECT_TRUE(lock.check_position(posLock));

        nolock.value_at(posNolock) = i;
        lock[posLock] = nolock[posNolock];
        EXPECT_EQ(lock.value_at(posLock), i);

        if (i == static_cast<int>(count - 1u))
        {
            EXPECT_TRUE(nolock.is_last_position(posNolock));
            EXPECT_TRUE(lock.is_last_position(posLock));
        }

        posNolock = nolock.next_position(posNolock);
        posLock = lock.next_position(posLock);

        EXPECT_FALSE(nolock.is_first_position(posNolock));
        EXPECT_FALSE(lock.is_first_position(posLock));
    }

    // Step 5: make sure that the positions are invalid since reached end of stack.
    EXPECT_TRUE(!nolock.is_valid_position(posNolock));
    EXPECT_TRUE(!lock.is_valid_position(posLock));

    // Step 6: compare 2 stacks that have same entries.
    EXPECT_EQ(nolock, lock);

    // Step 7: resize stacks, set new size 0, make sure they are empty.
    nolock.resize(0);
    lock.resize(0);
    EXPECT_TRUE(nolock.is_empty() && lock.is_empty());
    EXPECT_EQ(nolock.size(), 0u);
    EXPECT_EQ(lock.size(), 0u);
}

/**
 * \brief   Test StackBase position manipulation.
 **/
TEST(StackTest, test_lock_and_nolock_stack_position_manipulation)
{
    using StackBase = areg::StackBase<int>;
    using NolockStack = areg::Stack<int>;
    using LockStack = areg::ConcurrentStack<int>;
    using POS = StackBase::STACKPOS;

    constexpr uint32_t count{ 10 };
    NolockStack nolock;
    LockStack lock;

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        nolock.push_last(i);
    }

    EXPECT_EQ(nolock.first_entry(), 0);
    EXPECT_EQ(nolock.last_entry(), static_cast<int>(count - 1u));

    for (POS pos = nolock.first_position(); nolock.is_valid_position(pos); pos = nolock.next_position(pos))
    {
        EXPECT_EQ(nolock.value_at(pos), nolock.value_at(pos));
        int value = nolock.value_at(pos);
        lock.push_first(value);
        nolock.value_at(pos) = static_cast<int>(count) - (value + 1);
    }

    EXPECT_EQ(nolock, lock);
}

/**
 * \brief   Test StackBase resizing.
 **/
TEST(StackTest, test_lock_and_nolock_stack_resizing)
{
    using StackBase = areg::StackBase<int>;
    using NolockStack = areg::Stack<int>;
    using LockStack = areg::ConcurrentStack<int>;
    using POS = StackBase::STACKPOS;

    constexpr uint32_t count{ 10 };

    // Step 1: initialize 2 types of stacks -- lock and unlock
    NolockStack nolock;
    LockStack lock;
    EXPECT_TRUE(nolock.is_empty() && lock.is_empty());

    // Step 2: resize `nolock` stack, set new size equal to `count`, make sure they it is not empty.
    nolock.resize(count);
    EXPECT_FALSE(nolock.is_empty());
    EXPECT_EQ(nolock.size(), count);

    // Step 3: get the first position of the stacks and make sure that they are valid.
    POS posNolock = nolock.first_position();

    // Step 4: in the loop check each position of the stacks, check validity and check access via positioning.
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_TRUE(nolock.is_valid_position(posNolock));
        nolock[posNolock] = i;
        lock.push_last(nolock[posNolock]);

        posNolock = nolock.next_position(posNolock);
    }

    EXPECT_EQ(nolock, lock);

    nolock.resize(count * 2u);
    lock.resize(count / 2u);
    EXPECT_NE(nolock, lock);
    EXPECT_EQ(nolock.size(), count * 2u);
    EXPECT_EQ(lock.size(), count / 2u);

    for (int i = 0; i < static_cast<int>(count * 2u); ++i)
    {
        int val = nolock.pop_first();
        if (i < static_cast<int>(count))
        {
            EXPECT_EQ(val, i);
        }
    }

    for (int i = 0; i < static_cast<int>(count / 2u); ++i)
    {
        int val = lock.pop_first();
        EXPECT_EQ(val, i);
    }

    EXPECT_TRUE(nolock.is_empty() && lock.is_empty());
    EXPECT_EQ(nolock.size(), 0u);
    EXPECT_EQ(lock.size(), 0u);
}

/**
 * \brief   Test StackBase resizing.
 **/
TEST(StackTest, test_lock_and_nolock_stack_pushing)
{
    using NolockStack = areg::Stack<int>;
    using LockStack = areg::ConcurrentStack<int>;

    constexpr int arr[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr uint32_t count{ std::size(arr) };

    // Step 1: initialize 2 types of stacks -- lock and unlock
    NolockStack nolock;
    LockStack lock;
    EXPECT_TRUE(nolock.is_empty() && lock.is_empty());

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        uint32_t sizeNolock = nolock.push_first(arr[i]);
        EXPECT_EQ(nolock.first_entry(), arr[i]);
        uint32_t sizeLock = lock.push_last(arr[i]);
        EXPECT_EQ(lock.last_entry(), arr[i]);
        EXPECT_EQ(sizeNolock, sizeLock);
    }

    EXPECT_NE(nolock, lock);

    for (uint32_t i = 0; i < count; ++i)
    {
        EXPECT_EQ(nolock.data()[i], arr[count - (i + 1u)]);
        EXPECT_EQ(lock.data()[i], arr[i]);
    }


    for (uint32_t i = 0; i < count; ++i)
    {
        int valNolock = nolock.pop_first();
        int valLock = lock.pop_first();
        EXPECT_EQ(valNolock, arr[count - (i + 1u)]);
        EXPECT_EQ(valLock, arr[i]);
    }

    EXPECT_TRUE(nolock.is_empty() && lock.is_empty());
}

/**
 * \brief   Test StackBase search.
 **/
TEST(StackTest, test_lock_and_nolock_stack_search)
{
    using StackBase = areg::StackBase<int>;
    using NolockStack = areg::Stack<int>;
    using LockStack = areg::ConcurrentStack<int>;
    using POS = StackBase::STACKPOS;

    constexpr uint32_t count{ 10 };

    NolockStack nolock;
    LockStack lock;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_FALSE(nolock.contains(i));
        nolock.push_last(i);
        POS pos = nolock.find(i);
        EXPECT_TRUE(nolock.is_valid_position(pos));
    }

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_TRUE(nolock.contains(i));

        POS src = nolock.find(i);
        EXPECT_TRUE(nolock.is_valid_position(src));
        
        POS dst = lock.find(i);
        EXPECT_TRUE(!lock.is_valid_position(dst));
        
        lock.push_last(nolock.value_at(src));
    }

    EXPECT_EQ(nolock.data(), lock.data());
}


/**
 * \brief   Test StackBase streaming.
 **/
TEST(StackTest, test_lock_and_nolock_stack_streaming)
{
    using NolockStack = areg::Stack<int>;
    using LockStack = areg::ConcurrentStack<int>;

    constexpr uint32_t count{ 10 };

    NolockStack nolock;
    LockStack lock;
    areg::SharedBuffer stream;

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        lock.push_last(i);
    }

    EXPECT_EQ(lock.size(), count);
    stream << lock;
    EXPECT_FALSE(stream.is_empty());

    stream.move_to_begin();
    stream >> nolock;
    EXPECT_FALSE(nolock.is_empty());
    EXPECT_EQ(lock, nolock);
}

/**
 * \brief   Tests ascending sorting of stack.
 **/
TEST(StackTest, test_ascending)
{
    using Stack = areg::Stack<int>;

    constexpr int _arr1[]{ 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    constexpr int _res1[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr uint32_t _len{ std::size(_arr1) };

    Stack arr1(_arr1, _len), res1(_res1, _len);
    arr1.sort([](const int elem1, const int elem2) { return (elem1 < elem2); });

    EXPECT_NE(arr1, Stack(_arr1, _len));
    EXPECT_EQ(arr1.size(), _len);
    EXPECT_EQ(arr1, res1);
}

/**
 * \brief   Tests descending sorting of fixed array.
 **/
TEST(StackTest, test_descending)
{
    using Stack = areg::Stack<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr int _res1[]{ 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    constexpr uint32_t _len{ std::size(_arr1) };

    Stack arr1(_arr1, _len), res1(_res1, _len);
    arr1.sort([](const int elem1, const int elem2) { return (elem1 > elem2); });

    EXPECT_NE(arr1, Stack(_arr1, _len));
    EXPECT_EQ(arr1.size(), _len);
    EXPECT_EQ(arr1, res1);
}

/**
 * \brief   Test StackBase copy and move construction and assignment.
 **/
TEST(StackTest, test_lock_and_nolock_stack_copy_and_move)
{
    constexpr int count{ 6 };
    const int source[count]{ 1, 2, 3, 4, 5, 6 };

    areg::Stack<int> nolock(source, count);
    areg::ConcurrentStack<int> lock(source, count);
    ASSERT_EQ(nolock.size(), static_cast<uint32_t>(count));
    ASSERT_EQ(lock.size(), static_cast<uint32_t>(count));

    areg::Stack<int> nolockCopy(nolock);
    areg::ConcurrentStack<int> lockCopy(lock);
    EXPECT_EQ(nolockCopy.data(), nolock.data());
    EXPECT_EQ(lockCopy.data(), lock.data());

    areg::Stack<int> nolockMoved(std::move(nolockCopy));
    areg::ConcurrentStack<int> lockMoved(std::move(lockCopy));
    EXPECT_EQ(nolockMoved.data(), nolock.data());
    EXPECT_EQ(lockMoved.data(), lock.data());

    areg::Stack<int> nolockAssigned;
    areg::ConcurrentStack<int> lockAssigned;
    nolockAssigned = nolockMoved;
    lockAssigned = lockMoved;
    EXPECT_EQ(nolockAssigned.data(), nolock.data());
    EXPECT_EQ(lockAssigned.data(), lock.data());

    areg::Stack<int> nolockTarget;
    areg::ConcurrentStack<int> lockTarget;
    nolockTarget = std::move(nolockAssigned);
    lockTarget = std::move(lockAssigned);
    EXPECT_EQ(nolockTarget.data(), nolock.data());
    EXPECT_EQ(lockTarget.data(), lock.data());
    EXPECT_TRUE(nolockAssigned.is_empty());
    EXPECT_TRUE(lockAssigned.is_empty());
}

/**
 * \brief   Test StackBase access by position, push_first and element export.
 **/
TEST(StackTest, test_lock_and_nolock_stack_access_by_position)
{
    areg::Stack<int> nolock;
    areg::ConcurrentStack<int> lock;

    EXPECT_EQ(nolock.push_last(2), 1u);
    EXPECT_EQ(lock.push_last(2), 1u);
    EXPECT_EQ(nolock.push_first(1), 2u);
    EXPECT_EQ(lock.push_first(1), 2u);
    EXPECT_EQ(nolock.push_first(0), 3u);
    EXPECT_EQ(lock.push_first(0), 3u);

    const areg::Stack<int>& constNolock{ nolock };
    const areg::ConcurrentStack<int>& constLock{ lock };
    for (int i = 0; i < 3; ++i)
    {
        auto posNolock = nolock.find(i);
        auto posLock = lock.find(i);
        ASSERT_TRUE(nolock.is_valid_position(posNolock));
        ASSERT_TRUE(lock.is_valid_position(posLock));
        EXPECT_EQ(constNolock.value_at(posNolock), i);
        EXPECT_EQ(constLock.value_at(posLock), i);
        EXPECT_EQ(constNolock[posNolock], i);
        EXPECT_EQ(constLock[posLock], i);
    }

    EXPECT_FALSE(nolock.is_valid_position(nolock.invalid_position()));
    EXPECT_FALSE(lock.is_valid_position(lock.invalid_position()));

    int exported[5]{ -1, -1, -1, -1, -1 };
    EXPECT_EQ(nolock.elements(exported, 5u), 3u);
    EXPECT_EQ(exported[0], 0);
    EXPECT_EQ(exported[2], 2);
    EXPECT_EQ(exported[3], -1);

    int limited[2]{ -1, -1 };
    EXPECT_EQ(lock.elements(limited, 2u), 2u);
    EXPECT_EQ(limited[0], 0);
    EXPECT_EQ(limited[1], 1);
}

/**
 * \brief   Test StackBase search after a position.
 **/
TEST(StackTest, test_lock_and_nolock_stack_search_after_position)
{
    constexpr int values[]{ 7, 1, 7, 2, 7 };
    areg::Stack<int> nolock(values, 5u);
    areg::ConcurrentStack<int> lock(values, 5u);

    auto first = nolock.find(7);
    ASSERT_TRUE(nolock.is_first_position(first));
    auto second = nolock.find(7, first);
    ASSERT_TRUE(nolock.is_valid_position(second));
    EXPECT_NE(first, second);
    auto third = nolock.find(7, second);
    ASSERT_TRUE(nolock.is_valid_position(third));
    EXPECT_TRUE(nolock.is_last_position(third));
    EXPECT_FALSE(nolock.is_valid_position(nolock.find(7, third)));
    EXPECT_FALSE(nolock.is_valid_position(nolock.find(7, nolock.invalid_position())));

    auto lockFirst = lock.find(7);
    auto lockSecond = lock.find(7, lockFirst);
    EXPECT_TRUE(lock.is_valid_position(lockSecond));
    EXPECT_FALSE(lock.is_valid_position(lock.find(1, lock.find(2))));
}

/**
 * \brief   Test StackBase move, free_extra and release.
 **/
TEST(StackTest, test_lock_and_nolock_stack_move_and_release)
{
    constexpr int values[]{ 1, 2, 3 };
    areg::Stack<int> nolock(values, 3u);
    areg::ConcurrentStack<int> lock(values, 3u);

    areg::Stack<int> nolockTarget;
    areg::ConcurrentStack<int> lockTarget;
    EXPECT_EQ(nolockTarget.move(std::move(nolock)), 3u);
    EXPECT_EQ(lockTarget.move(std::move(lock)), 3u);
    EXPECT_EQ(nolockTarget.size(), 3u);
    EXPECT_EQ(lockTarget.size(), 3u);

    nolockTarget.clear();
    lockTarget.clear();
    nolockTarget.free_extra();
    lockTarget.free_extra();
    EXPECT_TRUE(nolockTarget.is_empty());
    EXPECT_TRUE(lockTarget.is_empty());

    nolockTarget.push_last(1);
    lockTarget.push_last(1);
    nolockTarget.release();
    lockTarget.release();
    EXPECT_TRUE(nolockTarget.is_empty());
    EXPECT_TRUE(lockTarget.is_empty());
}
