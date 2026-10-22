/************************************************************************
 * A request that cannot be answered must reach its caller as request_X_failed, exactly once.
 *
 * Usage: areg-request-failure-test <case>
 *   before-connect  a request sent before the proxy is connected fails with ServiceUnavailable
 *   provider-gone   a pending request fails with ServiceUnavailable after the provider leaves,
 *                   and the consumer sees Disconnected first
 *   remote-refused  a remote request whose provider queue refuses it is answered with
 *                   MessageUndelivered to the channel it came from
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/component/Channel.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/EventConsumer.hpp"
#include "areg/component/RemoteEventFactory.hpp"
#include "areg/component/StubAddress.hpp"
#include "examples/02_minimalipc/services/HelloServiceConsumerBase.hpp"
#include "examples/02_minimalipc/services/HelloServiceProviderBase.hpp"
#include "examples/02_minimalipc/services/private/HelloServiceEvents.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <mutex>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "02_generated")
#endif // _MSC_VER

namespace
{
    constexpr char const    _consumerModel[]    { "RfConsumerModel" };
    constexpr char const    _providerModel[]    { "RfProviderModel" };
    constexpr char const    _refuseModel[]      { "RfRefuseModel" };
    constexpr char const    _captureModel[]     { "RfCaptureModel" };
    constexpr char const    _consumerThread[]   { "RfConsumerThread" };
    constexpr char const    _providerThread[]   { "RfProviderThread" };
    constexpr char const    _refuseThread[]     { "RfRefuseThread" };
    constexpr char const    _captureThread[]    { "RfCaptureThread" };
    constexpr char const    _consumerRole[]     { "RfConsumer" };
    constexpr char const    _providerRole[]     { "RfProvider" };
    constexpr char const    _refuseRole[]       { "RfRefuser" };
    constexpr char const    _captureRole[]      { "RfCapture" };

    constexpr uint32_t      WAIT_MS             { 5000 };
    constexpr uint32_t      REFUSE_QUEUE        { areg::QUEUE_MIN_RING_CAPACITY };
    constexpr uint32_t      FLOOD_COUNT         { 2 * REFUSE_QUEUE };
    constexpr uint32_t      REMOTE_COOKIE       { 0x77 };
    constexpr uint32_t      REMOTE_MAGIC        { 0xBEEF };

    enum class Mode : uint32_t { BeforeConnect, ProviderGone };

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

    std::atomic<Mode>       gMode           { Mode::BeforeConnect };
    std::atomic_uint32_t    gStep           { 0 };
    std::atomic_uint32_t    gDisconnectStep { 0 };
    std::atomic_uint32_t    gFailStep       { 0 };
    std::atomic_uint32_t    gFailures       { 0 };
    std::atomic_uint32_t    gFailReason     { 0 };
    std::atomic_uint32_t    gResponses      { 0 };
    std::atomic_uint32_t    gStubKey        { 0 };
    std::atomic_uint32_t    gCaptureNumber  { 0 };
    std::atomic_uint32_t    gUndelivered    { 0 };
    std::atomic_uint32_t    gWrongReply     { 0 };
    std::atomic_uint32_t    gOtherReplies   { 0 };

    Gate                    gStarted;
    Gate                    gReceived;
    Gate                    gFailed;
    Gate                    gRelease;
    Gate                    gReplies;
}

//////////////////////////////////////////////////////////////////////////
// Consumer
//////////////////////////////////////////////////////////////////////////

//!< Sends one request, before the connection or after it, and records what comes back.
class RfConsumer final : public    areg::Component
                       , protected HelloServiceConsumerBase
{
public:
    RfConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , HelloServiceConsumerBase  ( _providerRole, static_cast<areg::Component &>(*this) )
    {
    }

    void startup_component(areg::ComponentThread & comThread) override
    {
        areg::Component::startup_component(comThread);
        if (gMode.load() == Mode::BeforeConnect)
        {
            request_hello_service();
        }

        gStarted.advance();
    }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) override
    {
        const bool result{ HelloServiceConsumerBase::service_connected(status, proxy) };
        if (areg::is_service_connected(status))
        {
            if (gMode.load() == Mode::ProviderGone)
            {
                request_hello_service();
            }
        }
        else if (gDisconnectStep.load() == 0)
        {
            gDisconnectStep.store(++ gStep);
        }

        return result;
    }

    void response_hello_service() override
    {
        gResponses.fetch_add(1);
    }

    void request_hello_service_failed(areg::ResultType reason) override
    {
        gFailReason.store(static_cast<uint32_t>(reason));
        gFailStep.store(++ gStep);
        gFailures.fetch_add(1);
        gFailed.advance();
    }
};

//////////////////////////////////////////////////////////////////////////
// Providers
//////////////////////////////////////////////////////////////////////////

//!< Takes the request and never answers it.
class RfProvider final : public    areg::Component
                       , protected HelloServiceProviderBase
{
public:
    RfProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , HelloServiceProviderBase  ( static_cast<areg::Component &>(*this) )
    {
    }

protected:
    void request_hello_service() override
    {
        gReceived.advance();
    }
};

//!< Blocks its thread in the first request until the test releases it.
class RfRefuser final : public    areg::Component
                      , protected HelloServiceProviderBase
{
public:
    RfRefuser(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , HelloServiceProviderBase  ( static_cast<areg::Component &>(*this) )
    {
        gStubKey.store(static_cast<uint32_t>(areg::StubBase::address()));
    }

    void startup_component(areg::ComponentThread & comThread) override
    {
        areg::Component::startup_component(comThread);
        gStarted.advance();
    }

protected:
    void request_hello_service() override
    {
        if (mBlocked == false)
        {
            mBlocked = true;
            gReceived.advance();
            static_cast<void>(gRelease.wait_for(1));
        }
    }

private:
    bool    mBlocked{ false };  //!< The first request has blocked the thread.
};

//////////////////////////////////////////////////////////////////////////
// Capture of the replies that a provider sends to a remote channel
//////////////////////////////////////////////////////////////////////////

//!< Stands for the router client: receives the responses posted to its thread.
class RfCapture final : public areg::Component
                      , public areg::EventConsumer
{
public:
    RfCapture(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component   ( entry, owner )
    {
    }

    void startup_component(areg::ComponentThread & comThread) override
    {
        areg::Component::startup_component(comThread);
        HelloServiceResponseEvent::add_listener(static_cast<areg::EventConsumer &>(*this), comThread);
        gCaptureNumber.store(static_cast<uint32_t>(comThread.address()));
        gStarted.advance();
    }

    void shutdown_component(areg::ComponentThread & comThread) override
    {
        HelloServiceResponseEvent::remove_listener(static_cast<areg::EventConsumer &>(*this), comThread);
        areg::Component::shutdown_component(comThread);
    }

    void start_event_processing(areg::Event & eventElem) override
    {
        const areg::EventHeader * hdr{ eventElem.header() };
        if (hdr == nullptr)
            return;

        const areg::ResultType result{ static_cast<areg::ResultType>(hdr->result) };
        if (result != areg::ResultType::MessageUndelivered)
        {
            gOtherReplies.fetch_add(1);
            return;
        }

        const bool right{ (hdr->messageId == static_cast<uint32_t>(HelloService::MessageIDs::MsgId_request_hello_service))
                       && (hdr->sequenceNr > 1) && (hdr->sequenceNr <= FLOOD_COUNT + 1)
                       && (hdr->consumer.number == REMOTE_MAGIC)
                       && (hdr->consumer.id == REMOTE_COOKIE) };
        if (right == false)
        {
            gWrongReply.fetch_add(1);
        }

        gUndelivered.fetch_add(1);
        gReplies.advance();
    }
};

//////////////////////////////////////////////////////////////////////////
// Models
//////////////////////////////////////////////////////////////////////////

namespace consumer_model
{
BEGIN_MODEL(_consumerModel)
    BEGIN_REGISTER_THREAD(_consumerThread)
        BEGIN_REGISTER_COMPONENT(_consumerRole, RfConsumer)
            REGISTER_DEPENDENCY(_providerRole)
        END_REGISTER_COMPONENT(_consumerRole)
    END_REGISTER_THREAD(_consumerThread)
END_MODEL(_consumerModel)
}

namespace provider_model
{
BEGIN_MODEL(_providerModel)
    BEGIN_REGISTER_THREAD(_providerThread)
        BEGIN_REGISTER_COMPONENT(_providerRole, RfProvider)
            REGISTER_IMPLEMENT_SERVICE(HelloService::ServiceName, HelloService::InterfaceVersion)
        END_REGISTER_COMPONENT(_providerRole)
    END_REGISTER_THREAD(_providerThread)
END_MODEL(_providerModel)
}

namespace refuse_model
{
BEGIN_MODEL(_refuseModel)
    BEGIN_REGISTER_THREAD_EX2(_refuseThread, areg::WATCHDOG_IGNORE, areg::DEFAULT_STACK_SIZE, REFUSE_QUEUE, areg::Bool::True, areg::WAIT_INFINITE)
        BEGIN_REGISTER_COMPONENT(_refuseRole, RfRefuser)
            REGISTER_IMPLEMENT_SERVICE(HelloService::ServiceName, HelloService::InterfaceVersion)
        END_REGISTER_COMPONENT(_refuseRole)
    END_REGISTER_THREAD(_refuseThread)
END_MODEL(_refuseModel)
}

namespace capture_model
{
BEGIN_MODEL(_captureModel)
    BEGIN_REGISTER_THREAD(_captureThread)
        BEGIN_REGISTER_COMPONENT(_captureRole, RfCapture)
        END_REGISTER_COMPONENT(_captureRole)
    END_REGISTER_THREAD(_captureThread)
END_MODEL(_captureModel)
}

//////////////////////////////////////////////////////////////////////////
// Cases
//////////////////////////////////////////////////////////////////////////

static bool before_connect()
{
    gMode.store(Mode::BeforeConnect);
    if (areg::Application::load_model(_consumerModel) == false)
        return false;

    const bool failed{ gFailed.wait_for(1) };
    areg::Application::unload_model(_consumerModel);

    printf("failures .................: %u\n", gFailures.load());
    printf("reason ...................: %s\n", areg::as_string(static_cast<areg::ResultType>(gFailReason.load())));
    return failed
        && (gFailures.load() == 1)
        && (gFailReason.load() == static_cast<uint32_t>(areg::ResultType::ServiceUnavailable));
}

static bool provider_gone()
{
    gMode.store(Mode::ProviderGone);
    if ((areg::Application::load_model(_consumerModel) == false) || (areg::Application::load_model(_providerModel) == false))
        return false;

    const bool received{ gReceived.wait_for(1) };
    areg::Application::unload_model(_providerModel);
    const bool failed{ received && gFailed.wait_for(1) };
    areg::Application::unload_model(_consumerModel);

    printf("request received .........: %s\n", received ? "yes" : "no");
    printf("failures .................: %u\n", gFailures.load());
    printf("reason ...................: %s\n", areg::as_string(static_cast<areg::ResultType>(gFailReason.load())));
    printf("disconnect step, fail step: %u, %u\n", gDisconnectStep.load(), gFailStep.load());
    return failed
        && (gFailures.load() == 1)
        && (gResponses.load() == 0)
        && (gFailReason.load() == static_cast<uint32_t>(areg::ResultType::ServiceUnavailable))
        && (gDisconnectStep.load() != 0)
        && (gDisconnectStep.load() < gFailStep.load());
}

//!< Routes one remote request with the given sequence number, as the router client does.
static void route_remote_request(SequenceNumber seqNr)
{
    areg::MessageEnvelope wire(static_cast<uint16_t>(areg::EventType::EventRemoteRequest)
                             , static_cast<uint8_t>(areg::EventPriority::NormalPrio));
    wire.set_event_id(HelloServiceRequestEvent::CLASS_ID);
    areg::EventHeader * hdr{ wire.header() };
    hdr->provider.number = gStubKey.load();
    hdr->messageId       = static_cast<uint32_t>(HelloService::MessageIDs::MsgId_request_hello_service);
    hdr->sequenceNr      = seqNr;
    hdr->source          = REMOTE_COOKIE;
    hdr->consumer.number = REMOTE_MAGIC;

    areg::Channel channel;
    channel.set_source(gCaptureNumber.load());
    static_cast<void>(areg::RemoteEventFactory::route_incoming_message(wire, channel));
}

static bool remote_refused()
{
    if ((areg::Application::load_model(_captureModel) == false) || (areg::Application::load_model(_refuseModel) == false))
        return false;

    const bool started{ gStarted.wait_for(2) };
    route_remote_request(1);
    const bool received{ started && gReceived.wait_for(1) };
    for (SequenceNumber seqNr = 2; received && (seqNr <= FLOOD_COUNT + 1); ++ seqNr)
    {
        route_remote_request(seqNr);
    }

    const bool replied{ received && gReplies.wait_for(FLOOD_COUNT - REFUSE_QUEUE) };
    gRelease.advance();
    areg::Application::unload_model(_refuseModel);
    areg::Application::unload_model(_captureModel);

    printf("provider blocked .........: %s\n", received ? "yes" : "no");
    printf("requests flooded .........: %u\n", received ? FLOOD_COUNT : 0u);
    printf("undelivered replies ......: %u\n", gUndelivered.load());
    printf("wrong replies ............: %u\n", gWrongReply.load());
    printf("other replies ............: %u\n", gOtherReplies.load());
    return replied
        && (gUndelivered.load() >= FLOOD_COUNT - REFUSE_QUEUE)
        && (gWrongReply.load() == 0);
}

int main(int argc, char * argv[])
{
    setvbuf(stdout, nullptr, _IONBF, 0);

    const char * name{ argc > 1 ? argv[1] : "before-connect" };
    areg::Application::setup(false, true, false, true, false, nullptr);

    bool ok{ false };
    if (std::strcmp(name, "before-connect") == 0)
    {
        ok = before_connect();
    }
    else if (std::strcmp(name, "provider-gone") == 0)
    {
        ok = provider_gone();
    }
    else if (std::strcmp(name, "remote-refused") == 0)
    {
        ok = remote_refused();
    }
    else
    {
        printf("unknown case [ %s ]\n", name);
        areg::Application::release();
        return 2;
    }

    areg::Application::release();

    printf("case .....................: %s\n", name);
    printf("%s\n", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
