/************************************************************************
 * A client deletes another client of the same proxy, or itself, inside a callback.
 * A deleted client is never called again, and a client created at the address of a
 * deleted one gets exactly one initial value.
 *
 * Usage: areg-consumer-delete-test <case>
 *   sibling-initial   two clients subscribe; the first initial value deletes the other client
 *   sibling-update    two clients subscribe; the first provider update deletes the other client
 *   self-delete       one client subscribes to two attributes and deletes itself in the first callback
 *   same-address      a client subscribes and is deleted, then a new client at the same address
 *                     subscribes: the new client gets one initial value
 *   connect-sibling   two clients wait for the connection; the first one told deletes the other
 *   free-proxy        a client created on a connected proxy is deleted with the last client: the
 *                     proxy is freed while the new client's connection event is queued
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "examples/25_pubsub/services/PubSubConsumerBase.hpp"
#include "examples/25_pubsub/services/PubSubProviderBase.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <new>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "25_generated")
#endif // _MSC_VER

namespace
{
    constexpr char const    _model[]        { "CdModel" };
    constexpr char const    _providerRole[] { "CdProvider" };

    constexpr uint32_t      WAIT_MS         { 5000 };
    constexpr uint32_t      MAGIC           { 0x600DF00Du };
    constexpr uint32_t      SLOT_COUNT      { 4 };
    constexpr std::size_t   SLOT_SIZE       { 1024 };
    constexpr uint32_t      TRIGGER_VALUE   { 7 };

    enum class Mode : uint32_t { SiblingInitial, SiblingUpdate, SelfDelete, SameAddress, ConnectSibling, FreeProxy };

    //!< A counter a thread waits on, with a deadline, until it reaches a value.
    class Gate
    {
    public:
        void advance()
        {
            {
                std::lock_guard<std::mutex> lock(mLock);
                ++ mValue;
            }

            mCond.notify_all();
        }

        bool wait_for(uint32_t value, uint32_t timeoutMs = WAIT_MS)
        {
            std::unique_lock<std::mutex> lock(mLock);
            return mCond.wait_for(lock, std::chrono::milliseconds(timeoutMs), [this, value] { return mValue >= value; });
        }

    private:
        std::mutex              mLock;      //!< Guards the value.
        std::condition_variable mCond;      //!< Signalled on every advance.
        uint32_t                mValue{ 0 };//!< The number of advances.
    };

    std::atomic<Mode>       gMode           { Mode::SiblingInitial };
    std::atomic_uint32_t    gStale          { 0 };      //!< Calls to a deleted client.
    std::atomic_uint32_t    gDeleted        { 0 };      //!< Clients deleted.
    std::atomic_uint32_t    gDeletedAtDone  { 0 };      //!< Clients deleted when the case finished.
    std::atomic_uint32_t    gNewClientValues{ 0 };      //!< Initial values the client at the reused address got.
    std::atomic_bool        gSameAddress    { false };  //!< The new client took the address of the deleted one.
    Gate                    gFirstValues;               //!< Advanced on the first value of each client.
    Gate                    gDone;                      //!< Advanced when the case finished.

    //!< The client slots: a deleted client is filled with 0xDD and its slot is reused first.
    alignas(std::max_align_t) unsigned char gSlots[SLOT_COUNT][SLOT_SIZE];
    bool gSlotUsed[SLOT_COUNT]{ false, false, false, false };
}

class Consumer;

//!< A client of the PubSub service that reports every callback to its owner.
class Client final : public PubSubConsumerBase
{
public:
    Client(Consumer & owner, const areg::String & role, areg::DispatcherThread & thread)
        : PubSubConsumerBase(role, thread)
        , mOwner    (owner)
    {
    }

    ~Client() override
    {
        mMagic = 0u;
        gDeleted.fetch_add(1u);
    }

    static void * operator new(std::size_t size) noexcept
    {
        if (size <= SLOT_SIZE)
        {
            for (uint32_t i = 0; i < SLOT_COUNT; ++i)
            {
                if (gSlotUsed[i] == false)
                {
                    gSlotUsed[i] = true;
                    return gSlots[i];
                }
            }
        }

        return nullptr;
    }

    static void operator delete(void * block, std::size_t size)
    {
        std::memset(block, 0xDD, size);
        const std::ptrdiff_t index{ (static_cast<unsigned char *>(block) - &gSlots[0][0]) / static_cast<std::ptrdiff_t>(SLOT_SIZE) };
        gSlotUsed[index] = false;
    }

    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) override;

    void on_integer_always_update(uint32_t value, areg::DataState state) override;

    void on_string_on_change_update(const areg::String & value, areg::DataState state) override;

    void subscribe_integer()
    {
        notify_on_integer_always_update(true);
    }

    void subscribe_string()
    {
        notify_on_string_on_change_update(true);
    }

    //!< Returns true if the object was not deleted.
    bool is_alive() const noexcept
    {
        return (mMagic == MAGIC);
    }

    uint32_t    mIntegers   { 0 };      //!< Integer values received.
    uint32_t    mStrings    { 0 };      //!< String values received.

private:
    Consumer &  mOwner;                 //!< The component that decides what a callback does.
    uint32_t    mMagic      { MAGIC };  //!< MAGIC while the object is alive.
};

static_assert(sizeof(Client) <= SLOT_SIZE, "A client must fit a slot");

//!< The service provider; its attributes are set from the main thread.
class Provider final : public areg::Component, protected PubSubProviderBase
{
public:
    static Provider * instance;

    Provider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component   (entry, owner)
        , PubSubProviderBase(static_cast<areg::Component &>(*this))
    {
        instance = this;
    }

    ~Provider() override
    {
        instance = nullptr;
    }

    void publish(uint32_t value)
    {
        set_integer_always(value);
    }
};

Provider * Provider::instance{ nullptr };

//!< Owns the clients and deletes them from their callbacks, as the case requires.
class Consumer final : public areg::Component, protected areg::TimerConsumer
{
public:
    Consumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component   (entry, owner)
        , areg::TimerConsumer( )
        , mRole     (entry.mDependencyServices[0].mRoleName)
        , mDone     (static_cast<areg::TimerConsumer &>(*this), "CdDone")
    {
    }

    void startup_component(areg::ComponentThread & thread) override
    {
        areg::Component::startup_component(thread);
        mThread = &thread;
        mA = new Client(*this, mRole, thread);
        const Mode mode{ gMode.load() };
        if ((mode == Mode::SiblingInitial) || (mode == Mode::SiblingUpdate) || (mode == Mode::ConnectSibling))
        {
            mB = new Client(*this, mRole, thread);
        }
    }

    void shutdown_component(areg::ComponentThread & thread) override
    {
        mDone.stop_timer();
        delete mA;
        delete mB;
        delete mC;
        mA = mB = mC = nullptr;
        areg::Component::shutdown_component(thread);
    }

    void client_connected(Client & client, bool connected)
    {
        if (connected == false)
            return;

        switch (gMode.load())
        {
        case Mode::SiblingInitial:
        case Mode::SiblingUpdate:
        case Mode::FreeProxy:
            client.subscribe_integer();
            break;

        case Mode::SelfDelete:
        case Mode::SameAddress:
            client.subscribe_integer();
            client.subscribe_string();
            break;

        case Mode::ConnectSibling:
            if (fire_once())
            {
                delete_peer(client);
                finish();
            }
            break;

        default:
            break;
        }
    }

    void client_integer(Client & client, uint32_t value, areg::DataState state)
    {
        // sibling-update waits for the provider's answer to the subscription, the second value.
        const uint32_t ready{ gMode.load() == Mode::SiblingUpdate ? 2u : 1u };
        if (++ client.mIntegers == ready)
        {
            gFirstValues.advance();
        }

        const bool valid{ state == areg::DataState::DataIsOK };
        switch (gMode.load())
        {
        case Mode::SiblingInitial:
            if (fire_once())
            {
                delete_peer(client);
                finish();
            }
            break;

        case Mode::SiblingUpdate:
            if (valid && (value >= 1u) && fire_once())
            {
                delete_peer(client);
                finish();
            }
            break;

        case Mode::SelfDelete:
            if (fire_once())
            {
                finish();
                delete_self(client);
            }
            break;

        case Mode::SameAddress:
            if (valid && (value == TRIGGER_VALUE) && fire_once())
            {
                reuse_address();
                finish();
            }
            break;

        case Mode::FreeProxy:
            if (fire_once())
            {
                delete new Client(*this, mRole, *mThread);
                delete_self(client);
                finish();
            }
            break;

        default:
            break;
        }
    }

    void client_string(Client & client)
    {
        ++ client.mStrings;
        switch (gMode.load())
        {
        case Mode::SelfDelete:
            if (fire_once())
            {
                finish();
                delete_self(client);
            }
            break;

        case Mode::SameAddress:
            if (&client == mA)
            {
                gFirstValues.advance();
            }
            break;

        default:
            break;
        }
    }

private:
    //!< Returns true on the first call only.
    bool fire_once()
    {
        const bool result{ mFired == false };
        mFired = true;
        return result;
    }

    //!< Deletes the other client of the pair.
    void delete_peer(Client & client)
    {
        Client * peer{ &client == mA ? mB : mA };
        if (peer == mA)
            mA = nullptr;
        else
            mB = nullptr;

        delete peer;
    }

    //!< Deletes the client inside its own callback.
    void delete_self(Client & client)
    {
        if (&client == mA)
            mA = nullptr;
        else if (&client == mB)
            mB = nullptr;

        delete &client;
    }

    //!< Subscribes a new client and deletes it, then subscribes another new client at its address.
    void reuse_address()
    {
        Client * deleted{ new Client(*this, mRole, *mThread) };
        deleted->subscribe_string();
        const void * address{ deleted };
        delete deleted;

        mC = new Client(*this, mRole, *mThread);
        gSameAddress.store(static_cast<const void *>(mC) == address);
        mC->subscribe_string();
    }

    //!< Ends the case with a timer event, queued after every event posted so far.
    void finish()
    {
        mDone.start_timer(1u, areg::Timer::ONE_TIME);
    }

    void process_timer(areg::Timer & /*timer*/) override
    {
        gDeletedAtDone.store(gDeleted.load());
        gNewClientValues.store(mC != nullptr ? mC->mStrings : 0u);
        gDone.advance();
    }

private:
    areg::String            mRole;                  //!< The role name of the provider.
    areg::Timer             mDone;                  //!< Fires once when the case finished.
    areg::DispatcherThread* mThread { nullptr };    //!< The thread of the component and its clients.
    Client *                mA      { nullptr };    //!< The first client.
    Client *                mB      { nullptr };    //!< The second client.
    Client *                mC      { nullptr };    //!< The client at a reused address.
    bool                    mFired  { false };      //!< The case action ran.
};

bool Client::service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    if (is_alive() == false)
    {
        gStale.fetch_add(1u);
        return false;
    }

    const bool result{ PubSubConsumerBase::service_connected(status, proxy) };
    mOwner.client_connected(*this, areg::is_service_connected(status));
    return result;
}

void Client::on_integer_always_update(uint32_t value, areg::DataState state)
{
    if (is_alive() == false)
    {
        gStale.fetch_add(1u);
        return;
    }

    mOwner.client_integer(*this, value, state);
}

void Client::on_string_on_change_update(const areg::String & /*value*/, areg::DataState /*state*/)
{
    if (is_alive() == false)
    {
        gStale.fetch_add(1u);
        return;
    }

    mOwner.client_string(*this);
}

BEGIN_MODEL(_model)
    BEGIN_REGISTER_THREAD("CdProviderThread")
        BEGIN_REGISTER_COMPONENT(_providerRole, Provider)
            REGISTER_IMPLEMENT_SERVICE(PubSub::ServiceName, PubSub::InterfaceVersion)
        END_REGISTER_COMPONENT(_providerRole)
    END_REGISTER_THREAD("CdProviderThread")
    BEGIN_REGISTER_THREAD("CdConsumerThread")
        BEGIN_REGISTER_COMPONENT("CdConsumer", Consumer)
            REGISTER_DEPENDENCY(_providerRole)
        END_REGISTER_COMPONENT("CdConsumer")
    END_REGISTER_THREAD("CdConsumerThread")
END_MODEL(_model)

namespace
{
    struct Case
    {
        const char *    name;
        Mode            mode;
    };

    constexpr Case _cases[]
    {
          { "sibling-initial"   , Mode::SiblingInitial  }
        , { "sibling-update"    , Mode::SiblingUpdate   }
        , { "self-delete"       , Mode::SelfDelete      }
        , { "same-address"      , Mode::SameAddress     }
        , { "connect-sibling"   , Mode::ConnectSibling  }
        , { "free-proxy"        , Mode::FreeProxy       }
    };

    //!< Waits for the clients to be ready and publishes the value that triggers the case.
    bool trigger(Mode mode)
    {
        if (mode == Mode::SiblingUpdate)
        {
            // Both clients got the initial value and the provider's answer.
            if ((gFirstValues.wait_for(2u) == false) || (Provider::instance == nullptr))
                return false;

            Provider::instance->publish(1u);
        }
        else if (mode == Mode::SameAddress)
        {
            // The first integer, then both string values: the initial one and the provider's answer.
            if ((gFirstValues.wait_for(3u) == false) || (Provider::instance == nullptr))
                return false;

            Provider::instance->publish(TRIGGER_VALUE);
        }

        return true;
    }

    //!< Returns the number of clients deleted by the action of the case.
    uint32_t deleted_by_action(Mode mode)
    {
        return (mode == Mode::FreeProxy ? 2u : 1u);
    }
}

int main(int argc, char * argv[])
{
    setvbuf(stdout, nullptr, _IONBF, 0);

    const char * name{ argc > 1 ? argv[1] : "sibling-initial" };
    const Case * selected{ nullptr };
    for (const Case & entry : _cases)
    {
        if (std::strcmp(name, entry.name) == 0)
            selected = &entry;
    }

    if (selected == nullptr)
    {
        printf("unknown case [ %s ]\n", name);
        return 2;
    }

    gMode.store(selected->mode);
    areg::Application::setup(false, true, false, true, false);
    areg::Application::load_model(_model);

    const bool triggered{ trigger(selected->mode) };
    const bool done{ triggered && gDone.wait_for(1u) };

    areg::Application::unload_model(_model);
    areg::Application::release();

    bool ok{ done && (gStale.load() == 0u) && (gDeletedAtDone.load() == deleted_by_action(selected->mode)) };
    if (selected->mode == Mode::SameAddress)
    {
        ok = ok && gSameAddress.load() && (gNewClientValues.load() == 1u);
    }

    printf("case .....................: %s\n", name);
    printf("finished .................: %s\n", done ? "yes" : "no");
    printf("calls to deleted clients .: %u\n", gStale.load());
    printf("clients deleted ..........: %u\n", gDeletedAtDone.load());
    if (selected->mode == Mode::SameAddress)
    {
        printf("same address .............: %s\n", gSameAddress.load() ? "yes" : "no");
        printf("initial values, new client: %u\n", gNewClientValues.load());
    }

    printf("%s\n", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
