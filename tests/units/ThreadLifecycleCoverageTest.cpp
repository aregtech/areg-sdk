#include "units/GUnitTest.hpp"
#include "units/ThreadCoverageHelpers.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadAddress.hpp"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/base/ThreadLocalStorage.hpp"

#include <atomic>
#include <functional>
#include <thread>
#include <utility>

namespace
{
    class CoverageThread : public areg::ThreadConsumer
    {
    public:
        explicit CoverageThread(const areg::String& name, std::function<void()> action = {}
            , uint32_t stackSize = areg::DEFAULT_STACK_SIZE)
            : mEntered()
            , mRelease()
            , mExited()
            , mRuns(0u)
            , mExits(0u)
            , mRegistrations(0u)
            , mUnregistrations(0u)
            , mAction(std::move(action))
            , mThread(*this, name, stackSize)
        {
        }

        ~CoverageThread() override
        {
            mRelease.arrive(100u);
            static_cast<void>(mThread.shutdown(areg::WAIT_INFINITE));
        }

        void on_run() override
        {
            const uint32_t run{ mRuns.fetch_add(1u) + 1u };
            if (mAction)
            {
                mAction();
            }

            mEntered.arrive();
            EXPECT_TRUE(mRelease.wait(run));
        }

        int32_t on_exit() override
        {
            ++mExits;
            mExited.arrive();
            return static_cast<int32_t>(areg::ThreadConsumer::ExitCode::Normal);
        }

        bool on_thread_registered(areg::Thread* thread) override
        {
            EXPECT_NE(thread, nullptr);
            ++mRegistrations;
            return true;
        }

        void on_thread_unregistering() override
        {
            ++mUnregistrations;
        }

        CoverageGate mEntered;
        CoverageGate mRelease;
        CoverageGate mExited;
        std::atomic<uint32_t> mRuns;
        std::atomic<uint32_t> mExits;
        std::atomic<uint32_t> mRegistrations;
        std::atomic<uint32_t> mUnregistrations;
        std::function<void()> mAction;
        areg::Thread mThread;
    };

    class VetoThread final : public areg::Thread
    {
    public:
        explicit VetoThread(areg::ThreadConsumer& consumer)
            : areg::Thread(consumer, "CoverageVetoThread")
        {
        }

    protected:
        bool on_pre_run() override
        {
            EXPECT_TRUE(areg::Thread::on_pre_run());
            return false;
        }
    };

    class RejectingConsumer final : public areg::ThreadConsumer
    {
    public:
        RejectingConsumer()
            : mRegistrations(0u)
            , mUnregistrations(0u)
            , mRuns(0u)
        {
        }

        bool on_thread_registered(areg::Thread*) override
        {
            ++mRegistrations;
            return false;
        }

        void on_thread_unregistering() override
        {
            ++mUnregistrations;
        }

        void on_run() override
        {
            ++mRuns;
        }

        std::atomic<uint32_t> mRegistrations;
        std::atomic<uint32_t> mUnregistrations;
        std::atomic<uint32_t> mRuns;
    };
}

TEST(ThreadApiCoverageTest, unstarted_thread_has_no_registration_or_resources)
{
    CoverageThread probe("CoverageUnstarted");
    auto& thread{ probe.mThread };
    EXPECT_FALSE(thread.is_running());
    EXPECT_FALSE(thread.is_valid());
    EXPECT_EQ(thread.id(), areg::Thread::INVALID_THREAD_ID);
    EXPECT_EQ(thread.priority(), areg::Thread::ThreadPriority::Undefined);
    EXPECT_EQ(thread.set_priority(areg::Thread::ThreadPriority::Low), areg::Thread::ThreadPriority::Undefined);
    EXPECT_EQ(thread.stack_size(), areg::DEFAULT_STACK_SIZE);
    EXPECT_EQ(thread.name(), "CoverageUnstarted");
    EXPECT_EQ(thread.number(), static_cast<uint32_t>(thread.address()));
    EXPECT_EQ(areg::Thread::find_by_address(thread.address()), nullptr);
    EXPECT_TRUE(thread.wait_completion(areg::DO_NOT_WAIT));
    EXPECT_TRUE(thread.wait_completion(COVERAGE_WAIT_MS));
    EXPECT_EQ(thread.shutdown(), areg::Thread::ThreadCompletion::Invalid);
    EXPECT_TRUE(thread.is_exit_requested());
    EXPECT_EQ(probe.mRegistrations.load(), 0u);
    EXPECT_EQ(probe.mUnregistrations.load(), 0u);
}

TEST(ThreadApiCoverageTest, registry_and_current_thread_identity_agree_until_shutdown)
{
    CoverageDeadline deadline;
    const uint32_t before{ areg::Thread::registry_generation() };
    areg::ThreadConsumer* expectedConsumer{ nullptr };
    CoverageThread probe("CoverageIdentity", [&expectedConsumer]()
        {
            auto* self{ areg::Thread::current_thread() };
            ASSERT_NE(self, nullptr);
            EXPECT_EQ(self->id(), areg::Thread::current_thread_id());
            EXPECT_EQ(areg::Thread::current_thread_name(), self->name());
            EXPECT_EQ(areg::Thread::current_thread_address(), self->address());
            EXPECT_EQ(&areg::Thread::current_thread_storage().owner_thread(), self);
            EXPECT_EQ(&areg::Thread::current_thread_consumer(), expectedConsumer);
#if defined(POSIX) || defined(_POSIX)
            EXPECT_GT(areg::Thread::current_stack_size(), 0u);
#endif
        });
    expectedConsumer = &probe;
    auto& thread{ probe.mThread };
    ASSERT_TRUE(thread.start(COVERAGE_WAIT_MS));
    ASSERT_TRUE(probe.mEntered.wait());
    EXPECT_TRUE(thread.is_valid());
    EXPECT_TRUE(thread.is_running());
    EXPECT_NE(areg::Thread::registry_generation(), 0u);
    EXPECT_NE(areg::Thread::registry_generation(), before);
    const id_type id{ thread.id() };
    EXPECT_EQ(areg::Thread::find_by_id(id), &thread);
    EXPECT_EQ(areg::Thread::find_by_number(thread.number()), &thread);
    EXPECT_EQ(areg::Thread::find_by_address(thread.address()), &thread);
    EXPECT_EQ(areg::Thread::address_by_id(id), thread.address());
    EXPECT_EQ(areg::Thread::address_by_number(thread.number()), thread.address());
    EXPECT_EQ(areg::Thread::thread_name(id), thread.name());
    EXPECT_EQ(areg::Thread::thread_address(id), thread.address());
    EXPECT_FALSE(thread.startup_phase());
    EXPECT_TRUE(thread.wait_completion(areg::DO_NOT_WAIT));
    EXPECT_FALSE(thread.wait_completion(1u));
    probe.mRelease.arrive();
    ASSERT_TRUE(thread.wait_completion(COVERAGE_WAIT_MS));
    EXPECT_TRUE(thread.wait_completion(COVERAGE_WAIT_MS));
    EXPECT_EQ(thread.shutdown(), areg::Thread::ThreadCompletion::Completed);
    EXPECT_FALSE(thread.is_valid());
    EXPECT_FALSE(thread.is_running());
    EXPECT_EQ(thread.priority(), areg::Thread::ThreadPriority::Undefined);
    EXPECT_EQ(areg::Thread::find_by_id(id), nullptr);
    EXPECT_EQ(areg::Thread::find_by_number(thread.number()), nullptr);
    EXPECT_TRUE(areg::Thread::thread_name(id).is_empty());
    EXPECT_FALSE(areg::Thread::thread_address(id).is_valid());
    EXPECT_FALSE(areg::Thread::address_by_id(id).is_valid());
    EXPECT_FALSE(areg::Thread::address_by_number(thread.number()).is_valid());
    EXPECT_EQ(probe.mRuns.load(), 1u);
    EXPECT_EQ(probe.mExits.load(), 1u);
    EXPECT_EQ(probe.mRegistrations.load(), 1u);
    EXPECT_EQ(probe.mUnregistrations.load(), 1u);
}

TEST(ThreadApiCoverageTest, detach_removes_lookup_without_waiting_for_consumer)
{
    CoverageDeadline deadline;
    CoverageThread probe("CoverageDetach");
    auto& thread{ probe.mThread };
    ASSERT_TRUE(thread.start(COVERAGE_WAIT_MS));
    ASSERT_TRUE(probe.mEntered.wait());
    const id_type id{ thread.id() };
    const uint32_t generation{ areg::Thread::registry_generation() };
    thread.detach_from_registry();
    EXPECT_TRUE(thread.is_exit_requested());
    EXPECT_TRUE(thread.is_running());
    EXPECT_EQ(areg::Thread::find_by_id(id), nullptr);
    EXPECT_EQ(areg::Thread::find_by_number(thread.number()), nullptr);
    EXPECT_NE(areg::Thread::registry_generation(), generation);
    EXPECT_EQ(probe.mUnregistrations.load(), 1u);
    thread.detach_from_registry();
    EXPECT_EQ(probe.mUnregistrations.load(), 1u);
    probe.mRelease.arrive();
    EXPECT_TRUE(thread.wait_completion(COVERAGE_WAIT_MS));
    EXPECT_EQ(thread.shutdown(), areg::Thread::ThreadCompletion::Completed);
    EXPECT_EQ(probe.mExits.load(), 1u);
    EXPECT_EQ(probe.mUnregistrations.load(), 1u);
}

TEST(ThreadApiCoverageTest, no_wait_shutdown_reports_stuck_and_keeps_routine_alive)
{
    CoverageDeadline deadline;
    CoverageThread probe("CoverageNoWait");
    auto& thread{ probe.mThread };
    ASSERT_TRUE(thread.start(COVERAGE_WAIT_MS));
    ASSERT_TRUE(probe.mEntered.wait());
    EXPECT_EQ(thread.shutdown(areg::DO_NOT_WAIT), areg::Thread::ThreadCompletion::Stuck);
    EXPECT_TRUE(thread.is_running());
    EXPECT_TRUE(thread.is_exit_requested());
    EXPECT_FALSE(thread.start(areg::DO_NOT_WAIT));
    EXPECT_EQ(probe.mExits.load(), 0u);
    probe.mRelease.arrive();
    EXPECT_TRUE(probe.mExited.wait());
}

TEST(ThreadApiCoverageTest, completed_runs_restart_with_fresh_storage_and_exact_callbacks)
{
    CoverageDeadline deadline;
    CoverageThread probe("CoverageRestart", []()
        {
            auto& tls{ areg::Thread::current_thread_storage() };
            EXPECT_FALSE(tls.exist("previous-run"));
            tls.set_item("previous-run", uint64_t{ 0x12345678ABCDEF01ULL });
        });
    for (uint32_t run = 1u; run <= 4u; ++run)
    {
        ASSERT_TRUE(probe.mThread.start(COVERAGE_WAIT_MS));
        ASSERT_TRUE(probe.mEntered.wait(run));
        EXPECT_FALSE(probe.mThread.is_exit_requested());
        probe.mRelease.arrive();
        ASSERT_TRUE(probe.mThread.wait_completion(COVERAGE_WAIT_MS));
        EXPECT_EQ(probe.mThread.shutdown(), areg::Thread::ThreadCompletion::Completed);
        EXPECT_EQ(probe.mRuns.load(), run);
        EXPECT_EQ(probe.mExits.load(), run);
        EXPECT_EQ(probe.mRegistrations.load(), run);
        EXPECT_EQ(probe.mUnregistrations.load(), run);
    }
}

TEST(ThreadApiCoverageTest, priorities_preserve_previous_value_and_reject_undefined_changes)
{
    CoverageDeadline deadline;
    CoverageThread probe("CoveragePriority");
    auto& thread{ probe.mThread };
    ASSERT_TRUE(thread.start(COVERAGE_WAIT_MS));
    ASSERT_TRUE(probe.mEntered.wait());
    using Priority = areg::Thread::ThreadPriority;
    EXPECT_EQ(thread.priority(), Priority::Normal);
    for (const Priority requested : { Priority::Lowest, Priority::Low, Priority::Normal, Priority::High, Priority::Highest })
    {
        const Priority old{ thread.priority() };
        EXPECT_EQ(thread.set_priority(requested), old);
        const Priority actual{ thread.priority() };
        EXPECT_TRUE((actual == old) || (actual == requested));
        EXPECT_EQ(thread.set_priority(Priority::Undefined), actual);
        EXPECT_EQ(thread.priority(), actual);
        EXPECT_EQ(thread.set_priority(actual), actual);
        EXPECT_EQ(thread.priority(), actual);
    }
    probe.mRelease.arrive();
    EXPECT_TRUE(thread.wait_completion(COVERAGE_WAIT_MS));
}

TEST(ThreadApiCoverageTest, generated_name_and_explicit_stack_are_preserved)
{
    CoverageDeadline deadline;
    constexpr uint32_t STACK_KB{ 2'048u };
    CoverageThread first(areg::String::empty_string(), []()
        {
#if defined(POSIX) || defined(_POSIX)
            EXPECT_GE(areg::Thread::current_stack_size(), static_cast<size_t>(STACK_KB) * 1024u);
#endif
        }, STACK_KB);
    CoverageThread second(areg::String::empty_string());
    EXPECT_FALSE(first.mThread.name().is_empty());
    EXPECT_FALSE(second.mThread.name().is_empty());
    EXPECT_NE(first.mThread.name(), second.mThread.name());
    EXPECT_NE(first.mThread.number(), second.mThread.number());
    EXPECT_EQ(first.mThread.stack_size(), STACK_KB);
    ASSERT_TRUE(first.mThread.start(COVERAGE_WAIT_MS));
    EXPECT_TRUE(first.mEntered.wait());
    first.mRelease.arrive();
    EXPECT_TRUE(first.mThread.wait_completion(COVERAGE_WAIT_MS));
}

#if defined(_WIN32) && !defined(__CYGWIN__)
TEST(ThreadApiCoverageTest, windows_stack_size_reports_allocated_stack)
{
    CoverageDeadline deadline;
    constexpr uint32_t STACK_KB{ 2'048u };
    CoverageThread probe("CoverageWindowsStack", []()
        {
            EXPECT_GE(areg::Thread::current_stack_size(), static_cast<size_t>(STACK_KB) * 1024u);
        }, STACK_KB);
    ASSERT_TRUE(probe.mThread.start(COVERAGE_WAIT_MS));
    EXPECT_TRUE(probe.mEntered.wait());
    probe.mRelease.arrive();
    EXPECT_TRUE(probe.mThread.wait_completion(COVERAGE_WAIT_MS));
}
#endif

TEST(ThreadApiCoverageTest, pre_run_veto_skips_consumer_but_calls_exit)
{
    CoverageDeadline deadline;
    CoverageThread consumer("CoverageUnusedConsumer");
    VetoThread thread(consumer);
    ASSERT_TRUE(thread.start(COVERAGE_WAIT_MS));
    EXPECT_TRUE(thread.wait_completion(COVERAGE_WAIT_MS));
    EXPECT_EQ(consumer.mRuns.load(), 0u);
    EXPECT_EQ(consumer.mExits.load(), 1u);
    EXPECT_EQ(consumer.mRegistrations.load(), 1u);
    EXPECT_EQ(consumer.mUnregistrations.load(), 1u);
    EXPECT_EQ(thread.shutdown(), areg::Thread::ThreadCompletion::Completed);
}

TEST(ThreadApiCoverageTest, rejected_registration_allows_immediate_destruction)
{
    CoverageDeadline deadline;
    RejectingConsumer consumer;
    constexpr uint32_t ROUNDS{ 64u };
    for (uint32_t round = 0u; round < ROUNDS; ++round)
    {
        SCOPED_TRACE(round);
        areg::Thread thread(consumer, "CoverageRejectedRegistration");
        EXPECT_FALSE(thread.start(areg::DO_NOT_WAIT));
        EXPECT_FALSE(thread.is_valid());
        EXPECT_FALSE(thread.is_running());
    }
    EXPECT_EQ(consumer.mRegistrations.load(), ROUNDS);
    EXPECT_EQ(consumer.mUnregistrations.load(), ROUNDS);
    EXPECT_EQ(consumer.mRuns.load(), 0u);
}

TEST(ThreadApiCoverageTest, exit_request_releases_infinite_wait_and_stays_sticky)
{
    CoverageDeadline deadline;
    CoverageGate waiting;
    CoverageThread probe("CoverageExitWait", [&]()
        {
            waiting.arrive();
            EXPECT_FALSE(areg::Thread::wait_exit(areg::WAIT_INFINITE));
            EXPECT_FALSE(areg::Thread::wait_exit(areg::DO_NOT_WAIT));
            EXPECT_FALSE(areg::Thread::wait_exit(areg::WAIT_INFINITE));
        });
    ASSERT_TRUE(probe.mThread.start(COVERAGE_WAIT_MS));
    const bool ready{ waiting.wait() };
    probe.mThread.request_exit();
    EXPECT_TRUE(ready);
    EXPECT_TRUE(probe.mEntered.wait());
    probe.mRelease.arrive();
    EXPECT_TRUE(probe.mThread.wait_completion(COVERAGE_WAIT_MS));
    EXPECT_EQ(probe.mThread.shutdown(), areg::Thread::ThreadCompletion::Completed);
    EXPECT_EQ(probe.mRuns.load(), 1u);
}

TEST(ThreadApiCoverageTest, foreign_thread_has_no_framework_identity)
{
    CoverageDeadline deadline;
    std::thread foreign([]()
        {
            EXPECT_EQ(areg::Thread::current_thread(), nullptr);
            EXPECT_TRUE(areg::Thread::current_thread_name().is_empty());
            EXPECT_FALSE(areg::Thread::current_thread_address().is_valid());
            EXPECT_EQ(areg::Thread::current_stack_size(), 0u);
            EXPECT_TRUE(areg::Thread::wait_exit(areg::DO_NOT_WAIT));
        });
    foreign.join();
}

TEST(ThreadStorageCoverageTest, all_value_types_replace_remove_and_clear)
{
    CoverageDeadline deadline;
    areg::ThreadConsumer* expectedConsumer{ nullptr };
    CoverageThread probe("CoverageStorage", [&expectedConsumer]()
        {
            auto& tls{ areg::Thread::current_thread_storage() };
            auto* self{ areg::Thread::current_thread() };
            EXPECT_EQ(&tls.owner_thread(), self);
            EXPECT_EQ(tls.name(), self->name());
            EXPECT_EQ(&areg::Thread::current_thread_consumer(), expectedConsumer);
            EXPECT_FALSE(self->startup_phase());
            self->set_startup_phase(true);
            EXPECT_TRUE(self->startup_phase());
            self->set_startup_phase(false);
            EXPECT_FALSE(self->startup_phase());
            const uint32_t baseline{ tls.size() };
            EXPECT_FALSE(tls.exist("absent"));
            EXPECT_EQ(tls.item("absent").valUInt64.mElement, areg::InvalidElement.valUInt64.mElement);
            EXPECT_EQ(tls.remove_item("absent").valUInt64.mElement, 0u);
            tls.set_item("u32", uint32_t{ 17u });
            tls.set_item("u64", uint64_t{ 0xFEDCBA9876543210ULL });
            tls.set_item("double", 1.25);
            uint32_t marker{ 42u };
            tls.set_item("pointer", &marker);
            areg::Primitive primitive{};
            primitive.valUInt64.mElement = 0x123456789ABCDEF0ULL;
            tls.set_item("primitive", primitive);
            EXPECT_EQ(tls.size(), baseline + 5u);
            EXPECT_TRUE(tls.exist("u32"));
            EXPECT_TRUE(tls.exist("primitive"));
            EXPECT_EQ(tls.item("u32").valUInt.mElement, 17u);
            EXPECT_EQ(tls.item("u64").valUInt64.mElement, 0xFEDCBA9876543210ULL);
            EXPECT_DOUBLE_EQ(tls.item("double").valDouble.mElement, 1.25);
            EXPECT_EQ(tls.item("pointer").valPtr.mElement, &marker);
            tls.set_item("pointer", static_cast<const void*>(nullptr));
            EXPECT_EQ(tls.item("pointer").valPtr.mElement, nullptr);
            EXPECT_EQ(tls.item("primitive").valUInt64.mElement, primitive.valUInt64.mElement);
            tls.set_item("u64", uint64_t{ 99u });
            EXPECT_EQ(tls.size(), baseline + 5u);
            EXPECT_EQ(tls.item("u64").valUInt64.mElement, 99u);
            EXPECT_EQ(tls.remove_item("u32").valUInt.mElement, 17u);
            EXPECT_FALSE(tls.exist("u32"));
            EXPECT_EQ(tls.remove_item("primitive").valUInt64.mElement, primitive.valUInt64.mElement);
            EXPECT_EQ(tls.size(), baseline + 3u);
            tls.clear();
            EXPECT_EQ(tls.size(), 0u);
            EXPECT_FALSE(tls.exist("double"));
            tls.clear();
            EXPECT_EQ(tls.remove_item("absent").valUInt64.mElement, 0u);
            tls.set_item(areg::String::empty_string(), uint32_t{ 7u });
            EXPECT_EQ(tls.size(), 1u);
            EXPECT_TRUE(tls.exist(areg::String::empty_string()));
            EXPECT_EQ(tls.remove_item(areg::String::empty_string()).valUInt.mElement, 7u);
            EXPECT_EQ(tls.size(), 0u);
        });
    expectedConsumer = &probe;
    ASSERT_TRUE(probe.mThread.start(COVERAGE_WAIT_MS));
    EXPECT_TRUE(probe.mEntered.wait());
    probe.mRelease.arrive();
    EXPECT_TRUE(probe.mThread.wait_completion(COVERAGE_WAIT_MS));
}

TEST(ThreadStorageCoverageTest, simultaneous_threads_keep_identical_keys_isolated)
{
    CoverageDeadline deadline;
    CoverageGate stored;
    auto action = [&stored](uint64_t value)
        {
            auto& tls{ areg::Thread::current_thread_storage() };
            tls.set_item("shared-key", value);
            stored.arrive();
            EXPECT_TRUE(stored.wait(2u));
            EXPECT_EQ(tls.item("shared-key").valUInt64.mElement, value);
            EXPECT_EQ(tls.remove_item("shared-key").valUInt64.mElement, value);
            EXPECT_FALSE(tls.exist("shared-key"));
        };
    CoverageThread first("CoverageStorageOne", [&]() { action(11u); });
    CoverageThread second("CoverageStorageTwo", [&]() { action(22u); });
    ASSERT_TRUE(first.mThread.start(COVERAGE_WAIT_MS));
    ASSERT_TRUE(second.mThread.start(COVERAGE_WAIT_MS));
    EXPECT_TRUE(first.mEntered.wait());
    EXPECT_TRUE(second.mEntered.wait());
    first.mRelease.arrive();
    second.mRelease.arrive();
    EXPECT_TRUE(first.mThread.wait_completion(COVERAGE_WAIT_MS));
    EXPECT_TRUE(second.mThread.wait_completion(COVERAGE_WAIT_MS));
}

TEST(ThreadAddressCoverageTest, invalid_names_and_numeric_zero_stay_invalid)
{
    const areg::ThreadAddress empty;
    const areg::ThreadAddress nullName(static_cast<const char*>(nullptr));
    const areg::ThreadAddress emptyName("");
    const areg::ThreadAddress reservedName("INVALID_THREAD_NAME");
    const areg::ThreadAddress zero(UniqueNumber{ areg::CHECKSUM_IGNORE });
    EXPECT_FALSE(empty.is_valid());
    EXPECT_FALSE(nullName.is_valid());
    EXPECT_FALSE(emptyName.is_valid());
    EXPECT_FALSE(reservedName.is_valid());
    EXPECT_FALSE(zero.is_valid());
    EXPECT_EQ(empty, areg::ThreadAddress::invalid_thread_address());
    EXPECT_EQ(&areg::ThreadAddress::invalid_thread_address(), &areg::ThreadAddress::invalid_thread_address());
}

TEST(ThreadAddressCoverageTest, copy_move_and_self_assignment_preserve_identity)
{
    const areg::ThreadAddress original("CoverageAddress");
    ASSERT_TRUE(original.is_valid());
    areg::ThreadAddress copy(original);
    EXPECT_EQ(copy.name(), original.name());
    EXPECT_EQ(copy, original);
    areg::ThreadAddress moved(std::move(copy));
    EXPECT_EQ(moved, original);
    EXPECT_FALSE(copy.is_valid());
    copy = original;
    EXPECT_EQ(copy, original);
    areg::ThreadAddress assigned("OtherAddress");
    assigned = std::move(copy);
    EXPECT_EQ(assigned, original);
    EXPECT_FALSE(copy.is_valid());
    auto& alias{ assigned };
    assigned = alias;
    EXPECT_EQ(assigned, original);
    assigned = std::move(alias);
    EXPECT_EQ(assigned, original);
    EXPECT_FALSE(original != assigned);
    const areg::ThreadAddress numeric(UniqueNumber{ 1u });
    EXPECT_EQ(original < numeric, static_cast<uint32_t>(original) < static_cast<uint32_t>(numeric));
    EXPECT_EQ(original > numeric, static_cast<uint32_t>(original) > static_cast<uint32_t>(numeric));
}

TEST(ThreadAddressCoverageTest, parsing_consumes_only_one_path_item)
{
    const char* next{ nullptr };
    const areg::ThreadAddress named{ areg::ThreadAddress::from_path("CoveragePath::remaining::tail", &next) };
    EXPECT_EQ(named, areg::ThreadAddress("CoveragePath"));
    ASSERT_NE(next, nullptr);
    EXPECT_STREQ(next, "remaining::tail");
    areg::ThreadAddress parsed("Stale");
    parsed.from_string("#12345::rest", &next);
    EXPECT_EQ(static_cast<uint32_t>(parsed), 12345u);
    EXPECT_TRUE(parsed.name().is_empty());
    EXPECT_STREQ(next, "rest");
    parsed.from_string("Replacement", &next);
    EXPECT_EQ(parsed, areg::ThreadAddress("Replacement"));
    EXPECT_EQ(parsed.to_string(), "Replacement");
    EXPECT_TRUE((next == nullptr) || (*next == '\0'));
}
