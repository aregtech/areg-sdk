/**
 * \file    proxy_churn_test.cpp
 * \brief   Dynamic client churn against a single proxy.
 *
 *          One proxy, one long living client, and many short living clients that
 *          are created and deleted on the heap while the service is down and
 *          while it is up. Deleted clients poison their own memory, so a stale
 *          entry left in the proxy connection listener list is a hard crash on
 *          the next connect or disconnect edge instead of a silent leak.
 */

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "examples/01_minimalrpc/services/HelloServiceProviderBase.hpp"
#include "examples/01_minimalrpc/services/HelloServiceConsumerBase.hpp"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <thread>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "01_generated")
#endif // _MSC_VER

namespace
{
    constexpr uint32_t  CHURN_ROUNDS { 25 };
    constexpr uint32_t  CHURN_CLIENTS{ 40 };

    std::atomic_uint32_t    gChurnCreated   { 0 };
    std::atomic_uint32_t    gChurnDeleted   { 0 };
    std::atomic_uint32_t    gChurnCallbacks { 0 };
    std::atomic_uint32_t    gLiveConnects   { 0 };
    std::atomic_uint32_t    gLiveDisconnects{ 0 };
    std::atomic_bool        gChurnDone      { false };
}

//!< Short living client. Created and deleted at runtime.
class ChurnClient final : public HelloServiceConsumerBase
{
public:
    ChurnClient(const areg::String & roleName, areg::DispatcherThread & ownerThread)
        : HelloServiceConsumerBase(roleName, ownerThread)
    {
        gChurnCreated.fetch_add(1);
    }

    virtual ~ChurnClient() override
    {
        gChurnDeleted.fetch_add(1);
    }

    //!< Fills the released block, so a stale listener entry faults immediately.
    static void operator delete (void * block, size_t size)
    {
        std::memset(block, 0xDD, size);
        ::operator delete(block);
    }

    static void * operator new (size_t size)
    {
        return ::operator new(size);
    }

    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) override
    {
        gChurnCallbacks.fetch_add(1);
        return HelloServiceConsumerBase::service_connected(status, proxy);
    }
};

//!< Long living client. Stays for the whole run and drives the churn.
class ChurnComponent final : public    areg::Component
                           , protected HelloServiceConsumerBase
{
public:
    ChurnComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , HelloServiceConsumerBase  ( entry.mDependencyServices[0].mRoleName, owner )
        , mRoleName                 ( entry.mDependencyServices[0].mRoleName )
        , mChurnedOnline            ( false )
    {
    }

    virtual void startup_component(areg::ComponentThread & comThread) override
    {
        areg::Component::startup_component(comThread);
        run_churn();    // service is still down here
    }

    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) override
    {
        const bool relevant{ HelloServiceConsumerBase::service_connected(status, proxy) };
        if (areg::is_service_connected(status))
        {
            gLiveConnects.fetch_add(1);
            if (!mChurnedOnline)
            {
                mChurnedOnline = true;
                run_churn();    // service is up here
                gChurnDone.store(true);
            }
        }
        else
        {
            gLiveDisconnects.fetch_add(1);
        }

        return relevant;
    }

private:
    void run_churn()
    {
        areg::DispatcherThread & thread{ static_cast<areg::DispatcherThread &>(master_thread()) };
        ChurnClient * clients[CHURN_CLIENTS]{ };
        for (uint32_t round = 0; round < CHURN_ROUNDS; ++ round)
        {
            for (uint32_t i = 0; i < CHURN_CLIENTS; ++ i)
                clients[i] = new ChurnClient(mRoleName, thread);

            for (uint32_t i = 0; i < CHURN_CLIENTS; ++ i)
                delete clients[i];
        }
    }

    areg::String    mRoleName;
    bool            mChurnedOnline;
};

BEGIN_MODEL("ConsumerModel")
    BEGIN_REGISTER_THREAD( "ConsumerThread" )
        BEGIN_REGISTER_COMPONENT( "ServiceConsumer", ChurnComponent )
            REGISTER_DEPENDENCY( "ServiceProvider" )
        END_REGISTER_COMPONENT( "ServiceConsumer" )
    END_REGISTER_THREAD( "ConsumerThread" )
END_MODEL("ConsumerModel")

int main()
{
    areg::Application::setup();
    areg::Application::load_model("ConsumerModel");
    areg::Application::load_model("ProviderModel");

    for (uint32_t i = 0; (i < 200) && !gChurnDone.load(); ++ i)
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Take the provider away. The disconnect edge walks the connection
    // listener list of the proxy, which is where a stale entry would be used.
    areg::Application::unload_model("ProviderModel");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    const uint32_t created  { gChurnCreated.load() };
    const uint32_t deleted  { gChurnDeleted.load() };
    const uint32_t callbacks{ gChurnCallbacks.load() };
    const uint32_t connects { gLiveConnects.load() };
    const uint32_t discos   { gLiveDisconnects.load() };

    std::printf("churn clients created ....: %u\n", created);
    std::printf("churn clients deleted ....: %u\n", deleted);
    std::printf("churn client callbacks ...: %u\n", callbacks);
    std::printf("live client connects .....: %u\n", connects);
    std::printf("live client disconnects ..: %u\n", discos);

    areg::Application::unload_model("ConsumerModel");
    areg::Application::release();

    const bool ok{ (created == deleted)
                && (created == (CHURN_ROUNDS * CHURN_CLIENTS * 2))
                && (connects >= 1)
                && (discos >= 1) };

    std::printf("%s\n", ok ? "RESULT: PASS" : "RESULT: FAIL");
    return ok ? 0 : 1;
}
