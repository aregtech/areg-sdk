/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/WorkerThreadTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests for the worker threads a model registers.
 *              Covers the consumer name a component is asked for, the worker
 *              thread it starts or does not start, an event the component sends
 *              running on the worker thread, and the worker stopping on unload.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/EventTemplate.hpp"
#include "areg/component/WorkerThread.hpp"
#include "areg/component/WorkerThreadConsumer.hpp"

#include <chrono>
#include <condition_variable>
#include <map>
#include <mutex>
#include <string>
#include <thread>

//! The value an event carries to the worker thread.
class WtWorkData
{
public:
    WtWorkData() = default;
    explicit WtWorkData(int value) : mValue(value) {}
    WtWorkData(const WtWorkData & src) = default;
    WtWorkData & operator = (const WtWorkData & src) = default;

    inline int value() const { return mValue; }

private:
    int mValue{ 0 };
};

AREG_DECLARE_EVENT(WtWorkData, WtWorkEvent, WtWorkEventConsumer);

namespace
{
    //!< The worker consumer name the model registers for every role but "WtEntry".
    constexpr char const _consumer[]    { "WtConsumer" };
    //!< The worker thread name the model registers for every role.
    constexpr char const _worker[]      { "WtWorkerThread" };
    //!< The model every test loads.
    constexpr char const _model[]       { "WtModel" };
    //!< How long a test waits for something to happen.
    constexpr std::chrono::seconds      _waitLimit{ 5 };

    //! What one component and its worker report to the test.
    struct Seen
    {
        std::string     askedName;
        std::string     workerName;
        bool            started{ false };
        bool            workerStarted{ false };
        bool            workerStopped{ false };
        int             processed{ 0 };
        std::thread::id componentThread{};
        std::thread::id workerThread{};
    };

    //! What every component reports, by role.
    class Observed
    {
    public:
        void reset()
        {
            std::lock_guard<std::mutex> guard(mLock);
            mSeen.clear();
        }

        template<typename Change>
        void update(const std::string & role, Change change)
        {
            {
                std::lock_guard<std::mutex> guard(mLock);
                change(mSeen[role]);
            }

            mChanged.notify_all();
        }

        template<typename Condition>
        bool wait(const std::string & role, Condition condition)
        {
            std::unique_lock<std::mutex> guard(mLock);
            return mChanged.wait_for(guard, _waitLimit, [&]() { return condition(mSeen[role]); });
        }

        Seen copy(const std::string & role)
        {
            std::lock_guard<std::mutex> guard(mLock);
            return mSeen[role];
        }

    private:
        std::mutex                  mLock;
        std::condition_variable     mChanged;
        std::map<std::string, Seen> mSeen;
    };

    Observed & observed()
    {
        static Observed _observed;
        return _observed;
    }

    //! Runs on the worker thread and counts the events it receives.
    class WtWorker final    : public    areg::WorkerThreadConsumer
                            , public    WtWorkEventConsumer
    {
    public:
        WtWorker(const areg::String & consumerName, const areg::String & role)
            : areg::WorkerThreadConsumer(consumerName)
            , mRole(role.as_string())
        { }

    protected:
        void register_event_consumers(areg::WorkerThread & /*workThread*/, areg::ComponentThread & /*masterThread*/) final
        {
            observed().update(mRole, [](Seen & seen) { seen.workerThread = std::this_thread::get_id(); });
        }

        void unregister_event_consumers(areg::WorkerThread & /*workThread*/) final
        {
            observed().update(mRole, [](Seen & seen) { seen.workerStopped = true; });
        }

        void process_event(const WtWorkData & data) final
        {
            const std::thread::id here{ std::this_thread::get_id() };
            const int value{ data.value() };
            observed().update(mRole, [here, value](Seen & seen)
                {
                    if (here == seen.workerThread)
                    {
                        seen.processed += value;
                    }
                });
        }

    private:
        const std::string   mRole;
    };

    //! Registers one worker thread and answers the consumer name as its role says:
    //! "WtEntry" and "WtTwin" with the name the model entry holds, "WtQualified" with the role and
    //! the registered name joined, "WtLiteral" with the registered name as written,
    //! "WtUnknown" with a name the model never registers.
    class WtHost final : public areg::Component
    {
    public:
        WtHost(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
            : areg::Component(entry, owner)
            , mRole(entry.mRoleName.as_string())
            , mWorker(_answer(entry), entry.mRoleName)
        { }

    protected:
        areg::WorkerThreadConsumer * worker_thread_consumer(const areg::String & consumerName,
                                                            const areg::String & workerThreadName) final
        {
            const std::string asked{ consumerName.as_string() };
            observed().update(mRole, [asked](Seen & seen) { seen.askedName = asked; });
            return mWorker.consumer_name() == consumerName
                 ? &mWorker
                 : areg::Component::worker_thread_consumer(consumerName, workerThreadName);
        }

        void notify_thread_started(areg::WorkerThreadConsumer & consumer, areg::WorkerThread & workerThread) final
        {
            if (&consumer == &mWorker)
            {
                mWorkerThread = &workerThread;
                WtWorkEvent::add_listener(static_cast<WtWorkEventConsumer &>(mWorker), workerThread);
                const std::string name{ workerThread.name().as_string() };
                observed().update(mRole, [name](Seen & seen)
                    {
                        seen.workerStarted = true;
                        seen.workerName = name;
                    });
            }
        }

        void startup_component(areg::ComponentThread & comThread) final
        {
            areg::Component::startup_component(comThread);
            observed().update(mRole, [](Seen & seen)
                {
                    seen.started = true;
                    seen.componentThread = std::this_thread::get_id();
                });

            if (mWorkerThread != nullptr)
            {
                WtWorkEvent::send_event(WtWorkData(1), static_cast<areg::DispatcherThread &>(*mWorkerThread));
            }
        }

        void shutdown_component(areg::ComponentThread & comThread) final
        {
            if (mWorkerThread != nullptr)
            {
                WtWorkEvent::remove_listener(static_cast<WtWorkEventConsumer &>(mWorker), *mWorkerThread);
            }

            areg::Component::shutdown_component(comThread);
        }

    private:
        static areg::String _answer(const areg::ComponentEntry & entry)
        {
            const areg::String & role{ entry.mRoleName };
            if ((role == "WtEntry") || (role == "WtTwin"))
            {
                return entry.mWorkerThreads[0].mConsumerName;
            }
            else if (role == "WtQualified")
            {
                return areg::create_component_item_name(role, _consumer);
            }
            else if (role == "WtLiteral")
            {
                return areg::String(_consumer);
            }

            return areg::String("WtNobodyRegistersThis");
        }

        const std::string       mRole;
        WtWorker                mWorker;
        areg::WorkerThread *    mWorkerThread{ nullptr };
    };

    //! Starts the application without logging and routing, runs the model, and stops.
    class WtModelRun
    {
    public:
        WtModelRun()
        {
            observed().reset();
            areg::Application::setup(false, true, false, true, false);
            mLoaded = areg::Application::load_model(_model);
        }

        ~WtModelRun()
        {
            areg::Application::unload_model(_model);
            areg::Application::release();
        }

        inline bool loaded() const { return mLoaded; }

    private:
        bool    mLoaded{ false };
    };
}

BEGIN_MODEL(_model)
    BEGIN_REGISTER_THREAD("WtEntryThread")
        BEGIN_REGISTER_COMPONENT("WtEntry", WtHost)
            REGISTER_WORKER_THREAD(_worker, "WtAnyRegisteredName")
        END_REGISTER_COMPONENT("WtEntry")
    END_REGISTER_THREAD("WtEntryThread")
    BEGIN_REGISTER_THREAD("WtTwinThread")
        BEGIN_REGISTER_COMPONENT("WtTwin", WtHost)
            REGISTER_WORKER_THREAD(_worker, "WtAnyRegisteredName")
        END_REGISTER_COMPONENT("WtTwin")
    END_REGISTER_THREAD("WtTwinThread")
    BEGIN_REGISTER_THREAD("WtQualifiedThread")
        BEGIN_REGISTER_COMPONENT("WtQualified", WtHost)
            REGISTER_WORKER_THREAD(_worker, _consumer)
        END_REGISTER_COMPONENT("WtQualified")
    END_REGISTER_THREAD("WtQualifiedThread")
    BEGIN_REGISTER_THREAD("WtLiteralThread")
        BEGIN_REGISTER_COMPONENT("WtLiteral", WtHost)
            REGISTER_WORKER_THREAD(_worker, _consumer)
        END_REGISTER_COMPONENT("WtLiteral")
    END_REGISTER_THREAD("WtLiteralThread")
    BEGIN_REGISTER_THREAD("WtUnknownThread")
        BEGIN_REGISTER_COMPONENT("WtUnknown", WtHost)
            REGISTER_WORKER_THREAD(_worker, _consumer)
        END_REGISTER_COMPONENT("WtUnknown")
    END_REGISTER_THREAD("WtUnknownThread")
END_MODEL(_model)

/**
 * \brief   A component answering with the consumer name of its model entry gets its worker
 *          thread, whatever name the model registered; an event it sends runs on that thread,
 *          and unloading the model stops the worker.
 **/
TEST(WorkerThreadTest, EntryNameStartsTheWorker)
{
    {
        WtModelRun run;
        ASSERT_TRUE(run.loaded());
        ASSERT_TRUE(observed().wait("WtEntry", [](Seen & seen) { return seen.processed == 1; }));

        const Seen seen{ observed().copy("WtEntry") };
        EXPECT_EQ(seen.askedName, "WtEntry:WtAnyRegisteredName");
        EXPECT_TRUE(seen.workerStarted);
        EXPECT_EQ(seen.workerName, "WtEntryThread:WtWorkerThread");
        EXPECT_NE(seen.workerThread, std::thread::id{});
        EXPECT_NE(seen.workerThread, seen.componentThread);
    }

    EXPECT_TRUE(observed().copy("WtEntry").workerStopped);
}

/**
 * \brief   A second component of the same class, registering the same worker names under
 *          another role, gets a worker thread of its own.
 **/
TEST(WorkerThreadTest, SecondRoleGetsItsOwnWorker)
{
    WtModelRun run;
    ASSERT_TRUE(run.loaded());
    ASSERT_TRUE(observed().wait("WtEntry", [](Seen & seen) { return seen.processed == 1; }));
    ASSERT_TRUE(observed().wait("WtTwin", [](Seen & seen) { return seen.processed == 1; }));

    const Seen first{ observed().copy("WtEntry") };
    const Seen second{ observed().copy("WtTwin") };
    EXPECT_EQ(second.askedName, "WtTwin:WtAnyRegisteredName");
    EXPECT_EQ(second.workerName, "WtTwinThread:WtWorkerThread");
    EXPECT_NE(second.workerThread, first.workerThread);
}

/**
 * \brief   The name a component is asked for is its role and the registered name joined by
 *          areg::COMPONENT_ITEM_SEPARATOR; answering with that starts the worker.
 **/
TEST(WorkerThreadTest, QualifiedNameStartsTheWorker)
{
    WtModelRun run;
    ASSERT_TRUE(run.loaded());
    ASSERT_TRUE(observed().wait("WtQualified", [](Seen & seen) { return seen.processed == 1; }));

    const Seen seen{ observed().copy("WtQualified") };
    EXPECT_EQ(seen.askedName, "WtQualified:WtConsumer");
    EXPECT_TRUE(seen.workerStarted);
}

/**
 * \brief   The registered name as written is not the name the component is asked for, so a
 *          component answering with it gets no worker thread, and nothing reports it.
 **/
TEST(WorkerThreadTest, LiteralNameStartsNoWorker)
{
    WtModelRun run;
    ASSERT_TRUE(run.loaded());
    ASSERT_TRUE(observed().wait("WtLiteral", [](Seen & seen) { return seen.started; }));

    const Seen seen{ observed().copy("WtLiteral") };
    EXPECT_EQ(seen.askedName, "WtLiteral:WtConsumer");
    EXPECT_FALSE(seen.workerStarted);
    EXPECT_EQ(seen.workerThread, std::thread::id{});
    EXPECT_EQ(seen.processed, 0);
}

/**
 * \brief   A component that answers to no registered name gets no worker thread, and the
 *          component itself still starts.
 **/
TEST(WorkerThreadTest, UnknownNameStartsNoWorker)
{
    WtModelRun run;
    ASSERT_TRUE(run.loaded());
    ASSERT_TRUE(observed().wait("WtUnknown", [](Seen & seen) { return seen.started; }));

    const Seen seen{ observed().copy("WtUnknown") };
    EXPECT_EQ(seen.askedName, "WtUnknown:WtConsumer");
    EXPECT_FALSE(seen.workerStarted);
    EXPECT_EQ(seen.processed, 0);
}
