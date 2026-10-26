/************************************************************************
 * Routes service events to providers and consumers from a foreign thread, the way the router
 * client receive thread or a provider of another thread does, while the target is loaded and
 * unloaded or deleted again and again. A deleted provider or proxy poisons its own memory, so
 * an event routed through a released object crashes instead of passing silently.
 *
 * Usage: areg-stub-teardown-test <case>
 *   stub-request   remote requests to a provider that is loaded and unloaded
 *   stub-address   the names of a provider that is loaded and unloaded, read by its key
 *   proxy-unload   remote responses to a consumer that is loaded and unloaded
 *   proxy-free     remote responses to a consumer deleted and created on a running thread
 *   local-unload   local responses to a consumer that is loaded and unloaded
 *   local-many     the same from several senders and from short-lived threads that end
 *   local-churn    local-many while another thread is created and ended again and again
 *   worker-unload  events posted by thread number to a worker thread that is loaded and unloaded
 *   watchdog-restart  events posted by thread number to a component thread that its watchdog
 *                  replaces with a new instance (failure policy Restart)
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/component/Channel.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/NotificationEvent.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/ProxyBase.hpp"
#include "areg/component/ProxyListener.hpp"
#include "areg/component/RemoteEventFactory.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/StubBase.hpp"
#include "areg/component/EventTemplate.hpp"
#include "areg/component/WorkerThread.hpp"
#include "areg/component/WorkerThreadConsumer.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"

#include <atomic>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <mutex>
#include <thread>
#include <vector>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
#endif // _MSC_VER

#if defined(__SANITIZE_THREAD__) || defined(__SANITIZE_ADDRESS__)
    #define AREG_TEARDOWN_SANITIZED
#elif defined(__has_feature)
    #if __has_feature(thread_sanitizer) || __has_feature(address_sanitizer)
        #define AREG_TEARDOWN_SANITIZED
    #endif  // __has_feature(thread_sanitizer) || __has_feature(address_sanitizer)
#endif  // defined(__SANITIZE_THREAD__) || defined(__SANITIZE_ADDRESS__)

namespace
{
    constexpr char const    _providerModel[]    { "StubTeardownModel" };
    constexpr char const    _consumerModel[]    { "ProxyTeardownModel" };
    constexpr char const    _hostModel[]        { "ProxyFreeModel" };
    constexpr char const    _providerRole[]     { "StubTeardownProvider" };
    constexpr char const    _consumerRole[]     { "ProxyTeardownConsumer" };
    constexpr char const    _hostRole[]         { "ProxyFreeHost" };
    constexpr char const    _serviceRole[]      { "StubTeardownService" };
    constexpr char const    _providerThread[]   { "StubTeardownThread" };
    constexpr char const    _consumerThread[]   { "ProxyTeardownThread" };
    constexpr char const    _hostThread[]       { "ProxyFreeThread" };
    constexpr char const    _serviceName[]      { "StubTeardownService" };
    constexpr char const    _workerModel[]      { "WorkerTeardownModel" };
    constexpr char const    _workerRole[]       { "WorkerTeardownOwner" };
    constexpr char const    _ownerThread[]      { "WorkerOwnerThread" };
    constexpr char const    _workerThread[]     { "WorkerTeardownThread" };
    constexpr char const    _workerConsumer[]   { "WorkerTeardownConsumer" };
    constexpr char const    _churnThread[]      { "TeardownChurnThread" };
    constexpr char const    _watchModel[]       { "WatchdogTeardownModel" };
    constexpr char const    _watchRole[]        { "WatchdogTeardownBlocker" };
    constexpr char const    _watchThread[]      { "WatchdogTeardownThread" };
    constexpr areg::Version _version            { 1, 0, 0 };

    constexpr uint32_t      LOAD_ROUNDS         { 200 };
#if defined(AREG_TEARDOWN_SANITIZED)
    constexpr uint32_t      CONSUMER_ROUNDS     { 200 };
    constexpr uint32_t      MANY_ROUNDS         { 200 };
#else   // defined(AREG_TEARDOWN_SANITIZED)
    constexpr uint32_t      CONSUMER_ROUNDS     { 2000 };
    constexpr uint32_t      MANY_ROUNDS         { 1000 };
#endif  // defined(AREG_TEARDOWN_SANITIZED)
    constexpr uint32_t      RECREATE_ROUNDS     { 2000 };
    constexpr uint32_t      RESPONSE_BURST      { 32 };
    constexpr uint32_t      SHORT_BURST         { 64 };
    constexpr uint32_t      WATCH_TIMEOUT       { 50 };
    constexpr uint32_t      WATCH_BLOCK         { 2000 };
    constexpr uint32_t      WATCH_RESTARTS      { 10 };

    constexpr uint32_t      _requestList[]      { areg::REQUEST_ID_FIRST };
    constexpr uint32_t      _noResponse[]       { areg::RESPONSE_ID_NONE };
    constexpr uint32_t      RESPONSE_ID         { areg::RESPONSE_ID_FIRST };

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

    //!< A counter a thread waits on until it reaches a value.
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

        void wait_for(uint32_t value)
        {
            std::unique_lock<std::mutex> lock(mLock);
            mCond.wait(lock, [this, value] { return mValue >= value; });
        }

    private:
        std::mutex              mLock;      //!< Guards the value.
        std::condition_variable mCond;      //!< Signalled on every advance.
        uint32_t                mValue{ 0 };//!< The number of advances.
    };

    std::atomic_uint32_t    gTargetKey  { 0 };
    std::atomic_bool        gStop       { false };
    std::atomic_uint32_t    gCreated    { 0 };
    std::atomic_uint32_t    gDeleted    { 0 };
    std::atomic_uint32_t    gHandled    { 0 };
    std::atomic_uint32_t    gNamed      { 0 };
    std::atomic_uint32_t    gWrongName  { 0 };
    std::atomic_uint64_t    gRouted     { 0 };
    std::atomic_uint32_t    gRound      { 0 };
    std::atomic_uint32_t    gRoutedRound{ 0 };
    std::atomic_uint32_t    gShortLived { 0 };
    std::atomic_uint32_t    gChurned    { 0 };
    std::atomic_uint32_t    gWorkerNumber{ 0 };
    std::atomic_uint32_t    gWatchNumber{ 0 };
    std::atomic_bool        gWatchCalm  { false };

    Gate                    gStarted;
    Gate                    gRoutedGate;
    Gate                    gRecreated;
    Gate                    gCalmStart;

    std::mutex              gAddressLock;
    areg::ProxyAddress      gProxyAddress;

    //!< Fills a released block, so a stale pointer to the object faults immediately.
    void _poison_delete(void * block, size_t size)
    {
        std::memset(block, 0xDD, size);
        ::operator delete(block);
    }

    //!< Counts a routed event and opens the gate of the current round once.
    void _count_routed()
    {
        gRouted.fetch_add(1);
        const uint32_t round{ gRound.load() };
        uint32_t seen{ gRoutedRound.load() };
        if ((seen != round) && gRoutedRound.compare_exchange_strong(seen, round))
        {
            gRoutedGate.advance();
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// Provider
//////////////////////////////////////////////////////////////////////////

//!< Service provider with one request, which it only counts.
class TeardownProvider final : public    areg::Component
                             , protected areg::StubBase
{
public:
    TeardownProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component   ( entry, owner )
        , areg::StubBase    ( static_cast<areg::Component &>(*this), _interfaceData )
    {
        gTargetKey.store(static_cast<uint32_t>(areg::StubBase::address()));
        gCreated.fetch_add(1);
    }

    ~TeardownProvider() override
    {
        gDeleted.fetch_add(1);
    }

    static void operator delete (void * block, size_t size)
    {
        _poison_delete(block, size);
    }

    static void * operator new (size_t size)
    {
        return ::operator new(size);
    }

    void startup_component(areg::ComponentThread & comThread) override
    {
        areg::Component::startup_component(comThread);
        gStarted.advance();
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

//////////////////////////////////////////////////////////////////////////
// Proxy and consumer
//////////////////////////////////////////////////////////////////////////

//!< The remote response event of the test service.
class TeardownRemoteResponse final : public areg::RemoteResponseEvent
{
    AREG_DECLARE_RUNTIME_EVENT(TeardownRemoteResponse)
public:
    explicit TeardownRemoteResponse(const areg::MessageEnvelope & envelope) noexcept
        : areg::RemoteResponseEvent(envelope)
    {
    }
};
AREG_IMPLEMENT_RUNTIME_EVENT(TeardownRemoteResponse, areg::RemoteResponseEvent)

//!< The local response event of the test service.
class TeardownLocalResponse final : public areg::LocalResponseEvent
{
    AREG_DECLARE_RUNTIME_EVENT(TeardownLocalResponse)
public:
    explicit TeardownLocalResponse(const areg::ProxyAddress & target)
        : areg::LocalResponseEvent(target, areg::ResultType::RequestOK, RESPONSE_ID)
    {
        header()->eventId = TeardownLocalResponse::CLASS_ID;
    }
};
AREG_IMPLEMENT_RUNTIME_EVENT(TeardownLocalResponse, areg::LocalResponseEvent)

//!< Proxy of the test service; it only counts the responses it gets.
class TeardownProxy final : public areg::ProxyBase
{
public:
    static std::shared_ptr<areg::ProxyBase> create(const areg::String & roleName, areg::ProxyListener & listener, areg::DispatcherThread & owner)
    {
        return areg::ProxyBase::acquire_proxy(roleName, _interfaceData, listener, &TeardownProxy::_create, owner);
    }

    static void operator delete (void * block, size_t size)
    {
        _poison_delete(block, size);
    }

    static void * operator new (size_t size)
    {
        return ::operator new(size);
    }

protected:
    TeardownProxy(const areg::String & roleName, areg::DispatcherThread * owner)
        : areg::ProxyBase(roleName, _interfaceData, owner)
    {
    }

    void process_response_event(areg::ServiceResponseEvent & eventElem) override
    {
        if (eventElem.response_id() == RESPONSE_ID)
        {
            gHandled.fetch_add(1);
        }
    }

    void process_custom_event(areg::Event & /*eventElem*/) override
    {
    }

    void register_service_listeners() override
    {
        areg::ProxyBase::register_service_listeners();
        TeardownRemoteResponse::add_listener(static_cast<areg::EventConsumer &>(*this), proxy_dispatcher_thread());
        TeardownLocalResponse::add_listener(static_cast<areg::EventConsumer &>(*this), proxy_dispatcher_thread());
    }

    void unregister_service_listeners() noexcept override
    {
        TeardownRemoteResponse::remove_listener(static_cast<areg::EventConsumer &>(*this), proxy_dispatcher_thread());
        TeardownLocalResponse::remove_listener(static_cast<areg::EventConsumer &>(*this), proxy_dispatcher_thread());
        areg::ProxyBase::unregister_service_listeners();
    }

    areg::ProxyBase::ServiceAvailableEvent create_service_available(areg::NotificationConsumer & consumer) override
    {
        return areg::ProxyBase::ServiceAvailableEvent(consumer);
    }

    areg::NotificationEvent create_client_notification(const areg::NotificationEventData & data) const override
    {
        return areg::NotificationEvent(data);
    }

    areg::ServiceRequestEvent create_request(const areg::SharedBuffer & /*args*/, uint32_t /*reqId*/) override
    {
        return areg::ServiceRequestEvent(areg::MessageEnvelope{});
    }

    areg::ServiceRequestEvent create_request(uint32_t /*reqId*/, uint32_t /*reserve*/) override
    {
        return areg::ServiceRequestEvent(areg::MessageEnvelope{});
    }

    areg::ServiceRequestEvent create_notification_request(uint32_t /*msgId*/, areg::RequestType /*reqType*/) override
    {
        return areg::ServiceRequestEvent(areg::MessageEnvelope{});
    }

    areg::ServiceResponseEvent create_request_failed(const areg::ProxyAddress & /*addrProxy*/, uint32_t /*msgId*/, areg::ResultType /*reason*/, const SequenceNumber & /*seqNr*/) const override
    {
        return areg::ServiceResponseEvent(areg::MessageEnvelope{});
    }

private:
    static areg::ProxyBase * _create(const areg::String & roleName, areg::DispatcherThread * owner)
    {
        return new TeardownProxy(roleName, owner);
    }
};

//!< Owns one proxy of the test service and publishes its key and address.
class TeardownClient : public areg::ProxyListener
{
public:
    explicit TeardownClient(areg::DispatcherThread & owner)
        : areg::ProxyListener   ( )
        , mProxy                ( TeardownProxy::create(_serviceRole, *this, owner) )
    {
        if (mProxy != nullptr)
        {
            std::lock_guard<std::mutex> lock(gAddressLock);
            gProxyAddress = mProxy->proxy_address();
            gTargetKey.store(static_cast<uint32_t>(mProxy->proxy_address()));
        }
    }

    ~TeardownClient() override
    {
        if (mProxy != nullptr)
        {
            mProxy->free_proxy(*this);
        }
    }

    bool service_connected(areg::ServiceConnectionState /*status*/, areg::ProxyBase & /*proxy*/) override
    {
        return true;
    }

    void process_notification_event(areg::NotificationEvent & /*eventElem*/) override
    {
    }

    void process_notification(uint32_t /*msgId*/, areg::ResultType /*result*/, SequenceNumber /*seqNr*/) override
    {
    }

private:
    std::shared_ptr<areg::ProxyBase> mProxy;    //!< The proxy, released by free_proxy().
};

//!< A component that is a consumer of the test service.
class TeardownConsumer final : public areg::Component
{
public:
    TeardownConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component   ( entry, owner )
        , mClient           ( owner )
    {
        gCreated.fetch_add(1);
    }

    ~TeardownConsumer() override
    {
        gDeleted.fetch_add(1);
    }

    void startup_component(areg::ComponentThread & comThread) override
    {
        areg::Component::startup_component(comThread);
        gStarted.advance();
    }

private:
    TeardownClient  mClient;    //!< The consumer of the test service.
};

//!< The request to delete and create the consumer again.
class RecreateData
{
public:
    RecreateData() = default;
    RecreateData(const RecreateData & /*src*/) = default;
    RecreateData & operator = (const RecreateData & /*src*/) = default;
};

AREG_DECLARE_EVENT(RecreateData, RecreateEvent, RecreateConsumer);

class ProxyFreeHost;
std::atomic<ProxyFreeHost *>        gHost       { nullptr };
std::atomic<areg::ComponentThread *> gHostThread { nullptr };

//!< Deletes and creates its consumer on every recreate event, on its own thread.
class ProxyFreeHost final : public    areg::Component
                          , public    RecreateConsumer
{
public:
    ProxyFreeHost(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component   ( entry, owner )
        , RecreateConsumer  ( )
        , mClient           ( nullptr )
    {
    }

    void startup_component(areg::ComponentThread & comThread) override
    {
        areg::Component::startup_component(comThread);
        mClient = new TeardownClient(comThread);
        gHostThread.store(&comThread);
        gHost.store(this);
        gStarted.advance();
    }

    void shutdown_component(areg::ComponentThread & comThread) override
    {
        gHost.store(nullptr);
        delete mClient;
        mClient = nullptr;
        areg::Component::shutdown_component(comThread);
    }

protected:
    void process_event(const RecreateData & /*data*/) override
    {
        delete mClient;
        mClient = new TeardownClient(master_thread());
        gRecreated.advance();
    }

private:
    TeardownClient *    mClient;    //!< The consumer deleted and created again.
};

//////////////////////////////////////////////////////////////////////////
// Worker thread owner
//////////////////////////////////////////////////////////////////////////

//!< The event posted to the worker thread.
class WorkData
{
public:
    WorkData() = default;
    WorkData(const WorkData & /*src*/) = default;
    WorkData & operator = (const WorkData & /*src*/) = default;
};

AREG_DECLARE_EVENT(WorkData, WorkEvent, WorkConsumer);

//!< A component whose worker thread counts the events posted to it.
class WorkerOwner final : public    areg::Component
                        , public    areg::WorkerThreadConsumer
                        , public    WorkConsumer
{
public:
    WorkerOwner(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , areg::WorkerThreadConsumer( _workerConsumer )
        , WorkConsumer              ( )
    {
        gCreated.fetch_add(1);
    }

    ~WorkerOwner() override
    {
        gDeleted.fetch_add(1);
    }

    areg::WorkerThreadConsumer * worker_thread_consumer(const areg::String & /*consumerName*/, const areg::String & /*workerThreadName*/) override
    {
        return this;
    }

    void notify_thread_started(areg::WorkerThreadConsumer & /*consumer*/, areg::WorkerThread & workerThread) override
    {
        static_cast<void>(WorkEvent::add_listener(static_cast<WorkConsumer &>(*this), workerThread));
        gWorkerNumber.store(static_cast<uint32_t>(workerThread.address()));
    }

    void startup_component(areg::ComponentThread & comThread) override
    {
        areg::Component::startup_component(comThread);
        gStarted.advance();
    }

protected:
    void process_event(const WorkData & /*data*/) override
    {
        gHandled.fetch_add(1);
    }
};

class BlockData
{
public:
    BlockData() = default;
    BlockData(const BlockData & /*src*/) = default;
    BlockData & operator = (const BlockData & /*src*/) = default;
};

AREG_DECLARE_EVENT(BlockData, BlockEvent, BlockConsumer);

//!< Blocks its thread past the watchdog timeout on every start, and counts the events posted to it.
class WatchdogBlocker final : public    areg::Component
                            , public    WorkConsumer
                            , public    BlockConsumer
{
public:
    WatchdogBlocker(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component   ( entry, owner )
        , WorkConsumer      ( )
        , BlockConsumer     ( )
    {
        gCreated.fetch_add(1);
    }

    ~WatchdogBlocker() override
    {
        gDeleted.fetch_add(1);
    }

    void startup_component(areg::ComponentThread & comThread) override
    {
        areg::Component::startup_component(comThread);
        static_cast<void>(WorkEvent::add_listener(static_cast<WorkConsumer &>(*this), comThread));
        static_cast<void>(BlockEvent::add_listener(static_cast<BlockConsumer &>(*this), comThread));
        gWatchNumber.store(static_cast<uint32_t>(comThread.address()));

        if (gWatchCalm.load())
        {
            gCalmStart.advance();
            return;
        }

        // High priority: queued even when the poster has already filled the queue of this thread.
        BlockEvent block(BlockData{ }, areg::EventPriority::HighPrio);
        if (block.register_for_thread(&comThread))
        {
            block.deliver_event();
        }

        gStarted.advance();
    }

protected:
    void process_event(const WorkData & /*data*/) override
    {
        gHandled.fetch_add(1);
    }

    void process_event(const BlockData & /*data*/) override
    {
        static_cast<void>(areg::Thread::wait_exit(WATCH_BLOCK));
    }
};

//!< A thread consumer that returns at once.
class ChurnConsumer final : public areg::ThreadConsumer
{
public:
    void on_run() override
    {
    }
};

namespace provider_model
{
BEGIN_MODEL(_providerModel)
    BEGIN_REGISTER_THREAD(_providerThread)
        BEGIN_REGISTER_COMPONENT(_providerRole, TeardownProvider)
            REGISTER_IMPLEMENT_SERVICE(_serviceName, _version)
        END_REGISTER_COMPONENT(_providerRole)
    END_REGISTER_THREAD(_providerThread)
END_MODEL(_providerModel)
}


namespace consumer_model
{
BEGIN_MODEL(_consumerModel)
    BEGIN_REGISTER_THREAD(_consumerThread)
        BEGIN_REGISTER_COMPONENT(_consumerRole, TeardownConsumer)
        END_REGISTER_COMPONENT(_consumerRole)
    END_REGISTER_THREAD(_consumerThread)
END_MODEL(_consumerModel)
}


namespace host_model
{
BEGIN_MODEL(_hostModel)
    BEGIN_REGISTER_THREAD(_hostThread)
        BEGIN_REGISTER_COMPONENT(_hostRole, ProxyFreeHost)
        END_REGISTER_COMPONENT(_hostRole)
    END_REGISTER_THREAD(_hostThread)
END_MODEL(_hostModel)
}


namespace watch_model
{
BEGIN_MODEL(_watchModel)
    BEGIN_REGISTER_THREAD_EX(_watchThread, WATCH_TIMEOUT)
        THREAD_FAILURE_POLICY(areg::FailurePolicy::Restart)
        BEGIN_REGISTER_COMPONENT(_watchRole, WatchdogBlocker)
        END_REGISTER_COMPONENT(_watchRole)
    END_REGISTER_THREAD(_watchThread)
END_MODEL(_watchModel)
}


namespace worker_model
{
BEGIN_MODEL(_workerModel)
    BEGIN_REGISTER_THREAD(_ownerThread)
        BEGIN_REGISTER_COMPONENT(_workerRole, WorkerOwner)
            REGISTER_WORKER_THREAD(_workerThread, _workerConsumer)
        END_REGISTER_COMPONENT(_workerRole)
    END_REGISTER_THREAD(_ownerThread)
END_MODEL(_workerModel)
}


//////////////////////////////////////////////////////////////////////////
// Routing threads
//////////////////////////////////////////////////////////////////////////

//!< Routes remote requests to the provider.
static void route_requests()
{
    const areg::Channel channel{ };
    while (gStop.load() == false)
    {
        areg::MessageEnvelope wire(static_cast<uint16_t>(areg::EventType::EventRemoteRequest)
                                 , static_cast<uint8_t>(areg::EventPriority::NormalPrio));
        areg::EventHeader * hdr{ wire.header() };
        hdr->provider.number = gTargetKey.load();
        hdr->messageId       = areg::REQUEST_ID_FIRST;

        if (areg::RemoteEventFactory::route_incoming_message(wire, channel))
            _count_routed();
    }
}

//!< Reads the names of the provider by its key, as a header carries it.
static void read_addresses()
{
    const areg::String roleName{ _providerRole };
    while (gStop.load() == false)
    {
        areg::EventHeader hdr{ };
        hdr.provider.number = gTargetKey.load();
        const areg::StubAddress named{ areg::StubBase::registered_address(areg::StubAddress(hdr)) };
        if (named.role_name() == roleName)
        {
            gNamed.fetch_add(1);
            _count_routed();
        }
        else if (named.role_name().is_empty() == false)
        {
            gWrongName.fetch_add(1);
        }
    }
}

//!< Routes remote responses to the proxy.
static void route_responses()
{
    const areg::Channel channel{ };
    while (gStop.load() == false)
    {
        areg::MessageEnvelope wire(static_cast<uint16_t>(areg::EventType::EventRemoteResponse)
                                 , static_cast<uint8_t>(areg::EventPriority::NormalPrio));
        areg::EventHeader * hdr{ wire.header() };
        hdr->consumer.number = gTargetKey.load();
        hdr->messageId       = RESPONSE_ID;
        hdr->eventId         = TeardownRemoteResponse::CLASS_ID;

        if (areg::RemoteEventFactory::route_incoming_message(wire, channel))
            _count_routed();
    }
}

//!< Routes one burst of remote responses to the current proxy.
static void route_burst()
{
    const areg::Channel channel{ };
    for (uint32_t i = 0; i < RESPONSE_BURST; ++ i)
    {
        areg::MessageEnvelope wire(static_cast<uint16_t>(areg::EventType::EventRemoteResponse)
                                 , static_cast<uint8_t>(areg::EventPriority::NormalPrio));
        areg::EventHeader * hdr{ wire.header() };
        hdr->consumer.number = gTargetKey.load();
        hdr->messageId       = RESPONSE_ID;
        hdr->eventId         = TeardownRemoteResponse::CLASS_ID;

        if (areg::RemoteEventFactory::route_incoming_message(wire, channel))
            gRouted.fetch_add(1);
    }
}

//!< Routes responses before and after each recreate request, so some are queued behind it.
static void route_around_recreation(uint32_t rounds)
{
    for (uint32_t round = 0; round < rounds; ++ round)
    {
        gRecreated.wait_for(round);
        route_burst();
        static_cast<void>(RecreateEvent::send_event(RecreateData(), *gHost.load(), *gHostThread.load()));
        route_burst();
    }
}

//!< Delivers one local response to the proxy, as a provider of another thread does.
static void deliver_response()
{
    areg::ProxyAddress target;
    {
        std::lock_guard<std::mutex> lock(gAddressLock);
        target = gProxyAddress;
    }

    TeardownLocalResponse response(target);
    if (target.deliver_service_event(response))
        _count_routed();
}

//!< Delivers local responses to the proxy until the case stops.
static void deliver_responses()
{
    while (gStop.load() == false)
    {
        deliver_response();
    }
}

//!< Starts threads one after another that deliver a burst of local responses each and end.
static void deliver_from_short_threads()
{
    while (gStop.load() == false)
    {
        std::thread burst([]
        {
            for (uint32_t i = 0; (i < SHORT_BURST) && (gStop.load() == false); ++ i)
            {
                deliver_response();
            }
        });

        burst.join();
        gShortLived.fetch_add(1);
    }
}

//!< Creates and ends a thread again and again, so the lookups of every thread are searched again.
static void churn_registry()
{
    ChurnConsumer consumer;
    while (gStop.load() == false)
    {
        areg::Thread thread(consumer, _churnThread);
        if (thread.start(areg::WAIT_INFINITE))
        {
            static_cast<void>(thread.shutdown(areg::WAIT_INFINITE));
        }

        gChurned.fetch_add(1);
    }
}

//!< Posts events to the worker thread by its number, as another thread of the process does.
static void post_to_worker()
{
    while (gStop.load() == false)
    {
        WorkEvent work(WorkData{ });
        if (work.register_for_thread(static_cast<UniqueNumber>(gWorkerNumber.load())))
        {
            work.deliver_event();
            _count_routed();
        }
    }
}

//!< Posts events by its number to the thread the watchdog restarts.
static void post_to_watched()
{
    while (gStop.load() == false)
    {
        WorkEvent work(WorkData{ });
        if (work.register_for_thread(static_cast<UniqueNumber>(gWatchNumber.load())))
        {
            work.deliver_event();
            _count_routed();
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// Cases
//////////////////////////////////////////////////////////////////////////

//!< Lets the watchdog restart the thread while another thread posts to it.
static uint32_t watchdog_restarts(uint32_t restarts)
{
    gRound.store(1);
    if (areg::Application::load_model(_watchModel) == false)
        return 0;

    gStarted.wait_for(1);
    std::thread poster(&post_to_watched);
    gStarted.wait_for(restarts + 1);
    gStop.store(true);
    poster.join();

    // Unloads after a start that does not block, while no restart runs.
    gWatchCalm.store(true);
    gCalmStart.wait_for(1);
    areg::Application::unload_model(_watchModel);
    return restarts;
}

//!< Loads and unloads the model while every routine runs on a thread of its own.
static uint32_t load_unload(const char * modelName, uint32_t rounds, std::initializer_list<void (*)()> routines)
{
    std::vector<std::thread> threads;
    for (void (*routine)() : routines)
    {
        threads.emplace_back(routine);
    }

    uint32_t round{ 0 };
    for ( ; round < rounds; ++ round)
    {
        gRound.store(round + 1);
        if (areg::Application::load_model(modelName) == false)
            break;

        gStarted.wait_for(round + 1);
        gRoutedGate.wait_for(round + 1);
        areg::Application::unload_model(modelName);
    }

    gStop.store(true);
    for (std::thread & thread : threads)
    {
        thread.join();
    }

    return round;
}

//!< Lets the host recreate its consumer while the responses are routed to it.
static uint32_t recreate(uint32_t rounds)
{
    gRound.store(1);
    if (areg::Application::load_model(_hostModel) == false)
        return 0;

    gStarted.wait_for(1);
    std::thread router(&route_around_recreation, rounds);
    gRecreated.wait_for(rounds);
    router.join();
    areg::Application::unload_model(_hostModel);
    return rounds;
}

int main(int argc, char * argv[])
{
    // Unbuffered: what the run printed before a crash still reaches the log.
    setvbuf(stdout, nullptr, _IONBF, 0);

    const char * name{ argc > 1 ? argv[1] : "stub-request" };
    const bool watchdog{ std::strcmp(name, "watchdog-restart") == 0 };
    areg::Application::setup(false, true, false, true, watchdog, nullptr);

    uint32_t expected{ 0 };
    uint32_t rounds{ 0 };
    bool created{ true };
    if (std::strcmp(name, "stub-request") == 0)
    {
        expected = LOAD_ROUNDS;
        rounds = load_unload(_providerModel, expected, { &route_requests });
    }
    else if (std::strcmp(name, "stub-address") == 0)
    {
        expected = LOAD_ROUNDS;
        rounds = load_unload(_providerModel, expected, { &read_addresses });
    }
    else if (std::strcmp(name, "proxy-unload") == 0)
    {
        expected = CONSUMER_ROUNDS;
        rounds = load_unload(_consumerModel, expected, { &route_responses });
    }
    else if (std::strcmp(name, "local-unload") == 0)
    {
        expected = CONSUMER_ROUNDS;
        rounds = load_unload(_consumerModel, expected, { &deliver_responses });
    }
    else if (std::strcmp(name, "local-many") == 0)
    {
        expected = MANY_ROUNDS;
        rounds = load_unload(_consumerModel, expected, { &deliver_responses, &deliver_responses, &deliver_responses, &deliver_responses, &deliver_from_short_threads, &deliver_from_short_threads });
    }
    else if (std::strcmp(name, "local-churn") == 0)
    {
        expected = MANY_ROUNDS;
        rounds = load_unload(_consumerModel, expected, { &deliver_responses, &deliver_responses, &deliver_responses, &deliver_from_short_threads, &churn_registry });
    }
    else if (std::strcmp(name, "worker-unload") == 0)
    {
        expected = MANY_ROUNDS;
        rounds = load_unload(_workerModel, expected, { &post_to_worker, &post_to_worker, &post_to_worker, &post_to_worker, &churn_registry });
    }
    else if (watchdog)
    {
        expected = WATCH_RESTARTS;
        rounds = watchdog_restarts(expected);
        created = false;
    }
    else if (std::strcmp(name, "proxy-free") == 0)
    {
        expected = RECREATE_ROUNDS;
        rounds = recreate(expected);
        created = false;
    }
    else
    {
        printf("unknown case [ %s ]\n", name);
        areg::Application::release();
        return 2;
    }

    areg::Application::release();

    const uint32_t numCreated{ gCreated.load() };
    const uint32_t numDeleted{ gDeleted.load() };
    const uint32_t handled{ gHandled.load() };
    const uint64_t routed{ gRouted.load() };
    const bool counted{ watchdog || (std::strcmp(name, "stub-address") == 0 ? (gNamed.load() > 0) && (gWrongName.load() == 0) : (handled > 0)) };

    printf("case .....................: %s\n", name);
    printf("rounds ...................: %u\n", rounds);
    printf("objects created ..........: %u\n", numCreated);
    printf("objects deleted ..........: %u\n", numDeleted);
    printf("events routed ............: %llu\n", static_cast<unsigned long long>(routed));
    printf("events handled ...........: %u\n", handled);
    printf("names read, wrong names ..: %u, %u\n", gNamed.load(), gWrongName.load());
    printf("short threads, churned ...: %u, %u\n", gShortLived.load(), gChurned.load());

    const bool ok{ (rounds == expected)
                && ((created == false) || ((numCreated == expected) && (numDeleted == numCreated)))
                && ((watchdog == false) || ((numCreated > expected) && (numDeleted == numCreated)))
                && (routed >= expected)
                && counted };

    printf("%s\n", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
