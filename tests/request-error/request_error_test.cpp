/************************************************************************
 * A provider's error of a request reaches its caller as request_X_failed, never as response_X,
 * and never reaches a consumer that only subscribed to the response.
 *
 * Usage: areg-request-error-test <case>
 *   error              error_request(request, false) fails the waiting caller with RequestError
 *   error-unblocked    error_request(request, false) of an unblocked request fails nobody
 *   cancel             cancel_all_requests() fails the waiting caller with RequestCanceled
 *   cancel-session     cancel_all_requests() fails an unblocked caller and invalidates its session
 *   response-error     error_request(response, false) fails the waiting caller with RequestError
 *   error-all          error_all_requests() fails the waiting caller once with RequestError
 *   subscribed-caller  a caller that also subscribed is failed once, then answered once
 *   busy               a second caller gets RequestBusy; no subscriber gets it
 *
 * Caller A and subscriber D share one thread (one proxy), subscriber B has its own, caller C and
 * subscriber E share a third. A response with no waiting request is sent after each error.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "examples/24_pubunblock/services/HelloUnblockConsumerBase.hpp"
#include "examples/24_pubunblock/services/HelloUnblockProviderBase.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <functional>
#include <mutex>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "24_generated")
#endif // _MSC_VER

namespace
{
    constexpr char const    _model[]        { "ReModel" };
    constexpr char const    _providerThread[]{ "ReProviderThread" };
    constexpr char const    _callerThread[] { "ReCallerThread" };
    constexpr char const    _watchThread[]  { "ReWatchThread" };
    constexpr char const    _otherThread[]  { "ReOtherThread" };
    constexpr char const    _providerRole[] { "ReProvider" };
    constexpr char const    _callerA[]      { "ReCallerA" };
    constexpr char const    _subscriberD[]  { "ReSubscriberD" };
    constexpr char const    _subscriberB[]  { "ReSubscriberB" };
    constexpr char const    _callerC[]      { "ReCallerC" };
    constexpr char const    _subscriberE[]  { "ReSubscriberE" };

    constexpr uint32_t      WAIT_MS         { 30000 };
    constexpr uint32_t      TAG_A           { 1 };
    constexpr uint32_t      TAG_C           { 3 };

    enum class Case : uint32_t { Error, ErrorUnblocked, Cancel, CancelSession, ResponseError, ErrorAll, SubscribedCaller, Busy };

    std::atomic<Case>       gCase           { Case::Error };

    //!< What one consumer received.
    struct Received
    {
        std::atomic_uint32_t    responses   { 0 };  //!< response_hello_unblock calls.
        std::atomic_uint32_t    lastSeq     { 0 };  //!< sequenceNr of the last response_hello_unblock.
        std::atomic_uint32_t    failures    { 0 };  //!< request_hello_ublock_failed calls.
        std::atomic_uint32_t    reason      { 0 };  //!< reason of the last request_hello_ublock_failed.
        std::atomic_uint32_t    otherFailed { 0 };  //!< request_identifier_failed calls.
    };

    Received                gA;
    Received                gB;
    Received                gC;
    Received                gD;
    Received                gE;
    std::atomic_int32_t     gPrepareAfterCancel { -1 };

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

    //!< The number of subscribers that confirm their subscription with request_identifier.
    uint32_t subscribers()
    {
        switch (gCase.load())
        {
        case Case::SubscribedCaller:
            return 3u;
        case Case::Busy:
            return 3u;
        default:
            return 2u;
        }
    }

    Received & received_of(const areg::String & role)
    {
        if (role == _callerA)
            return gA;
        if (role == _subscriberB)
            return gB;
        if (role == _callerC)
            return gC;
        if (role == _subscriberD)
            return gD;
        return gE;
    }
}

//////////////////////////////////////////////////////////////////////////
// Provider
//////////////////////////////////////////////////////////////////////////

//!< Holds the first request until every subscription is confirmed, then runs the case.
class ReProvider final : public    areg::Component
                       , protected HelloUnblockProviderBase
{
public:
    ReProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , HelloUnblockProviderBase  ( static_cast<areg::Component &>(*this) )
    {
    }

protected:
    void request_identifier() override
    {
        ++ mSyncs;
        response_identifier(mSyncs);
        try_act();
    }

    void request_hello_ublock(uint32_t clientId, uint32_t sequenceNr) override
    {
        if (mActed)
        {
            response_hello_unblock(clientId, sequenceNr);
            return;
        }

        mHeld = true;
        mTag  = clientId;
        if ((gCase.load() == Case::ErrorUnblocked) || (gCase.load() == Case::CancelSession))
        {
            mSession = unblock_current_request();
        }

        try_act();
    }

private:
    void try_act()
    {
        const Case which{ gCase.load() };
        const uint32_t needed{ subscribers() + (which == Case::Busy ? 1u : 0u) };
        if (mActed || !mHeld || (mSyncs < needed))
            return;

        mActed = true;
        switch (which)
        {
        case Case::Error:
        case Case::SubscribedCaller:
            error_request(static_cast<uint32_t>(HelloUnblock::MessageIDs::MsgId_request_hello_ublock), false);
            response_hello_unblock(mTag, 0);
            break;

        case Case::ErrorUnblocked:
            error_request(static_cast<uint32_t>(HelloUnblock::MessageIDs::MsgId_request_hello_ublock), false);
            if (prepare_response(mSession))
            {
                response_hello_unblock(mTag, 1);
            }
            break;

        case Case::Cancel:
            cancel_all_requests();
            response_hello_unblock(mTag, 0);
            break;

        case Case::CancelSession:
            cancel_all_requests();
            gPrepareAfterCancel.store(prepare_response(mSession) ? 1 : 0);
            response_hello_unblock(mTag, 0);
            break;

        case Case::ResponseError:
            error_request(static_cast<uint32_t>(HelloUnblock::MessageIDs::MsgId_response_hello_unblock), false);
            response_hello_unblock(mTag, 0);
            break;

        case Case::ErrorAll:
            error_all_requests();
            response_hello_unblock(mTag, 0);
            break;

        case Case::Busy:
            response_hello_unblock(mTag, 1);
            break;
        }

        changed();
    }

    uint32_t            mSyncs  { 0 };      //!< request_identifier calls received.
    bool                mHeld   { false };  //!< The first request is held.
    bool                mActed  { false };  //!< The case action ran.
    uint32_t            mTag    { 0 };      //!< clientId of the held request.
    areg::SessionID     mSession{ 0 };      //!< Session of the held request, when unblocked.
};

//////////////////////////////////////////////////////////////////////////
// Consumers
//////////////////////////////////////////////////////////////////////////

//!< A caller (A, C) or a subscriber (B, D, E) of response_hello_unblock, chosen by its role.
class ReConsumer final : public    areg::Component
                       , protected HelloUnblockConsumerBase
{
public:
    ReConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , HelloUnblockConsumerBase  ( _providerRole, static_cast<areg::Component &>(*this) )
        , mReceived                 ( received_of(entry.mRoleName) )
        , mRole                     ( entry.mRoleName )
    {
    }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) override
    {
        const bool result{ HelloUnblockConsumerBase::service_connected(status, proxy) };
        if (!areg::is_service_connected(status))
            return result;

        const Case which{ gCase.load() };
        const bool caller{ (mRole == _callerA) || ((mRole == _callerC) && (which == Case::Busy)) };
        const bool subscriber{ (mRole == _subscriberB) || (mRole == _subscriberD)
                            || ((mRole == _subscriberE) && (which == Case::Busy))
                            || ((mRole == _callerA) && (which == Case::SubscribedCaller)) };
        if (subscriber)
        {
            notify_on_response_hello_unblock(true);
            request_identifier();
        }

        if (caller)
        {
            request_hello_ublock(mRole == _callerA ? TAG_A : TAG_C, 1);
        }

        return result;
    }

    void response_hello_unblock(uint32_t /* clientId */, uint32_t sequenceNr) override
    {
        mReceived.lastSeq.store(sequenceNr);
        mReceived.responses.fetch_add(1);
        if ((mRole == _callerA) && (sequenceNr == 1) && (gCase.load() != Case::Busy))
        {
            request_hello_ublock(TAG_A, 2);
        }

        changed();
    }

    void request_hello_ublock_failed(areg::ResultType reason) override
    {
        mReceived.reason.store(static_cast<uint32_t>(reason));
        mReceived.failures.fetch_add(1);
        if (gCase.load() == Case::Busy)
        {
            request_identifier();
        }
        else if (mRole == _callerA)
        {
            request_hello_ublock(TAG_A, 2);
        }

        changed();
    }

    void response_identifier(uint32_t /* clientId */) override
    {
        changed();
    }

    void request_identifier_failed(areg::ResultType /* reason */) override
    {
        mReceived.otherFailed.fetch_add(1);
        changed();
    }

private:
    Received &      mReceived;  //!< What this consumer received.
    areg::String    mRole;      //!< The role name of this consumer.
};

//////////////////////////////////////////////////////////////////////////
// Model
//////////////////////////////////////////////////////////////////////////

namespace re_model
{
BEGIN_MODEL(_model)
    BEGIN_REGISTER_THREAD(_providerThread)
        BEGIN_REGISTER_COMPONENT(_providerRole, ReProvider)
            REGISTER_IMPLEMENT_SERVICE(HelloUnblock::ServiceName, HelloUnblock::InterfaceVersion)
        END_REGISTER_COMPONENT(_providerRole)
    END_REGISTER_THREAD(_providerThread)

    BEGIN_REGISTER_THREAD(_callerThread)
        BEGIN_REGISTER_COMPONENT(_callerA, ReConsumer)
            REGISTER_DEPENDENCY(_providerRole)
        END_REGISTER_COMPONENT(_callerA)
        BEGIN_REGISTER_COMPONENT(_subscriberD, ReConsumer)
            REGISTER_DEPENDENCY(_providerRole)
        END_REGISTER_COMPONENT(_subscriberD)
    END_REGISTER_THREAD(_callerThread)

    BEGIN_REGISTER_THREAD(_watchThread)
        BEGIN_REGISTER_COMPONENT(_subscriberB, ReConsumer)
            REGISTER_DEPENDENCY(_providerRole)
        END_REGISTER_COMPONENT(_subscriberB)
    END_REGISTER_THREAD(_watchThread)

    BEGIN_REGISTER_THREAD(_otherThread)
        BEGIN_REGISTER_COMPONENT(_callerC, ReConsumer)
            REGISTER_DEPENDENCY(_providerRole)
        END_REGISTER_COMPONENT(_callerC)
        BEGIN_REGISTER_COMPONENT(_subscriberE, ReConsumer)
            REGISTER_DEPENDENCY(_providerRole)
        END_REGISTER_COMPONENT(_subscriberE)
    END_REGISTER_THREAD(_otherThread)
END_MODEL(_model)
}

//////////////////////////////////////////////////////////////////////////
// Cases
//////////////////////////////////////////////////////////////////////////

static void print(const char * name, const Received & received)
{
    printf("%s responses, last seq, failures, reason: %u, %u, %u, %s\n", name
          , received.responses.load(), received.lastSeq.load(), received.failures.load()
          , received.failures.load() != 0 ? areg::as_string(static_cast<areg::ResultType>(received.reason.load())) : "-");
}

//!< True if the subscriber got exactly the expected responses and no failure.
static bool subscriber_ok(const Received & received, uint32_t responses)
{
    return (received.responses.load() == responses) && (received.failures.load() == 0) && (received.otherFailed.load() == 0);
}

//!< Runs a case whose caller A is failed (or not) and then answered with sequence 2.
static bool run_error_case(areg::ResultType expected)
{
    const bool failsA{ expected != areg::ResultType::RequestOK };
    const uint32_t answers{ failsA ? 1u : 2u };
    const bool done{ wait_until([answers] { return (gA.responses.load() >= answers) && (gA.lastSeq.load() == 2)
                                                && (gB.responses.load() >= answers) && (gD.responses.load() >= answers); }) };
    areg::Application::unload_model(_model);
    print("A", gA);
    print("B", gB);
    print("D", gD);
    const bool aOk{ failsA ? ((gA.failures.load() == 1) && (gA.reason.load() == static_cast<uint32_t>(expected)))
                           : (gA.failures.load() == 0) };
    return done && aOk
        && (gA.responses.load() == answers) && (gA.otherFailed.load() == 0)
        && subscriber_ok(gB, answers)
        && subscriber_ok(gD, answers);
}

static bool run_busy_case()
{
    const bool done{ wait_until([] { return ((gA.failures.load() + gC.failures.load()) == 1)
                                         && ((gA.responses.load() + gC.responses.load()) == 1)
                                         && (gB.responses.load() == 1) && (gD.responses.load() == 1) && (gE.responses.load() == 1); }) };
    areg::Application::unload_model(_model);
    print("A", gA);
    print("C", gC);
    print("B", gB);
    print("D", gD);
    print("E", gE);
    const Received & busy{ gA.failures.load() == 1 ? gA : gC };
    const Received & holder{ gA.failures.load() == 1 ? gC : gA };
    return done
        && (busy.reason.load() == static_cast<uint32_t>(areg::ResultType::RequestBusy))
        && (busy.responses.load() == 0)
        && (holder.failures.load() == 0) && (holder.responses.load() == 1)
        && subscriber_ok(gB, 1) && subscriber_ok(gD, 1) && subscriber_ok(gE, 1);
}

int main(int argc, char * argv[])
{
    setvbuf(stdout, nullptr, _IONBF, 0);

    struct Entry { const char * name; Case which; areg::ResultType expected; };
    const Entry cases[]
    {
          { "error"             , Case::Error           , areg::ResultType::RequestError    }
        , { "error-unblocked"   , Case::ErrorUnblocked  , areg::ResultType::RequestOK       }
        , { "cancel"            , Case::Cancel          , areg::ResultType::RequestCanceled }
        , { "cancel-session"    , Case::CancelSession   , areg::ResultType::RequestCanceled }
        , { "response-error"    , Case::ResponseError   , areg::ResultType::RequestError    }
        , { "error-all"         , Case::ErrorAll        , areg::ResultType::RequestError    }
        , { "subscribed-caller" , Case::SubscribedCaller, areg::ResultType::RequestError    }
        , { "busy"              , Case::Busy            , areg::ResultType::RequestBusy     }
    };

    const char * name{ argc > 1 ? argv[1] : "error" };
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
        ok = (selected->which == Case::Busy) ? run_busy_case() : run_error_case(selected->expected);
        if (selected->which == Case::CancelSession)
        {
            printf("prepare after cancel .....: %d\n", gPrepareAfterCancel.load());
            ok = ok && (gPrepareAfterCancel.load() == 0);
        }
    }

    areg::Application::release();

    printf("case .....................: %s\n", name);
    printf("%s\n", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
