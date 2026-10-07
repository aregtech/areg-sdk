/************************************************************************
 * Routes remote requests to a service provider from a foreign thread, the way the
 * router client receive thread does, while the provider is loaded and unloaded
 * again and again. A deleted provider poisons its own memory, so a request routed
 * through a released provider crashes instead of passing silently.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/component/Channel.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/RemoteEventFactory.hpp"
#include "areg/component/StubBase.hpp"

#include <atomic>
#include <cstdio>
#include <cstring>
#include <thread>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
#endif // _MSC_VER

namespace
{
    constexpr char const    _modelName[]    { "StubTeardownModel" };
    constexpr char const    _roleName[]     { "StubTeardownProvider" };
    constexpr char const    _threadName[]   { "StubTeardownThread" };
    constexpr char const    _serviceName[]  { "StubTeardownService" };
    constexpr areg::Version _version        { 1, 0, 0 };

    constexpr uint32_t      LOAD_ROUNDS     { 200 };
    constexpr uint32_t      START_WAIT_MS   { 5000 };

    constexpr uint32_t      _requestList[]  { areg::REQUEST_ID_FIRST };
    constexpr uint32_t      _noResponse[]   { areg::RESPONSE_ID_NONE };

    const areg::InterfaceData _interfaceData
    {
          _serviceName
        , _version
        , areg::ServiceType::Local
        , 1
        , 0
        , 0
        , _requestList
        , nullptr
        , nullptr
        , _noResponse
        , nullptr
    };

    std::atomic_uint32_t    gStubKey    { 0 };
    std::atomic_bool        gStarted    { false };
    std::atomic_bool        gStop       { false };
    std::atomic_uint32_t    gCreated    { 0 };
    std::atomic_uint32_t    gDeleted    { 0 };
    std::atomic_uint32_t    gHandled    { 0 };
    std::atomic_uint64_t    gRouted     { 0 };
}

//!< Service provider with one request, which it only counts.
class TeardownProvider final : public    areg::Component
                             , protected areg::StubBase
{
public:
    TeardownProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component   ( entry, owner )
        , areg::StubBase    ( static_cast<areg::Component &>(*this), _interfaceData )
    {
        gStubKey.store(static_cast<uint32_t>(areg::StubBase::address()));
        gCreated.fetch_add(1);
    }

    ~TeardownProvider() override
    {
        gDeleted.fetch_add(1);
    }

    //!< Fills the released block, so a stale pointer to the provider faults immediately.
    static void operator delete (void * block, size_t size)
    {
        std::memset(block, 0xDD, size);
        ::operator delete(block);
    }

    static void * operator new (size_t size)
    {
        return ::operator new(size);
    }

    void startup_component(areg::ComponentThread & comThread) override
    {
        areg::Component::startup_component(comThread);
        gStarted.store(true);
    }

    void shutdown_component(areg::ComponentThread & comThread) override
    {
        gStarted.store(false);
        areg::Component::shutdown_component(comThread);
    }

protected:
    void send_notification(uint32_t /*msgId*/) override
    {
    }

    void error_request(uint32_t /*msgId*/, bool /*msgCancel*/) override
    {
    }

    void process_request_event(areg::ServiceRequestEvent & /*eventElem*/) override
    {
        gHandled.fetch_add(1);
        cancel_current_request();
    }

    void process_attribute_event(areg::ServiceRequestEvent & /*eventElem*/) override
    {
    }
};

BEGIN_MODEL(_modelName)
    BEGIN_REGISTER_THREAD(_threadName)
        BEGIN_REGISTER_COMPONENT(_roleName, TeardownProvider)
            REGISTER_IMPLEMENT_SERVICE(_serviceName, _version)
        END_REGISTER_COMPONENT(_roleName)
    END_REGISTER_THREAD(_threadName)
END_MODEL(_modelName)

//!< Routes remote requests to the provider until it is told to stop.
static void route_requests()
{
    const areg::Channel channel{ };
    while (gStop.load() == false)
    {
        areg::MessageEnvelope wire(static_cast<uint16_t>(areg::EventType::EventRemoteRequest)
                                 , static_cast<uint8_t>(areg::EventPriority::NormalPrio));
        areg::EventHeader * hdr{ wire.header() };
        hdr->provider.number = gStubKey.load();
        hdr->messageId       = areg::REQUEST_ID_FIRST;

        if (areg::RemoteEventFactory::route_incoming_message(wire, channel))
            gRouted.fetch_add(1);
    }
}

int main()
{
    // Unbuffered: what the run printed before a crash still reaches the log.
    setvbuf(stdout, nullptr, _IONBF, 0);

    areg::Application::setup(false, true, false, true, false, nullptr);

    std::thread receiver(&route_requests);

    uint32_t rounds{ 0 };
    for ( ; rounds < LOAD_ROUNDS; ++ rounds)
    {
        if (areg::Application::load_model(_modelName) == false)
            break;

        for (uint32_t i = 0; (i < START_WAIT_MS) && (gStarted.load() == false); ++ i)
            areg::Thread::sleep(1);

        const uint64_t before{ gRouted.load() };
        for (uint32_t i = 0; (i < START_WAIT_MS) && (gRouted.load() == before); ++ i)
            areg::Thread::sleep(1);

        areg::Application::unload_model(_modelName);
    }

    gStop.store(true);
    receiver.join();

    areg::Application::release();

    const uint32_t created{ gCreated.load() };
    const uint32_t deleted{ gDeleted.load() };
    const uint32_t handled{ gHandled.load() };
    const uint64_t routed { gRouted.load() };

    printf("load and unload rounds ...: %u\n", rounds);
    printf("providers created ........: %u\n", created);
    printf("providers deleted ........: %u\n", deleted);
    printf("requests routed ..........: %llu\n", static_cast<unsigned long long>(routed));
    printf("requests handled .........: %u\n", handled);

    const bool ok{ (rounds == LOAD_ROUNDS)
                && (created == LOAD_ROUNDS)
                && (deleted == created)
                && (routed >= LOAD_ROUNDS)
                && (handled > 0) };

    printf("%s\n", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
