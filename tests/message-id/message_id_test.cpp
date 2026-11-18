/************************************************************************
 * Failure reports of requests that share a response, a request the provider does not know,
 * and stop_all_notifications(). Uses the generated SharedResponse interface.
 *
 * Usage: areg-message-id-test <case>
 *   shared-busy            second request while it is pending: request_second_failed(RequestBusy)
 *   shared-cancel          cancel_all_requests() of a waiting second: request_second_failed(RequestCanceled)
 *   shared-error-request   error_request(second) of a waiting second: request_second_failed(RequestError)
 *   shared-error-response  error_request(done) of a waiting second: request_second_failed(RequestError)
 *   shared-unavailable     second called with no provider: request_second_failed(ServiceUnavailable)
 *   unknown-request        a request ID the interface does not have leaves the provider serving
 *   unsupported-request    a foreign proxy calls a request ID the provider does not have:
 *                          the call fails once with RequestUnsupported and its own sequence number
 *   remove-all             after stop_all_notifications() the provider sends no attribute update
 *
 * In every shared case request_first_failed must not run.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/ProxyBase.hpp"
#include "units/TestMessageTable.hpp"
#include "tests/message-id/services/SharedResponseConsumerBase.hpp"
#include "tests/message-id/services/SharedResponseProviderBase.hpp"
#include "tests/message-id/services/private/SharedResponseEvents.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <functional>
#include <mutex>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "message_id_generated")
#endif // _MSC_VER

namespace
{
    constexpr char const    _model[]        { "MiModel" };
    constexpr char const    _modelAlone[]   { "MiModelAlone" };
    constexpr char const    _providerThread[]{ "MiProviderThread" };
    constexpr char const    _consumerThread[]{ "MiConsumerThread" };
    constexpr char const    _providerRole[] { "MiProvider" };
    constexpr char const    _consumerRole[] { "MiConsumer" };
    constexpr char const    _aloneThread[]  { "MiAloneThread" };
    constexpr char const    _aloneRole[]    { "MiAloneConsumer" };
    constexpr char const    _missingRole[]  { "MiMissing" };

    constexpr uint32_t      WAIT_MS         { 30000 };
    constexpr uint32_t      TAG_HELD        { 1 };
    constexpr uint32_t      TAG_BUSY        { 2 };
    constexpr uint32_t      TAG_SYNC        { 7 };

    //!< A request ID that the SharedResponse interface does not have.
    constexpr uint32_t      UNKNOWN_REQUEST { areg::crc32_calculate("request_not_in_the_interface") };

    enum class Case : uint32_t { Busy, Cancel, ErrorRequest, ErrorResponse, Unavailable, Unknown, RemoveAll, Unsupported };

    constexpr char const    _modelForeign[]   { "MiModelForeign" };
    constexpr char const    _foreignThread[]  { "MiForeignThread" };
    constexpr char const    _foreignRole[]    { "MiForeignConsumer" };
    constexpr char const    _foreignProviderThread[]{ "MiForeignProviderThread" };
    constexpr char const    _foreignProvider[]{ "MiForeignProvider" };

    //!< The entries a foreign proxy sends: sync, and a request with a response the provider does not have.
    constexpr uint32_t      REQ_SYNC        { SharedResponse::message_id(SharedResponse::RequestIDs::MsgId_request_sync) };
    constexpr uint32_t      RESP_SYNCED     { SharedResponse::message_id(SharedResponse::ResponseIDs::MsgId_response_synced) };
    constexpr uint32_t      REQ_ADDED       { areg::crc32_calculate("request_added") };
    constexpr uint32_t      RESP_ADDED      { areg::crc32_calculate("response_added") };
    constexpr uint32_t      FOREIGN_REQUESTS[]    { REQ_SYNC, REQ_ADDED };
    constexpr uint32_t      FOREIGN_RESPONSES[]   { RESP_SYNCED, RESP_ADDED };
    constexpr uint32_t      FOREIGN_PARAMS[]      { 1u, 0u };
    constexpr uint16_t      FOREIGN_REQUEST_TO_RESPONSE[]{ 0u, 1u };
    static_assert((areg::count_message_id(FOREIGN_REQUESTS, REQ_ADDED) == 1u) && (areg::count_message_id(FOREIGN_RESPONSES, RESP_ADDED) == 1u), "the added entries are unique in their kind");

    const areg::InterfaceData & foreign_interface()
    {
        static const areg_test::TestMessageTable requests{ FOREIGN_REQUESTS, 2u };
        static const areg_test::TestMessageTable responses{ FOREIGN_RESPONSES, 2u };
        static const areg::InterfaceData ifData
        {
            areg_test::make_interface( SharedResponse::ServiceName, SharedResponse::InterfaceVersion, SharedResponse::interface_data().idServiceType
                                     , requests.table(), responses.table(), areg::MessageTable{ }, areg::MessageTable{ }
                                     , FOREIGN_REQUEST_TO_RESPONSE, FOREIGN_PARAMS )
        };

        return ifData;
    }

    std::atomic_uint32_t    gUnsupported    { 0 };  //!< RequestUnsupported failures of the added request.
    std::atomic_uint32_t    gOtherFailures  { 0 };  //!< any other failure seen by the foreign consumer.
    std::atomic<SequenceNumber> gAddedSeq   { 0 };  //!< sequence number of the added request.
    std::atomic<SequenceNumber> gFailedSeq  { 0 };  //!< sequence number of its failure.
    std::atomic_bool        gAddedLeft      { true };   //!< the proxy still had a listener of the added response.

    std::atomic<Case>       gCase           { Case::Busy };

    std::atomic_uint32_t    gFirstFailed    { 0 };  //!< request_first_failed calls.
    std::atomic_uint32_t    gSecondFailed   { 0 };  //!< request_second_failed calls.
    std::atomic_uint32_t    gReason         { 0 };  //!< reason of the last failure of first or second.
    std::atomic_uint32_t    gDone           { 0 };  //!< response_done calls.
    std::atomic_uint32_t    gSynced         { 0 };  //!< response_synced calls.
    std::atomic_uint32_t    gUpdates        { 0 };  //!< valid on_counter_update calls.
    std::atomic_uint32_t    gUpdatesAfter   { 0 };  //!< valid on_counter_update calls after stop_all_notifications().
    std::atomic_bool        gStopped        { false };

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

    //!< Reaches the protected request and notification calls of a generated proxy.
    class ProxyAccess : public areg::ProxyBase
    {
    public:
        static void stop_all(areg::ProxyBase & proxy)
        {
            (proxy.*(&ProxyAccess::stop_all_notifications))();
        }

        static void send_raw_request(areg::ProxyBase & proxy, uint32_t reqId)
        {
            areg::ServiceRequestEvent evt{ (proxy.*(&ProxyAccess::create_request_event))(reqId, 0u) };
            using SendFunc = SequenceNumber (areg::ProxyBase::*)(areg::ServiceRequestEvent &, areg::NotificationConsumer *, uint32_t);
            const SendFunc send{ &ProxyAccess::send_request_event };
            static_cast<void>((proxy.*send)(evt, nullptr, areg::RESPONSE_INDEX_NONE));
        }
    };
}

//////////////////////////////////////////////////////////////////////////
// Provider
//////////////////////////////////////////////////////////////////////////

//!< Holds the first call of second, fails it by the case, and answers sync.
class MiProvider final  : public    areg::Component
                        , protected SharedResponseProviderBase
{
public:
    MiProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , SharedResponseProviderBase( static_cast<areg::Component &>(*this) )
    {
    }

protected:
    void startup_service_interface(areg::Component & holder) override
    {
        SharedResponseProviderBase::startup_service_interface(holder);
        set_counter(1u);
    }

    void request_first(uint32_t tag) override
    {
        response_done(tag);
    }

    void request_second(uint32_t tag) override
    {
        mHeld = tag;
        switch (gCase.load())
        {
        case Case::Cancel:
            cancel_all_requests();
            break;

        case Case::ErrorRequest:
            error_request(areg::CallType::Request, static_cast<uint32_t>(SharedResponse::RequestIDs::MsgId_request_second), false);
            break;

        case Case::ErrorResponse:
            error_request(areg::CallType::Response, static_cast<uint32_t>(SharedResponse::ResponseIDs::MsgId_response_done), false);
            break;

        default:
            break;
        }
    }

    void request_sync(uint32_t tag) override
    {
        if ((gCase.load() == Case::Busy) && (mHeld != 0u))
        {
            response_done(mHeld);
            mHeld = 0u;
        }

        set_counter(counter() + 1u);
        response_synced(tag);
    }

private:
    uint32_t    mHeld{ 0u };    //!< Tag of the held call of second, 0 if none.
};

//////////////////////////////////////////////////////////////////////////
// Consumer
//////////////////////////////////////////////////////////////////////////

//!< Starts the case and records every callback.
class MiConsumer final  : public    areg::Component
                        , protected SharedResponseConsumerBase
{
public:
    MiConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , SharedResponseConsumerBase( gCase.load() == Case::Unavailable ? _missingRole : _providerRole, static_cast<areg::Component &>(*this) )
    {
    }

protected:
    void startup_component(areg::ComponentThread & comThread) override
    {
        areg::Component::startup_component(comThread);
        if (gCase.load() == Case::Unavailable)
        {
            request_second(TAG_HELD);
        }
    }

    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) override
    {
        const bool result{ SharedResponseConsumerBase::service_connected(status, proxy) };
        if (!areg::is_service_connected(status))
            return result;

        switch (gCase.load())
        {
        case Case::Busy:
            request_second(TAG_HELD);
            request_second(TAG_BUSY);
            break;

        case Case::Cancel:          // fall through
        case Case::ErrorRequest:    // fall through
        case Case::ErrorResponse:
            request_second(TAG_HELD);
            break;

        case Case::Unknown:
            ProxyAccess::send_raw_request(proxy, UNKNOWN_REQUEST);
            request_sync(TAG_SYNC);
            break;

        case Case::RemoveAll:
            notify_on_counter_update(true);
            break;

        default:
            break;
        }

        return result;
    }

    void request_first_failed(areg::ResultType reason) override
    {
        gReason.store(static_cast<uint32_t>(reason));
        gFirstFailed.fetch_add(1);
        after_failure();
    }

    void request_second_failed(areg::ResultType reason) override
    {
        gReason.store(static_cast<uint32_t>(reason));
        gSecondFailed.fetch_add(1);
        after_failure();
    }

    void response_done(uint32_t /* tag */) override
    {
        gDone.fetch_add(1);
        changed();
    }

    void response_synced(uint32_t /* tag */) override
    {
        gSynced.fetch_add(1);
        changed();
    }

    void on_counter_update(uint32_t /* Counter */, areg::DataState state) override
    {
        if (state != areg::DataState::DataIsOK)
            return;

        gUpdates.fetch_add(1);
        if (gStopped.load())
        {
            gUpdatesAfter.fetch_add(1);
        }
        else
        {
            gStopped.store(true);
            ProxyAccess::stop_all(const_cast<SharedResponseProxy &>(*service_proxy()));
            request_sync(TAG_SYNC);
        }

        changed();
    }

private:
    //!< Ends a connected case with a sync round trip; the unavailable case ends here.
    void after_failure()
    {
        if (gCase.load() != Case::Unavailable)
        {
            request_sync(TAG_SYNC);
        }

        changed();
    }
};

//////////////////////////////////////////////////////////////////////////
// Consumer with a foreign interface
//////////////////////////////////////////////////////////////////////////

//!< A proxy of SharedResponse built from foreign interface data.
class ForeignProxy final : public areg::ProxyBase
{
public:
    static areg::ProxyBase * create(const areg::String & role, areg::DispatcherThread * owner)
    {
        return new ForeignProxy(role, owner);
    }

    SequenceNumber request(uint32_t reqId, uint32_t respId, areg::NotificationConsumer & caller)
    {
        areg::SharedBuffer args;
        if (reqId == REQ_SYNC)
            args << TAG_SYNC;

        areg::ServiceRequestEvent evt{ create_request(args, reqId) };
        return send_request_event(evt, &caller, foreign_interface().find(areg::CallType::Response, respId));
    }

protected:
    ForeignProxy(const areg::String & role, areg::DispatcherThread * owner)
        : areg::ProxyBase(role, foreign_interface(), owner)
    {
    }

    void process_response_event(areg::ServiceResponseEvent & eventElem) override
    {
        const areg::ResultType result{ static_cast<areg::ResultType>(eventElem.envelope().result()) };
        const uint32_t msgId{ eventElem.response_id() };
        const areg::CallType kind{ eventElem.call_kind() };
        if (kind != areg::CallType::Request)
            notify_listeners(kind, foreign_interface().find(kind, msgId), result, eventElem.sequence_number());
        else if (msgId == REQ_ADDED)
            notify_request_failed(1u, 1u, result, eventElem.sequence_number());
        else if (msgId == REQ_SYNC)
            notify_request_failed(0u, 0u, result, eventElem.sequence_number());
        else
            gOtherFailures.fetch_add(1u);
    }

    void process_custom_event(areg::Event &) override
    {
    }

    void register_service_listeners() override
    {
        areg::ProxyBase::register_service_listeners();
        SharedResponseResponseEvent::add_listener(static_cast<areg::EventConsumer &>(*this), proxy_dispatcher_thread());
    }

    void unregister_service_listeners() noexcept override
    {
        SharedResponseResponseEvent::remove_listener(static_cast<areg::EventConsumer &>(*this), proxy_dispatcher_thread());
        areg::ProxyBase::unregister_service_listeners();
    }

    ServiceAvailableEvent create_service_available(areg::NotificationConsumer & consumer) override
    {
        return ServiceAvailableEvent(consumer);
    }

    areg::NotificationEvent create_client_notification(const areg::NotificationEventData & data) const override
    {
        return areg::NotificationEvent(data);
    }

    areg::ServiceRequestEvent create_request(const areg::SharedBuffer & args, uint32_t reqId) override
    {
        return SharedResponseRequestEvent(args, proxy_address(), stub_address(), reqId);
    }

    areg::ServiceRequestEvent create_request(uint32_t reqId, uint32_t reserve) override
    {
        return SharedResponseRequestEvent(proxy_address(), stub_address(), reqId, reserve);
    }

    areg::ServiceRequestEvent create_notification_request(uint32_t msgId, areg::RequestType reqType, areg::CallType kind) override
    {
        return SharedResponseNotifyRequestEvent(proxy_address(), stub_address(), msgId, reqType, kind);
    }

    areg::ServiceResponseEvent create_request_failed(const areg::ProxyAddress & target, uint32_t msgId, areg::CallType kind, areg::ResultType reason, const SequenceNumber & seqNr) const override
    {
        return SharedResponseResponseEvent(target, reason, msgId, kind, seqNr);
    }
};

//!< Calls the added request, then sync, through the foreign proxy.
class MiForeignConsumer final : public areg::Component
                            , public areg::ProxyListener
{
public:
    MiForeignConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component   ( entry, owner )
        , areg::ProxyListener( )
        , mProxy            ( areg::ProxyBase::acquire_proxy(areg::String(_foreignProvider), foreign_interface(), *this, &ForeignProxy::create, owner) )
    {
    }

    ~MiForeignConsumer() override
    {
        if (mProxy != nullptr)
            mProxy->free_proxy(*this);
    }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & /* proxy */) override
    {
        if (areg::is_service_connected(status) && (mProxy != nullptr))
            gAddedSeq.store(proxy().request(REQ_ADDED, RESP_ADDED, *this));

        return true;
    }

    void process_notification_event(areg::NotificationEvent & eventElem) override
    {
        const areg::NotificationEventData & data{ static_cast<const areg::NotificationEvent &>(eventElem).data() };
        process_notification(data.notify_kind(), data.notify_index(), data.notify_type(), data.sequence());
    }

    void process_notification(areg::CallType kind, uint32_t index, areg::ResultType result, SequenceNumber seqNr) override
    {
        const uint32_t msgId{ foreign_interface().message_id(kind, index) };
        if ((kind == areg::CallType::Request) && (msgId == REQ_ADDED) && (result == areg::ResultType::RequestUnsupported))
        {
            gUnsupported.fetch_add(1);
            gFailedSeq.store(seqNr);
            gAddedLeft.store(mProxy->has_any_listener(areg::CallType::Response, 1u));
            static_cast<void>(proxy().request(REQ_SYNC, RESP_SYNCED, *this));
        }
        else if ((kind == areg::CallType::Response) && (msgId == RESP_SYNCED) && (result == areg::ResultType::RequestOK))
        {
            gSynced.fetch_add(1);
        }
        else
        {
            gOtherFailures.fetch_add(1);
        }

        changed();
    }

private:
    ForeignProxy & proxy()
    {
        return static_cast<ForeignProxy &>(*mProxy);
    }

    std::shared_ptr<areg::ProxyBase> mProxy;
};

//////////////////////////////////////////////////////////////////////////
// Models
//////////////////////////////////////////////////////////////////////////

namespace mi_model
{
BEGIN_MODEL(_model)
    BEGIN_REGISTER_THREAD(_providerThread)
        BEGIN_REGISTER_COMPONENT(_providerRole, MiProvider)
            REGISTER_IMPLEMENT_SERVICE(SharedResponse::ServiceName, SharedResponse::InterfaceVersion)
        END_REGISTER_COMPONENT(_providerRole)
    END_REGISTER_THREAD(_providerThread)

    BEGIN_REGISTER_THREAD(_consumerThread)
        BEGIN_REGISTER_COMPONENT(_consumerRole, MiConsumer)
            REGISTER_DEPENDENCY(_providerRole)
        END_REGISTER_COMPONENT(_consumerRole)
    END_REGISTER_THREAD(_consumerThread)
END_MODEL(_model)
}

namespace mi_model_foreign
{
BEGIN_MODEL(_modelForeign)
    BEGIN_REGISTER_THREAD(_foreignProviderThread)
        BEGIN_REGISTER_COMPONENT(_foreignProvider, MiProvider)
            REGISTER_IMPLEMENT_SERVICE(SharedResponse::ServiceName, SharedResponse::InterfaceVersion)
        END_REGISTER_COMPONENT(_foreignProvider)
    END_REGISTER_THREAD(_foreignProviderThread)

    BEGIN_REGISTER_THREAD(_foreignThread)
        BEGIN_REGISTER_COMPONENT(_foreignRole, MiForeignConsumer)
        END_REGISTER_COMPONENT(_foreignRole)
    END_REGISTER_THREAD(_foreignThread)
END_MODEL(_modelForeign)
}

namespace mi_model_alone
{
BEGIN_MODEL(_modelAlone)
    BEGIN_REGISTER_THREAD(_aloneThread)
        BEGIN_REGISTER_COMPONENT(_aloneRole, MiConsumer)
            REGISTER_DEPENDENCY(_missingRole)
        END_REGISTER_COMPONENT(_aloneRole)
    END_REGISTER_THREAD(_aloneThread)
END_MODEL(_modelAlone)
}

//////////////////////////////////////////////////////////////////////////
// Cases
//////////////////////////////////////////////////////////////////////////

static void print()
{
    printf("first failed, second failed, reason ...: %u, %u, %s\n", gFirstFailed.load(), gSecondFailed.load()
          , (gFirstFailed.load() + gSecondFailed.load()) != 0 ? areg::as_string(static_cast<areg::ResultType>(gReason.load())) : "-");
    printf("done, synced ..........................: %u, %u\n", gDone.load(), gSynced.load());
    printf("updates, updates after stop ...........: %u, %u\n", gUpdates.load(), gUpdatesAfter.load());
}

//!< A failure of second is reported as second, once, with the expected reason.
static bool run_shared_case(const char * model, areg::ResultType expected, bool synced)
{
    const bool done{ wait_until([synced] { return ((gFirstFailed.load() + gSecondFailed.load()) >= 1) && (!synced || (gSynced.load() >= 1)); }) };
    areg::Application::unload_model(model);
    print();
    return done
        && (gFirstFailed.load() == 0)
        && (gSecondFailed.load() == 1)
        && (gReason.load() == static_cast<uint32_t>(expected));
}

int main(int argc, char * argv[])
{
    setvbuf(stdout, nullptr, _IONBF, 0);

    struct Entry { const char * name; Case which; areg::ResultType expected; };
    const Entry cases[]
    {
          { "shared-busy"           , Case::Busy         , areg::ResultType::RequestBusy        }
        , { "shared-cancel"         , Case::Cancel       , areg::ResultType::RequestCanceled    }
        , { "shared-error-request"  , Case::ErrorRequest , areg::ResultType::RequestError       }
        , { "shared-error-response" , Case::ErrorResponse, areg::ResultType::RequestError       }
        , { "shared-unavailable"    , Case::Unavailable  , areg::ResultType::ServiceUnavailable }
        , { "unknown-request"       , Case::Unknown      , areg::ResultType::RequestOK          }
        , { "remove-all"            , Case::RemoveAll    , areg::ResultType::RequestOK          }
        , { "unsupported-request"   , Case::Unsupported  , areg::ResultType::RequestUnsupported }
    };

    const char * name{ argc > 1 ? argv[1] : "shared-busy" };
    const Entry * selected{ nullptr };
    for (const Entry & entry : cases)
    {
        if (std::strcmp(name, entry.name) == 0)
            selected = &entry;
    }

    if (selected == nullptr)
    {
        printf("unknown case [ %s ]\n", name);
        return 2;
    }

    gCase.store(selected->which);
    const char * model{ selected->which == Case::Unavailable ? _modelAlone : (selected->which == Case::Unsupported ? _modelForeign : _model) };
    areg::Application::setup(false, true, false, true, false, nullptr);
    bool ok{ areg::Application::load_model(model) };
    if (ok)
    {
        switch (selected->which)
        {
        case Case::Unknown:
            ok = wait_until([] { return gSynced.load() >= 1; });
            areg::Application::unload_model(model);
            print();
            ok = ok && (gSynced.load() == 1);
            break;

        case Case::Unsupported:
            ok = wait_until([] { return gSynced.load() >= 1; });
            areg::Application::unload_model(model);
            printf("unsupported, other failures ...........: %u, %u\n", gUnsupported.load(), gOtherFailures.load());
            printf("sequence of call, of failure ..........: %llu, %llu\n", static_cast<unsigned long long>(gAddedSeq.load()), static_cast<unsigned long long>(gFailedSeq.load()));
            ok = ok && (gUnsupported.load() == 1) && (gOtherFailures.load() == 0) && (gSynced.load() == 1)
                    && (gAddedSeq.load() != areg::SEQUENCE_NUMBER_ANY) && (gFailedSeq.load() == gAddedSeq.load()) && (gAddedLeft.load() == false);
            break;

        case Case::RemoveAll:
            ok = wait_until([] { return gSynced.load() >= 1; });
            areg::Application::unload_model(model);
            print();
            ok = ok && (gUpdates.load() == 1) && (gUpdatesAfter.load() == 0);
            break;

        case Case::Busy:
            ok = run_shared_case(model, selected->expected, true) && (gDone.load() == 1);
            break;

        default:
            ok = run_shared_case(model, selected->expected, selected->which != Case::Unavailable);
            break;
        }
    }

    areg::Application::release();

    printf("case ..................................: %s\n", name);
    printf("%s\n", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
