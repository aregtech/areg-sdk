/**
 * \file    proxy_attach_test.cpp
 * \brief   Clients that attach to a shared proxy: before and after it is connected, deleted
 *          before their notification, across a provider reload, and from a foreign thread,
 *          which is refused. Every client counts its own connection callbacks.
 *
 * Usage: areg-proxy-attach-test <case>
 *   same-thread            late clients created on the proxy thread
 *   foreign-thread         a client created on the main thread for the proxy thread is refused
 *   deleted-before-notify  late clients deleted before their notification, then one kept
 *   reconnect              a late client across a provider unload and load
 *   before-connect         a client created before the provider is loaded
 */

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/Process.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "examples/01_minimalrpc/services/HelloServiceConsumerBase.hpp"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>

#if defined(_MSC_VER) && defined(DEBUG)
    #include <crtdbg.h>
#endif // defined(_MSC_VER) && defined(DEBUG)

#ifndef _WIN32
    #include <sys/wait.h>
#endif // !_WIN32

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "01_generated")
#endif // _MSC_VER

namespace
{
    constexpr uint32_t  MAX_CLIENTS     { 8 };
    constexpr uint32_t  KEEPER_SLOT     { MAX_CLIENTS };
    constexpr uint32_t  SLOT_COUNT      { MAX_CLIENTS + 1 };
    constexpr uint32_t  DELETED_ROUNDS  { 32 };
    constexpr int       WAIT_STEP_MS    { 10 };
    constexpr int       WAIT_LIMIT_MS   { 5000 };
    constexpr int       SETTLE_MS       { 300 };

    const char * const  ROLE_NAME       { "ServiceProvider" };
    const char * const  CONSUMER_THREAD { "AttachConsumerThread" };

    enum class TestCase { None, SameThread, ForeignThread, ForeignThreadChild, DeletedBeforeNotify, Reconnect, BeforeConnect };

    TestCase                gCase           { TestCase::None };
    std::atomic_uint32_t    gConnects[SLOT_COUNT]   { };
    std::atomic_uint32_t    gDisconnects[SLOT_COUNT]{ };
    std::atomic_bool        gAnchorConnected{ false };
    std::atomic_bool        gActionDone     { false };

    //!< Waits until the predicate is true or the limit expires. Returns the predicate.
    template<typename Predicate>
    bool wait_for(Predicate pred)
    {
        for (int waited = 0; (waited < WAIT_LIMIT_MS) && !pred(); waited += WAIT_STEP_MS)
            std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_STEP_MS));

        return pred();
    }

    void settle()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(SETTLE_MS));
    }

    //!< Checks the callback counts of a slot range. Prints every mismatch if requested.
    bool check_slots(uint32_t first, uint32_t last, uint32_t connects, uint32_t disconnects, bool report = true)
    {
        bool ok{ true };
        for (uint32_t i = first; i < last; ++i)
        {
            const uint32_t c{ gConnects[i].load() };
            const uint32_t d{ gDisconnects[i].load() };
            if ((c != connects) || (d != disconnects))
            {
                if (report)
                    std::printf("slot %u: connects %u (expected %u), disconnects %u (expected %u)\n", i, c, connects, d, disconnects);
                ok = false;
            }
        }

        return ok;
    }

    //!< Waits until a slot range has the given counts, then checks and reports it.
    bool expect_slots(uint32_t first, uint32_t last, uint32_t connects, uint32_t disconnects)
    {
        static_cast<void>(wait_for([=]() { return check_slots(first, last, connects, disconnects, false); }));
        return check_slots(first, last, connects, disconnects);
    }
}

//!< Client of the shared proxy. Counts its own connection callbacks in its slot.
class AttachClient final : public HelloServiceConsumerBase
{
public:
    AttachClient(uint32_t slot, areg::DispatcherThread & ownerThread)
        : HelloServiceConsumerBase(ROLE_NAME, ownerThread)
        , mSlot(slot)
    {
    }

    AttachClient(uint32_t slot, const areg::String & ownerThread)
        : HelloServiceConsumerBase(ROLE_NAME, ownerThread)
        , mSlot(slot)
    {
    }

    //!< Returns true if the client has a proxy.
    bool is_attached() noexcept
    {
        return (dispatcher_thread() != nullptr);
    }

    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) override
    {
        const bool relevant{ HelloServiceConsumerBase::service_connected(status, proxy) };
        if (relevant)
        {
            if (areg::is_service_connected(status))
                gConnects[mSlot].fetch_add(1);
            else
                gDisconnects[mSlot].fetch_add(1);
        }

        return relevant;
    }

private:
    const uint32_t  mSlot;
};

//!< First client of the proxy. Runs the same-thread actions of a case when it is connected.
class AnchorComponent final : public    areg::Component
                            , protected HelloServiceConsumerBase
{
public:
    AnchorComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , HelloServiceConsumerBase  ( entry.mDependencyServices[0].mRoleName, owner )
        , mClients                  { }
    {
    }

    virtual ~AnchorComponent() override
    {
        for (AttachClient *& client : mClients)
        {
            delete client;
            client = nullptr;
        }
    }

    virtual void startup_component(areg::ComponentThread & comThread) override
    {
        areg::Component::startup_component(comThread);
        if (gCase == TestCase::BeforeConnect)
            mClients[0] = new AttachClient(0, comThread);
    }

    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) override
    {
        const bool relevant{ HelloServiceConsumerBase::service_connected(status, proxy) };
        if (relevant && areg::is_service_connected(status) && !gAnchorConnected.exchange(true))
        {
            run_action();
            gActionDone.store(true);
        }

        return relevant;
    }

private:
    void run_action()
    {
        areg::DispatcherThread & thread{ static_cast<areg::DispatcherThread &>(master_thread()) };
        switch (gCase)
        {
        case TestCase::SameThread:
            for (uint32_t i = 0; i < MAX_CLIENTS; ++i)
                mClients[i] = new AttachClient(i, thread);
            break;

        case TestCase::DeletedBeforeNotify:
            for (uint32_t round = 0; round < DELETED_ROUNDS; ++round)
            {
                for (uint32_t i = 0; i < MAX_CLIENTS; ++i)
                    mClients[i] = new AttachClient(i, thread);

                for (uint32_t i = 0; i < MAX_CLIENTS; ++i)
                {
                    delete mClients[i];
                    mClients[i] = nullptr;
                }
            }

            mClients[0] = new AttachClient(KEEPER_SLOT, thread);
            break;

        case TestCase::Reconnect:
            mClients[0] = new AttachClient(0, thread);
            break;

        case TestCase::ForeignThread:
        case TestCase::ForeignThreadChild:
            mClients[1] = new AttachClient(1, thread);
            break;

        default:
            break;
        }
    }

    AttachClient *  mClients[MAX_CLIENTS];
};

BEGIN_MODEL("AttachConsumerModel")
    BEGIN_REGISTER_THREAD( "AttachConsumerThread" )
        BEGIN_REGISTER_COMPONENT( "AttachAnchor", AnchorComponent )
            REGISTER_DEPENDENCY( "ServiceProvider" )
        END_REGISTER_COMPONENT( "AttachAnchor" )
    END_REGISTER_THREAD( "AttachConsumerThread" )
END_MODEL("AttachConsumerModel")

namespace
{
    bool run_same_thread()
    {
        areg::Application::load_model("AttachConsumerModel");
        areg::Application::load_model("AttachProviderModel");
        bool ok{ wait_for([]() { return gActionDone.load(); }) };
        ok = expect_slots(0, MAX_CLIENTS, 1, 0) && ok;
        settle();
        ok = check_slots(0, MAX_CLIENTS, 1, 0) && ok;

        areg::Application::unload_model("AttachProviderModel");
        ok = expect_slots(0, MAX_CLIENTS, 1, 1) && ok;
        areg::Application::unload_model("AttachConsumerModel");
        return check_slots(0, MAX_CLIENTS, 1, 1) && ok;
    }

    //!< Creates a client in the main thread for the consumer thread. Returns true if it is refused.
    bool attach_from_foreign_thread()
    {
        AttachClient * client{ new AttachClient(0, areg::String(CONSUMER_THREAD)) };
        const bool refused{ client->is_attached() == false };
        delete client;
        if (refused == false)
            std::printf("the client of a foreign thread got a proxy\n");

        return refused;
    }

    //!< Release build: the attempt is refused, and the proxy and its valid client are unaffected.
    bool run_foreign_thread_inline()
    {
        areg::Application::load_model("AttachConsumerModel");
        areg::Application::load_model("AttachProviderModel");
        bool ok{ wait_for([]() { return gActionDone.load(); }) };
        ok = expect_slots(1, 2, 1, 0) && ok;

        ok = attach_from_foreign_thread() && ok;
        settle();
        ok = check_slots(0, 1, 0, 0) && ok;
        ok = check_slots(1, 2, 1, 0) && ok;

        areg::Application::unload_model("AttachProviderModel");
        ok = expect_slots(1, 2, 1, 1) && ok;
        areg::Application::unload_model("AttachConsumerModel");
        ok = check_slots(0, 1, 0, 0) && ok;
        return check_slots(1, 2, 1, 1) && ok;
    }

    //!< Debug build child: the attempt must stop the process at the assertion.
    int run_foreign_thread_child()
    {
#if defined(_MSC_VER) && defined(DEBUG)
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#endif // defined(_MSC_VER) && defined(DEBUG)

        areg::Application::load_model("AttachConsumerModel");
        areg::Application::load_model("AttachProviderModel");
        static_cast<void>(wait_for([]() { return gActionDone.load(); }));
        static_cast<void>(attach_from_foreign_thread());
        std::printf("the foreign thread attempt did not stop the process\n");
        areg::Application::unload_model("AttachProviderModel");
        areg::Application::unload_model("AttachConsumerModel");
        areg::Application::release();
        return 0;
    }

#ifdef DEBUG
    //!< Debug build: runs the child and expects it to be stopped by the assertion.
    bool run_foreign_thread_assert()
    {
        const std::string command{ std::string("\"") + areg::Process::instance().full_path().as_string() + "\" foreign-thread-child" };
        const int status{ std::system(command.c_str()) };
#ifdef _WIN32
        const bool stopped{ (status != 0) && (status != 2) };
#else   // _WIN32
        const bool stopped{ (WIFEXITED(status) == false) || ((WEXITSTATUS(status) != 0) && (WEXITSTATUS(status) != 2)) };
#endif  // _WIN32
        std::printf("child foreign-thread-child: status %d, %s\n", status, stopped ? "stopped" : "not stopped");
        return stopped;
    }
#endif // DEBUG

    bool run_deleted_before_notify()
    {
        areg::Application::load_model("AttachConsumerModel");
        areg::Application::load_model("AttachProviderModel");
        bool ok{ wait_for([]() { return gActionDone.load(); }) };
        ok = expect_slots(KEEPER_SLOT, SLOT_COUNT, 1, 0) && ok;
        settle();
        ok = check_slots(0, MAX_CLIENTS, 0, 0) && ok;
        ok = check_slots(KEEPER_SLOT, SLOT_COUNT, 1, 0) && ok;

        areg::Application::unload_model("AttachProviderModel");
        ok = expect_slots(KEEPER_SLOT, SLOT_COUNT, 1, 1) && ok;
        areg::Application::unload_model("AttachConsumerModel");
        ok = check_slots(0, MAX_CLIENTS, 0, 0) && ok;
        return check_slots(KEEPER_SLOT, SLOT_COUNT, 1, 1) && ok;
    }

    bool run_reconnect()
    {
        areg::Application::load_model("AttachConsumerModel");
        areg::Application::load_model("AttachProviderModel");
        bool ok{ wait_for([]() { return gActionDone.load(); }) };
        ok = expect_slots(0, 1, 1, 0) && ok;

        areg::Application::unload_model("AttachProviderModel");
        ok = expect_slots(0, 1, 1, 1) && ok;

        areg::Application::load_model("AttachProviderModel");
        ok = expect_slots(0, 1, 2, 1) && ok;
        settle();
        ok = check_slots(0, 1, 2, 1) && ok;

        areg::Application::unload_model("AttachProviderModel");
        ok = expect_slots(0, 1, 2, 2) && ok;
        areg::Application::unload_model("AttachConsumerModel");
        return check_slots(0, 1, 2, 2) && ok;
    }

    bool run_before_connect()
    {
        areg::Application::load_model("AttachConsumerModel");
        settle();
        bool ok{ check_slots(0, 1, 0, 0) };

        areg::Application::load_model("AttachProviderModel");
        ok = expect_slots(0, 1, 1, 0) && ok;
        settle();
        ok = check_slots(0, 1, 1, 0) && ok;

        areg::Application::unload_model("AttachProviderModel");
        ok = expect_slots(0, 1, 1, 1) && ok;
        areg::Application::unload_model("AttachConsumerModel");
        return check_slots(0, 1, 1, 1) && ok;
    }
}

int main(int argc, char * argv[])
{
    const char * name{ argc > 1 ? argv[1] : "" };
    if (std::strcmp(name, "same-thread") == 0)
        gCase = TestCase::SameThread;
    else if (std::strcmp(name, "foreign-thread") == 0)
        gCase = TestCase::ForeignThread;
    else if (std::strcmp(name, "foreign-thread-child") == 0)
        gCase = TestCase::ForeignThreadChild;
    else if (std::strcmp(name, "deleted-before-notify") == 0)
        gCase = TestCase::DeletedBeforeNotify;
    else if (std::strcmp(name, "reconnect") == 0)
        gCase = TestCase::Reconnect;
    else if (std::strcmp(name, "before-connect") == 0)
        gCase = TestCase::BeforeConnect;

    if (gCase == TestCase::None)
    {
        std::printf("Usage: areg-proxy-attach-test <same-thread|foreign-thread|deleted-before-notify|reconnect|before-connect>\n");
        return 2;
    }

    if (gCase == TestCase::ForeignThread)
    {
#ifdef DEBUG
        const bool stopped{ run_foreign_thread_assert() };
        std::printf("%s: %s\n", name, stopped ? "PASS" : "FAIL");
        return stopped ? 0 : 1;
#endif // DEBUG
    }

    areg::Application::setup();

    bool ok{ false };
    switch (gCase)
    {
    case TestCase::SameThread:          ok = run_same_thread();             break;
    case TestCase::ForeignThread:       ok = run_foreign_thread_inline();   break;
    case TestCase::ForeignThreadChild:  return run_foreign_thread_child();
    case TestCase::DeletedBeforeNotify: ok = run_deleted_before_notify();   break;
    case TestCase::Reconnect:           ok = run_reconnect();               break;
    case TestCase::BeforeConnect:       ok = run_before_connect();          break;
    default:                                                                break;
    }

    areg::Application::release();

    std::printf("%s: %s\n", name, ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
