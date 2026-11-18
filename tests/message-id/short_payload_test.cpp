/************************************************************************
 * Payloads that hold fewer bytes than the parameter list of their entry, and an error of a
 * broadcast. Uses the generated ShortPayload interface.
 *
 * Usage: areg-short-payload-test <case>
 *   short-request          request_pair with only its first parameter: request_pair_failed(RequestUnsupported),
 *                          the provider's request_pair does not run
 *   empty-request          request_pair with no payload: the same
 *   short-string-request   request_label whose text count runs past the payload: request_label_failed(RequestUnsupported)
 *   short-response         response_paired with only its first parameter: response_paired does not run,
 *                          invalid_response(response_paired) runs, the waiting call leaves no listener
 *   short-broadcast        broadcast_pushed with only its first parameter: broadcast_pushed does not run,
 *                          invalid_broadcast(broadcast_pushed) runs
 *   short-attribute        an update of Value with two bytes: on_value_update(DataIsInvalid)
 *   broadcast-error        error_request() of broadcast_pushed: invalid_broadcast(broadcast_pushed) runs
 *                          once in the subscriber, the next broadcast arrives with its values
 *
 * Every case ends with a complete message of the same kind, which must arrive with its own values.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/ProxyBase.hpp"
#include "areg/component/ServiceResponseEvent.hpp"
#include "tests/message-id/services/ShortPayloadConsumerBase.hpp"
#include "tests/message-id/services/ShortPayloadProviderBase.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <functional>
#include <mutex>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "message_id_short_generated")
#endif // _MSC_VER

namespace
{
    constexpr char const    _model[]        { "SpModel" };
    constexpr char const    _providerThread[]{ "SpProviderThread" };
    constexpr char const    _consumerThread[]{ "SpConsumerThread" };
    constexpr char const    _providerRole[] { "SpProvider" };
    constexpr char const    _consumerRole[] { "SpConsumer" };

    constexpr uint32_t      WAIT_MS         { 30000 };

    //!< Values of the complete messages that end a case.
    constexpr uint32_t      FIRST           { 1u };
    constexpr uint32_t      SECOND          { 2u };
    constexpr uint32_t      LAST_FIRST      { 5u };
    constexpr uint32_t      LAST_SECOND     { 6u };
    constexpr uint32_t      LAST_VALUE      { 7u };

    //!< The message IDs of the requests sent raw.
    constexpr uint32_t      REQ_PAIR        { ShortPayload::message_id(ShortPayload::RequestIDs::MsgId_request_pair) };
    constexpr uint32_t      REQ_LABEL       { ShortPayload::message_id(ShortPayload::RequestIDs::MsgId_request_label) };
    //!< The indexes of the entries sent short.
    constexpr uint32_t      RESP_PAIRED     { static_cast<uint32_t>(ShortPayload::ResponseIDs::MsgId_response_paired) };
    constexpr uint32_t      BC_PUSHED       { static_cast<uint32_t>(ShortPayload::BroadcastIDs::MsgId_broadcast_pushed) };
    constexpr uint32_t      ATTR_VALUE      { static_cast<uint32_t>(ShortPayload::AttributeIDs::MsgId_Value) };

    enum class Case : uint32_t { ShortRequest, EmptyRequest, ShortString, ShortResponse, ShortBroadcast, ShortAttribute, BroadcastError };

    std::atomic<Case>       gCase           { Case::ShortRequest };

    std::atomic_uint32_t    gPairCalls      { 0 };  //!< request_pair runs in the provider.
    std::atomic_uint32_t    gLabelCalls     { 0 };  //!< request_label runs in the provider.
    std::atomic_uint32_t    gWrongArgs      { 0 };  //!< provider calls with arguments nobody sent.
    std::atomic_uint32_t    gPairFailed     { 0 };  //!< request_pair_failed calls.
    std::atomic_uint32_t    gLabelFailed    { 0 };  //!< request_label_failed calls.
    std::atomic_uint32_t    gReason         { 0 };  //!< reason of the last request failure.
    std::atomic_uint32_t    gPaired         { 0 };  //!< response_paired calls.
    std::atomic_uint32_t    gPairedWrong    { 0 };  //!< response_paired calls with values that were not sent.
    std::atomic_uint32_t    gPushed         { 0 };  //!< broadcast_pushed calls.
    std::atomic_uint32_t    gPushedLast     { 0 };  //!< broadcast_pushed calls with the last values.
    std::atomic_uint32_t    gInvalidResp    { 0 };  //!< invalid_response or invalid_broadcast calls with the entry of the case.
    std::atomic_uint32_t    gInvalidOther   { 0 };  //!< invalid_response or invalid_broadcast calls with any other entry.
    std::atomic_uint32_t    gValueOk        { 0 };  //!< on_value_update calls with DataIsOK.
    std::atomic_uint32_t    gValueInvalid   { 0 };  //!< on_value_update calls with DataIsInvalid after the first valid value.
    std::atomic_uint32_t    gValueLast      { 0 };  //!< on_value_update calls with the last value.
    std::atomic_bool        gListenerLeft   { true };   //!< the proxy kept a listener of the answered response.

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

    //!< Reaches the protected request call of a generated proxy.
    class ProxyAccess : public areg::ProxyBase
    {
    public:
        static void send_raw_request(areg::ProxyBase & proxy, uint32_t reqId, const areg::SharedBuffer & args, areg::NotificationConsumer & caller)
        {
            using SendFunc = SequenceNumber (areg::ProxyBase::*)(uint32_t, const areg::SharedBuffer &, areg::NotificationConsumer *, uint32_t);
            const SendFunc send{ &ProxyAccess::send_request_event };
            static_cast<void>((proxy.*send)(reqId, args, &caller, RESP_PAIRED));
        }
    };
}

//////////////////////////////////////////////////////////////////////////
// Provider
//////////////////////////////////////////////////////////////////////////

//!< Answers requests, and sends the short message of the case once.
class SpProvider final  : public    areg::Component
                        , protected ShortPayloadProviderBase
{
public:
    SpProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , ShortPayloadProviderBase  ( static_cast<areg::Component &>(*this) )
    {
    }

protected:
    void startup_service_interface(areg::Component & holder) override
    {
        ShortPayloadProviderBase::startup_service_interface(holder);
        set_value(FIRST);
    }

    void request_pair(uint32_t first, uint32_t second) override
    {
        gPairCalls.fetch_add(1);
        if ((first != FIRST) || (second != SECOND))
            gWrongArgs.fetch_add(1);

        if ((gCase.load() == Case::ShortResponse) && !mSent)
        {
            mSent = true;
            send_short(areg::CallType::Response, RESP_PAIRED);
        }
        else
        {
            response_paired(first, second);
        }
    }

    void request_label(const areg::String & /* text */, uint32_t number) override
    {
        gLabelCalls.fetch_add(1);
        if (number != SECOND)
            gWrongArgs.fetch_add(1);

        response_paired(FIRST, number);
    }

    void request_trigger(uint32_t /* what */) override
    {
        switch (gCase.load())
        {
        case Case::ShortResponse:
            broadcast_pushed(LAST_FIRST, LAST_SECOND);
            break;

        case Case::ShortBroadcast:
            send_short(areg::CallType::Broadcast, BC_PUSHED);
            broadcast_pushed(LAST_FIRST, LAST_SECOND);
            break;

        case Case::BroadcastError:
            error_request(areg::CallType::Broadcast, BC_PUSHED, false);
            broadcast_pushed(LAST_FIRST, LAST_SECOND);
            break;

        case Case::ShortAttribute:
            {
                areg::SharedBuffer data;
                const uint16_t half{ 0xABCDu };
                data << half;
                send_update_event(areg::CallType::Attribute, ATTR_VALUE, data, areg::ResultType::DataOK);
                set_value(LAST_VALUE);
            }
            break;

        default:
            break;
        }
    }

private:
    //!< Sends a response or a broadcast that holds only its first parameter.
    void send_short(areg::CallType kind, uint32_t index)
    {
        areg::StubBase::StubListenerList listeners;
        areg::ServiceResponseEvent evt{ prepare_response_event(kind, index, areg::ResultType::RequestOK, static_cast<uint32_t>(sizeof(uint32_t)), listeners) };
        if (!evt.is_valid())
            return;

        evt.write_stream() << FIRST;
        send_response_notification(listeners, evt);
    }

    bool    mSent{ false };     //!< The short response was sent.
};

//////////////////////////////////////////////////////////////////////////
// Consumer
//////////////////////////////////////////////////////////////////////////

//!< Starts the case and records every callback.
class SpConsumer final  : public    areg::Component
                        , protected ShortPayloadConsumerBase
{
public:
    SpConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , ShortPayloadConsumerBase  ( _providerRole, static_cast<areg::Component &>(*this) )
    {
    }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) override
    {
        const bool result{ ShortPayloadConsumerBase::service_connected(status, proxy) };
        if (!areg::is_service_connected(status))
            return result;

        areg::NotificationConsumer & caller{ static_cast<areg::NotificationConsumer &>(*this) };
        switch (gCase.load())
        {
        case Case::ShortRequest:
            {
                areg::SharedBuffer args;
                args << FIRST;
                ProxyAccess::send_raw_request(proxy, REQ_PAIR, args, caller);
            }
            break;

        case Case::EmptyRequest:
            ProxyAccess::send_raw_request(proxy, REQ_PAIR, areg::SharedBuffer{}, caller);
            break;

        case Case::ShortString:
            {
                areg::SharedBuffer args;
                const uint32_t count{ 100u };
                args << count;
                args.write(reinterpret_cast<const uint8_t *>("abc"), 3u);
                ProxyAccess::send_raw_request(proxy, REQ_LABEL, args, caller);
            }
            break;

        case Case::ShortResponse:
            notify_on_broadcast_pushed(true);
            request_pair(FIRST, SECOND);
            request_trigger(0u);
            break;

        case Case::ShortBroadcast:  // fall through
        case Case::BroadcastError:
            notify_on_broadcast_pushed(true);
            request_trigger(0u);
            break;

        case Case::ShortAttribute:
            notify_on_value_update(true);
            break;
        }

        return result;
    }

    void request_pair_failed(areg::ResultType reason) override
    {
        gReason.store(static_cast<uint32_t>(reason));
        gPairFailed.fetch_add(1);
        request_pair(FIRST, SECOND);
        changed();
    }

    void request_label_failed(areg::ResultType reason) override
    {
        gReason.store(static_cast<uint32_t>(reason));
        gLabelFailed.fetch_add(1);
        request_pair(FIRST, SECOND);
        changed();
    }

    void response_paired(uint32_t first, uint32_t second) override
    {
        gPaired.fetch_add(1);
        if ((first != FIRST) || (second != SECOND))
            gPairedWrong.fetch_add(1);

        changed();
    }

    void broadcast_pushed(uint32_t first, uint32_t second) override
    {
        gPushed.fetch_add(1);
        if ((first == LAST_FIRST) && (second == LAST_SECOND))
        {
            gListenerLeft.store(service_proxy()->has_any_listener(areg::CallType::Response, RESP_PAIRED));
            gPushedLast.fetch_add(1);
        }

        changed();
    }

    void invalid_response(ShortPayload::ResponseIDs respId) override
    {
        if ((gCase.load() == Case::ShortResponse) && (respId == ShortPayload::ResponseIDs::MsgId_response_paired))
            gInvalidResp.fetch_add(1);
        else
            gInvalidOther.fetch_add(1);

        changed();
    }

    void invalid_broadcast(ShortPayload::BroadcastIDs broadcastId) override
    {
        if (((gCase.load() == Case::ShortBroadcast) || (gCase.load() == Case::BroadcastError)) && (broadcastId == ShortPayload::BroadcastIDs::MsgId_broadcast_pushed))
            gInvalidResp.fetch_add(1);
        else
            gInvalidOther.fetch_add(1);

        changed();
    }

    void on_value_update(uint32_t value, areg::DataState state) override
    {
        if (state == areg::DataState::DataIsOK)
        {
            gValueOk.fetch_add(1);
            if (value == FIRST)
            {
                request_trigger(0u);
            }
            else if (value == LAST_VALUE)
            {
                gValueLast.fetch_add(1);
            }
        }
        else if ((state == areg::DataState::DataIsInvalid) && (gValueOk.load() != 0))
        {
            gValueInvalid.fetch_add(1);
        }

        changed();
    }
};

//////////////////////////////////////////////////////////////////////////
// Model
//////////////////////////////////////////////////////////////////////////

namespace sp_model
{
BEGIN_MODEL(_model)
    BEGIN_REGISTER_THREAD(_providerThread)
        BEGIN_REGISTER_COMPONENT(_providerRole, SpProvider)
            REGISTER_IMPLEMENT_SERVICE(ShortPayload::ServiceName, ShortPayload::InterfaceVersion)
        END_REGISTER_COMPONENT(_providerRole)
    END_REGISTER_THREAD(_providerThread)

    BEGIN_REGISTER_THREAD(_consumerThread)
        BEGIN_REGISTER_COMPONENT(_consumerRole, SpConsumer)
            REGISTER_DEPENDENCY(_providerRole)
        END_REGISTER_COMPONENT(_consumerRole)
    END_REGISTER_THREAD(_consumerThread)
END_MODEL(_model)
}

//////////////////////////////////////////////////////////////////////////
// Cases
//////////////////////////////////////////////////////////////////////////

static void print()
{
    printf("provider pair, label, wrong arguments .: %u, %u, %u\n", gPairCalls.load(), gLabelCalls.load(), gWrongArgs.load());
    printf("pair failed, label failed, reason ....: %u, %u, %s\n", gPairFailed.load(), gLabelFailed.load()
          , (gPairFailed.load() + gLabelFailed.load()) != 0 ? areg::as_string(static_cast<areg::ResultType>(gReason.load())) : "-");
    printf("paired, paired with wrong values .....: %u, %u\n", gPaired.load(), gPairedWrong.load());
    printf("pushed, pushed last, listener left ...: %u, %u, %s\n", gPushed.load(), gPushedLast.load(), gListenerLeft.load() ? "yes" : "no");
    printf("value ok, invalid, last ..............: %u, %u, %u\n", gValueOk.load(), gValueInvalid.load(), gValueLast.load());
    printf("invalid response, other ID ...........: %u, %u\n", gInvalidResp.load(), gInvalidOther.load());
}

int main(int argc, char * argv[])
{
    setvbuf(stdout, nullptr, _IONBF, 0);

    struct Entry { const char * name; Case which; };
    const Entry cases[]
    {
          { "short-request"         , Case::ShortRequest    }
        , { "empty-request"         , Case::EmptyRequest    }
        , { "short-string-request"  , Case::ShortString     }
        , { "short-response"        , Case::ShortResponse   }
        , { "short-broadcast"       , Case::ShortBroadcast  }
        , { "short-attribute"       , Case::ShortAttribute  }
        , { "broadcast-error"       , Case::BroadcastError  }
    };

    const char * name{ argc > 1 ? argv[1] : "short-request" };
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
    areg::Application::setup(false, true, false, true, false, nullptr);
    bool ok{ areg::Application::load_model(_model) };
    if (ok)
    {
        const areg::ResultType unsupported{ areg::ResultType::RequestUnsupported };
        switch (selected->which)
        {
        case Case::ShortRequest:    // fall through
        case Case::EmptyRequest:
            ok = wait_until([] { return gPaired.load() >= 1; });
            areg::Application::unload_model(_model);
            print();
            ok = ok && (gPairFailed.load() == 1) && (gReason.load() == static_cast<uint32_t>(unsupported))
                    && (gPairCalls.load() == 1) && (gWrongArgs.load() == 0) && (gPaired.load() == 1) && (gPairedWrong.load() == 0);
            break;

        case Case::ShortString:
            ok = wait_until([] { return gPaired.load() >= 1; });
            areg::Application::unload_model(_model);
            print();
            ok = ok && (gLabelFailed.load() == 1) && (gReason.load() == static_cast<uint32_t>(unsupported))
                    && (gLabelCalls.load() == 0) && (gPairCalls.load() == 1) && (gWrongArgs.load() == 0) && (gPaired.load() == 1);
            break;

        case Case::ShortResponse:
            ok = wait_until([] { return gPushedLast.load() >= 1; });
            areg::Application::unload_model(_model);
            print();
            ok = ok && (gPaired.load() == 0) && (gPairCalls.load() == 1) && (gWrongArgs.load() == 0)
                    && (gPushed.load() == 1) && (gListenerLeft.load() == false);
            ok = ok && (gInvalidResp.load() == 1) && (gInvalidOther.load() == 0);
            break;

        case Case::ShortBroadcast:  // fall through
        case Case::BroadcastError:
            ok = wait_until([] { return gPushedLast.load() >= 1; });
            areg::Application::unload_model(_model);
            print();
            ok = ok && (gPushed.load() == 1) && (gPushedLast.load() == 1);
            ok = ok && (gInvalidResp.load() == 1) && (gInvalidOther.load() == 0);
            break;

        case Case::ShortAttribute:
            ok = wait_until([] { return gValueLast.load() >= 1; });
            areg::Application::unload_model(_model);
            print();
            ok = ok && (gValueOk.load() == 2) && (gValueInvalid.load() == 1) && (gValueLast.load() == 1);
            break;
        }
    }

    areg::Application::release();

    printf("case ..................................: %s\n", name);
    printf("%s\n", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
