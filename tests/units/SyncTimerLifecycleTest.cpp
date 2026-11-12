#include "units/GUnitTest.hpp"
#include "units/ThreadCoverageHelpers.hpp"
#include "areg/base/SyncPrimitives.hpp"

#if defined(AREG_TEST_PRIVATE_POSIX)
    #include "areg/base/private/posix/WaitableTimerPosix.hpp"
#endif  // defined(AREG_TEST_PRIVATE_POSIX)

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

TEST(SyncTimerLifecycleTest, attributes_and_unarmed_state_match_constructor)
{
    areg::SyncTimer oneShot(5u, false, true, true);
    areg::SyncTimer periodic(7u, true, false, false);
    EXPECT_TRUE(oneShot.is_valid());
    EXPECT_EQ(oneShot.type(), areg::SyncObject::SyncKind::SoTimer);
    EXPECT_EQ(oneShot.due_time(), 5u);
    EXPECT_FALSE(oneShot.is_periodic());
    EXPECT_TRUE(oneShot.is_autoreset());
    EXPECT_FALSE(oneShot.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(periodic.is_valid());
    EXPECT_EQ(periodic.due_time(), 7u);
    EXPECT_TRUE(periodic.is_periodic());
    EXPECT_FALSE(periodic.is_autoreset());
    EXPECT_FALSE(periodic.lock(areg::DO_NOT_WAIT));
}

TEST(SyncTimerLifecycleTest, armed_timer_can_be_rearmed_before_expiry)
{
    areg::SyncTimer timer(COVERAGE_NEVER_MS, true, true);
    ASSERT_TRUE(timer.set_timer());
    ASSERT_TRUE(timer.set_timer());
    EXPECT_FALSE(timer.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(timer.cancel_timer());
    EXPECT_TRUE(timer.lock(COVERAGE_WAIT_MS));
    EXPECT_FALSE(timer.lock(areg::DO_NOT_WAIT));
    ASSERT_TRUE(timer.set_timer());
    EXPECT_FALSE(timer.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(timer.unlock());
    EXPECT_TRUE(timer.lock(COVERAGE_WAIT_MS));
    EXPECT_FALSE(timer.lock(areg::DO_NOT_WAIT));
}

TEST(SyncTimerLifecycleTest, manual_cancel_is_sticky_and_rearm_clears_the_signal)
{
    areg::SyncTimer timer(COVERAGE_NEVER_MS, false, false);
    ASSERT_TRUE(timer.cancel_timer());
    EXPECT_TRUE(timer.lock(COVERAGE_WAIT_MS));
    EXPECT_TRUE(timer.lock(areg::DO_NOT_WAIT));
    ASSERT_TRUE(timer.set_timer());
    EXPECT_FALSE(timer.lock(areg::DO_NOT_WAIT));
    ASSERT_TRUE(timer.cancel_timer());
    EXPECT_TRUE(timer.lock(COVERAGE_WAIT_MS));
    EXPECT_TRUE(timer.lock(areg::DO_NOT_WAIT));
    EXPECT_EQ(timer.due_time(), COVERAGE_NEVER_MS);
    EXPECT_FALSE(timer.is_periodic());
    EXPECT_FALSE(timer.is_autoreset());
}

TEST(SyncTimerLifecycleTest, repeated_one_shot_expiry_and_rearm_preserve_auto_reset)
{
    areg::SyncTimer timer(1u, false, true);
    for (uint32_t round = 0u; round < 8u; ++round)
    {
        SCOPED_TRACE(round);
        ASSERT_TRUE(timer.set_timer());
        EXPECT_TRUE(timer.lock(COVERAGE_WAIT_MS));
        EXPECT_FALSE(timer.lock(areg::DO_NOT_WAIT));
    }
    EXPECT_TRUE(timer.cancel_timer());
}

TEST(SyncTimerLifecycleTest, cancelled_timer_wakes_wait_all_with_other_ready_object)
{
    CoverageDeadline deadline;
    areg::SyncTimer timer(COVERAGE_NEVER_MS, false, true);
    areg::SyncEvent ready(false, true);
    ASSERT_TRUE(timer.set_timer());
    CoverageGate starting;
    bool acquired{ false };
    std::thread waiter([&]()
        {
            areg::SyncObject* objects[]{ &ready, &timer };
            areg::MultiLock multi(objects, 2, false);
            starting.arrive();
            acquired = multi.lock(COVERAGE_WAIT_MS, true) == areg::MultiLock::LOCK_INDEX_ALL;
        });
    EXPECT_TRUE(starting.wait());
    EXPECT_TRUE(timer.cancel_timer());
    waiter.join();
    EXPECT_TRUE(acquired);
    EXPECT_FALSE(ready.lock(areg::DO_NOT_WAIT));
    EXPECT_FALSE(timer.lock(areg::DO_NOT_WAIT));
}

TEST(SyncTimerLifecycleTest, auto_cancel_handoff_releases_exactly_one_wait_per_cancel)
{
    CoverageDeadline deadline;
    constexpr uint32_t WAITERS{ 4u };
    areg::SyncTimer timer(COVERAGE_NEVER_MS, false, true);
    ASSERT_TRUE(timer.set_timer());
    CoverageGate starting;
    CoverageGate released;
    std::atomic<uint32_t> count{ 0u };
    std::vector<std::thread> waiters;
    for (uint32_t index = 0u; index < WAITERS; ++index)
    {
        waiters.emplace_back([&]()
            {
                starting.arrive();
                if (timer.lock(COVERAGE_WAIT_MS))
                {
                    ++count;
                    released.arrive();
                }
                else
                {
                    ADD_FAILURE() << "Cancelled timer did not release a waiter";
                }
            });
    }
    EXPECT_TRUE(starting.wait(WAITERS));
    for (uint32_t index = 1u; index <= WAITERS; ++index)
    {
        EXPECT_TRUE(timer.cancel_timer());
        EXPECT_TRUE(released.wait(index));
        EXPECT_EQ(count.load(), index);
    }
    for (auto& waiter : waiters)
    {
        waiter.join();
    }
    EXPECT_EQ(count.load(), WAITERS);
    EXPECT_FALSE(timer.lock(areg::DO_NOT_WAIT));
}

TEST(SyncTimerLifecycleTest, destroy_armed_timers_preserves_surviving_timer)
{
    areg::SyncTimer survivor(1u, false, true);
    std::vector<std::unique_ptr<areg::SyncTimer>> armed;
    for (uint32_t index = 0u; index < 24u; ++index)
    {
        armed.emplace_back(std::make_unique<areg::SyncTimer>(COVERAGE_NEVER_MS + index, true, true));
        ASSERT_TRUE(armed.back()->set_timer());
    }
    armed.erase(armed.begin() + 12);
    armed.erase(armed.begin());
    armed.pop_back();
    armed.clear();
    ASSERT_TRUE(survivor.set_timer());
    EXPECT_TRUE(survivor.lock(COVERAGE_WAIT_MS));
    EXPECT_FALSE(survivor.lock(areg::DO_NOT_WAIT));
    EXPECT_TRUE(survivor.cancel_timer());
}

TEST(SyncTimerLifecycleTest, last_armed_timer_destruction_allows_fresh_timer_lifecycle)
{
    for (uint32_t round = 0u; round < 4u; ++round)
    {
        SCOPED_TRACE(round);
        {
            areg::SyncTimer armed(COVERAGE_NEVER_MS, true);
            ASSERT_TRUE(armed.set_timer());
            EXPECT_FALSE(armed.lock(areg::DO_NOT_WAIT));
        }
        areg::SyncTimer fresh(1u, false);
        ASSERT_TRUE(fresh.set_timer());
        EXPECT_TRUE(fresh.lock(COVERAGE_WAIT_MS));
        EXPECT_FALSE(fresh.lock(areg::DO_NOT_WAIT));
    }
}

#if defined(AREG_TEST_PRIVATE_POSIX)

TEST(SyncTimerLifecycleTest, waitable_timer_is_valid_until_its_resources_are_freed)
{
    areg::os::WaitableTimerPosix timer;
    EXPECT_TRUE(timer.is_valid());
    timer.free_resources();
    EXPECT_FALSE(timer.is_valid());
}

#endif  // defined(AREG_TEST_PRIVATE_POSIX)
