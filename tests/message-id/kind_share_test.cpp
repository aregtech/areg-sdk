/************************************************************************
 * Entries of two kinds with one message ID, and IDs below 0x10000, reach their own handlers.
 * Uses the generated KindShare interface: the request pingSDFzY and the broadcast tickxGUOt
 * have the message ID 0xDF177184, the request createLight has 0x0000BF29 and the attribute
 * NameId has 0x000012E9.
 *
 * Usage: areg-kind-share-test
 *   The consumer sends pingSDFzY(1) and createLight(2) and subscribes to tickxGUOt and NameId.
 *   The provider answers pong(1) and lightCreated(2), broadcasts tickxGUOt(3) and sets NameId(4).
 *   Every call reaches its own handler once, with its own value.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "tests/message-id/services/KindShareConsumerBase.hpp"
#include "tests/message-id/services/KindShareProviderBase.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <mutex>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "message_id_kind_share_generated")
#endif // _MSC_VER

static_assert(KindShare::message_id(KindShare::RequestIDs::MsgId_request_pingSDFzY) == 0xDF177184u, "the request has the ID of the broadcast");
static_assert(KindShare::message_id(KindShare::BroadcastIDs::MsgId_broadcast_tickxGUOt) == 0xDF177184u, "the broadcast has the ID of the request");
static_assert(KindShare::message_id(KindShare::RequestIDs::MsgId_request_createLight) == 0x0000BF29u, "an ID below 0x10000");
static_assert(KindShare::message_id(KindShare::AttributeIDs::MsgId_NameId) == 0x000012E9u, "an ID below 0x10000");

namespace
{
    constexpr char const    _model[]        { "KsModel" };
    constexpr char const    _providerThread[]{ "KsProviderThread" };
    constexpr char const    _consumerThread[]{ "KsConsumerThread" };
    constexpr char const    _providerRole[] { "KsProvider" };
    constexpr char const    _consumerRole[] { "KsConsumer" };

    constexpr uint32_t      WAIT_MS         { 30000 };
    constexpr uint32_t      PING_VALUE      { 1u };
    constexpr uint32_t      LIGHT_VALUE     { 2u };
    constexpr uint32_t      TICK_VALUE      { 3u };
    constexpr uint32_t      NAME_VALUE      { 4u };

    std::atomic_uint32_t    gPings          { 0 };  //!< request_pingSDFzY calls in the provider.
    std::atomic_uint32_t    gLights         { 0 };  //!< request_createLight calls in the provider.
    std::atomic_uint32_t    gPongs          { 0 };  //!< response_pong calls in the consumer.
    std::atomic_uint32_t    gCreated        { 0 };  //!< response_lightCreated calls in the consumer.
    std::atomic_uint32_t    gTicks          { 0 };  //!< broadcast_tickxGUOt calls in the consumer.
    std::atomic_uint32_t    gNames          { 0 };  //!< valid on_name_id_update calls with the value set.
    std::atomic_uint32_t    gWrong          { 0 };  //!< calls with a value that belongs to another entry.

    std::mutex              gLock;
    std::condition_variable gChanged;

    //!< Wakes the main thread to check its condition again.
    void changed()
    {
        { std::lock_guard<std::mutex> lock(gLock); }
        gChanged.notify_all();
    }

    //!< Waits until the condition holds or the deadline passes.
    bool wait_until(const std::function<bool()> & condition)
    {
        std::unique_lock<std::mutex> lock(gLock);
        return gChanged.wait_for(lock, std::chrono::milliseconds(WAIT_MS), condition);
    }

    //!< Counts the call and counts a wrong value.
    void seen(std::atomic_uint32_t & counter, uint32_t value, uint32_t expected)
    {
        counter.fetch_add(1u);
        if (value != expected)
            gWrong.fetch_add(1u);

        changed();
    }
}

//////////////////////////////////////////////////////////////////////////
// Provider
//////////////////////////////////////////////////////////////////////////

//!< Answers both requests; the ping also broadcasts and sets the attribute.
class KsProvider final  : public    areg::Component
                        , protected KindShareProviderBase
{
public:
    KsProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , KindShareProviderBase     ( static_cast<areg::Component &>(*this) )
    {
    }

protected:
    void startup_service_interface(areg::Component & holder) override
    {
        KindShareProviderBase::startup_service_interface(holder);
        set_name_id(0u);
    }

    void request_pingSDFzY(uint32_t value) override
    {
        gPings.fetch_add(1u);
        if (value != PING_VALUE)
            gWrong.fetch_add(1u);

        response_pong(value);
        broadcast_tickxGUOt(TICK_VALUE);
        set_name_id(NAME_VALUE);
    }

    void request_createLight(uint32_t value) override
    {
        gLights.fetch_add(1u);
        if (value != LIGHT_VALUE)
            gWrong.fetch_add(1u);

        response_lightCreated(value);
    }
};

//////////////////////////////////////////////////////////////////////////
// Consumer
//////////////////////////////////////////////////////////////////////////

//!< Subscribes, sends both requests and records every callback.
class KsConsumer final  : public    areg::Component
                        , protected KindShareConsumerBase
{
public:
    KsConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , KindShareConsumerBase     ( _providerRole, static_cast<areg::Component &>(*this) )
    {
    }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) override
    {
        const bool result{ KindShareConsumerBase::service_connected(status, proxy) };
        if (areg::is_service_connected(status))
        {
            notify_on_broadcast_tickxGUOt(true);
            notify_on_name_id_update(true);
            request_createLight(LIGHT_VALUE);
            request_pingSDFzY(PING_VALUE);
        }

        return result;
    }

    void response_pong(uint32_t value) override
    {
        seen(gPongs, value, PING_VALUE);
    }

    void response_lightCreated(uint32_t value) override
    {
        seen(gCreated, value, LIGHT_VALUE);
    }

    void broadcast_tickxGUOt(uint32_t value) override
    {
        seen(gTicks, value, TICK_VALUE);
    }

    void on_name_id_update(uint32_t value, areg::DataState state) override
    {
        if ((state == areg::DataState::DataIsOK) && (value == NAME_VALUE))
        {
            gNames.fetch_add(1u);
            changed();
        }
    }
};

//////////////////////////////////////////////////////////////////////////
// Model
//////////////////////////////////////////////////////////////////////////

namespace ks_model
{
BEGIN_MODEL(_model)
    BEGIN_REGISTER_THREAD(_providerThread)
        BEGIN_REGISTER_COMPONENT(_providerRole, KsProvider)
            REGISTER_IMPLEMENT_SERVICE(KindShare::ServiceName, KindShare::InterfaceVersion)
        END_REGISTER_COMPONENT(_providerRole)
    END_REGISTER_THREAD(_providerThread)

    BEGIN_REGISTER_THREAD(_consumerThread)
        BEGIN_REGISTER_COMPONENT(_consumerRole, KsConsumer)
            REGISTER_DEPENDENCY(_providerRole)
        END_REGISTER_COMPONENT(_consumerRole)
    END_REGISTER_THREAD(_consumerThread)
END_MODEL(_model)
}

int main()
{
    setvbuf(stdout, nullptr, _IONBF, 0);

    areg::Application::setup(false, true, false, true, false, nullptr);
    bool ok{ areg::Application::load_model(_model) };
    if (ok)
    {
        ok = wait_until([] { return (gPongs.load() >= 1u) && (gCreated.load() >= 1u) && (gTicks.load() >= 1u) && (gNames.load() >= 1u); });
        areg::Application::unload_model(_model);
    }

    areg::Application::release();

    printf("provider ping, light ..................: %u, %u\n", gPings.load(), gLights.load());
    printf("consumer pong, created, tick, name .....: %u, %u, %u, %u\n", gPongs.load(), gCreated.load(), gTicks.load(), gNames.load());
    printf("calls with a wrong value ...............: %u\n", gWrong.load());
    ok = ok && (gPings.load() == 1u) && (gLights.load() == 1u) && (gPongs.load() == 1u) && (gCreated.load() == 1u)
            && (gTicks.load() == 1u) && (gNames.load() == 1u) && (gWrong.load() == 0u);
    printf("%s\n", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
